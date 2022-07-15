
#ifndef TableDescriptionView_h
#define TableDescriptionView_h

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
#include <functional>

namespace ECE141 {

	class TableDescriptionView : public View {
	public:
		
		TableDescriptionView(Entity& anEntity) : entity(anEntity) {}
		using attrFields = std::vector<std::string>;
		attrFields theFields = { "Field", "Type", "Null", "Key", "Default", "Extra" };

		// USE: create header for Entity View...
		TableDescriptionView& showHeader(const std::vector<size_t> aWidthList, std::ostream& anOutput) {
			borderView(aWidthList, anOutput);
			rowView(theFields, aWidthList, anOutput);
			borderView(aWidthList, anOutput);
			return *this;
		}

		// USE: create body for Entity View...
		TableDescriptionView& showBody(const std::vector<size_t> aWidthList, std::ostream& anOutput) {
			
			std::vector<std::vector<std::string>> rowList;
			for (size_t i = 0; i < fieldNames.size(); ++i) {
				std::vector<std::string> tempList;
				tempList.push_back(fieldNames[i]);
				tempList.push_back(dataTypes[i]);
				tempList.push_back(isNull[i]);
				tempList.push_back(isKey[i]);
				tempList.push_back(hasDefault[i]);
				tempList.push_back(hasExtra[i]);
				rowList.push_back(tempList);
			}
			for (std::vector<std::string> row : rowList) {
				rowView(row, aWidthList, anOutput);
			}			
			borderView(aWidthList, anOutput);
			return *this;
		}

		// USE: create footer for Entity View...
		TableDescriptionView& showFooter(size_t anNumOfRow, double anElapsedTime, std::ostream& anOutput) {
			anOutput << anNumOfRow << " rows in set(" << anElapsedTime << " sec)\n"; // the footer
			return *this;
		}		

		
		
		//using getAttr = std::function<std::string(Attribute&)>;

		// USE: this is the main show() for the view, where it presents all the rows...
		bool show(std::ostream& anOutput) {
	
			Timer theTimer = Config::getTimer();
			AttributeList attrList =  entity.getAttributes();
			for (Attribute attr : attrList) {
			
				fieldNames.push_back(attr.getName());
				std::string theTypeStr = Helpers::dataTypeToString(attr.getType());
				dataTypes.push_back(theTypeStr);
				if (attr.isNullable()) {
					isNull.push_back("YES");
				}
				else {
					isNull.push_back("NO");
				}
				if (attr.isPrimaryKey()) {
					isKey.push_back("YES");
					hasExtra.push_back("auto_increment primary key");
				}
				else {
					isKey.push_back(" ");
					hasExtra.push_back(" ");
				}
				
				hasDefault.push_back(attr.getDefaultVal());
				
				
			}
			for (size_t i = 0; i < theFields.size() - 1; ++i) {
				widthList.push_back(theFields[i].length());
			}
			widthList.push_back(static_cast<std::string>("auto_increment primary key").length());

			for (std::string name : fieldNames) {
				if (name.length() > widthList[0]) {
					widthList[0] = name.length();
				}
			}
			for (std::string type : dataTypes) {
				if (type.length() > widthList[1]) {
					widthList[1] = type.length();
				}
			}
			for (std::string df : hasDefault) {
				if (df.length() > widthList[4]) {
					widthList[4] = df.length();
				}
			}
			

			showHeader(widthList, anOutput);
			showBody(widthList, anOutput);
			showFooter(fieldNames.size(), theTimer.elapsed(), anOutput);

			return true;
		}

	protected:
		//Entity              &entity; //if necessary?
		std::string		title = "Tables_in_mydb";
		Entity			&entity;

		std::vector<std::string>	fieldNames;
		std::vector<std::string>	dataTypes;
		std::vector<std::string>	isNull;
		std::vector<std::string>	isKey;
		std::vector<std::string>	hasDefault;
		std::vector<std::string>	hasExtra;
		std::vector<size_t>			widthList;
	};

}

#endif /* EntityView_h */

