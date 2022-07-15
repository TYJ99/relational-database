
#ifndef QueryView_hpp
#define QueryView_hpp

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include "View.hpp"
#include "Entity.hpp"
#include "Helpers.hpp"
#include "Attribute.hpp"
#include "BasicTypes.hpp"
#include "Query.hpp"
#include "Joins.hpp"
#include <functional>
#include <algorithm>


namespace ECE141 {

	using RowDataCollectionMap = std::unordered_map<uint32_t, std::vector<StringList>>;
	using AttrNamePosMap = std::unordered_map<std::string, uint32_t>;

	class QueryView : public View {
	public:

		QueryView(std::unordered_map<Entity*, RowList> &aEntityRowMap, 
				  std::vector<std::string> &aFieldNameList, const Query& aQuery, const JoinList& aJoinList) :
			entityRowMap(aEntityRowMap), fieldNameList(aFieldNameList), 
			numOfSlectedRow(0), query(aQuery), joinList(aJoinList) {}

		// USE: create header for Entity View...
		QueryView& showHeader(std::ostream& anOutput) {			
			borderView(widthList, anOutput);
			rowView(fieldNameList, widthList, anOutput);
			borderView(widthList, anOutput);
			return *this;
		}

		// USE: create body for Entity View...
		QueryView& showBody(uint32_t aSortedPos, DataTypes aSortedDType, 
			std::vector<StringList>& aFinalRowCollection, AttrNamePosMap& anAttrNamePosMap, 
			std::ostream& anOutput) {
			
			// handle order by
			if (!query.getOrderBy().empty()) {
				if (DataTypes::int_type == aSortedDType) {
					auto sortByAttr = [&](StringList& aRow1, StringList& aRow2) {
						return std::stoi(aRow1[aSortedPos]) < std::stoi(aRow2[aSortedPos]);
					};
					std::sort(aFinalRowCollection.begin(), aFinalRowCollection.end(), sortByAttr);
				}
				else if (DataTypes::float_type == aSortedDType) {
					auto sortByAttr = [&](StringList& aRow1, StringList& aRow2) {
						return std::stof(aRow1[aSortedPos]) < std::stof(aRow2[aSortedPos]);
					};
					std::sort(aFinalRowCollection.begin(), aFinalRowCollection.end(), sortByAttr);
				}
				else {
					auto sortByAttr = [&](StringList& aRow1, StringList& aRow2) {
						return aRow1[aSortedPos] < aRow2[aSortedPos];
					};
					std::sort(aFinalRowCollection.begin(), aFinalRowCollection.end(), sortByAttr);
				}
			}

			// According to the selected attrs to collect the final columns.
			std::vector<StringList> theFinalColumns;			
			if (!query.getSelectAll()) {
				for (auto& values : aFinalRowCollection) {
					StringList theRow;
					for (auto& fieldName : fieldNameList) {
						auto thePos = anAttrNamePosMap[fieldEntityMap[fieldName] + fieldName];
						theRow.push_back(values[thePos]);
					}
					theFinalColumns.push_back(theRow);
				}
			}
			else {
				theFinalColumns = aFinalRowCollection;
			}
			
			size_t theLimit = theFinalColumns.size();
			uint32_t theOffset = 0;
			uint32_t theCount = 0;
			if (query.getHasLimit()) {
				theLimit = query.getLimit();
				theOffset = query.getOffset();
			}
			
			for (auto& finalRow : theFinalColumns) {
				if (theCount < theLimit && theCount >= theOffset) {
					rowView(finalRow, widthList, anOutput);
				}
				++theCount;
			}
			borderView(widthList, anOutput);
			return *this;
		}

		// USE: create footer for Entity View...
		QueryView& showFooter(double anElapsedTime, std::ostream& anOutput) {
			uint32_t theDisplayRow = numOfSlectedRow;
			if (query.getHasLimit()) {
				theDisplayRow = std::min(query.getLimit(), numOfSlectedRow) - query.getOffset();
			}
			anOutput << theDisplayRow << " rows in set(" << anElapsedTime << " sec)\n"; // the footer
			return *this;
		}

		void DFSToShowRow(RowDataCollectionMap& aRowDataCollectionMap, StringList& aFinalRow,
			std::vector<StringList>& aFinalRowCollection, uint32_t aCount, std::vector<size_t>& anInitWidthList,
			const JoinList& aJoinList, AttrNamePosMap anAttrNamePosMap, std::ostream& anOutput) {

			if (aCount == aRowDataCollectionMap.size()) {
				if (!aJoinList.empty()) {
					for (auto& expr: joinList[0].getExpressions()) {
						if (aFinalRow[anAttrNamePosMap[expr.lhs.entityName + expr.lhs.name]]
							== aFinalRow[anAttrNamePosMap[expr.rhs.entityName + expr.rhs.name]]) {
							++numOfSlectedRow;
							for (size_t i = 0; i < aFinalRow.size(); ++i) {
								anInitWidthList[i] = std::max(aFinalRow[i].length(), anInitWidthList[i]);
							}
							aFinalRowCollection.push_back(aFinalRow);
						}
						
					}
				}
				else {
					++numOfSlectedRow;
					for (size_t i = 0; i < aFinalRow.size(); ++i) {
						anInitWidthList[i] = std::max(aFinalRow[i].length(), anInitWidthList[i]);
					}
					aFinalRowCollection.push_back(aFinalRow);
				}				
				//rowView(aFinalRow, widthList, anOutput);
				return;
			}

			for (StringList values : aRowDataCollectionMap[aCount]) {
				StringList theFinalRow = aFinalRow;
				theFinalRow.insert(theFinalRow.end(), values.begin(), values.end());
				DFSToShowRow(aRowDataCollectionMap, theFinalRow, aFinalRowCollection, aCount + 1, 
					anInitWidthList,aJoinList, anAttrNamePosMap, anOutput);
			}
		}

