//
//  Helpers.hpp
//  Database3
//
//  Created by rick gessner on 4/14/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Helpers_h
#define Helpers_h

#include "BasicTypes.hpp"
#include "keywords.hpp"
#include "Tokenizer.hpp"
#include <algorithm>

//#include "FormatString.hpp"

namespace ECE141 {

	template<typename T, size_t aSize>
	bool in_array(T(&anArray)[aSize], const T& aValue) {
		for (size_t i = 0; i < aSize; i++) {
			if (anArray[i] == aValue) return true;
		}
		return false;
	}


	static std::map<std::string, Operators> gExpressionOps = {
	  {"=",   Operators::equal_op},
	  {"<",   Operators::lt_op},
	  {"<=",  Operators::lte_op},
	  {">",   Operators::gt_op},
	  {">=",  Operators::gte_op},
	  {"!=",  Operators::notequal_op},
	  {"not", Operators::notequal_op},
	};

	static std::map<std::string, Operators> gOperators = {
	  std::make_pair(".",   Operators::dot_op),
	  std::make_pair("+",   Operators::add_op),
	  std::make_pair("-",   Operators::subtract_op),
	  std::make_pair("*",   Operators::multiply_op),
	  std::make_pair("/",   Operators::divide_op),
	  std::make_pair("^",   Operators::power_op),
	  std::make_pair("%",   Operators::mod_op),
	  std::make_pair("=",   Operators::equal_op),
	  std::make_pair("!=",  Operators::notequal_op),
	  std::make_pair("not", Operators::notequal_op),
	  std::make_pair("<",   Operators::lt_op),
	  std::make_pair("<=",  Operators::lte_op),
	  std::make_pair(">",   Operators::gt_op),
	  std::make_pair(">=",  Operators::gte_op),
	  std::make_pair("and", Operators::and_op),
	  std::make_pair("or",  Operators::or_op),
	  std::make_pair("nor", Operators::nor_op),
	  std::make_pair("between", Operators::between_op),
	};

	static std::map<std::string, Logical> gLogicals = {
	  std::make_pair("and",   Logical::and_op),
	  std::make_pair("or",    Logical::or_op),
	  std::make_pair("not",   Logical::not_op),	  
	};

	//a list of known functions...
	static std::map<std::string, int> gFunctions = {
	  std::make_pair("avg", 10),
	  std::make_pair("count", 20),
	  std::make_pair("max", 30),
	  std::make_pair("min", 40)
	};

	static ECE141::Keywords gJoinTypes[] = {
		ECE141::Keywords::cross_kw,ECE141::Keywords::full_kw, ECE141::Keywords::inner_kw,
		ECE141::Keywords::left_kw, ECE141::Keywords::right_kw
	};


