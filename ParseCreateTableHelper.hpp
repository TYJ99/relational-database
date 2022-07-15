
#ifndef ParseCreateTableHelper_hpp
#define ParseCreateTableHelper_hpp

#include <iostream>
#include "Tokenizer.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include "Helpers.hpp"
#include "Errors.hpp"
#include "Config.hpp"


namespace ECE141{
	class ParseCreateTableHelper {
	public:
		ParseCreateTableHelper(Tokenizer& aTokenizer) :tokenizer(aTokenizer) {}
		StatusResult parseAttribute(Entity& anEntity);
		StatusResult parseProperty(Attribute& anAttr, Entity& anEntity);

	protected:
		Tokenizer& tokenizer;
	};
}












#endif
