//
//  Database.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include "Database.hpp"
#include "Config.hpp"
#include "Application.hpp"

namespace ECE141 {

	Database::Database(const std::string aName, std::ostream& anOutput)
		: name(aName), changed(true), output(anOutput), Storage(stream), 
							initialized(false), entityForIndex(std::nullopt) {

		path = Config::getDBPath(name);
		stream.clear();
		stream.open(path.c_str(), OpenExisiting);
		std::stringstream theInfo;
		load(theInfo, 0);
		decode(theInfo);
		updateAvailableBlock();
		for (auto& entity : tables) {
			theInfo.str("");
			theInfo.clear();
			load(theInfo, entity.getBlockNum());
			decode(theInfo, entity);
		}
		// decode rows back to each entity
		theInfo.str("");
		theInfo.clear();
		load(theInfo, 1);
		decode(theInfo);
		initialized = true;
		//create for read/write
	}
	/*
	Database::Database(const std::string aName, OpenDB)
		: name(aName), changed(false) {

		path = Config::getDBPath(name);
		//open for read/write
	}
	*/
	Database::~Database() {
		if (changed) {
			//stuff to save?
		}
	}

	std::string Database::getName() {
		return name;
	}

	std::string Database::getPath() {
		return path;
	}

	TableList Database::getTables() {
		return tables;
	}

	tableOpt Database::getTable(std::string aName) {
		for (auto& entity : tables) {
			if (entity.getName() == aName) {
				return &entity;
			}
		}
		return std::nullopt;
	}

	void Database::updateShowIndexes(Entity& anEntity) {
		if (anEntity.getPrimaryKey().has_value()) {
			showIndexesMap[anEntity.getName()] = anEntity.getPrimaryKey().value().getName();
		}
		else {
			showIndexesMap[anEntity.getName()] = std::nullopt;
		}
	}

	void Database::updateIndexBlockMap(Entity& anEntity, Row& aRow, bool insertIntoMap) {
		auto& theIndex = showIndexesMap[anEntity.getName()];
		if (theIndex.has_value()) {
			auto& theIndexVal = std::get<std::string>(aRow.getData()[theIndex.value()]);
			auto& theBlockNum = aRow.blockNumber;
			insertIntoMap ? anEntity.insertIndexBlockMap(theIndexVal, theBlockNum):
						   anEntity.eraseIndexBlockMap(theIndexVal);							
		}
	}

	void updateAttr(std::string aName, std::string aVal, Attribute& anAttr, Entity& anEntity) {

		if ("name" == aName) {
			anAttr.setName(aVal);
		}
		else if ("type" == aName) {
			anAttr.setDataType(Helpers::stringToDataType(aVal));
		}
		else if ("size" == aName) {
			uint16_t theSize;
			std::stringstream theSS(aVal);
			theSS >> theSize;
			anAttr.setSize(theSize);
		}
		else if ("auto" == aName) {
			if ("1" == aVal) {
				anAttr.setAutoIncrement(true);
			}
			else {
				anAttr.setAutoIncrement(false);
			}

		}
		else if ("primary" == aName) {
			if ("1" == aVal) {
				anEntity.setPrimaryKey(anAttr);
				anAttr.setPrimaryKey(true);
			}
			else {
				anAttr.setPrimaryKey(false);
			}
		}
		else if ("null" == aName) {
			if ("1" == aVal) {
				anAttr.setNullable(true);
			}
			else {
				anAttr.setNullable(false);
			}
		}
		else if ("hasDefault" == aName) {
			if ("1" == aVal) {
				anAttr.setHasDefault(true);
			}
			else {
				anAttr.setHasDefault(false);
			}
		}
		else if ("defaultVal" == aName) {
			anAttr.setDefaultVal(aVal);
		}
	}

