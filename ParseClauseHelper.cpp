#include "ParseClauseHelper.hpp"
#include "Filters.hpp"
#include <algorithm>

namespace ECE141 {

	StatusResult ParseClauseHelper::verifySelectAttr(StringList aEntityNameList, StringList aAttrNameList) {
		StatusResult theResult;
		AttributeList theAttrList;
		StringList theVerifyList;
		for (auto& entityName : aEntityNameList) {
			if (db->getTable(entityName).has_value()) {
				theAttrList = db->getTable(entityName).value()->getAttributes();
			}
			else {
				theResult = invalidAttribute;
				break;
			}

			for (auto& attrName : aAttrNameList) {
				if (db->getTable(entityName).value()->getAttribute(attrName)) {
					theVerifyList.push_back(attrName);
				}
			}
		}
		if (theVerifyList.size() != aAttrNameList.size()) {
			theResult = invalidAttribute;
		}
		return theResult;
	}


	StatusResult ParseClauseHelper::parseOrderBy() {	
		StringList theAttrNameList;
		if ("by" == tokenizer.current().data) {
			while (tokenizer.next()) {
				Token& theToken = tokenizer.current();
				if (TokenType::identifier == theToken.type) {
					theAttrNameList.push_back(theToken.data);
				}
				else if (TokenType::number == theToken.type) {
					size_t theIndex = std::stoi(theToken.data);
					if (theIndex <= query.getfieldsSelected().size()) {
						theAttrNameList.push_back(query.getfieldsSelected()[theIndex - 1]);
					}
					else {
						return StatusResult{ unknownColumn };
					}
				}
				else {
					break;
				}
			}			

			if (!theAttrNameList.empty()) {
				if (verifySelectAttr(query.getTableNameList(), theAttrNameList)) {
					query.addOrderBy(theAttrNameList);
					return StatusResult{ noError };
				}
				else {
					return StatusResult{ unknownColumn };
				}
			}			
		}
		return StatusResult{ syntaxError };
	}

	StatusResult ParseClauseHelper::parseLimit() {
		StatusResult theResult{ syntaxError };
		query.setHasLimit(true);
		Token& theToken = tokenizer.current();
		if (TokenType::number == tokenizer.current().type) {			
			if (theToken.data.find('.') == std::string::npos) {
				query.setLimit(std::stoi(theToken.data));
				theResult = noError;
			}
			if (tokenizer.next() && tokenizer.skipIf(',')) {
				if (TokenType::number == tokenizer.current().type) {
					if (theToken.data.find('.') == std::string::npos) {
						query.setOffset(query.getLimit());
						query.setLimit(std::stoi(tokenizer.current().data));
						theResult = noError;
						tokenizer.next();
					}
				}
			}			
		}
		//tokenizer.next();
		return theResult;		
	}

	StatusResult ParseClauseHelper::parseGroupBy() {
		return StatusResult{ noError };
	}

	StatusResult ParseClauseHelper::parseLeft(JoinList& aJoinList) {
		if (Keywords::join_kw == tokenizer.current().keyword) {
			return tokenizer.next() ? parseJoin(aJoinList, Keywords::left_kw) : StatusResult{ invalidCommand };
		}
		return StatusResult{ invalidCommand };
	}

	StatusResult ParseClauseHelper::parseRight(JoinList& aJoinList) {
		if (Keywords::join_kw == tokenizer.current().keyword) {
			return tokenizer.next() ? parseJoin(aJoinList, Keywords::right_kw) : StatusResult{ invalidCommand };
		}
		return StatusResult{ invalidCommand };
	}

	StatusResult ParseClauseHelper::parseJoin(JoinList& aJoinList, Keywords aJoinType) {
		if (TokenType::identifier == tokenizer.current().type) {
			std::string theJoinTable = tokenizer.current().data;
			query.addALLTableName(theJoinTable);
			Join theJoin(theJoinTable, aJoinType);
			aJoinList.push_back(theJoin);

			if (tokenizer.next()) {
				if (aJoinType != Keywords::inner_kw && Keywords::on_kw != tokenizer.current().keyword) {
					return StatusResult{ invalidCommand };
				}
				return StatusResult{ noError };
			}
		}
		return StatusResult{ invalidCommand };
	}

