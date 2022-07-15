//
//  Entity.cpp
//  PA3
//
//  Created by rick gessner on 3/2/22.
//

#include <stdio.h>
#include <vector>
#include <optional>
#include <memory>
#include <string>
#include "Entity.hpp"

namespace ECE141 {

	//STUDENT: Implement this class...

	Entity::Entity(const std::string aName) :
		name(aName), autoincr(1), blockNum(-1), nextRowPos(0), primaryKey(std::nullopt) {}

	Entity& Entity::operator=(const Entity& anEntity) {
		name = anEntity.name;
		autoincr = anEntity.autoincr;
		attributes = anEntity.attributes;
		blockNum = anEntity.blockNum;
		rows = anEntity.rows;
		nextRowPos = anEntity.nextRowPos;
		primaryKey = anEntity.primaryKey;
		indexBlockMap = anEntity.indexBlockMap;
		return *this;
	}

	Entity::Entity(const Entity& aCopy) {
		*this = aCopy;
	}

	Entity::~Entity() {
		//std::cout << "~Entity()\n";
	}

	Entity& Entity::addAttribute(const Attribute& anAttribute) {
		if (!getAttribute(anAttribute.getName())) {
			attributes.push_back(anAttribute);
		}
		return *this;
	}

	AttributeOpt Entity::getAttribute(const std::string& aName) const {
		for (auto &attribute : attributes) {     //Go through vector of attributes and return matching attribute if found
			if (aName == attribute.getName()) {
				return attribute;  //The caller of this function needs to deallocate this memory at end of lifecycle
			}
		}
		return std::nullopt;
	}

	// USE: ask the entity for name of primary key (may not have one...)
	AttributeOpt Entity::getPrimaryKey() const {
		return primaryKey;
		/*
		for (auto &attribute : attributes) {     //Go through vector of attributes and return primary key if found
			if (attribute.isPrimaryKey()) {
				return attribute; //The caller of this function needs to deallocate this memory at end of lifecycle
			}
		}
		return std::nullopt;
		*/
	}

	void Entity::setBlockNum(int32_t aNum) {
		blockNum = aNum;
	}

	int32_t Entity::getBlockNum() {
		return blockNum;
	}

	void Entity::addRow(Row aRow) {
		rows.push_back(aRow);
	}

	void Entity::updateAutoIncr() {
		for (auto& attr : attributes) {
			if (attr.isAutoIncrement()) {
				++autoincr;
				break;
			}
		}
	}

	void Entity::updateAutoIncr(int aVal) {
		for (auto& attr : attributes) {
			if (attr.isAutoIncrement()) {
				autoincr = aVal + 1;
			}
		}
	}

	autoIncrOpt	Entity::getAutoIncrAttr() {
		for (auto& attr : attributes) {
			if (attr.isAutoIncrement()) {
				return attr;
			}
		}
		return std::nullopt;
	}

	int Entity::getAutoIncr() {
		return autoincr;
	}

	void Entity::setAutoIncr(uint32_t anInt) {
		autoincr = anInt;
	}

	void Entity::insertIndexBlockMap(std::string& anIndex, uint32_t& aBlockNum) {
		indexBlockMap[anIndex] = aBlockNum;
	}

	void Entity::eraseIndexBlockMap(std::string& anIndex) {
		indexBlockMap.erase(anIndex);
	}
	
}