	//This map binds a keyword string with a Keyword (token)...
	static std::map<std::string, Keywords> gDictionary = {
	  std::make_pair("add",       Keywords::add_kw),
	  std::make_pair("all",       Keywords::all_kw),
	  std::make_pair("alter",     Keywords::alter_kw),
	  std::make_pair("and",       Keywords::and_kw),
	  std::make_pair("as",        Keywords::as_kw),
	  std::make_pair("asc",       Keywords::asc_kw),
	  std::make_pair("avg",       ECE141::Keywords::avg_kw),
	  std::make_pair("auto_increment", Keywords::auto_increment_kw),
	  std::make_pair("between",   ECE141::Keywords::between_kw),
	  std::make_pair("boolean",   ECE141::Keywords::boolean_kw),
	  std::make_pair("by",        ECE141::Keywords::by_kw),
	  std::make_pair("change",    ECE141::Keywords::change_kw),
	  std::make_pair("changed",    ECE141::Keywords::changed_kw),
	  std::make_pair("char",      ECE141::Keywords::char_kw),
	  std::make_pair("column",    ECE141::Keywords::column_kw),
	  std::make_pair("count",     ECE141::Keywords::count_kw),
	  std::make_pair("create",    ECE141::Keywords::create_kw),
	  std::make_pair("cross",     ECE141::Keywords::cross_kw),
	  std::make_pair("current_date",  Keywords::current_date_kw),
	  std::make_pair("current_time",  Keywords::current_time_kw),
	  std::make_pair("current_timestamp", Keywords::current_timestamp_kw),
	  std::make_pair("database",  ECE141::Keywords::database_kw),
	  std::make_pair("databases", ECE141::Keywords::databases_kw),
	  std::make_pair("datetime",  ECE141::Keywords::datetime_kw),
	  std::make_pair("decimal",   ECE141::Keywords::decimal_kw),
	  std::make_pair("desc",      ECE141::Keywords::desc_kw),
	  std::make_pair("delete",    ECE141::Keywords::delete_kw),
	  std::make_pair("describe",  ECE141::Keywords::describe_kw),
	  std::make_pair("distinct",  ECE141::Keywords::distinct_kw),
	  std::make_pair("double",    ECE141::Keywords::double_kw),
	  std::make_pair("drop",      ECE141::Keywords::drop_kw),
	  std::make_pair("dump",      ECE141::Keywords::dump_kw),
	  std::make_pair("enum",      ECE141::Keywords::enum_kw),
	  std::make_pair("explain",   ECE141::Keywords::explain_kw),
	  std::make_pair("false",     ECE141::Keywords::false_kw),
	  std::make_pair("float",     ECE141::Keywords::float_kw),
	  std::make_pair("foreign",   ECE141::Keywords::foreign_kw),
	  std::make_pair("from",      ECE141::Keywords::from_kw),
	  std::make_pair("full",      ECE141::Keywords::full_kw),
	  std::make_pair("group",     ECE141::Keywords::group_kw),
	  std::make_pair("help",      ECE141::Keywords::help_kw),
	  std::make_pair("in",        ECE141::Keywords::in_kw),
	  std::make_pair("index",     ECE141::Keywords::index_kw),
	  std::make_pair("indexes",   ECE141::Keywords::indexes_kw),
	  std::make_pair("inner",     ECE141::Keywords::inner_kw),
	  std::make_pair("insert",    ECE141::Keywords::insert_kw),
	  std::make_pair("int",       ECE141::Keywords::integer_kw),
	  std::make_pair("integer",   ECE141::Keywords::integer_kw),
	  std::make_pair("into",      ECE141::Keywords::into_kw),
	  std::make_pair("join",      ECE141::Keywords::join_kw),
	  std::make_pair("key",       ECE141::Keywords::key_kw),
	  std::make_pair("last",      ECE141::Keywords::last_kw),
	  std::make_pair("left",      ECE141::Keywords::left_kw),
	  std::make_pair("like",      ECE141::Keywords::like_kw),
	  std::make_pair("limit",     ECE141::Keywords::limit_kw),
	  std::make_pair("max",       ECE141::Keywords::max_kw),
	  std::make_pair("min",       ECE141::Keywords::min_kw),
	  std::make_pair("modify",    ECE141::Keywords::modify_kw),
	  std::make_pair("not",       ECE141::Keywords::not_kw),
	  std::make_pair("null",      ECE141::Keywords::null_kw),
	  std::make_pair("on",        ECE141::Keywords::on_kw),
	  std::make_pair("or",        ECE141::Keywords::or_kw),
	  std::make_pair("order",     ECE141::Keywords::order_kw),
	  std::make_pair("outer",     ECE141::Keywords::outer_kw),
	  std::make_pair("primary",   ECE141::Keywords::primary_kw),
	  std::make_pair("query",     ECE141::Keywords::query_kw),
	  std::make_pair("quit",      ECE141::Keywords::quit_kw),
	  std::make_pair("references", ECE141::Keywords::references_kw),
	  std::make_pair("right",     ECE141::Keywords::right_kw),
	  std::make_pair("rows",      ECE141::Keywords::rows_kw),
	  std::make_pair("select",    ECE141::Keywords::select_kw),
	  std::make_pair("self",      ECE141::Keywords::self_kw),
	  std::make_pair("set",       ECE141::Keywords::set_kw),
	  std::make_pair("show",      ECE141::Keywords::show_kw),
	  std::make_pair("sum",       ECE141::Keywords::sum_kw),
	  std::make_pair("table",     ECE141::Keywords::table_kw),
	  std::make_pair("tables",    ECE141::Keywords::tables_kw),
	  std::make_pair("true",      ECE141::Keywords::true_kw),
	  std::make_pair("unique",    ECE141::Keywords::unique_kw),
	  std::make_pair("update",    ECE141::Keywords::update_kw),
	  std::make_pair("use",       ECE141::Keywords::use_kw),
	  std::make_pair("values",    ECE141::Keywords::values_kw),
	  std::make_pair("varchar",   ECE141::Keywords::varchar_kw),
	  std::make_pair("version",   ECE141::Keywords::version_kw),
	  std::make_pair("where",     ECE141::Keywords::where_kw),
	  std::make_pair(";",         ECE141::Keywords::separator_kw),
	  std::make_pair("default",   ECE141::Keywords::default_kw),
	  std::make_pair("timestamp", ECE141::Keywords::timestamp_kw)
	};