	void createRow(std::string aName, std::string aVal, Row& aRow, std::string aFieldName) {

		if ("entityName" == aName) {
			aRow.entityName = aVal;
		}
		else if ("blockNumber" == aName) {
			aRow.blockNumber = std::stoi(aVal);
		}		
		else if("value" == aName) {
			aRow.set(aFieldName, aVal);
		}
	}

	StatusResult Database::encode(std::ostream& anOutput) {
		for (auto& entity : tables) {
			anOutput << "@" + entity.getName() << " " << entity.getBlockNum() << " ";
		}
		return StatusResult{ noError };
	}

	StatusResult Database::decode(std::istream& anInput, Entity& anEntity) {
		char theIdentifier;
		std::string theItemName;
		std::string theItemVal;
		while (anInput >> theIdentifier) {
			if ('E' == theIdentifier) {
				Attribute theAttr;
				while (anInput >> theIdentifier) {
					if ('@' == theIdentifier) {
						anInput >> theItemName;
						anInput >> theItemVal;
						if ("name" == theItemName && DataTypes::no_type != theAttr.getType()) {
							anEntity.addAttribute(theAttr);
						}
						updateAttr(theItemName, theItemVal, theAttr, anEntity);
					}
				}
				// add the last attribute
				anEntity.addAttribute(theAttr);
				updateShowIndexes(anEntity);
			}
		}
		return StatusResult{ noError };
	}

	StatusResult Database::decode(std::istream& anInput) {
		char theIdentifier;		
		std::string theItemName;
		std::string theItemVal;
		while (anInput >> theIdentifier) {
			if ('M' == theIdentifier) {
				while (anInput >> theIdentifier) {
					if ('@' == theIdentifier) {
						anInput >> theItemName;
						anInput >> theItemVal;
						Entity theEntity(theItemName);
						std::stringstream theSS(theItemVal);
						int32_t theBlockNum;
						theSS >> theBlockNum;
						theEntity.setBlockNum(theBlockNum);
						tables.push_back(theEntity);
					}
				}
			}
			else if ('D' == theIdentifier) {	
				Row theRow;				
				std::string theFieldName = "";
				while (anInput >> theIdentifier) {
					if ('@' == theIdentifier) {
						anInput >> theItemName;
						anInput >> theItemVal;
						if ("fieldName" == theItemName) {
							theFieldName = theItemVal;
						}
						createRow(theItemName, theItemVal, theRow, theFieldName);
					}
				}
				if (initialized) {
					addSelectedRow(theRow);
				}
				else {
					getTable(theRow.entityName).value()->addRow(theRow); // initialize the rows in each entity in database
					updateIndexBlockMap(*getTable(theRow.entityName).value(), theRow, true); // initialize the indexBlockMap in the correspoding entity
					 
				}
			}
			else if ('I' == theIdentifier) {
				std::string theEntityName;
				std::string theName;
				uint32_t theBlockNum;
				while (anInput >> theIdentifier) {
					if ('@' == theIdentifier) {
						anInput >> theName;
						if ("AutoIncr" == theName) {
							anInput >> theBlockNum;
							getTable(theEntityName).value()->setAutoIncr(theBlockNum);
						}
						else {
							theEntityName = theName;
							while (anInput >> theBlockNum) {
								std::stringstream theInfo;
								load(theInfo, theBlockNum);
								decode(theInfo);
							}
							anInput.clear();
						}
					}
				}
			}

		}
		return StatusResult{ noError };
	}

	void createAttrStream(std::ostream& anOutput, Entity& anEntity) {
		auto& theAttrList = anEntity.getAttributes();
		for (auto& attr : theAttrList) {
			anOutput << "@name" << " " << attr.getName() << " "
				<< "@type" << " " << Helpers::dataTypeToString(attr.getType()) << " "
				<< "@size" << " " << attr.getSize() << " "
				<< "@auto" << " " << attr.isAutoIncrement() << " "
				<< "@primary" << " " << attr.isPrimaryKey() << " "
				<< "@null" << " " << attr.isNullable() << " "
				<< "@hasDefault" << " " << attr.hasDefault() << " "
				<< "@defaultVal" << " " << attr.getDefaultVal() << " ";
		}
	}

