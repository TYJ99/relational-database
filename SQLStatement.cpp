
#include "Tokenizer.hpp"
#include "SQLStatement.hpp"
#include <vector>
#include <map>
#include <algorithm>

namespace ECE141 {

	/*
	INSERT INTO users 
	('first_name', 'last_name', 'email') 
	VALUES 
	('David','He', 'dyhe@ucsd.edu'),
	('Sai', 'Komatineni', 'saikomatineni@engucsd.edu');
	*/
	
	//using ClauseParser = StatusResult(*)(Tokenizer&, Query&, Database*);

	bool SQLStatement::checkTableName(std::string& aName) {
		auto theTableList = db->getTables();
		for (auto& entity : theTableList) {
			if (aName == entity.getName()) {
				return true;
			}
		}
		return false;
	}	

	StatusResult SQLStatement::parseHeader(KWList& aKWList, Tokenizer& aTokenizer) {
		TestSequencer theSequencer(aTokenizer);
		StatusResult theResult;
		if (theSequencer.nextIs(aKWList)) {
			Token& theCurrToken = aTokenizer.current();
			if (TokenType::identifier == theCurrToken.type) {
				tableName = theCurrToken.data;
				if (Keywords::create_kw == *aKWList.begin() && checkTableName(tableName)) {
					theResult.error = tableExists;
				}
				else if (Keywords::insert_kw == *aKWList.begin() && !checkTableName(tableName)) {
					theResult.error = unknownTable;
				}

			}
			else if (TokenType::keyword == theCurrToken.type) {			// when * 
				if (Keywords::from_kw == theCurrToken.keyword) {
					aTokenizer.next();
					Token& theCurrToken = aTokenizer.current();
					tableName = theCurrToken.data;
					if (!checkTableName(tableName)) {
						theResult.error = unknownTable;
					}
				}
			}
			else {
				theResult.error = invalidCommand;
			}
		}
		else {
			theResult.error = invalidCommand;
		}
		aTokenizer.next();
		return theResult;
	}
	

	// create
	StatusResult CreateTableStatement::recognize(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::create_kw, Keywords::table_kw };
		return Helpers::helpRecog(aTokenizer, theKWList);
	}

	StatusResult CreateTableStatement::parse(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::create_kw, Keywords::table_kw };		
		StatusResult theResult;
		theResult = parseHeader(theKWList, aTokenizer);
		entity = std::make_unique<Entity>(tableName);
		ParseCreateTableHelper parseHelper(aTokenizer);
		if (theResult) {
			if (aTokenizer.skipIf('(')) {
				bool hasComma = true;
				while (theResult && hasComma) {
					if (theResult = parseHelper.parseAttribute(*entity.get())) {
						hasComma = aTokenizer.skipIf(',');
					}
				}
				if (theResult) {
					if (!aTokenizer.skipIf(')')) {
						theResult = invalidCommand;
					}
					else {
						if (!aTokenizer.skipIf(';')) {
							theResult = invalidCommand;
						}
					}
				}
			}
		}			
		return theResult;
	}	

	StatusResult  CreateTableStatement::run(std::ostream& aStream) const {
		return db->createTable(*entity.get());
	}

	// drop
	StatusResult DropTableStatement::recognize(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::drop_kw, Keywords::table_kw };
		return Helpers::helpRecog(aTokenizer, theKWList);
	}

	StatusResult DropTableStatement::parse(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKWList = { Keywords::drop_kw, Keywords::table_kw };
		return Helpers::helpParse(tableName, aTokenizer, theKWList, true);
	}
	StatusResult  DropTableStatement::run(std::ostream& aStream) const {
		return db->dropTable(tableName);
	}

	// show tables
	StatusResult ShowTableStatement::recognize(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::show_kw, Keywords::tables_kw };
		return Helpers::helpRecog(aTokenizer, theKWList);
	}

	StatusResult ShowTableStatement::parse(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKWList{ Keywords::show_kw, Keywords::tables_kw };
		std::string theName = "noName";
		return Helpers::helpParse(theName, aTokenizer, theKWList, false);
	}
	StatusResult  ShowTableStatement::run(std::ostream& aStream) const {
		return db->showTables(aStream);
	}

	// describe
	StatusResult DescribeTableStatement::recognize(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::describe_kw };
		return Helpers::helpRecog(aTokenizer, theKWList);
	}

	StatusResult DescribeTableStatement::parse(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::describe_kw };
		return Helpers::helpParse(tableName, aTokenizer, theKwList, true);		
	}
	StatusResult  DescribeTableStatement::run(std::ostream& aStream) const {
		return db->describeTable(tableName, aStream);
	}

