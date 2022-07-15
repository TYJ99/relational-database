//
//  FolderView.hpp
//  Assignment2
//
//  Created by rick gessner on 2/15/21.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderView_h
#define FolderView_h

#include "FolderReader.hpp"
#include "View.hpp"
#include <iomanip>
#include <vector>

namespace ECE141 {
	// USE: provide view class that lists db files in storage path...
	class FolderView : public View {
	
	public:
		FolderView(const char* aPath, const char* anExtension = ".db")
			: reader(aPath), extension(anExtension) {}
		/*-------------------------------------------------------------------
		*  try to make our view as below.
			+ -------------------+
			| Database			 |
			+--------------------+
			| foo                |
			| bar                |
			| wiltz				 |
			+--------------------+
			3 rows in set(0.02 sec)
		
		-------------------------------------------------------------------------*/

		void showHeader(size_t aBorder, std::ostream& anOutput) {
			borderView(aBorder, anOutput);
			rowView("Database", aBorder, anOutput);
			borderView(aBorder, anOutput);
		}

		void showBody(size_t aBorder, std::vector<std::string>& aDBList, std::ostream& anOutput) {
			for (auto iter = aDBList.begin(); iter != aDBList.end(); ++iter) {
				rowView(*iter, aBorder, anOutput);
			}
			borderView(aBorder, anOutput);
		}

		void showFooter(size_t anNumOfRow, double anElapsedTime, std::ostream& anOutput) {
			anOutput << anNumOfRow << " rows in set(" << anElapsedTime << " sec)\n"; // the footer
		}


		virtual bool show(std::ostream& anOutput) {
			Timer theTimer = Config::getTimer();
			size_t theBorder = 18; // default length of border
			std::vector<std::string> theDBList;
			reader.each(extension, [&](const std::string& aString) {
				const std::filesystem::path theStringToPath{ aString };				
				theDBList.push_back(theStringToPath.stem().string());
				theBorder = std::max(theBorder, theStringToPath.stem().string().length()); // make sure the name will never larger than border.
				return true;
				});

			//theBorder += 4; // make sure there is still some space from name to border.
			showHeader(theBorder, anOutput);
			showBody(theBorder, theDBList, anOutput);
			size_t theNumOfRow = theDBList.size();
			showFooter(theNumOfRow, theTimer.elapsed(), anOutput);
			
			return true;
		}

		FolderReader  reader;
		const char* extension;

	};
	

}

#endif /* FolderView_h */