	void createRowStream(std::ostream& anOutput, Row aRow) {
		anOutput << "@entityName" << " " << aRow.entityName << " "
			<< "@blockNumber" << " " << aRow.blockNumber << " ";
		for (auto& dataMap : aRow.getData()) {
			std::string theVal = std::get<std::string>(dataMap.second);
			anOutput << "@fieldName" << " " << dataMap.first << " "
				<< "@value" << " " << theVal << " ";
		}
	}

	void createEntityRowStream(std::ostream& anOutput, TableList& aTableList) {
		for (auto& entity : aTableList) {
			// @entityname block number of rows
			anOutput << "@" + entity.getName() << " ";
			for (auto& row : entity.getRows()) {
				anOutput << row.blockNumber << " ";
			}
			anOutput << "@AutoIncr " << entity.getAutoIncr();
		}


	}

	StatusResult Database::removeTable(const std::string aName, size_t& aNumOfDropBlock) {
		int theCount{};
		for (auto& table : tables) {
			if (aName == table.getName()) {
				// remove the block of entity's rows
				for (auto& row : table.getRows()) {
					markBlockAsFree(row.blockNumber);
					++aNumOfDropBlock;
				}
				// remove the block of entity attrs
				markBlockAsFree(table.getBlockNum());
				tables.erase(tables.begin() + theCount);
				++aNumOfDropBlock;
				Block theBlock(BlockType::meta_block);
				std::stringstream theInfo;
				encode(theInfo);
				theInfo.str().copy(theBlock.payload, sizeof(theBlock.payload), 0);
				blockIO->writeBlock(0, theBlock);

				// update the index block
				theInfo.clear();
				theInfo.str("");
				createEntityRowStream(theInfo, tables);
				//theInfo << "@AutoIncr " << anEntity.getAutoIncr();
				Block theIndexBlock(BlockType::index_block);
				theInfo.str().copy(theIndexBlock.payload, sizeof(theIndexBlock.payload), 0);
				blockIO->writeBlock(1, theIndexBlock);

				return StatusResult{};
			}
			theCount++;
		}
		return StatusResult{ unknownTable };
	}

	StatusResult Database::createTable(Entity& anEntity) {
		Timer theTimer = Config::getTimer();
		updateShowIndexes(anEntity);
		stream.clear();
		stream.open(Config::getDBPath(name).c_str(), OpenExisiting);
		Block theBlock(BlockType::meta_block);
		int32_t theBlockNum = getFreeBlock();
		anEntity.setBlockNum(theBlockNum);
		tables.push_back(anEntity);
		std::stringstream theInfo;
		encode(theInfo);
		theInfo.str().copy(theBlock.payload, sizeof(theBlock.payload), 0);
		blockIO->writeBlock(0, theBlock);
		stream.clear();
		theInfo.str("");
		theInfo.clear();
		createAttrStream(theInfo, anEntity);
		Block theEntityBlock(BlockType::entity_block);
		theInfo.str().copy(theEntityBlock.payload, sizeof(theEntityBlock.payload), 0);
		blockIO->writeBlock(theBlockNum, theEntityBlock);
		stream.close();
		output << "Query OK, 1 rows affected (" << theTimer.elapsed() << " secs)\n";
		return StatusResult{ noError };
	}

	StatusResult Database::describeTable(const std::string& aName, std::ostream& anOutput) {
		Timer theTimer = Config::getTimer();
		for (auto iter = tables.begin(); iter != tables.end(); ++iter) {
			if (aName == iter->getName()) {
				TableDescriptionView(*iter).show(anOutput);
				return StatusResult{ noError };
			}

		}
		return StatusResult{ unknownTable };

	}

