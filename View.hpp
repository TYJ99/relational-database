//
//  View.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//
#include <iomanip>
#include "Config.hpp"

#ifndef View_h
#define View_h


#include <iostream>
#include <vector>

namespace ECE141 {

	//completely generic view, which you will subclass to show information
	class View {
	private:
		std::string borderSeparator = "+";
		std::string rowSeparator = "|";
		char border = '-';
	public:
		virtual         ~View() {}
		virtual bool    show(std::ostream& aStream) = 0;
		// int seperator;
		// add border i.e. +-------------------+
		// for single column
		void            borderView(const size_t& aBorder, std::ostream& anOutput) {
			std::string theStart = borderSeparator + border; // +-
			std::string theEnd = border + borderSeparator;   // -+
			anOutput << theStart << std::setw(aBorder)
								 << std::left
								 << std::setfill(border) << border << theEnd << "\n";

		}

		// add row view i.e. | Database         |
		// for single column
		void            rowView(const std::string& aName, const size_t& aBorder, std::ostream& anOutput) {
			std::string theStart = rowSeparator + " "; // "| "
			std::string theEnd = " " + rowSeparator;   // " |"
			anOutput << theStart << std::setw(aBorder)
								 << std::left
								 << std::setfill(' ') << aName << theEnd << "\n";

		}

		// for multiple column
		void            borderView(const std::vector<size_t> aWidthList, std::ostream& anOutput) {
			std::string theEnd = border + borderSeparator;   // -+
			anOutput << borderSeparator;
			for (size_t theWidth: aWidthList) {
				anOutput << border << std::setw(theWidth)
								   << std::left
								   << std::setfill(border) << border << theEnd;
			}
			anOutput << "\n";
		}

		// for multiple column
		void            rowView(const std::vector<std::string> aNameList, const std::vector<size_t> aWidthList,
							std::ostream& anOutput) {							
			std::string theEnd = " " + rowSeparator;   // " |"			
			anOutput << rowSeparator;
			for (size_t i = 0; i < aNameList.size(); ++i) {
				anOutput << " " << std::setw(aWidthList[i])
								<< std::left
								<< std::setfill(' ') << aNameList[i] << theEnd;
			}
			anOutput << "\n";

		}

	
		std::string getBorderSeparator() {
			return borderSeparator;
		}

		std::string getRowSeparator() {
			return rowSeparator;
		}

	};

}

#endif /* View_h */
