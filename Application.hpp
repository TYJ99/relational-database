//
//  AppProcessor.hpp
//  Database1
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "DBProcessor.hpp"
#include "SQLProcessor.hpp"
#include "Database.hpp"
#include "Config.hpp" 
#include "LRUCache.hpp"


namespace ECE141 {
	const auto CreateNew = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;
	const auto OpenExisiting = std::ios::in | std::ios::out | std::ios::binary;
	
	inline size_t Config::cacheSize[] = { 0, 0, 0 };

	class DBProcessor;
	class Database;	

	class Application : public CmdProcessor {
	public:
		
		Application(std::ostream& anOutput);
		virtual ~Application();

		//app api...
		std::string   getVersion() { return "1.0"; }

		virtual StatusResult  handleInput(std::istream& anInput);
		CmdProcessor* recognizes(Tokenizer& aTokenizer) override;
		Statement* makeStatement(Tokenizer& aTokenizer,
			StatusResult& aResult) override;
		StatusResult  run(Statement* aStmt) override;
		
		//dbProcessor interface...
		StatusResult  createDatabase(const std::string& aName);
		StatusResult  dumpDatabase(const std::string& aName);
		StatusResult  dropDatabase(const std::string& aName);
		StatusResult  showDatabases() const;
		StatusResult  useDatabase(const std::string& aName);

		bool          dbExists(const std::string& aDBName);
		Database*	  getDatabaseInUse();

	protected:
		std::unique_ptr<DBProcessor>    nextLevelDB;
		std::unique_ptr<Database>		databaseInUse;
		std::fstream					dbFile; //File Stream for creating new DB files
	};

}

#endif /* Application_hpp */
