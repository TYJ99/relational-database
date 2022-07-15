#include "ParseCreateTableHelper.hpp"

namespace ECE141 {	

	StatusResult ParseCreateTableHelper::parseAttribute(Entity& anEntity) {
		StatusResult theResult = invalidCommand;
		Attribute theAttr;
		if (TokenType::identifier == tokenizer.current().type) {
			if (Helpers::getKeywordId(tokenizer.current().data) == Keywords::unknown_kw) {
				theAttr.setName(tokenizer.current().data);
				auto hasNext = tokenizer.next();
				if (hasNext && Helpers::isDatatype(tokenizer.current().keyword)) {
					auto theDatatype = Helpers::keywordToDatatype(tokenizer.current().keyword);
					if (DataTypes::timestamp_type == theDatatype) {
						theAttr.setDefaultVal(Config::getTimer().getCurrentTime());
						theAttr.setHasDefault(true);
					}
					theAttr.setDataType(theDatatype);
					hasNext = tokenizer.next();

					// handle the number in varchar(50) and char(50)
					if (hasNext && (DataTypes::varchar_type == theDatatype || DataTypes::char_type == theDatatype)) {
						if (tokenizer.skipIf('(')) {
							theAttr.setSize(std::stoi(tokenizer.current().data));
						}
						hasNext = tokenizer.next();
						if (hasNext && tokenizer.skipIf(')')) {
							theResult = parseProperty(theAttr, anEntity);
						}
					}
					else if (hasNext) {
						theResult = parseProperty(theAttr, anEntity);
					}

				}
			}
		}
		else {
			theResult = invalidCommand;
		}
		if (theResult) {
			// To check that the primary key cannot be null
			if (theAttr.isPrimaryKey()) {
				if (theAttr.isNullable()) {
					theResult = primaryKeyNull;
				}
			}
			// To check that the auto_increment cannot have default value.
			if (theResult && theAttr.isAutoIncrement()) {
				if (theAttr.hasDefault()) {
					theResult = invalidDefaultValue;
				}
			}
			if (theResult) {
				anEntity.addAttribute(theAttr);
			}
		}
		return theResult;
	}

	// parse the properties after name and type.
	StatusResult ParseCreateTableHelper::parseProperty(Attribute& anAttr, Entity& anEntity) {
		StatusResult theResult;
		if ("," == tokenizer.current().data || ")" == tokenizer.current().data) {
			return theResult;
		}
		if (!tokenizer.more() || TokenType::keyword != tokenizer.current().type) {
			theResult = invalidCommand;
			return theResult;
		}
		while (theResult && tokenizer.more() && TokenType::keyword == tokenizer.current().type) {
			switch (tokenizer.current().keyword) {
			case Keywords::null_kw:				
				anAttr.setNullable(true);
				tokenizer.next();
				break;
			case Keywords::not_kw:												// Set NOT NULL if needed 								
				if (tokenizer.next()) {
					auto& theNextToken = tokenizer.current();
					if (Keywords::null_kw == theNextToken.keyword) {
						anAttr.setNullable(false);
					}
					else {
						theResult = invalidCommand;
					}
				}
				else {
					theResult = invalidCommand;
				}
				tokenizer.next();
				break;
			case Keywords::auto_increment_kw:
				anAttr.setAutoIncrement(true); // Set auto Increment true
				tokenizer.next();
				break;
			case Keywords::primary_kw:
				if (tokenizer.next()) {
					auto& theNextToken = tokenizer.current();
					if (Keywords::key_kw == theNextToken.keyword) {
						anEntity.setPrimaryKey(anAttr);
						anAttr.setPrimaryKey(true);
						anAttr.setNullable(false); // Since the default nullable is true, we need to set it false here.
					}
					else {
						theResult = invalidCommand;
					}
				}
				else {
					theResult = invalidCommand;
				}
				tokenizer.next();
				break;
				// case Keywords::key_kw										// Key is only used for primary key, which primary case covers
				// 	break;
			case Keywords::default_kw:
				if (tokenizer.next()) {
					auto& theNextToken = tokenizer.current();
					// if its type is keywords, it has to be true or false.
					if (Keywords::true_kw == theNextToken.keyword || Keywords::false_kw == theNextToken.keyword) {
						anAttr.setDefaultVal(theNextToken.data);
					}
					// if it is not like the format "name" or 'sdfsdf', it has to be a valid number.
					else if ("'" != theNextToken.data && "\"" != theNextToken.data) {
						bool hasDot = false;
						for (char ch : theNextToken.data) {
							if ('.' == ch) {
								if (hasDot) {
									theResult = invalidCommand;
									break;
								}
								hasDot = true;
							}
							else if (!std::isdigit(ch)) {
								theResult = invalidCommand;
								break;
							}
						}
						if (theResult) {
							anAttr.setDefaultVal(theNextToken.data);
						}
					}
					else if ("'" == theNextToken.data) {
						std::string theStr = "";
						if (tokenizer.next()) {
							while (tokenizer.more() && "'" != tokenizer.current().data) {
								theStr += tokenizer.current().data + " ";
								tokenizer.next();
							}
							if (!tokenizer.more()) {
								theResult = invalidCommand;
							}
							else {
								anAttr.setDefaultVal(theStr);
							}
						}
						else {
							theResult = invalidCommand;
						}
					}
					else if ("\"" == theNextToken.data) {
						std::string theStr = "";
						if (tokenizer.next()) {
							while (tokenizer.more() && "\"" != tokenizer.current().data) {
								theStr += tokenizer.current().data + " ";
								tokenizer.next();
							}
							if (!tokenizer.more()) {
								theResult = invalidCommand;
							}
							else {
								anAttr.setDefaultVal(theStr);
							}
						}
						else {
							theResult = invalidCommand;
						}
					}
					else {
						theResult = invalidCommand;
					}
				}
				else {
					theResult = invalidCommand;
				}

				if (theResult) {
					anAttr.setHasDefault(true);
				}

				tokenizer.next();
				break;
			default:
				theResult = invalidCommand;
				tokenizer.next();
				break;
			}
		}
		return theResult;
	}

}