	StatusResult ParseClauseHelper::parseTableField(std::string& aString, Entity& anEntity, 
														Attribute& anAttr, JoinList& aJoinList) {
		std::string theAttrName = "";
		std::string theEntityName = "";
		if (TokenType::identifier == tokenizer.current().type) {
			theAttrName = tokenizer.current().data;
			if (tokenizer.next()) {
				if (tokenizer.skipIf('.')) {
					theEntityName = theAttrName;
					theAttrName = tokenizer.current().data;
					tokenizer.next();
				}
				StringList theEntityNameList = query.getAllTableNameList();				
				if ("" == theEntityName) {					
					StringList theAttrNameList{theAttrName};					
					if (verifySelectAttr(theEntityNameList, theAttrNameList)) {
						for (auto& entityName : theEntityNameList) {
							if (db->getTable(entityName).value()->getAttribute(theAttrName)) {
								anEntity = *db->getTable(entityName).value();
								anAttr = db->getTable(entityName).value()->getAttribute(theAttrName).value();
								return StatusResult{ noError };
							}
						}
					}
					aString = theAttrName;
					return StatusResult{ noError };
				}
				else {
					if (std::find(theEntityNameList.begin(), theEntityNameList.end(), theEntityName) != theEntityNameList.end()) {
						if (db->getTable(theEntityName).value()->getAttribute(theAttrName)) {
							anEntity = *db->getTable(theEntityName).value();
							anAttr = db->getTable(theEntityName).value()->getAttribute(theAttrName).value();
							return StatusResult{ noError };
						}						
					}
				}
			}
		}
		return StatusResult{ invalidCommand };
	}	

	StatusResult ParseClauseHelper::parseOperand(Operand& anOperand, JoinList& aJoinList) {																
		StatusResult theResult{ noError };
		Token& theToken = tokenizer.current();

		// handle quote and apostrophe
		if ("\"" == theToken.data) {
			theResult = Filters::parseQuote(theToken.data, tokenizer, anOperand);
		}
		else if ("'" == theToken.data) {
			theResult = Filters::parseQuote(theToken.data, tokenizer, anOperand);
		}
		else if (TokenType::identifier == theToken.type) {
			// find theAttr which is actually exist in the entity we tend to select.
			Attribute theAttr;
			Entity theEntity("");
			std::string theString = "";
			bool isAttr = false;
			if (parseTableField(theString, theEntity, theAttr, aJoinList)) {
				if (theString.length() == 0) {
					isAttr = true;
				}
				if (isAttr) {
					anOperand.ttype = TokenType::identifier;
					anOperand.name = theAttr.getName(); //hang on to name...
					anOperand.entityId = Helpers::hashString(theEntity.getName().c_str());
					anOperand.dtype = theAttr.getType();
					anOperand.entityName = theEntity.getName();
				}
				else {
					anOperand.ttype = TokenType::string;
					anOperand.dtype = DataTypes::varchar_type;
					anOperand.value = theString;
				}
			}
			else {
				return StatusResult{ invalidCommand };
			}
			return StatusResult{ noError };
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
		if (theResult) tokenizer.next();
		return theResult;
	}	

	StatusResult ParseClauseHelper::parseExpr(JoinList& aJoinList) {
		if (2 > tokenizer.remaining()) return StatusResult{ syntaxError };
		StatusResult  theResult{ noError };
		Logical theLogical{ Logical::unknown_op };
		while (theResult && (2 < tokenizer.remaining() || Logical::no_op != theLogical)) {
			bool existNot = false;
			// check if there is "NOT" logical operator
			if (TokenType::keyword == tokenizer.current().type) {
				if ("not" == tokenizer.current().data) {
					existNot = true;
					tokenizer.next();
				}
			}
			Operand theLHS, theRHS;
			Token& theToken = tokenizer.current();
			// check there must be a number or identifier after the Logical operators
			if (theToken.type != TokenType::identifier && theToken.type != TokenType::number) {
				if (existNot || Logical::no_op != theLogical) {
					theResult = syntaxError;
				}
				return theResult;
			}
			if ((theResult = parseOperand(theLHS, aJoinList))) {
				Token& theToken = tokenizer.current();
				if (theToken.type == TokenType::operators) {
					Operators theOp = Helpers::toOperator(theToken.data);
					tokenizer.next();
					if ((theResult = parseOperand(theRHS, aJoinList))) {
						if (Filters::validateOperands(theLHS, theRHS)) {
							theLogical = Logical::no_op;
							// check if there is a logic operator
							if (TokenType::keyword == tokenizer.current().type) {
								theLogical = Helpers::toLogical(tokenizer.current().data);
								if (Logical::not_op == theLogical) {
									theResult = syntaxError;
									break;
								}
								else if (Logical::no_op != theLogical) {
									tokenizer.next();
								}
							}
							Expression theExpr(theLHS, theOp, theRHS, theLogical, existNot);
							aJoinList[aJoinList.size() - 1].add(theExpr);
						}
						else theResult.error = syntaxError;
					}
				}
			}
			else theResult.error = syntaxError;
		}
		return theResult;
	}

	StatusResult ParseClauseHelper::parseOn(JoinList& aJoinList) {
		auto& theCurrJoin = aJoinList[aJoinList.size() - 1];
		return parseExpr(aJoinList);
	}

	StatusResult ParseClauseHelper::parseWhere() {
		Filters* theFilter = query.getFilters();
		auto theEntityList = db->getTables();
		return theFilter->parse(tokenizer, theEntityList);
	}
}