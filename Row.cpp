//
//  Row.cpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//


#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include "Row.hpp"


namespace ECE141 {

	Row::Row(std::string anEntityName, uint32_t aBlockNum): blockNumber(aBlockNum),entityName(anEntityName){}
	Row::Row(const Row& aRow) { *this = aRow; }

	Row::~Row() {}

	Row& Row::operator=(const Row& aRow) { 
		blockNumber = aRow.blockNumber;
		entityName = aRow.entityName;
		data = aRow.data;
		return *this; 
	}
	bool Row::operator==(Row& aCopy) const { 		
		return false; 
	}
	
	Row& Row::set(const std::string& aKey, const Value& aValue) {
		data[aKey] = aValue;
		return *this;
	}

}