	static std::map<Keywords, DataTypes> gKeywordTypes = {
	  std::make_pair(Keywords::boolean_kw, DataTypes::bool_type),
	  std::make_pair(Keywords::datetime_kw, DataTypes::datetime_type),
	  std::make_pair(Keywords::float_kw, DataTypes::float_type),
	  std::make_pair(Keywords::integer_kw, DataTypes::int_type),
	  std::make_pair(Keywords::varchar_kw, DataTypes::varchar_type),
	  std::make_pair(Keywords::timestamp_kw, DataTypes::timestamp_type)
	};

	class Helpers {
	public:

		static uint32_t hashString(const char* str) {
			const int gMultiplier = 37;
			uint32_t h{ 0 };
			unsigned char* p;
			for (p = (unsigned char*)str; *p != '\0'; p++)
				h = gMultiplier * h + *p;
			return h;
		}

		static Keywords getKeywordId(const std::string aKeyword) {
			auto theIter = gDictionary.find(aKeyword);
			if (theIter != gDictionary.end()) {
				return theIter->second;
			}
			return Keywords::unknown_kw;
		}

		//convert from char to keyword...
		static Keywords charToKeyword(char aChar) {
			switch (toupper(aChar)) {
			case 'I': return Keywords::integer_kw;
			case 'D': return Keywords::datetime_kw;
			case 'B': return Keywords::boolean_kw;
			case 'F': return Keywords::float_kw;
			case 'V': return Keywords::varchar_kw;
			case 'T': return Keywords::timestamp_kw;
			default:  return Keywords::unknown_kw;
			}
		}

		static const char* dataTypeToString(DataTypes aType) {
			switch (aType) {
			case DataTypes::no_type:        return "none";
			case DataTypes::bool_type:      return "bool";
			case DataTypes::datetime_type:  return "datetime";
			case DataTypes::float_type:     return "float";
			case DataTypes::int_type:       return "int";
			case DataTypes::varchar_type:   return "varchar";
			case DataTypes::char_type:      return "char";
			case DataTypes::timestamp_type: return "timestamp";
			default:						return "none";
			}
		}

		static DataTypes stringToDataType(std::string aString) {
			if ("none" == aString) {
				return DataTypes::no_type;
			}
			else if ("bool" == aString) {
				return DataTypes::bool_type;
			}
			else if ("datetime" == aString) {
				return DataTypes::datetime_type;
			}
			else if ("float" == aString) {
				return DataTypes::float_type;
			}
			else if ("int" == aString) {
				return DataTypes::int_type;
			}
			else if ("varchar" == aString) {
				return DataTypes::varchar_type;
			}
			else if ("char" == aString) {
				return DataTypes::char_type;
			}
			else if ("timestamp" == aString) {
				return DataTypes::timestamp_type;
			}
			else {
				return DataTypes::no_type;
			}
		}

