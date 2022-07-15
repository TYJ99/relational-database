//
//  Filters.cpp
//  Datatabase5
//
//  Created by rick gessner on 3/5/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#include "Filters.hpp"
#include <string>
#include <limits>
#include "keywords.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include "Compare.hpp"

namespace ECE141 {

	using Comparitor = bool (*)(Value& aLHS, Value& aRHS);

	bool equals(Value& aLHS, Value& aRHS) {
		bool theResult = false;

		std::visit([&](auto const& aLeft) {
			std::visit([&](auto const& aRight) {
				theResult = isEqual(aLeft, aRight);
				}, aRHS);
			}, aLHS);
		return theResult;
	}

	bool less(Value& aLHS, Value& aRHS) {
		bool theResult = false;

		std::visit([&](auto const& aLeft) {
			std::visit([&](auto const& aRight) {
				theResult = isLess(aLeft, aRight);
				}, aRHS);
			}, aLHS);
		return theResult;
	}

	bool lessEquals(Value& aLHS, Value& aRHS) {
		bool theResult = false;

		std::visit([&](auto const& aLeft) {
			std::visit([&](auto const& aRight) {
				theResult = !isGreater(aLeft, aRight);
				}, aRHS);
			}, aLHS);
		return theResult;
	}

	bool greater(Value& aLHS, Value& aRHS) {
		bool theResult = false;

		std::visit([&](auto const& aLeft) {
			std::visit([&](auto const& aRight) {
				theResult = isGreater(aLeft, aRight);
				}, aRHS);
			}, aLHS);
		return theResult;
	}

	bool greaterEquals(Value& aLHS, Value& aRHS) {
		bool theResult = false;

		std::visit([&](auto const& aLeft) {
			std::visit([&](auto const& aRight) {
				theResult = !isLess(aLeft, aRight);
				}, aRHS);
			}, aLHS);
		return theResult;
	}

	static std::map<Operators, Comparitor> comparitors{
	  {Operators::equal_op, equals},
	  {Operators::lt_op,	less},
	  {Operators::lte_op,	lessEquals},
	  {Operators::gt_op,	greater},
	  {Operators::gte_op,	greaterEquals}
	  //STUDENT: Add more for other operators...
	};

	bool Expression::operator()(KeyValues& aList) {
		Value theLHS{ lhs.value };
		Value theRHS{ rhs.value };

		if (TokenType::identifier == lhs.ttype) {
			DataTypes theAttrType = lhs.dtype;
			theLHS = aList[lhs.name]; //get row value
			if (DataTypes::int_type == theAttrType) {
				theLHS = std::stoi(std::get<std::string>(theLHS));
			}
			else if (DataTypes::float_type == theAttrType) {
				theLHS = std::stof(std::get<std::string>(theLHS));
			}
		}

		if (TokenType::identifier == rhs.ttype) {
			DataTypes theAttrType = rhs.dtype;
			theRHS = aList[rhs.name]; //get row value
			if (DataTypes::int_type == theAttrType) {
				theRHS = std::stoi(std::get<std::string>(theRHS));
			}
			else if (DataTypes::float_type == theAttrType) {
				theRHS = std::stof(std::get<std::string>(theRHS));
			}
		}

		return comparitors.count(op)
			? comparitors[op](theLHS, theRHS) : false;
	}

	//--------------------------------------------------------------

	Filters::Filters() {}

	Filters::Filters(const Filters& aCopy) {
	}

	Filters::~Filters() {
		//no need to delete expressions, they're unique_ptrs!
	}

	Filters& Filters::add(Expression* anExpression) {
		expressions.push_back(std::unique_ptr<Expression>(anExpression));
		return *this;
	}

	//compare expressions to row; return true if matches
	bool Filters::matches(KeyValues& aList) const {

		//STUDENT: You'll need to add code here to deal with
		//         logical combinations (AND, OR, NOT):
		//         like:  WHERE zipcode=92127 AND age>20
		Logical theLogic = Logical::no_op;
		bool	thePrevResult = true;
		for (auto& theExpr : expressions) {
			bool	theCurrResult = true;
			if (theExpr->existNot) {				
				theCurrResult = !(*theExpr)(aList);
			}
			else {
				theCurrResult = (*theExpr)(aList);
			}
			
			if (Logical::and_op == theLogic) {
				thePrevResult = theCurrResult && thePrevResult;
			}
			else if (Logical::or_op == theLogic) {
				thePrevResult = theCurrResult || thePrevResult;
			}
			else {
				thePrevResult = theCurrResult;
			}
			theLogic = theExpr->logic;
		}
		return thePrevResult;
	}

	StatusResult Filters::parseQuote(std::string& aQuote, Tokenizer& aTokenizer, Operand& anOperand) {
			std::string theValue = "";
			while (aTokenizer.next() && aQuote != aTokenizer.current().data) {
				theValue += aTokenizer.current().data + " ";
			}
			if (aTokenizer.more()) {
				theValue = theValue.substr(0, theValue.length() - 1); // trim the trailing whitespace 
				if (std::isdigit(theValue[0])) {
					anOperand.ttype = TokenType::number;
					anOperand.dtype = DataTypes::int_type;
					if (theValue.find('.') != std::string::npos) {
						anOperand.dtype = DataTypes::float_type;
						anOperand.value = std::stof(theValue);
					}
					else anOperand.value = std::stoi(theValue);
				}
				else {
					anOperand.ttype = TokenType::string;
					anOperand.dtype = DataTypes::varchar_type;
					anOperand.value = theValue;					
				}
				return StatusResult{ noError };
			}
			return StatusResult{ unknownClause };
	}

