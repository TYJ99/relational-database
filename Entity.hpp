//
//  Entity.hpp
//  Assignment3
//
//  Created by rick gessner on 3/18/22.
//  Copyright © 2022 rick gessner. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include <vector>
#include <optional>
#include <memory>
#include <string>

#include "Attribute.hpp"
#include "Errors.hpp"
#include "BasicTypes.hpp"
#include "Row.hpp"

namespace ECE141 {

	using AttributeOpt = std::optional<Attribute>;
	using AttributeList = std::vector<Attribute>;
	using RowList = std::vector<Row>;
	using autoIncrOpt = std::optional<Attribute>;
	using PrimaryKeyOpt = std::optional<std::string>;
	using IndexBlockMap = std::map<std::string, std::optional<uint32_t>>;

	//------------------------------------------------

	class Entity {
	public:
		Entity(const std::string aName);
		Entity(const Entity& aCopy);
		Entity& operator=(const Entity& anEntity);

		~Entity();

		const std::string& getName() const { return name; }

		Entity&	   addAttribute(const Attribute& anAttribute);
		void	   addRow(Row aRow);
		RowList&   getRows() { return rows; }

		AttributeList&		 getAttributes() { return attributes; }		
		AttributeOpt		 getAttribute(const std::string& aName) const;
		AttributeOpt		 getPrimaryKey() const;
		void				 setBlockNum(int32_t aNum);
		int32_t				 getBlockNum();
		void				 updateAutoIncr();
		void				 updateAutoIncr(int aVal);
		autoIncrOpt			 getAutoIncrAttr();
		int					 getAutoIncr();
		void				 setAutoIncr(uint32_t anInt);
		void				 setNextRowPos(size_t aPos) { nextRowPos = aPos; }
		size_t				 getNextRowPos() { return nextRowPos; }
		void				 setPrimaryKey(Attribute& anAttr) { primaryKey = anAttr; }
		void				 insertIndexBlockMap(std::string& anIndex, uint32_t& aBlockNum);
		void				 eraseIndexBlockMap(std::string& anIndex);
		IndexBlockMap&		 getIndexBlockMap() { return indexBlockMap; }

	protected:

		AttributeList   attributes;
		std::string     name;
		uint32_t        autoincr;  //auto_increment
		int32_t			blockNum;
		RowList			rows;
		size_t			nextRowPos = 0;
		AttributeOpt	primaryKey = std::nullopt;
		IndexBlockMap   indexBlockMap;
	};

}
#endif /* Entity_hpp */
