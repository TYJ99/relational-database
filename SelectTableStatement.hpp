#pragma once

#ifndef SelectTableStatement_hpp
#define SelectTableStatement_hpp

#include "SQLStatement.hpp"
#include "Query.hpp"
#include "Joins.hpp"


namespace ECE141 {
	

	class SelectTableStatement : public SQLStatement {
	public:
		SelectTableStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SQLStatement(aDatabase, Keywords::select_kw) {}
		StatusResult		  run(std::ostream& aStream) const override;
		static StatusResult	  recognize(Tokenizer& aTokenizer);
		StatusResult		  parse(Tokenizer& aTokenizer) override;
		StatusResult		  parseHeader(KWList& aKWList, Tokenizer& aTokenizer) override;
		StatusResult		  parseClause(Keywords& aKW, Tokenizer& aTokenizer);
		StatusResult		  verifySelectEntity(StringList aEntityNameList);
		StatusResult		  verifySelectAttr(StringList aEntityNameList, StringList aAttrNameList);
		SelectTableStatement& addJoin(Join& aJoin);
		JoinList&			  getJoins() { return joins; }
		Query				  query;

	protected:
		JoinList joins;
		
	};

}




#endif // !SelectTableStatement_hpp


