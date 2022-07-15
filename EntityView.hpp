
#ifndef EntityView_h
#define EntityView_h

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include "View.hpp"

namespace ECE141 {

	class EntityView : public View {
	public:

		EntityView(std::vector<std::string>& anEntityList ): entityList(anEntityList) {}
			

		// USE: create header for Entity View...
		EntityView& showHeader(size_t aBorder, std::ostream& anOutput) {
			borderView(aBorder, anOutput);
			rowView(title, aBorder, anOutput);
			borderView(aBorder, anOutput);
			return *this;
		}

		// USE: create body for Entity View...
		EntityView& showBody(size_t aBorder, std::vector<std::string>& anEntityList, std::ostream& anOutput) {
			for (auto iter = anEntityList.begin(); iter != anEntityList.end(); ++iter) {
				rowView(*iter, aBorder, anOutput);
			}
			borderView(aBorder, anOutput);
			return *this;
		}

		// USE: create footer for Entity View...
		EntityView& showFooter(size_t anNumOfRow, double anElapsedTime, std::ostream& anOutput) {
			anOutput << anNumOfRow << " rows in set(" << anElapsedTime << " sec)\n"; // the footer
			return *this;
		}

		

		// USE: this is the main show() for the view, where it presents all the rows...
		bool show(std::ostream& anOutput) {
			Timer theTimer = Config::getTimer();
			size_t theBorder = title.length();
			for (std::string entity: entityList) {
				theBorder = std::max(theBorder, entity.length());
			}
			showHeader(theBorder, anOutput);
			showBody(theBorder, entityList, anOutput);
			showFooter(entityList.size(), theTimer.elapsed(), anOutput);

			return true;
		}

	protected:
		//Entity              &entity; //if necessary?
		std::string					title = "Tables_in_mydb";
		std::vector<std::string>    entityList;
	};

}

#endif /* EntityView_h */