	StatusResult Database::dropTable(const std::string& aName) {
		Timer theTimer = Config::getTimer();
		showIndexesMap.erase(aName);
		stream.clear();
		stream.open(Config::getDBPath(name).c_str(), OpenExisiting);
		//auto& theRows = getTable(aName).value()->getRows();
		size_t theNumOfDropBlock = 0;
		StatusResult dropResult = removeTable(aName, theNumOfDropBlock);
		output << "Query OK, " << theNumOfDropBlock << " rows affected (" << theTimer.elapsed() << " secs)\n";
		stream.clear();
		stream.close();
		return dropResult;
	}

	StatusResult Database::dumpTable(const std::string& aName) {
		return StatusResult{ noError };
	}

	StatusResult Database::showTables(std::ostream& anOutput) {
		Timer theTimer = Config::getTimer();
		std::vector<std::string> theEntityNames;
		for (auto iter = tables.begin(); iter != tables.end(); ++iter) {
			theEntityNames.push_back(iter->getName());
		}
		EntityView(theEntityNames).show(anOutput);
		return StatusResult{};
	}

	StatusResult Database::showIndexes(std::ostream& anOutput) {
		Timer theTimer = Config::getTimer();		
		StringList theTitleList{ "table", "field(s)" };
		IndexView(showIndexesMap, theTitleList).show(anOutput);
		return StatusResult{};
	}

	StatusResult Database::showIndexFrom(std::ostream& anOutput) {
		Timer theTimer = Config::getTimer();
		auto& theIndexBlockMap = getTable(entityForIndex.value()).value()->getIndexBlockMap();
		StringList theTitleList{ "key", "block#" };
		IndexView(theIndexBlockMap, theTitleList).show(anOutput);
		return StatusResult{};
	}


	// USE: Call this to dump the db for debug purposes...
	StatusResult Database::dump(std::ostream& anOutput) {
		return StatusResult{ noError };
	}

	StatusResult Database::insertTable(Entity& anEntity) {
		Timer theTimer = Config::getTimer();
		auto theRowStartPos = anEntity.getNextRowPos();
		stream.clear();
		stream.open(Config::getDBPath(name).c_str(), OpenExisiting);

		auto &theRowList = anEntity.getRows();
		for (size_t i = theRowStartPos; i < theRowList.size(); i++) {
			auto& theRow = theRowList[i];						
			
			if (0 == theRow.blockNumber) {
				Block theBlock(BlockType::data_block);
				int32_t theBlockNum = getFreeBlock();
				theRow.blockNumber = theBlockNum;
				std::stringstream theInfo;
				createRowStream(theInfo, theRow);
				theInfo.str().copy(theBlock.payload, sizeof(theBlock.payload), 0);
				blockIO->writeBlock(theBlockNum, theBlock);
				// update indexBlockMap
				updateIndexBlockMap(anEntity, theRow, true);
			}
		}

		// update the rows in the entity
		std::stringstream theInfo;
		createEntityRowStream(theInfo, tables);
		//theInfo << "@AutoIncr " << anEntity.getAutoIncr();
		Block theBlock(BlockType::index_block);
		theInfo.str().copy(theBlock.payload, sizeof(theBlock.payload), 0);
		blockIO->writeBlock(1, theBlock);
		
		stream.clear();
		stream.close();
		output << "Query OK, " << theRowList.size() - theRowStartPos << " rows affected(" << theTimer.elapsed() << " secs)\n";
		return StatusResult{ noError };
	}

