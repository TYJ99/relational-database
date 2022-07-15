//
//  CommandProcessor.cpp
//
//  Created by rick gessner on 3/30/20
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "Application.hpp"
#include "Tokenizer.hpp"
#include <memory>
#include <algorithm>
#include <vector>
#include "Config.hpp"
#include "FolderView.hpp"

namespace ECE141 {

	Application::Application(std::ostream& anOutput)
		: CmdProcessor(anOutput) {
		nextLevelDB = std::make_unique<DBProcessor>(this, anOutput);	
	}

	Application::~Application() {}

	// USE: -----------------------------------------------------

	bool isKnown(Keywords aKeyword) {
		static Keywords theKnown[] =
		{ Keywords::quit_kw,Keywords::version_kw, Keywords::help_kw };
		auto theIt = std::find(std::begin(theKnown),
			std::end(theKnown), aKeyword);
		return theIt != std::end(theKnown);
	}

	CmdProcessor* Application::recognizes(Tokenizer& aTokenizer) {
		if (isKnown(aTokenizer.current().keyword)) {
			return this;
		}
		return nextLevelDB->recognizes(aTokenizer);
	}

	StatusResult Application::run(Statement* aStatement) {
		switch (aStatement->getType()) {
		case Keywords::quit_kw:
			output << "DB::141 is shutting down\n";
			return StatusResult(ECE141::userTerminated);
			break;
		case Keywords::version_kw:
			output << "Version " << getVersion() << "\n"; break;
		case Keywords::help_kw:
			output << "Help system available\n"; break;
		default: break;
		}
		return StatusResult{ Errors::noError };
	}

	// USE: retrieve a statement based on given text input...
	Statement* Application::makeStatement(Tokenizer& aTokenizer,
		StatusResult& aResult) {
		Token theToken = aTokenizer.current();
		if (isKnown(theToken.keyword)) {
			aTokenizer.next(); //skip ahead...
			return new Statement(theToken.keyword);
		}
		return nullptr;
	}

	//build a tokenizer, tokenize input, ask processors to handle...
	StatusResult Application::handleInput(std::istream& anInput) {
		Tokenizer theTokenizer(anInput);
		StatusResult theResult = theTokenizer.tokenize();

		while (theResult && theTokenizer.more()) {
			Config::getTimer().reset();
			if (auto* theProc = recognizes(theTokenizer)) {
				if (auto* theCmd = theProc->makeStatement(theTokenizer, theResult)) {
					theResult = theProc->run(theCmd);
					if (theResult) {
						theTokenizer.skipIf(';');
					}
					delete theCmd;
				}
				
			}
			else theResult = unknownCommand;
		}
		
		return theResult;
	}

	//==========================================================================

	Database* Application::getDatabaseInUse() {
		return databaseInUse.get();
	}
	

	// Check if database exists
	bool Application::dbExists(const std::string& aDBName) {
		std::filesystem::path thePath = Config::getDBPath(aDBName);
		return std::filesystem::exists(thePath);
	}

	// Handle CREATE Command
	StatusResult Application::createDatabase(const std::string& aName) {
		Timer theTimer = Config::getTimer();
		if (dbExists(aName)) { 
			output << "Query Failed, 0 row affected (" << theTimer.elapsed() << " secs)\n";
			return StatusResult{ databaseExists }; 
		}
		dbFile.clear();
		dbFile.open(Config::getDBPath(aName).c_str(), CreateNew);
		if (!dbFile) {
			return StatusResult{ databaseCreationError };
		}
		Block theBlock(BlockType::meta_block);
		Block theIndexBlock(BlockType::index_block);
		BlockIO theBlockIO(dbFile);		
		theBlockIO.writeBlock(0, theBlock);
		theBlockIO.writeBlock(1, theIndexBlock);
		dbFile.close();
		output << "Query OK, 1 row affected (" << theTimer.elapsed() << " secs)\n";
		return StatusResult{ noError };
	}