		//using getAttr = std::function<std::string(Attribute&)>;

		// USE: this is the main show() for the view, where it presents all the rows...
		bool show(std::ostream& anOutput) {

			Timer theTimer = Config::getTimer();			
			// theRowDataCollectionMap: <index:stringList of each value for each row in each entity>
			// 0:{('David', 'He'), ('Andy', 'Tseng')...}
			// 1:{('David', 'He'), ('Andy', 'Tseng')...}
			// Each StringList in vector<StringList> in RowDataCollectionMap is a list of
			// values that we want to show in a single row.
			// Combine each entity's StringList to get a final row which we want to show in a single row.
			RowDataCollectionMap theRowDataCollectionMap;
			AttrNamePosMap		 theAttrNamePosMap;	
			std::vector<size_t>	 theInitWidthList;
			uint32_t			 theSortedPos = 0;
			uint32_t			 theCount = 0;
			uint32_t			 theAttrCount = 0;
			DataTypes			 theSortedDType = DataTypes::varchar_type;
			bool				 foundSortedPos = false;
			StringList			 theFinalTableNameList = query.getAllTableNameList();
			if (!joinList.empty()) {
				if (joinList[0].joinType == Keywords::right_kw) {
					std::reverse(theFinalTableNameList.begin(), theFinalTableNameList.end());
				}
			}

			for (auto& entityName: theFinalTableNameList) {
				auto theEntity = query.getTableMap()[entityName];
				std::vector<StringList>			 theRowDataCollection;
				for (Row row : entityRowMap[theEntity]) {
					StringList theRowData;
					for (auto& attr : theEntity->getAttributes()) {
						if (!query.getOrderBy().empty()) {
							std::string	theSortedAttr = query.getOrderBy()[0];
							if (!foundSortedPos && attr.getName() != theSortedAttr) { ++theSortedPos; }
							else if (attr.getName() == theSortedAttr) {
								theSortedDType = attr.getType();
								foundSortedPos = true;
							}
						}						
						theRowData.push_back(std::get<std::string>(row.getData()[attr.getName()]));
					}
					if (!theRowData.empty()) {
						theRowDataCollection.push_back(theRowData);
					}
				}								
				for (auto& attr : theEntity->getAttributes()) {
					theInitWidthList.push_back(attr.getName().length()); // initialize widthList
					theAttrNamePosMap[theEntity->getName()+attr.getName()] = theAttrCount++;
				}
				theRowDataCollectionMap[theCount++] = theRowDataCollection;
			}
			StringList theFinalRow;
			std::vector<StringList> theFinalRowCollection;
			DFSToShowRow(theRowDataCollectionMap, theFinalRow, theFinalRowCollection, 0, theInitWidthList, joinList, theAttrNamePosMap, anOutput); // include initialize the widthList			
			if (!joinList.empty()) {
				auto theEntity = query.getTableMap()[theFinalTableNameList[0]];
				for (auto& row1 : theRowDataCollectionMap[0]) {
					bool doesExist = false;
					for (auto& row2 : theFinalRowCollection) {
						if (row1[0] == row2[0]) {
							doesExist = true;
							break;
						}						
					}
					if (!doesExist) {
						++numOfSlectedRow;
						StringList theAdditionalRow = row1;
						StringList theTemp;
						for (size_t i = 0; i < theRowDataCollectionMap[1][0].size(); ++i) {
							theTemp.push_back("NULL");
						}
						theAdditionalRow.insert(theAdditionalRow.end(), theTemp.begin(), theTemp.end());
						theFinalRowCollection.push_back(theAdditionalRow);
					}
				}
			}
			
			
			if (!query.getSelectAll()) {
				uint32_t theCount = 0;
				for (auto& entityName : theFinalTableNameList) {
					auto theEntity = query.getTableMap()[entityName];
					if (theCount >= fieldNameList.size()) {
						break;
					}
					for (auto& attrName : fieldNameList) {
						if (theEntity->getAttribute(attrName)) {
							++theCount;
							fieldEntityMap[attrName] = theEntity->getName();
							widthList.push_back(theInitWidthList[theAttrNamePosMap[theEntity->getName()+attrName]]);
						}
						
					}
				}
				
			}
			else {
				for (auto& entityName : theFinalTableNameList) {
					auto theEntity = query.getTableMap()[entityName];					
					for (auto& attrName : fieldNameList) {
						fieldEntityMap[attrName] = theEntity->getName();
						widthList.push_back(theInitWidthList[theAttrNamePosMap[theEntity->getName() + attrName]]);
					}
				}
				widthList = theInitWidthList;
			}
			
			showHeader(anOutput);
			showBody(theSortedPos, theSortedDType,theFinalRowCollection, theAttrNamePosMap, anOutput);
			showFooter(theTimer.elapsed(), anOutput);

			return true;
		}

	protected:
		//Entity              &entity; //if necessary?

		uint32_t									numOfSlectedRow = 0;
		std::unordered_map<Entity*, RowList>		entityRowMap;
		std::vector<std::string>					fieldNameList;	
		std::map<std::string, std::string>			fieldEntityMap;
		std::vector<size_t>							widthList;
		const Query&								query;
		const JoinList&								joinList;
	};

}

#endif /* EntityView_h */

#pragma once