	StatusResult Database::findSelectedEntityBlock(const Query &aQuery, std::stringstream &anInput, std::string& aString) {
		// find selected entities and their blocks
		char theIdentifier;
		std::string theItemName;
		std::string theItemVal;
		StringList theTableNameList = aQuery.getAllTableNameList();
		if (anInput >> theIdentifier) {
			if ('I' == theIdentifier) {
				aString += theIdentifier;

				std::string theEntityName;
				std::string theName;
				uint32_t theBlockNum;
				bool foundEntity = false;
				while (anInput >> theIdentifier) {
					if ('@' == theIdentifier) {								
						anInput >> theName;						
						if ("AutoIncr" == theName) {
							anInput >> theBlockNum;
							if (foundEntity) {
								aString += theIdentifier + theName + " " + std::to_string(theBlockNum) + " ";
								foundEntity = false;
							}
						}
						else {
							theEntityName = theName;
							if (0 != std::count(theTableNameList.begin(), theTableNameList.end(), theEntityName)) {
								foundEntity = true;
								aString += theIdentifier + theEntityName + " ";
							}
							while (anInput >> theBlockNum) {
								if (foundEntity) {
									aString += std::to_string(theBlockNum) + " ";
								}
							}
							anInput.clear();
						}
					}
				}
			}
			else {
				return StatusResult{ unknownEntity };
			}
		}
		return StatusResult{ noError };
	}

	StatusResult Database::selectRows(const Query& aQuery) {
		StatusResult theResult;
		Timer theTimer = Config::getTimer();
		stream.clear();
		stream.open(Config::getDBPath(name).c_str(), OpenExisiting);
		stream.clear();
		bool canUseIndex{ true };
		for (auto& pair : aQuery.getTableMap()) {
			auto theEntity = pair.second;
			if (!theEntity->getPrimaryKey().has_value()) {
				canUseIndex = false;
				break;
			}
		}
		std::stringstream theInfo;
		std::string theStringToDecode = "";
		if (!canUseIndex) {			
			load(theInfo, 1);
			if (theResult = findSelectedEntityBlock(aQuery, theInfo, theStringToDecode)) {
				theInfo.str("");
				theInfo.clear();
				theInfo.str(theStringToDecode);
				decode(theInfo);
				//aCollection = rowCollection;
			}
		}
		else {
			auto theTableMap = aQuery.getTableMap();
			for (auto& pair : theTableMap) {
				auto theEntity = pair.second;
				auto theIndexBlockMap = theEntity->getIndexBlockMap();
				for (auto& map : theIndexBlockMap) {
					auto& theBlockNum = map.second.value();
					theInfo.str("");
					theInfo.clear();
					load(theInfo, theBlockNum);					
					decode(theInfo);
				}
				
			}
		}
		
		stream.close();
		return theResult;
	}

	StatusResult Database::showQuery(const Query& aQuery, std::ostream& anOutput, const JoinList& aJoinList) {
		StatusResult theResult;
		
		//<entityName:row>
		std::unordered_map<Entity*, RowList> theEntityRowMap;
		StringList theFieldNameList;
		if (theResult = selectRows(aQuery)) {
			if (aQuery.getSelectAll()) {
				for (auto& entityName : aQuery.getAllTableNameList()) {
					StringList theAttrNameList;
					for (auto &attr: getTable(entityName).value()->getAttributes()) {
						theFieldNameList.push_back(attr.getName());	
						theAttrNameList.push_back(attr.getName());
					}
					RowList theRows;
					for (Row row : rowCollection) {
						if (entityName == row.entityName) {
							theRows.push_back(row);							
						}
					}
					theEntityRowMap.emplace(getTable(entityName).value(), theRows);
				}
			}
			else {
				for (auto& attrName : aQuery.getfieldsSelected()) {
					theFieldNameList.push_back(attrName);
				}				
				for (auto& entityName : aQuery.getAllTableNameList()) {					
					RowList theRows;
					for (Row row : rowCollection) {
						if (entityName == row.entityName) {
							theRows.push_back(row);							
						}
					}
					theEntityRowMap[getTable(entityName).value()] = theRows;

					StringList theAttrNameList;
					for (auto& attrName : aQuery.getfieldsSelected()) {						
						for (auto& attr : getTable(entityName).value()->getAttributes()) {
							if (attrName == attr.getName()) {
								theAttrNameList.push_back(attrName);
								break;
							}
						}
					}
				}
				
			}

			std::unordered_map<Entity*, RowList> theNewEntityRowMap; // new EntityRowMap after filtering						
			Filters* theFilter = aQuery.getFilters();
			if (theFilter->getCount()) {
				// handle clause "where"
				for (auto& rowMap : theEntityRowMap) {
					RowList theFilteredRows;
					for (auto& row : rowMap.second) {
						if (theFilter->matches(row.getData())) {
							theFilteredRows.push_back(row);
						}
					}
					theNewEntityRowMap[rowMap.first] = theFilteredRows;
				}
			}
			else {
				theNewEntityRowMap = theEntityRowMap;
			}

			QueryView(theNewEntityRowMap, theFieldNameList, aQuery, aJoinList).show(anOutput); // show selected data

		}		
		
		rowCollection.clear(); // clear rowCollection in database, otherwise it will keep storing the selected row until the application ends.
		return theResult;

	}

