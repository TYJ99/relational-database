//
//  Join.hpp
//  RGAssignment8
//
//  Created by rick gessner on 5/21/21.
//

#ifndef Join_h
#define Join_h

#include <string>
#include "BasicTypes.hpp"
#include "Errors.hpp"
#include "keywords.hpp"
#include "Row.hpp"
#include "Filters.hpp"

namespace ECE141 {

	class Join {
	public:
		Join(const std::string& aTable, Keywords aType)
			: table(aTable), joinType(aType) {
		}

		std::string table; // the table we want to join
		Keywords    joinType;  //left, right, etc...
		Join& add(Expression& anExpression) {
			expressions.push_back(anExpression);
			return *this;
		}
		const std::vector<Expression>& getExpressions() const{
			return expressions;
		}

	protected:
		std::vector<Expression> expressions;
	};

	using JoinList = std::vector<Join>;

}

#endif /* Join_h */