	// Handle CREATE Command
	StatusResult Application::showDatabases() const {
		//make a view, load with DB names from storage folder...
		FolderView(Config::getStoragePath().c_str()).show(output);
		// debugging
		/*
		if (databaseInUse) {
			std::cout << "database: " << databaseInUse->getName() << "\n";
		}
		*/
		return StatusResult{ noError };
	}

	// USE: call this to perform the dropping of a database (remove the file)...
	StatusResult Application::dropDatabase(const std::string& aName) {
		Timer theTimer = Config::getTimer();
		// Make db file path into c_str so remove function can be used (doesn't accept std::string argument)
		if (dbExists(aName)) {			
			if (databaseInUse && databaseInUse->getName() == aName) {
				databaseInUse.reset();
			}
			std::remove((Config::getDBPath(aName)).c_str());
			output << "Query OK, 0 row affected (" << theTimer.elapsed() << " secs)\n";
			return StatusResult{ noError };
		}
		output << "Query failed, 0 row affected (" << theTimer.elapsed() << " secs)\n";
		return StatusResult{ unknownDatabase };
	}

	// USE: DB dump all storage blocks
	StatusResult Application::dumpDatabase(const std::string& aName) {
		Timer theTimer = Config::getTimer();
		if (dbExists(aName)) {
			dbFile.open(Config::getDBPath(aName), OpenExisiting);

		}
		BlockIO theBlockIO(dbFile);
		uint32_t theNumOfBlock = theBlockIO.getBlockCount();
		//std::cout << "blockCount: " << theNumOfBlock << "\n";
		std::vector<std::string> columns{ "Type" };
		columns.push_back("BlockNum");
		std::vector<size_t> theViewSize{ 10, 20 };
		std::map<char, size_t> theMapForViewSize;
		//for (size_t i = 0; i < theMapForViewSize.size(); ++i) {
		//	theMapForViewSize[] = theViewSize[theCount++];
		//}

		//columns.push_back("Extra");

		// Initialize the view with column
		output << "+";
		// build borders
		for (uint32_t j = 0; j < theViewSize.size(); ++j) {
			output << std::setfill('-') << std::setw(theViewSize[j]) << "+";
		}
		output << "\n";


		// build rows
		for (uint32_t j = 0; j < columns.size(); ++j) {
			output << "| " << std::setfill(' ') << std::left << std::setw(theViewSize[j]) << columns[j] << std::setfill(' ');
		}
		output << "\n";

		output << "+";
		// build borders
		for (uint32_t j = 0; j < theViewSize.size(); ++j) {
			output << std::setfill('-') << std::setw(theViewSize[j]) << "+";
		}
		output << "\n";

		size_t theBlockNum = 0;
		char theType = ' ';
		for (uint32_t i = 0; i < 10; ++i) {
			if (1 == i % 2) {
				output << "+";
				// build borders
				for (uint32_t j = 0; j < theViewSize.size(); ++j) {
					output << std::setfill('-') << std::setw(theViewSize[j]) << "+";
				}
				output << "\n";
			}
			else {
				// build rows
				output << "| " << std::setfill(' ') << std::left << std::setw(theViewSize[0]) << theType << std::setfill(' ');
				output << "| " << std::setfill(' ') << std::left << std::setw(theViewSize[1]) << theBlockNum << std::setfill(' ');

				output << "\n";
			}
		}
		output << theNumOfBlock << " rows in set(" << theTimer.elapsed() << " sec)\n";
		return StatusResult{ ECE141::noError };
	}

	// USE: call DB object to be loaded into memory...
	StatusResult Application::useDatabase(const std::string& aName) {
		if (dbExists(aName)) {
			databaseInUse.reset(new Database(aName, output));
			
			//dbFile.open(Config::getDBPath(aName), OpenExisiting);
			output << "Database changed\n";
			return StatusResult{ ECE141::noError };
		}
		return StatusResult{ ECE141::unknownDatabase };
	}




}
