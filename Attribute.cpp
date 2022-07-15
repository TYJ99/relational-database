//
//  Attribute.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include <iostream>
#include "Attribute.hpp"

namespace ECE141 {

	Attribute::Attribute(DataTypes aType)
		:name(""), type(aType), size(0), autoIncrement(0), primary(0), nullable(1), defaultVal("NULL"), hasDefaultVal(false) {}

	Attribute::Attribute(std::string aName, DataTypes aType, uint32_t aSize) {
		name = aName;
		type = aType;
		size = aSize;
		autoIncrement = 0;
		primary = 0;
		nullable = 1;
		defaultVal = "NULL"; 
		hasDefaultVal = false;
	}

	Attribute& Attribute::operator=(const Attribute& aAttr) {
		name = aAttr.name;
		type = aAttr.type;
		size = aAttr.size;
		autoIncrement = aAttr.autoIncrement;
		primary = aAttr.primary;
		nullable = aAttr.nullable;
		defaultVal = aAttr.defaultVal;
		hasDefaultVal = aAttr.hasDefaultVal;
		return *this;
	}

	Attribute::Attribute(const Attribute& aCopy) {
		*this = aCopy;
	}

	Attribute::~Attribute() {
	}

	Attribute& Attribute::setName(std::string& aName) {
		name = aName;
		return *this;
	}

	Attribute& Attribute::setDataType(DataTypes aType) {
		type = aType;
		return *this;
	}

	Attribute& Attribute::setSize(int aSize) {
		size = aSize; 
		return *this;
	}

	Attribute& Attribute::setAutoIncrement(bool anAuto) {
		autoIncrement = anAuto; 
		return *this;
	}

	Attribute& Attribute::setPrimaryKey(bool aPrimary) {
		primary = aPrimary; 
		return *this;
	}

	Attribute& Attribute::setNullable(bool aNullable) {
		nullable = aNullable; 
		return *this;
	}

	Attribute& Attribute::setDefaultVal(const std::string& aValue) {
		defaultVal = aValue;
		return *this;
	}

	Attribute& Attribute::setHasDefault(bool aDefault) {
		hasDefaultVal = aDefault;
		return *this;
	}

	bool Attribute::isValid() {
		return true;
	}

	Value Attribute::toValue(const std::string& aValue) const {
		//might need to do type-casting here...    
		return Value{ aValue };
	}

	std::string Attribute::getDefaultVal() const {
		return defaultVal;
	}

}
