
#include "DBStatement.hpp"
#include "Tokenizer.hpp"
#include <vector>

namespace ECE141 {
	
	// CreateDBStatement
	StatusResult CreateDBStatement::recognize(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::create_kw, Keywords::database_kw };
		return Helpers::helpRecog(aTokenizer, theKwList);
	}

	StatusResult CreateDBStatement::parse(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::create_kw, Keywords::database_kw };
		return Helpers::helpParse(name, aTokenizer, theKwList, true);
	}

	StatusResult CreateDBStatement::run(std::ostream& aStream) const {
		return app->createDatabase(name);
	}

	// DropDBStatement
	StatusResult DropDBStatement::recognize(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::drop_kw, Keywords::database_kw };
		return Helpers::helpRecog(aTokenizer, theKwList);
	}

	StatusResult DropDBStatement::parse(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::drop_kw, Keywords::database_kw };
		return Helpers::helpParse(name, aTokenizer, theKwList, true);
	}

	StatusResult DropDBStatement::run(std::ostream& aStream) const {
		return app->dropDatabase(name);
	}

	// ShowDBStatement
	StatusResult ShowDBStatement::recognize(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::show_kw, Keywords::databases_kw };
		return Helpers::helpRecog(aTokenizer, theKwList);		
	}

	StatusResult ShowDBStatement::parse(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::show_kw, Keywords::databases_kw };
		return Helpers::helpParse(name, aTokenizer, theKwList, false);
	}

	StatusResult ShowDBStatement::run(std::ostream& aStream) const {
		return app->showDatabases();
	}

	// UseDBStatement
	StatusResult UseDBStatement::recognize(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::use_kw};
		return Helpers::helpRecog(aTokenizer, theKwList);
	}

	StatusResult UseDBStatement::parse(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::use_kw };
		return Helpers::helpParse(name, aTokenizer, theKwList, true);
	}

	StatusResult UseDBStatement::run(std::ostream& aStream) const {
		return app->useDatabase(name);
	}

	// DumpDBStatement
	StatusResult DumpDBStatement::recognize(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::dump_kw, Keywords::database_kw };
		return Helpers::helpRecog(aTokenizer, theKwList);
	}

	StatusResult DumpDBStatement::parse(Tokenizer& aTokenizer) {
		std::vector<Keywords> theKwList{ Keywords::dump_kw, Keywords::database_kw };
		return Helpers::helpParse(name, aTokenizer, theKwList, true);
	}

	StatusResult DumpDBStatement::run(std::ostream& aStream) const {
		return app->dumpDatabase(name);
	}

}