	//where operand is field, number, string...
	StatusResult parseOperand(Tokenizer& aTokenizer,
		std::vector<Entity>& anEntityList, Operand& anOperand) {
		StatusResult theResult{ noError };
		Token& theToken = aTokenizer.current();

		// handle quote and apostrophe
		if ("\"" == theToken.data) {
			theResult = Filters::parseQuote(theToken.data, aTokenizer, anOperand);
		}
		else if ("'" == theToken.data) {
			theResult = Filters::parseQuote(theToken.data, aTokenizer, anOperand);
		}
		else if (TokenType::identifier == theToken.type) {
			// find theAttr which is actually exist in the entity we tend to select.
			Attribute theAttr;
			Entity theEntity = anEntityList[0];
			bool isAttr = false;
			for (auto& entity : anEntityList) {
				if (entity.getAttribute(theToken.data)) {
					theAttr = entity.getAttribute(theToken.data).value();
					theEntity = entity;
					isAttr = true;
					break;
				}
			}

			if (isAttr) {
				anOperand.ttype = theToken.type;
				anOperand.name = theToken.data; //hang on to name...
				anOperand.entityId = Helpers::hashString(theToken.data.c_str());
				anOperand.dtype = theAttr.getType();
				anOperand.entityName = theEntity.getName();
			}
			else {
				anOperand.ttype = TokenType::string;
				anOperand.dtype = DataTypes::varchar_type;
				anOperand.value = theToken.data;
			}
		}
		else if (TokenType::number == theToken.type) {
			anOperand.ttype = TokenType::number;
			anOperand.dtype = DataTypes::int_type;
			if (theToken.data.find('.') != std::string::npos) {
				anOperand.dtype = DataTypes::float_type;
				anOperand.value = std::stof(theToken.data);
			}
			else anOperand.value = std::stoi(theToken.data);
		}
		else theResult.error = syntaxError;
		if (theResult) aTokenizer.next();
		return theResult;
	}

	//STUDENT: Add validation here...
	bool Filters::validateOperands(Operand& aLHS, Operand& aRHS) {
		if (TokenType::identifier == aLHS.ttype) { //most common case...
			if (aLHS.dtype == aRHS.dtype || 
				(aLHS.dtype == DataTypes::char_type && aRHS.dtype == DataTypes::varchar_type)) {
				return true;
			}				
		}
		else if (TokenType::identifier == aRHS.ttype) {
			if (aLHS.dtype == aRHS.dtype ||
				(aRHS.dtype == DataTypes::char_type && aLHS.dtype == DataTypes::varchar_type)) {
				return true;
			}
		}
		else if (TokenType::identifier == aLHS.ttype && TokenType::identifier == aRHS.ttype) {
			return true;
		}
		return false;
	}

	//STUDENT: This starting point code may need adaptation...
	StatusResult Filters::parse(Tokenizer& aTokenizer, std::vector<Entity>& anEntityList) {
		if (2 > aTokenizer.remaining()) return StatusResult{syntaxError};
		StatusResult  theResult{ noError };
		Logical theLogical{ Logical::unknown_op };
		while (theResult && (2 < aTokenizer.remaining() || Logical::no_op != theLogical)) {
			bool existNot = false;
			// check if there is "NOT" logical operator
			if (TokenType::keyword == aTokenizer.current().type) {
				if ("not" == aTokenizer.current().data) {
					existNot = true;
					aTokenizer.next();
				}
			}
			Operand theLHS, theRHS;
			Token& theToken = aTokenizer.current();
			// check there must be a number or identifier after the Logical operators
			if (theToken.type != TokenType::identifier && theToken.type != TokenType::number) {
				if (existNot || Logical::no_op != theLogical) {
					theResult = syntaxError;
				}
				return theResult;
			}
			if ((theResult = parseOperand(aTokenizer, anEntityList, theLHS))) {
				Token& theToken = aTokenizer.current();
				if (theToken.type == TokenType::operators) {
					Operators theOp = Helpers::toOperator(theToken.data);
					aTokenizer.next();
					if ((theResult = parseOperand(aTokenizer, anEntityList, theRHS))) {
						if (validateOperands(theLHS, theRHS)) {
							theLogical = Logical::no_op ;
							// check if there is a logic operator
							if (TokenType::keyword == aTokenizer.current().type) {
								theLogical = Helpers::toLogical(aTokenizer.current().data);
								if (Logical::not_op == theLogical) {
									theResult = syntaxError;
									break;
								}
								else if (Logical::no_op != theLogical) {
									aTokenizer.next();
								}
							}									
							add(new Expression(theLHS, theOp, theRHS, theLogical, existNot));
							//if (aTokenizer.skipIf(semicolon)) {
							//	break;
							//}
						}
						else theResult.error = syntaxError;
					}
				}
			}
			else theResult.error = syntaxError;
		}
		return theResult;
	}

}