	StatusResult Database::updateRowsHelper(Row& aRow) {
		// write it back to the block
		stream.clear();
		stream.open(Config::getDBPath(name).c_str(), OpenExisiting);
		stream.clear();
		Block theBlock(BlockType::data_block);
		int32_t theBlockNum = aRow.blockNumber;
		aRow.blockNumber = theBlockNum;
		std::stringstream theInfo;
		createRowStream(theInfo, aRow);
		theInfo.str().copy(theBlock.payload, sizeof(theBlock.payload), 0);
		blockIO->writeBlock(theBlockNum, theBlock);
		stream.close();
		return StatusResult{ noError };
	}

	StatusResult Database::updateRows(const Query& aQuery) {
		Timer theTimer = Config::getTimer();
		uint32_t	 theAffectedRow = 0;
		Filters* theFilter = aQuery.getFilters();
		uint32_t theLimit = aQuery.getTableMap().begin()->second->getRows().size();
		// handle limit			
		if (aQuery.getHasLimit()) {
			if (aQuery.getOffset() != 0) { return StatusResult{ invalidCommand }; }
			theLimit = aQuery.getLimit();
		}
		if (theFilter->getCount()) {			
			// handle clause "where"
			for (auto& theEntityMap : aQuery.getTableMap()) {
				for (auto& row : theEntityMap.second->getRows()) {
					if (theAffectedRow >= theLimit) { break; }
					if (theFilter->matches(row.getData())) {
						++theAffectedRow;
						for (auto& attrValMap : aQuery.getAttrValMap()) {
							row.getData()[attrValMap.first] = attrValMap.second;
						}
						// write it back to the block
						updateRowsHelper(row);
					}
				}
			}	
		}		
		else {
			for (auto& theEntityMap : aQuery.getTableMap()) {
				if (theAffectedRow >= theLimit) { break; }
				for (auto& row : theEntityMap.second->getRows()) {
					++theAffectedRow;
					// write it back to the block
					updateRowsHelper(row);
				}
			}
		}
		output << "Query OK, " << theAffectedRow << " rows affected(" << theTimer.elapsed() << " secs)\n";
		return StatusResult{ noError };
	}	

