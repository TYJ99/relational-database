#include "DeleteStatement.hpp"

namespace ECE141 {
	// parse Update to Set
	StatusResult  DeleteStatement::parseHeader(KWList& aKWList, Tokenizer& aTokenizer) {
		TestSequencer	    theSequencer(aTokenizer);
		StatusResult		theResult;
		AttributeValueMap	theAttrValMap;
		StringList			theAttrNameList;
		if (theSequencer.nextIs(aKWList)) {

			if (TokenType::identifier != aTokenizer.current().type) {
				theResult = invalidCommand;
			}
			StringList theEntityNameList;
			ParseHelper theParseHelper(aTokenizer);
			theParseHelper.parseIdentifierList(theEntityNameList);
			query.addTableName(theEntityNameList);
			
			if (theResult && verifySelectEntity(query.getTableNameList())) {				
				TableMap theEntityMap;
				for (auto& entityName : query.getTableNameList()) {
					theEntityMap.emplace(entityName, db->getTable(entityName).value());
				}
				query.addTableMap(theEntityMap);				
			}
			else {
				theResult = unknownEntity;
			}
		}
		return theResult;
	}

	StatusResult DeleteStatement::recognize(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::delete_kw, Keywords::from_kw };
		return Helpers::helpRecog(aTokenizer, theKWList);
	}

	StatusResult DeleteStatement::parse(Tokenizer& aTokenizer) {
		StatusResult theResult;
		std::vector<Attribute> theAttrList;
		KWList theKWList{ Keywords::delete_kw, Keywords::from_kw };
		theResult = parseHeader(theKWList, aTokenizer);
		while (theResult && aTokenizer.more()) {
			if (TokenType::keyword == aTokenizer.current().type) {
				theResult = parseClause(aTokenizer.current().keyword, aTokenizer);
			}
			else {
				if (";" == aTokenizer.current().data) {
					if (!aTokenizer.next()) {
						return theResult;
					}
				}
				break;
			}
		}
		if (theResult) {
			return StatusResult{ invalidCommand }; // the last char is not ';'
		}
		return theResult;
	}

	StatusResult DeleteStatement::run(std::ostream& aStream) const {
		//RowList rows;
		return db->deleteRows(query);
	}
}