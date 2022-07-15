
#ifndef ParseInsertHelper_hpp
#define ParseInsertHelper_hpp

#include <iostream>
#include "Tokenizer.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include "Helpers.hpp"
#include "Errors.hpp"
#include "Config.hpp"
#include "Database.hpp"

namespace ECE141 {

	class ParseInsertHelper {
	public:
		ParseInsertHelper(Tokenizer &aTokenizer, Database* aDatabase, const std::string& aTableName):
										tokenizer(aTokenizer), db(aDatabase), tableName(aTableName) {
			db->getTable(aTableName).value()->setNextRowPos(db->getTable(aTableName).value()->getRows().size());
		}
		bool		  checkAttribute(std::string& aName);
		StatusResult  parseQuote(std::string& aFieldName, std::string& aQuote, bool isAttr);
			
		StatusResult  parseAttrName(std::vector<Attribute>& anAttrList);
		StatusResult  parseValues(std::vector<std::string>& aValList);
		StatusResult  verifyAttrsVals(std::vector<std::string>& aValList, std::vector<Attribute>& anAttrList);
		StatusResult  validate(const Attribute& anAttr, std::string& aValue);
			
	protected:
		Tokenizer&         tokenizer;
		Database*		   db;
		const std::string& tableName;
	};




}




#endif // !ParseInsertHelper_hpp