/*==================================================================================================
										Insert
 =================================================================================================*/
	StatusResult InsertTableStatement::recognize(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::insert_kw, Keywords::into_kw };
		return Helpers::helpRecog(aTokenizer, theKWList);
	}
	
	StatusResult InsertTableStatement::parse(Tokenizer& aTokenizer) {
		StatusResult theResult;
		std::vector<Attribute> theAttrList;
		KWList theKWList{ Keywords::insert_kw, Keywords::into_kw };
		theResult = parseHeader(theKWList, aTokenizer);
		ParseInsertHelper theInsertHelper(aTokenizer, db, tableName);

		if (theResult && aTokenizer.skipIf('(')) {
			bool hasComma = true;
			while (theResult && hasComma) {
				if (theResult = theInsertHelper.parseAttrName(theAttrList)) {
					hasComma = aTokenizer.skipIf(',');
				}
			}

			if (!aTokenizer.skipIf(')')) {
				theResult = invalidCommand;
			}
			
		}
		KWList theKWList2{ Keywords::values_kw };
		TestSequencer theSequencer(aTokenizer);
		if (!theSequencer.nextIs(theKWList2)) {
			theResult = invalidCommand;
		}
		bool hasComma = true;
		while (theResult && hasComma) {
			std::vector<std::string> theValues;
			if (theResult && aTokenizer.skipIf('(')) {
				while (theResult && ")" != aTokenizer.current().data) {
					theResult = theInsertHelper.parseValues(theValues);
				}

				if (!aTokenizer.skipIf(')')) {
					theResult = invalidCommand;
				}
				if (!aTokenizer.skipIf(',')) {
					hasComma = false;
				}

			}
			else {
				theResult = invalidCommand;
			}

			// start verifying
			if (theResult = theInsertHelper.verifyAttrsVals(theValues, theAttrList)) {
				Row theRow(tableName);				
				bool assignedAutoIncr = false;				
				if (0 == theAttrList.size()) {	
					if (db->getTable(tableName).value()->getAutoIncrAttr()) { assignedAutoIncr = true; }
					for (size_t i = 0; i < theValues.size(); ++i) {
						auto &theAttr = db->getTable(tableName).value()->getAttributes()[i];
						if (theResult = theInsertHelper.validate(theAttr, theValues[i])) {
							theRow.set(theAttr.getName(), theValues[i]);
						}
						else {
							break;
						}
					}					
				}
				else {						
					// verify that the attribute in theAttrList exist in the entity						
					for (auto& attr : db->getTable(tableName).value()->getAttributes()) {
						bool doesExist = false;
						size_t thePos = 0;
						for (size_t i = 0; i < theAttrList.size(); i++) {
							if (attr.getName() == theAttrList[i].getName()) {
								doesExist = true;
								thePos = i;
								break;
							}
						}
						if (doesExist) {
							if (attr.isAutoIncrement()) {
								assignedAutoIncr = true;
							}
							if (theResult = theInsertHelper.validate(theAttrList[thePos], theValues[thePos])) {
								// If the attribute is primary key, its value cannot duplicate in the entity.
								if (theAttrList[thePos].isPrimaryKey()) {
									for (auto& row : db->getTable(tableName).value()->getRows()) {
										for (auto& data : row.getData()) {
											if (data.first == theAttrList[thePos].getName()) {
												if (std::get<std::string>(data.second) == theValues[thePos]) {
													theResult = primaryKeyValueExists;
													return theResult;
												}
											}
										}
									}
								}
								if (theResult) {
									theRow.set(theAttrList[thePos].getName(), theValues[thePos]);
								}
							}
							else {
								return theResult;
							}
						}		
						else {
							if (attr.isAutoIncrement()) {
								theRow.set(attr.getName(), std::to_string(db->getTable(tableName).value()->getAutoIncr()));
							}
							else { theRow.set(attr.getName(), attr.getDefaultVal()); }
								
						}
					}
					// update the row which will be inserted into the entity.
						
					
				}								
				// update autoIncrement
				if (assignedAutoIncr) {
					std::string theName = db->getTable(tableName).value()->getAutoIncrAttr().value().getName();
					std::string theVal = std::get<std::string>(theRow.getData()[theName]);
					int theInt = std::stoi(theVal);
					theInt = std::max(theInt, db->getTable(tableName).value()->getAutoIncr());
					db->getTable(tableName).value()->updateAutoIncr(theInt);
				}
				else {
					if (db->getTable(tableName).value()->getAutoIncrAttr()) {
						db->getTable(tableName).value()->updateAutoIncr();
					}
				}
				db->getTable(tableName).value()->addRow(theRow);				
			}
		}

		if (theResult) {
			if (!aTokenizer.more()) {
				theResult = invalidCommand;
			}
			else {
				if (";" != aTokenizer.current().data) {
					theResult = invalidCommand;
				}
			}
		}
		
		return theResult;
	}

	StatusResult InsertTableStatement::run(std::ostream& aStream) const {
		return db->insertTable(*(db->getTable(tableName).value()));
	}

	// ShowIndexStatement
	StatusResult ShowIndexStatement::recognize(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::show_kw, Keywords::indexes_kw };
		return Helpers::helpRecog(aTokenizer, theKWList);
	}

	StatusResult ShowIndexStatement::parse(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::show_kw, Keywords::indexes_kw };
		std::string theName = "noName";
		return Helpers::helpParse(theName, aTokenizer, theKWList, false);
	}

	StatusResult ShowIndexStatement::run(std::ostream& aStream) const {
		//RowList rows;
		return db->showIndexes(aStream);
	}

	// ShowIndexStatement
	StatusResult ShowIndexFromStatement::recognize(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::show_kw, Keywords::index_kw };
		return Helpers::helpRecog(aTokenizer, theKWList);
	}

	StatusResult ShowIndexFromStatement::parse(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::show_kw, Keywords::index_kw };
		TestSequencer theSequencer(aTokenizer);
		StatusResult theResult;
		if (theSequencer.nextIs(theKWList)) {			
			StringList theAttrNameList;
			ParseHelper theParseHelper(aTokenizer);
			theParseHelper.parseIdentifierList(theAttrNameList);
			if (!aTokenizer.more() || 1 != theAttrNameList.size()) {
				theResult = invalidCommand;
			}

			if (theResult) {
				if (!aTokenizer.skipIf(Keywords::from_kw)) {
					theResult = invalidCommand;
					return theResult;
				}
				StringList theEntityNameList;
				ParseHelper theParseHelper(aTokenizer);
				theParseHelper.parseIdentifierList(theEntityNameList);	
				
				// verify if the field(s) we name in the field list {field1,...} must match 
				// the fields stored in the key of the index.
				auto& theAttrName = theAttrNameList[0];
				auto& theEntityName = theEntityNameList[0];
				auto& theShowIndexesMap = db->getShowIndexesMap();
				if (theShowIndexesMap.count(theEntityName)) {
					if (theShowIndexesMap[theEntityName].has_value()) {
						if (theShowIndexesMap[theEntityName].value() != theAttrName) {
							theResult = unknownIndex;
						}
					}
					else {
						theResult = unknownIndex;
					}
				}
				else {
					theResult = unknownEntity;
				}
				// set the entity for showIndexFrom
				if (theResult) {
					db->setEntityForIndex(theEntityName);
				}
			}

		}		
		return theResult;
	}

	StatusResult ShowIndexFromStatement::run(std::ostream& aStream) const {
		//RowList rows;
		return db->showIndexFrom(aStream);
	}

	// AlterAddStatement
	StatusResult AlterAddStatement::recognize(Tokenizer& aTokenizer) {
		StatusResult theResult;
		KWList theKWList{ Keywords::alter_kw, Keywords::table_kw };
		theResult = Helpers::helpRecog(aTokenizer, theKWList);
		if (theResult) {
			if (!(Keywords::add_kw == aTokenizer.peek(3).keyword)) {
				theResult = invalidCommand;
			}
		}
		return theResult;
	}

	StatusResult AlterAddStatement::parse(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::alter_kw, Keywords::table_kw };
		StatusResult theResult;
		theResult = parseHeader(theKWList, aTokenizer); // parse "alter table {tableName}"
		aTokenizer.next(); // skip keyword add_kw		
		// verify that the table we want to alter exists
		if (!db->getTable(tableName)) {
			return StatusResult{ unknownTable };
		}
		Attribute theAttr;
		if (TokenType::identifier == aTokenizer.current().type) {
			if (Helpers::getKeywordId(aTokenizer.current().data) == Keywords::unknown_kw) {
				theAttr.setName(aTokenizer.current().data);
				auto hasNext = aTokenizer.next();
				if (hasNext && Helpers::isDatatype(aTokenizer.current().keyword)) {
					auto theDatatype = Helpers::keywordToDatatype(aTokenizer.current().keyword);
					if (DataTypes::timestamp_type == theDatatype) {
						theAttr.setDefaultVal(Config::getTimer().getCurrentTime());
						theAttr.setHasDefault(true);
					}
					theAttr.setDataType(theDatatype);
					hasNext = aTokenizer.next();

					// handle the number in varchar(50) and char(50)
					if (hasNext && (DataTypes::varchar_type == theDatatype || DataTypes::char_type == theDatatype)) {
						if (aTokenizer.skipIf('(')) {
							theAttr.setSize(std::stoi(aTokenizer.current().data));
						}
						hasNext = aTokenizer.next();
						if (hasNext && aTokenizer.skipIf(')')) {
							theResult = noError;
						}
					}
					
				}
			}
		}
		else {
			theResult = invalidCommand;
		}	
		if (theResult) {
			db->getTable(tableName).value()->addAttribute(theAttr);
		}
		return theResult;
	}

	StatusResult AlterAddStatement::run(std::ostream& aStream) const {		
		return db->alterAdd(tableName);
	}

	// AlterDropStatement
	StatusResult AlterDropStatement::recognize(Tokenizer& aTokenizer) {
		StatusResult theResult;
		KWList theKWList{ Keywords::alter_kw, Keywords::table_kw };
		theResult = Helpers::helpRecog(aTokenizer, theKWList);
		if (theResult) {
			if (!(Keywords::drop_kw == aTokenizer.peek(3).keyword)) {
				theResult = invalidCommand;
			}
		}
		return theResult;
	}

	StatusResult AlterDropStatement::parse(Tokenizer& aTokenizer) {
		KWList theKWList{ Keywords::alter_kw, Keywords::table_kw };
		StatusResult theResult;
		theResult = parseHeader(theKWList, aTokenizer); // parse "alter table {tableName}"
		aTokenizer.next(); // skip keyword drop_kw
		// verify that the table we want to alter exists
		if (!db->getTable(tableName)) {
			return StatusResult{ unknownTable };
		}
		std::string theAttrName = "";
		if (TokenType::identifier == aTokenizer.current().type) {
			theAttrName = aTokenizer.current().data;
			aTokenizer.next();
		}
		else {
			theResult = invalidCommand;
		}

		// verify that the attribute exists in the table we want to alter
		if (aTokenizer.more() && theResult && db->getTable(tableName).value()->getAttribute(theAttrName).has_value()) {
			if (";" != aTokenizer.current().data) {
				theResult = invalidCommand;
			}
		}
		if (theResult) {
			Entity& theEntity = *db->getTable(tableName).value();
			auto& theAttrs = theEntity.getAttributes();
			for (size_t i = 0; i < theAttrs.size(); ++i) {
				if (theAttrs[i].getName() == theAttrName) {
					theAttrs.erase(theAttrs.begin() + i);
					break;
				}
			}

			for (auto& row : db->getTable(tableName).value()->getRows()) {
				row.getData().erase(theAttrName);
			}

		}
		return theResult;
	}

	StatusResult AlterDropStatement::run(std::ostream& aStream) const {
		//RowList rows;
		return db->alterDrop(tableName);
	}

}