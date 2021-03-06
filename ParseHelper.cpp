//
//  ParseHelpers.cpp
//  RGAssignment4
//
//  Created by rick gessner on 4/18/21.
//

#include "ParseHelper.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"

namespace ECE141 {

	// USE: gets properties following the type in an attribute decl...
	StatusResult ParseHelper::parseAttributeOptions(Attribute& anAttribute) {
		bool          options = true;
		StatusResult  theResult{ noError };
		char          thePunct[] = { ");," };

		while (theResult && options && tokenizer.more()) {
			Token& theToken = tokenizer.current();
			switch (theToken.type) {
			case TokenType::keyword:
				switch (theToken.keyword) {
				case Keywords::auto_increment_kw:
					anAttribute.setAutoIncrement(true);
					break;
				case Keywords::primary_kw:
					anAttribute.setPrimaryKey(true);
					break;
				case Keywords::not_kw:
					tokenizer.next();
					theToken = tokenizer.current();
					if (Keywords::null_kw == theToken.keyword) {
						anAttribute.setNullable(false);
					}
					else theResult.error = syntaxError;
					break;

				default: break;
				}
				break;

			case TokenType::punctuation: //fall thru...
				options = !in_array<char>(thePunct, theToken.data[0]);
				break;

			default:
				options = false;
				theResult.error = syntaxError;
			} //switch
			tokenizer.next(); //skip ahead...
		} //while
		return theResult;
	}

	//USE : parse an individual attribute (name type [options])
	StatusResult ParseHelper::parseAttribute(Attribute& anAttribute) {
		StatusResult theResult{ noError };

		if (tokenizer.more()) {
			Token& theToken = tokenizer.current();
			if (Helpers::isDatatype(theToken.keyword)) {
				DataTypes theType = Helpers::getTypeForKeyword(theToken.keyword);
				anAttribute.setDataType(theType);
				tokenizer.next();

				if (DataTypes::varchar_type == theType) {
					if ((tokenizer.skipIf(left_paren))) {
						theToken = tokenizer.current();
						tokenizer.next();
						if ((tokenizer.skipIf(right_paren))) {
							anAttribute.setSize(atoi(theToken.data.c_str()));
							// return theResult;
						}
					}
				}

				if (theResult) {
					theResult = parseAttributeOptions(anAttribute);
					if (!anAttribute.isValid()) {
						theResult.error = invalidAttribute;
					}
				}

			} //if
			else theResult.error = invalidAttribute;
		} //if
		return theResult;
	}

	StatusResult  ParseHelper::parseQuote(std::string& aFieldName, std::string& aQuote) {
		if (tokenizer.next()) {
			while (tokenizer.more() && aQuote != tokenizer.current().data) {
				aFieldName += tokenizer.current().data;
				tokenizer.next();
			}
			tokenizer.next();
			return StatusResult{ noError };
		}
		return StatusResult{ invalidCommand };
	}

	StatusResult ParseHelper::parseSetAttribute(AttributeValueMap& anAttrValMap, StringList& anAttrNameList) {
		StatusResult theResult{ noError };
		bool hasComma = true;

		while (theResult && tokenizer.more() && hasComma) {
			std::string theAttrName;
			std::string theVal;
			Token& theToken = tokenizer.current();
			// start parsing "attr=val"
			if (tokenizer.remaining() > 2 && TokenType::identifier == tokenizer.current().type) {
				theAttrName = tokenizer.current().data;
				anAttrNameList.push_back(tokenizer.current().data);
				tokenizer.next();
				if (tokenizer.skipIf('=')) {
					if ("'" == tokenizer.current().data) {
						theResult = parseQuote(theVal, tokenizer.current().data);
					}
					else if ("\"" == tokenizer.current().data) {
						theResult = parseQuote(theVal, tokenizer.current().data);
					}
					else if (TokenType::identifier == tokenizer.current().type || TokenType::number == tokenizer.current().type
						|| "true" == tokenizer.current().data || "false" == tokenizer.current().data) {
						theVal += tokenizer.current().data;
						tokenizer.next();
					}
				}
				else {
					theResult.error = syntaxError;
				}
				if (!tokenizer.skipIf(comma)) {
					hasComma = false;
				}
				anAttrValMap[theAttrName] = theVal;
			}
			else theResult.error = syntaxError;
		}
		return theResult;
	}

	//USE: parse a comma-sep list of (unvalidated) identifiers;
	//     AUTO stop if keyword (or term)
	StatusResult ParseHelper::parseIdentifierList(StringList& aList) {
		StatusResult theResult{ noError };

		while (theResult && tokenizer.more()) {
			Token& theToken = tokenizer.current();
			if (TokenType::identifier == tokenizer.current().type) {
				aList.push_back(theToken.data);
				tokenizer.next(); //skip identifier...
				tokenizer.skipIf(comma);
			}
			else if (theToken.type == TokenType::keyword) {
				break; //Auto stop if we see a keyword...
			}
			else if (tokenizer.skipIf(right_paren)) {
				break;
			}
			else if (semicolon == theToken.data[0]) {
				break;
			}
			else theResult.error = syntaxError;
		}
		return theResult;
	}

	//** USE: get a list of values (identifiers, strings, numbers...)
	StatusResult ParseHelper::parseValueList(StringList& aList) {
		StatusResult theResult{ noError };

		while (theResult && tokenizer.more()) {
			Token& theToken = tokenizer.current();
			if (TokenType::identifier == theToken.type || TokenType::number == theToken.type) {
				aList.push_back(theToken.data);
				tokenizer.next(); //skip identifier...
				tokenizer.skipIf(comma);
			}
			else if (tokenizer.skipIf(right_paren)) {
				break;
			}
			else theResult.error = syntaxError;
		}
		return theResult;
	}
	/*
	//read a comma-sep list of key/value pairs...
	StatusResult ParseHelper::parseKeyValues(KeyValues &aList, Entity &anEntity) {
	  StatusResult theResult{noError};
	  while(theResult && tokenizer.more()) {
		TokenSequencer thePhrase(tokenizer);
		std::string theName;
		int theValue{0};
		if(thePhrase.hasId(theName).hasOp(Operators::equal_op).hasNumber(theValue)) {
		  tokenizer.next(3);
			//Add code here to finish this...
		}
		theResult.error=syntaxError;
	  }
	  return theResult;
	}
	*/
}