		static const char* keywordToString(Keywords aType) {
			switch (aType) {
			case Keywords::boolean_kw:    return "bool";
			case Keywords::create_kw:     return "create";
			case Keywords::database_kw:   return "database";
			case Keywords::databases_kw:  return "databases";
			case Keywords::datetime_kw:   return "datetime";
			case Keywords::describe_kw:   return "describe";
			case Keywords::drop_kw:       return "drop";
			case Keywords::float_kw:      return "float";
			case Keywords::integer_kw:    return "integer";
			case Keywords::show_kw:       return "show";
			case Keywords::table_kw:      return "table";
			case Keywords::tables_kw:     return "tables";
			case Keywords::use_kw:        return "use";
			case Keywords::varchar_kw:    return "varchar";
			case Keywords::timestamp_kw:  return "timestamp";
			default:                      return "unknown";
			}
		}

		static const DataTypes keywordToDatatype(Keywords aType) {
			switch (aType) {
			case Keywords::integer_kw:		return DataTypes::int_type;
			case Keywords::float_kw:		return DataTypes::float_type;
			case Keywords::boolean_kw:		return DataTypes::bool_type;
			case Keywords::datetime_kw:		return DataTypes::datetime_type;
			case Keywords::varchar_kw:		return DataTypes::varchar_type;
			case Keywords::char_kw:			return DataTypes::char_type;
			case Keywords::timestamp_kw:    return DataTypes::timestamp_type;
			default:						return DataTypes::no_type;
			}
		}		

		// USE: ---validate that given keyword is a datatype...
		static bool isDatatype(Keywords aKeyword) {
			switch (aKeyword) {
			case Keywords::char_kw:
			case Keywords::datetime_kw:
			case Keywords::float_kw:
			case Keywords::integer_kw:
			case Keywords::varchar_kw:
			case Keywords::boolean_kw:
			case Keywords::timestamp_kw:
				return true;
			default: return false;
			}
		}

		static DataTypes getTypeForKeyword(Keywords aKeyword) {
			return gKeywordTypes.count(aKeyword)
				? gKeywordTypes[aKeyword] : DataTypes::no_type;
		}

		static Operators toOperator(std::string aString) {
			auto theIter = gOperators.find(aString);
			if (theIter != gOperators.end()) {
				return theIter->second;
			}
			return Operators::unknown_op;
		}

		static Logical toLogical(std::string aString) {
			auto theIter = gLogicals.find(aString);
			if (theIter != gLogicals.end()) {
				return theIter->second;
			}
			return Logical::no_op;
		}

		static int getFunctionId(const std::string anIdentifier) {
			auto theIter = gFunctions.find(anIdentifier);
			if (theIter != gFunctions.end()) {
				return theIter->second;
			}
			return 0;
		}

		static bool isNumericKeyword(Keywords aKeyword) {
			static Keywords theTypes[] = { Keywords::decimal_kw, Keywords::double_kw, Keywords::float_kw, Keywords::integer_kw };
			for (auto k : theTypes) {
				if (aKeyword == k) return true;
			}
			return false;
		}

		/*
		static std::string QueryOk(size_t aCount, size_t anElapsed) {
		  return formatString("Query Ok, {0} rows affected ({1} secs)\n",
							  aCount, anElapsed);
		}
		*/

		static StatusResult helpRecog(Tokenizer& aTokenizer, std::vector<Keywords> akwList) {
			uint32_t theOffset = 0;
			for (Keywords kw : akwList) {
				try {
					if (kw != aTokenizer.peek(theOffset++).keyword) {
						return StatusResult{ invalidCommand };
					}
				}													
				catch (...) {
					return StatusResult{ invalidCommand };
				}								
			}			
			return StatusResult{ noError };
		}

		static StatusResult helpParse(std::string& aName, Tokenizer& aTokenizer, 
														std::vector<Keywords> akwList, bool hasName) {
			for (Keywords kw : akwList) {
				if (kw != aTokenizer.current().keyword) {
					return StatusResult{ invalidCommand };
				}
				if (!aTokenizer.next()) {
					return StatusResult{ invalidCommand };
				}
			}			
			if (hasName) {
				if (TokenType::identifier != aTokenizer.current().type) {
					return StatusResult{ invalidCommand };
				}
				aName = aTokenizer.current().data;
				if (!aTokenizer.next()) { return StatusResult{ invalidCommand }; }
			}			
			if (";" != aTokenizer.current().data) { return StatusResult{ invalidCommand }; }
			return StatusResult{ noError };
		}

	};

}

#endif /* Helpers_h */
