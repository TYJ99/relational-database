//
//  Attribute.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include "keywords.hpp"
#include "BasicTypes.hpp"

namespace ECE141 {

	/*
	  struct Property {
		Property(std::string aName, int aTableId=0) : name(aName), tableId(aTableId), desc(true) {}
		std::string     name;
		int             tableId;
		bool            desc;
	  };

	  using PropertyList = std::vector<Property>;
	*/

	class Attribute {
	protected:
		std::string   name;
		DataTypes     type;
		uint16_t      size : 10; //max=1000, for the number in char(50) and varchar(50)
		uint16_t      autoIncrement : 1;
		uint16_t      primary : 1;
		uint16_t      nullable : 1;
		std::string   defaultVal = "NULL";
		bool		  hasDefaultVal = false;
		//Others?

	public:

		Attribute(DataTypes aType = DataTypes::no_type);
		Attribute(std::string aName, DataTypes aType, uint32_t aSize = 0);
		Attribute(const Attribute& aCopy);
		Attribute& operator=(const Attribute& aAttr);
		~Attribute();

		Attribute& setName(std::string& aName);
		Attribute& setDataType(DataTypes aType);
		Attribute& setSize(int aSize);
		Attribute& setAutoIncrement(bool anAuto);
		Attribute& setPrimaryKey(bool aPrimary);
		Attribute& setNullable(bool aNullable);
		Attribute& setHasDefault(bool aDefault);
		Attribute& setDefaultVal(const std::string& aValue);

		bool                isValid(); //is this Entity valid?

		const std::string&  getName() const { return name; }
		DataTypes           getType() const { return type; }
		size_t              getSize() const { return size; }
		std::string			getDefaultVal() const;
		Value               toValue(const std::string& aValue) const;
		

		bool                isPrimaryKey() const { return primary; }
		bool                isNullable() const { return nullable; }
		bool                isAutoIncrement() const { return autoIncrement; }
		bool                hasDefault() const { return hasDefaultVal; }
	};

	using AttributeOpt = std::optional<Attribute>;
	using AttributeList = std::vector<Attribute>;

}


#endif /* Attribute_hpp */
