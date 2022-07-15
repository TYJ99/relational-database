#include "ParseInsertHelper.hpp"

namespace ECE141 {

	bool ParseInsertHelper::checkAttribute(std::string& aName) {
		auto theTables = db->getTables();
		for (auto& table : theTables) {
			if (table.getName() == tableName) {
				auto& theAttrList = table.getAttributes();
				for (auto& attr : theAttrList) {
					if (attr.getName() == aName) {
						return true;
					}
				}
			}
		}
		return false;
	}

	StatusResult  ParseInsertHelper::parseQuote(std::string& aFieldName, std::string& aQuote, bool isAttr) {
		if (tokenizer.next()) {
			while (tokenizer.more() && aQuote != tokenizer.current().data) {
				aFieldName += tokenizer.current().data;
				tokenizer.next();
			}
			tokenizer.next();
			// if the value which is quoted is not an attribute
			if (!isAttr) {
				return StatusResult{ noError };
			}
			// if the value which is quoted is an attribute, 
			// need to check if the attribute exist in the tableList.
			if (checkAttribute(aFieldName)) {
				return StatusResult{ noError };
			}
		}
		return StatusResult{ invalidCommand };
	}

	StatusResult ParseInsertHelper::parseAttrName(std::vector<Attribute>& anAttrList) {
		StatusResult theResult{ invalidCommand };
		std::string theFieldName = "";
		if (")" == tokenizer.current().data || !tokenizer.more()) {
			return theResult;
		}

		if ("'" == tokenizer.current().data) {
			theResult = parseQuote(theFieldName, tokenizer.current().data, true);
		}
		else if ("\"" == tokenizer.current().data) {
			theResult = parseQuote(theFieldName, tokenizer.current().data, true);
		}
		else if (TokenType::identifier == tokenizer.current().type) {
			theFieldName += tokenizer.current().data;
			tokenizer.next();
			if (checkAttribute(theFieldName)) {
				anAttrList.push_back(db->getTable(tableName).value()->getAttribute(theFieldName).value());
				return StatusResult{ noError };
			}
		}

		return theResult;
	}

	StatusResult ParseInsertHelper::parseValues(std::vector<std::string>& aValList) {
		StatusResult theResult;
		std::string theValueName;
		if ("'" == tokenizer.current().data) {
			theResult = parseQuote(theValueName, tokenizer.current().data, false);
		}
		else if ("\"" == tokenizer.current().data) {
			theResult = parseQuote(theValueName, tokenizer.current().data, false);
		}
		else if (TokenType::identifier == tokenizer.current().type || TokenType::number == tokenizer.current().type
				 || "true" == tokenizer.current().data || "false" == tokenizer.current().data) {
			theValueName += tokenizer.current().data;
			tokenizer.next();
		}
		aValList.push_back(theValueName);
		if (!tokenizer.skipIf(',')) {
			if (tokenizer.more()) {
				if (")" != tokenizer.current().data) {
					theResult = invalidCommand;
				}
			}
			else {
				theResult = invalidCommand;
			}
		}
		return theResult;
	}

	//verify Attributes and Values
	StatusResult  ParseInsertHelper::verifyAttrsVals(std::vector<std::string>& aValList, std::vector<Attribute>& anAttrList) {
		StatusResult theResult;
		if (0 != anAttrList.size()) {
			if (aValList.size() != anAttrList.size()) {
				theResult = keyValueMismatch;
			}

			// check if the attribute cannot be null, it must have a value. (default value or assigned value)
			if (theResult) {
				auto theTables = db->getTables();
				for (auto& table : theTables) {
					if (table.getName() == tableName) {
						auto& theAttrs = table.getAttributes();
						for (Attribute attr : theAttrs) {
							bool doesExist = false;
							for (auto& newAttr : anAttrList) {
								if (attr.getName() == newAttr.getName()) {
									doesExist = true;
									break;
								}
							}
							if (!doesExist) {
								if (!attr.isNullable() && !attr.hasDefault() && !attr.isAutoIncrement()) {
									theResult = nullableAttribute;
									break;
								}
							}
						}

					}
				}
			}

		}
		else {
			auto theTables = db->getTables();
			for (auto& table : theTables) {
				if (table.getName() == tableName) {
					if (aValList.size() != table.getAttributes().size()) {
						theResult = keyValueMismatch;
					}
				}
			}
		}
		return theResult;
	}

	StatusResult  ParseInsertHelper::validate(const Attribute& anAttr, std::string& aValue) {
		StatusResult theResult;
		auto theDataType = anAttr.getType();
		if (DataTypes::int_type == theDataType) {
			std::stringstream theSS(aValue);
			uint32_t theInt;
			std::string theStr;
			if (theSS >> theInt) {
				theSS >> theStr;
			}
			if (theStr.length() > 0) {
				theResult = incorrectAttrType;
			}
		}
		else if (DataTypes::float_type == theDataType) {
			std::stringstream theSS(aValue);
			double theDouble;
			std::string theStr;
			if (theSS >> theDouble) {
				theSS >> theStr;
			}
			if (theStr.length() > 0) {
				theResult = incorrectAttrType;
			}
		}
		else if (DataTypes::char_type == theDataType) {
			if (anAttr.getSize() - aValue.length() > 0) {
				aValue.insert(aValue.length(), anAttr.getSize() - aValue.length(), ' ');
			}
			else {
				std::string theNewString = aValue.substr(0, anAttr.getSize());
				aValue = theNewString;
			}

		}
		else if (DataTypes::varchar_type == theDataType) {
			std::string theNewString = aValue.substr(0, anAttr.getSize());
			aValue = theNewString;
		}
		else if (DataTypes::bool_type == theDataType) {
			if ("true" != aValue && "false" != aValue && "0" != aValue && "1" != aValue) {
				theResult = incorrectAttrType;
			}
		}
		return theResult;
	}


}