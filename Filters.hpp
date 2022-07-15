//
//  Filters.hpp
//  RGAssignment5
//
//  Created by rick gessner on 4/4/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#ifndef Filters_h
#define Filters_h

#include <stdio.h>
#include <vector>
#include <memory>
#include <string>
#include "Errors.hpp"
#include "Tokenizer.hpp"
#include "BasicTypes.hpp"


namespace ECE141 {

	class Row;
	class Entity;

	struct Operand {
		Operand() : ttype(TokenType::unknown), dtype(DataTypes::varchar_type), name(""),
			entityId(0), entityName("") {}
		Operand(std::string& aName, TokenType aType, Value& aValue, size_t anId = 0)
			: ttype(aType), dtype(DataTypes::varchar_type), name(aName),
			value(aValue), entityId(anId), entityName("") {}

		Operand& operator =(const Operand& anOperand) {
			ttype = anOperand.ttype;
			dtype = anOperand.dtype;
			name = anOperand.name;
			value = anOperand.value;
			entityId = anOperand.entityId;
			entityName = anOperand.entityName;
			return *this;
		}

		Operand(const Operand& anOperand) {
			*this = anOperand;
		}

		TokenType   ttype; //is it a field, or const (#, string)...
		DataTypes   dtype;
		std::string name;  //attr name
		Value       value;
		size_t      entityId;
		std::string entityName;
	};

	//---------------------------------------------------

	struct Expression {
		Operand     lhs;  //id
		Operand     rhs;  //usually a constant; maybe a field...
		Operators   op;   //=     //users.id=books.author_id
		Logical     logic; //and, or, not...
		bool	    existNot;

		Expression(Operand& aLHSOperand, Operators anOp,
			Operand& aRHSOperand, Logical& aLogic, bool aNot)
			: lhs(aLHSOperand), rhs(aRHSOperand),
			op(anOp), logic(aLogic), existNot(aNot) {}

		Expression& operator =(const Expression& anExp) {
			lhs = anExp.lhs;
			rhs = anExp.rhs;
			op = anExp.op;
			logic = anExp.logic;
			existNot = anExp.existNot;
			return *this;
		}

		Expression(const Expression& anExp) {
			*this = anExp;
		}
		

		bool operator()(KeyValues& aList);
	};

	using Expressions = std::vector<std::unique_ptr<Expression> >;

	//---------------------------------------------------

	class Filters {
	public:

		Filters();
		Filters(const Filters& aFilters);
		~Filters();

		size_t        getCount() const { return expressions.size(); }
		bool          matches(KeyValues& aList) const;
		Filters&	  add(Expression* anExpression);

		StatusResult  parse(Tokenizer& aTokenizer, std::vector<Entity>& anEntityList);
		static StatusResult  parseQuote(std::string& aQuote, Tokenizer& aTokenizer, Operand& anOperand);
		static bool validateOperands(Operand& aLHS, Operand& aRHS);

	protected:
		Expressions   expressions;
	};

}

#endif /* Filters_h */
