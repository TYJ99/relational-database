
#include "UpdateStatement.hpp"

namespace ECE141 {

	// parse Update to Set
	StatusResult  UpdateStatement::parseHeader(KWList& aKWList, Tokenizer& aTokenizer) {
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
			
			if (aTokenizer.skipIf(Keywords::set_kw)) {
				theResult = theParseHelper.parseSetAttribute(theAttrValMap, theAttrNameList);
			}
			if (theResult) {
				query.addField(theAttrNameList);
				query.addAttrValMap(theAttrValMap);
			}
			if (theResult && verifySelectEntity(query.getTableNameList())) {
				if (!verifySelectAttr(query.getTableNameList(), query.getfieldsSelected())) {
					theResult = unknownAttribute;
				}
				else {
					TableMap theEntityMap;
					for (auto& entityName : query.getTableNameList()) {
						theEntityMap.emplace(entityName, db->getTable(entityName).value());
					}
					query.addTableMap(theEntityMap);
				}
			}
			else {
				theResult = unknownEntity;
			}
		}
		return theResult;
	}

	StatusResult UpdateStatement::recognize(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::update_kw };
		return Helpers::helpRecog(aTokenizer, theKWList);
	}

	StatusResult UpdateStatement::parse(Tokenizer& aTokenizer) {
		StatusResult theResult;
		std::vector<Attribute> theAttrList;
		KWList theKWList{ Keywords::update_kw };
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

	StatusResult UpdateStatement::run(std::ostream& aStream) const {
		//RowList rows;
		return db->updateRows(query);
	}
}