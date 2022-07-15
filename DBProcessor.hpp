//
//  DBProcessor.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef DBProcessor_hpp
#define DBProcessor_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"
#include "Database.hpp"
#include "Config.hpp"
#include "DBStatement.hpp"
#include "SQLProcessor.hpp"
#include <filesystem>



namespace ECE141 {
	
	//the ways to open files
	//const auto CreateNew = std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary;
	//const auto OpenExisiting = std::ios::in | std::ios::out | std::ios::binary;

	class DBStatement;
	class Application;
	class SQLProcessor;

	class DBProcessor : public CmdProcessor {
	public:

		DBProcessor(Application* anApp, std::ostream& anOutput);
		~DBProcessor();

		//cmd processor interface...
		CmdProcessor* recognizes(Tokenizer& aTokenizer) override;
		Statement*    makeStatement(Tokenizer& aTokenizer, StatusResult& aResult) override;
		StatusResult  run(Statement* aStmt) override;

		// bool isKnown(Keywords aKeyword);

		  

	protected:
		//stuff?	
		Application*					app;
		std::unique_ptr<SQLProcessor>	nextLevelSQL;
	};

}
#endif /* DBProcessor_hpp */
