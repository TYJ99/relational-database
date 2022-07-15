//
//  DBProcessor.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include "DBProcessor.hpp"

#include <map>
#include <fstream>
#include "Config.hpp"
#include "DBProcessor.hpp"
#include "FolderView.hpp"
#include "FolderReader.hpp"
#include "Database.hpp"
#include "Application.hpp"
#include "Helpers.hpp"
#include "BlockIO.hpp"

namespace ECE141 {

	
	// function pointer(like a typedef!!!)
	using StmtFactory = DBStatement * (*)(Application *anApp, Tokenizer& aTokenize);

	DBProcessor::DBProcessor(Application* anApp, std::ostream& anOutput)
		: CmdProcessor(anOutput), app(anApp) {
		nextLevelSQL = std::make_unique<SQLProcessor>(app->getDatabaseInUse(), output);
	}

	DBProcessor::~DBProcessor() {
	}

	// to check if the first word of command is valid.
	bool isKnownDB(Keywords aKeyword) {
		static Keywords theKnown[] =
		{ Keywords::create_kw, Keywords::drop_kw, Keywords::use_kw, Keywords::show_kw, Keywords::dump_kw };
		auto theIt = std::find(std::begin(theKnown),
			std::end(theKnown), aKeyword);
		return theIt != std::end(theKnown);
	}

	// to check if the first word of command is valid.
	bool isKnownSQL(Keywords aKeyword) {
		static Keywords theKnown[] =
		{ Keywords::create_kw, Keywords::drop_kw, Keywords::use_kw, Keywords::show_kw, Keywords::dump_kw };
		auto theIt = std::find(std::begin(theKnown),
			std::end(theKnown), aKeyword);
		return theIt != std::end(theKnown);
	}

	//CmdProcessor interface ... 04/28/2022
	// recognize the current token
	CmdProcessor* DBProcessor::recognizes(Tokenizer& aTokenizer) {
		//auto currentKW = aTokenizer.current().keyword;
		nextLevelSQL->setActiveDB(app->getDatabaseInUse());
		return CreateDBStatement::recognize(aTokenizer) ||
			DropDBStatement::recognize(aTokenizer) ||
			ShowDBStatement::recognize(aTokenizer) ||
			UseDBStatement::recognize(aTokenizer) ||
			DumpDBStatement::recognize(aTokenizer) ? this : nextLevelSQL->recognizes(aTokenizer);
	}


	// run
	StatusResult DBProcessor::run(Statement* aStmt) {
		return aStmt->run(output);		
	}

	/*========================================================================================
									DBStatement Factory
	========================================================================================*/

	DBStatement* createDBstatement(Application* anApp, Tokenizer& aTokenizer) {
		return new CreateDBStatement(anApp, aTokenizer);
	}
	DBStatement* dropDBstatement(Application* anApp, Tokenizer& aTokenizer) {
		return new DropDBStatement(anApp, aTokenizer);
	}
	DBStatement* showDBstatement(Application* anApp, Tokenizer& aTokenizer) {
		return new ShowDBStatement(anApp, aTokenizer);
	}
	DBStatement* useDBstatement(Application* anApp, Tokenizer& aTokenizer) {
		return new UseDBStatement(anApp, aTokenizer);
	}
	DBStatement* dumpDBstatement(Application* anApp, Tokenizer& aTokenizer) {
		return new DumpDBStatement(anApp, aTokenizer);
	}

	// USE: retrieve a statement based on given text input...
	Statement* DBProcessor::makeStatement(Tokenizer& aTokenizer, StatusResult& aResult) {
		Token theToken = aTokenizer.current();
		std::map<Keywords, StmtFactory> stmtMap = {
			{Keywords::create_kw, createDBstatement},
			{Keywords::drop_kw,	  dropDBstatement},
			{Keywords::show_kw,	  showDBstatement },
			{Keywords::use_kw,	  useDBstatement},
			{Keywords::dump_kw,	  dumpDBstatement}
		};
		if (stmtMap.count(theToken.keyword)) {
			auto theStmt = stmtMap[theToken.keyword](app, aTokenizer);			
			if (theStmt->parse(aTokenizer)) {
				return theStmt;
			}
		}
		aResult = invalidCommand;
		return nullptr; //means we don't know the statement...
	}

}
