#ifndef IndexView_hpp
#define IndexView_hpp

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include "View.hpp"

namespace ECE141 {

	using ShowIndexesMap = std::map<std::string, std::optional<std::string>>;

	template <typename T1>
	class IndexView : public View {
	public:

		IndexView(T1& aMap, StringList& aTitleList) : 
							map(aMap), titleList(aTitleList) {
			for (auto& title : aTitleList) {
				widthList.push_back(title.length());
			}
		}


		// USE: create header for Entity View...
		IndexView& showHeader(std::ostream& anOutput) {
			borderView(widthList, anOutput);
			rowView(titleList, widthList, anOutput);
			borderView(widthList, anOutput);
			return *this;
		}

		// USE: create body for Index View...
		IndexView& showBody(std::vector<StringList> aRows, std::ostream& anOutput) {
			for (auto& row : aRows) {
				rowView(row, widthList, anOutput);
			}
			borderView(widthList, anOutput);
			return *this;
		}

		// USE: create footer for Entity View...
		IndexView& showFooter(double anElapsedTime, std::ostream& anOutput) {
			anOutput << map.size() << " rows in set(" << anElapsedTime << " sec)\n"; // the footer
			return *this;
		}



		// USE: this is the main show() for the view, where it presents all the rows...
		bool show(std::ostream& anOutput) {
			Timer theTimer = Config::getTimer();
			std::vector<StringList> theRows;
			for (auto& pair : map) {
				StringList theRow;
				theRow.push_back(pair.first);
				std::stringstream theSS;
				if (pair.second.has_value()) {
					theSS << pair.second.value();
					theRow.push_back(theSS.str());
				}
				else {
					theRow.push_back("NULL");
				}
				theRows.push_back(theRow);
				for (size_t i = 0; i < theRow.size(); ++i) {
					widthList[i] = std::max(widthList[i], theRow[i].length());
				}						
			}
			showHeader(anOutput);
			showBody(theRows, anOutput);
			showFooter(theTimer.elapsed(), anOutput);

			return true;
		}

	protected:
		T1							map;
		std::vector<size_t>	        widthList;
		StringList					titleList;
	};

}

#endif // !IndexView_hpp

