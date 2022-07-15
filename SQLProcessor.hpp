//
//  SQLProcessor.hpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "SQLStatement.hpp"
#include "Tokenizer.hpp"
#include "Application.hpp"

namespace ECE141 {

	class Statement;
	class DBProcessor; //define this later...
	class Entity;
	class Database;

	//Should this processor have it's own Statement types?

	class SQLProcessor : public CmdProcessor {
	public:

		SQLProcessor(Database* aDatabase ,std::ostream& anOutput);
		virtual ~SQLProcessor();

		CmdProcessor* recognizes(Tokenizer& aTokenizer) override;
		Statement* makeStatement(Tokenizer& aTokenizer, StatusResult& aResult) override;
	
		StatusResult  run(Statement* aStmt) override;
		void setActiveDB(Database* aDB);

		

	protected:
		//do you need other data members?
		Database* activeDB;

	};

}
#endif /* SQLProcessor_hpp */
