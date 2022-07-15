#ifndef ParseClauseHelper_hpp
#define ParseClauseHelper_hpp

#include "Query.hpp"
#include "keywords.hpp"
#include "Database.hpp"
#include "Entity.hpp"
#include "Joins.hpp"
#include <map>


namespace ECE141 {
	class Joins;
	class ParseClauseHelper {
	public:

		ParseClauseHelper(Tokenizer& aTokenizer, Query& aQuery, Database* aDatabase):
								query(aQuery), tokenizer(aTokenizer), db(aDatabase) {}

		StatusResult verifySelectAttr(StringList aEntityNameList, StringList aAttrNameList);
		StatusResult parseOrderBy();
		StatusResult parseLimit();		
		StatusResult parseGroupBy();
		StatusResult parseLeft(JoinList& aJoinList);
		StatusResult parseRight(JoinList& aJoinList);
		StatusResult parseJoin(JoinList& aJoinList, Keywords aJoinType);
		StatusResult parseWhere();
		StatusResult parseOn(JoinList& aJoinList);
		StatusResult parseExpr(JoinList& aJoinList);
		StatusResult parseTableField(std::string& aString, Entity& anEntity, Attribute& anAttr, JoinList& aJoinList);
		StatusResult parseOperand(Operand& anOperand, JoinList& aJoinList);
		
	protected:
		Query&     query;
		Tokenizer& tokenizer;
		Database*  db;
	};
}







#endif // !ParseClauseHelper_hpp