	StatusResult Database::deleteRows(const Query& aQuery) {
		Timer theTimer = Config::getTimer();
		uint32_t			theAffectedRow = 0;
		std::vector<Row>	theNewRows;
		Filters* theFilter = aQuery.getFilters();
		stream.clear();
		stream.open(Config::getDBPath(name).c_str(), OpenExisiting);
		stream.clear();
		if (theFilter->getCount()) {
			// handle clause "where"
			for (auto& theEntityMap : aQuery.getTableMap()) {
				for (auto& row : theEntityMap.second->getRows()) {
					if (theFilter->matches(row.getData())) {
						updateIndexBlockMap(*theEntityMap.second, row, false);										
						++theAffectedRow;						
						markBlockAsFree(row.blockNumber); // remove the row which is satisfy the condition							
					}
					else {
						theNewRows.push_back(row);
					}
				}
			}			
		}
		else {
			for (auto& theEntityMap : aQuery.getTableMap()) {
				for (auto& row : theEntityMap.second->getRows()) {
					updateIndexBlockMap(*theEntityMap.second, row, false);
					++theAffectedRow;
					markBlockAsFree(row.blockNumber);
				}
			}
		}

		// delete the rows in the entity
		for (auto& theEntityMap : aQuery.getTableMap()) {
			theEntityMap.second->setNextRowPos(theNewRows.size()); // update the nextRowPos in entity
			theEntityMap.second->getRows().clear();
			for (auto& row : theNewRows) {
				theEntityMap.second->addRow(row);
			}
		}						

		// update the index block
		std::stringstream theInfo;
		createEntityRowStream(theInfo, tables);
		//theInfo << "@AutoIncr " << anEntity.getAutoIncr();
		Block theIndexBlock(BlockType::index_block);
		theInfo.str().copy(theIndexBlock.payload, sizeof(theIndexBlock.payload), 0);
		blockIO->writeBlock(1, theIndexBlock);

		output << "Query OK, " << theAffectedRow << " rows affected(" << theTimer.elapsed() << " secs)\n";
		stream.close();
		return StatusResult{ noError };
	}

	StatusResult Database::alterAdd(std::string aTableName) {
		Timer theTimer = Config::getTimer();
		stream.clear();
		stream.open(Config::getDBPath(name).c_str(), OpenExisiting);				
		// write back to block 
		int32_t theBlockNum = getTable(aTableName).value()->getBlockNum();
		std::stringstream theInfo;
		createAttrStream(theInfo, *getTable(aTableName).value()); // write back to entity block(Attributes)
		Block theEntityBlock(BlockType::entity_block);
		theInfo.str().copy(theEntityBlock.payload, sizeof(theEntityBlock.payload), 0);
		blockIO->writeBlock(theBlockNum, theEntityBlock);

		// update rows in entity
		auto& theAttr = getTable(aTableName).value()->getAttributes().back();
		std::string theAttrName = theAttr.getName();
		std::string theAttrVal = theAttr.getDefaultVal();
		for (auto& row : getTable(aTableName).value()->getRows()) {
			row.set(theAttrName, theAttrVal);
			updateRowsHelper(row); // write back to data block
		}

		stream.close();
		output << "Query OK, " << getTable(aTableName).value()->getRows().size() << " rows affected (" << theTimer.elapsed() << " secs)\n";
		return StatusResult{ noError };
	}

	StatusResult Database::alterDrop(std::string aTableName) {
		Timer theTimer = Config::getTimer();
		stream.clear();
		stream.open(Config::getDBPath(name).c_str(), OpenExisiting);
		// write back to block 
		int32_t theBlockNum = getTable(aTableName).value()->getBlockNum();
		std::stringstream theInfo;
		createAttrStream(theInfo, *getTable(aTableName).value()); // write back to entity block(Attributes)
		Block theEntityBlock(BlockType::entity_block);
		theInfo.str().copy(theEntityBlock.payload, sizeof(theEntityBlock.payload), 0);
		blockIO->writeBlock(theBlockNum, theEntityBlock);

		// update rows in entity
		auto& theAttr = getTable(aTableName).value()->getAttributes().back();
		std::string theAttrName = theAttr.getName();
		std::string theAttrVal = theAttr.getDefaultVal();
		for (auto& row : getTable(aTableName).value()->getRows()) {
			updateRowsHelper(row); // write back to data block
		}

		stream.close();
		output << "Query OK, " << getTable(aTableName).value()->getRows().size() << " rows affected (" << theTimer.elapsed() << " secs)\n";
		return StatusResult{ noError };	
	}

}
