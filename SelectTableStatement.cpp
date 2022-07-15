
#include "SelectTableStatement.hpp"
#include "ParseClauseHelper.hpp"

namespace ECE141 {

	using ClauseParser = StatusResult(*)(Tokenizer&, Query&, Database*, JoinList&);

	StatusResult  SelectTableStatement::verifySelectAttr(StringList aEntityNameList, StringList aAttrNameList) {
		StatusResult theResult;
		AttributeList theAttrList;
		StringList verifyList;
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
					verifyList.push_back(attrName);
				}
			}
		}
		if (verifyList.size() != aAttrNameList.size()) {
			theResult = invalidAttribute;
		}
		return theResult;
	}

	StatusResult  SelectTableStatement::verifySelectEntity(StringList aEntityNameList) {
		StatusResult theResult;
		for (auto& entityName : aEntityNameList) {
			if (!db->getTable(entityName).has_value()) {
				theResult = unknownEntity;
				break;
			}
		}
		return theResult;
	}

	// parse SELECT to FROM
	StatusResult  SelectTableStatement::parseHeader(KWList& aKWList, Tokenizer& aTokenizer) {
		TestSequencer theSequencer(aTokenizer);
		StatusResult theResult;
		if (theSequencer.nextIs(aKWList)) {
			if ("*" == aTokenizer.current().data) {
				query.setSelectAll();
				if (!aTokenizer.next()) {
					theResult = invalidCommand;
				}
			}
			else {
				StringList theAttrNameList;
				ParseHelper theParseHelper(aTokenizer);
				theParseHelper.parseIdentifierList(theAttrNameList);
				if (!aTokenizer.more()) {
					theResult = invalidCommand;
				}
				query.addField(theAttrNameList);
			}

			if (theResult) {
				if (!aTokenizer.skipIf(Keywords::from_kw)) {
					theResult = invalidCommand;
				}
				StringList theEntityNameList;
				ParseHelper theParseHelper(aTokenizer);
				theParseHelper.parseIdentifierList(theEntityNameList);
				query.addTableName(theEntityNameList);
				query.addALLTableName(theEntityNameList);
			}
			
		}
		return theResult;
	}

	StatusResult parseOrderBy(Tokenizer& aTokenizer, Query& aQuery, Database* db, JoinList& aJoinList) {
		ParseClauseHelper theParseClauseHelper(aTokenizer, aQuery, db);
		return theParseClauseHelper.parseOrderBy();
	}

	StatusResult parseLimit(Tokenizer& aTokenizer, Query& aQuery, Database* db, JoinList& aJoinList) {
		ParseClauseHelper theParseClauseHelper(aTokenizer, aQuery, db);
		return theParseClauseHelper.parseLimit();
	}

	StatusResult parseGroupBy(Tokenizer& aTokenizer, Query& aQuery, Database* db, JoinList& aJoinList) {
		ParseClauseHelper theParseClauseHelper(aTokenizer, aQuery, db);
		return theParseClauseHelper.parseGroupBy();
	}

	StatusResult parseLeft(Tokenizer& aTokenizer, Query& aQuery, Database* db, JoinList& aJoinList) {
		ParseClauseHelper theParseClauseHelper(aTokenizer, aQuery, db);
		return theParseClauseHelper.parseLeft(aJoinList);
	}

	StatusResult parseRight(Tokenizer& aTokenizer, Query& aQuery, Database* db, JoinList& aJoinList) {
		ParseClauseHelper theParseClauseHelper(aTokenizer, aQuery, db);
		return theParseClauseHelper.parseRight(aJoinList);
	}

	StatusResult parseJoin(Tokenizer& aTokenizer, Query& aQuery, Database* db, JoinList& aJoinList) {
		ParseClauseHelper theParseClauseHelper(aTokenizer, aQuery, db);
		return theParseClauseHelper.parseJoin(aJoinList, Keywords::inner_kw);
	}

	StatusResult parseWhere(Tokenizer& aTokenizer, Query& aQuery, Database* db, JoinList& aJoinList) {
		ParseClauseHelper theParseClauseHelper(aTokenizer, aQuery, db);
		return theParseClauseHelper.parseWhere();
	}

	StatusResult parseOn(Tokenizer& aTokenizer, Query& aQuery, Database* db, JoinList& aJoinList) {
		ParseClauseHelper theParseClauseHelper(aTokenizer, aQuery, db);
		return theParseClauseHelper.parseOn(aJoinList);
	}

	StatusResult  SelectTableStatement::parseClause(Keywords& aKW, Tokenizer& aTokenizer) {
		std::map<Keywords, ClauseParser> theDispatch{
			{Keywords::order_kw, parseOrderBy},
			{Keywords::limit_kw, parseLimit},
			{Keywords::group_kw, parseGroupBy},
			{Keywords::left_kw,  parseLeft},
			{Keywords::right_kw, parseRight},
			{Keywords::join_kw,  parseJoin},
			{Keywords::where_kw, parseWhere},
			{Keywords::on_kw,	 parseOn}
		};
		if (theDispatch.count(aKW)) {
			return aTokenizer.next() ?
				theDispatch[aKW](aTokenizer, query, db, joins) : StatusResult{ invalidCommand };
		}
		return StatusResult{ unknownKeyword };
	}

	StatusResult SelectTableStatement::recognize(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::select_kw };
		return Helpers::helpRecog(aTokenizer, theKWList);
	}

	StatusResult SelectTableStatement::parse(Tokenizer& aTokenizer) {
		StatusResult theResult;
		std::vector<Attribute> theAttrList;
		KWList theKWList{ Keywords::select_kw };
		theResult = parseHeader(theKWList, aTokenizer);
		while (theResult && aTokenizer.more()) {
			if (TokenType::keyword == aTokenizer.current().type) {
				theResult = parseClause(aTokenizer.current().keyword, aTokenizer);
			}
			else {
				if (";" == aTokenizer.current().data) {
					if (!aTokenizer.next()) {
						StringList theFinalTableNameList = query.getAllTableNameList();						
						if (verifySelectEntity(theFinalTableNameList)) {
							if (!query.getSelectAll() && !verifySelectAttr(theFinalTableNameList, query.getfieldsSelected())) {
								theResult = unknownAttribute;
							}
							else {
								TableMap theEntityMap;
								for (auto& entityName : theFinalTableNameList) {
									theEntityMap.emplace(entityName, db->getTable(entityName).value());
								}
								query.addTableMap(theEntityMap);
							}
						}
						else {
							theResult = unknownEntity;
						}
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

	StatusResult SelectTableStatement::run(std::ostream& aStream) const {
		//RowList rows;
		return db->showQuery(query, aStream, joins);
	}

	SelectTableStatement& SelectTableStatement::addJoin(Join& aJoin) {
		joins.push_back(aJoin);
		return *this;
	}

}