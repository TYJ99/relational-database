//
//  SQLProcessor.cpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#include "SQLProcessor.hpp"
#include "SelectTableStatement.hpp"
#include "UpdateStatement.hpp"
#include "DeleteStatement.hpp"

namespace ECE141 {

	using StmtFactory = Statement * (*)(Database*, Tokenizer&);
	SQLProcessor::SQLProcessor(Database* aDatabase, std::ostream& anOutput)
		: CmdProcessor(anOutput), activeDB(aDatabase) {
	}

	SQLProcessor::~SQLProcessor() {}

	CmdProcessor* SQLProcessor::recognizes(Tokenizer& aTokenizer) {
		return  CreateTableStatement::recognize(aTokenizer) ||
				DropTableStatement::recognize(aTokenizer) ||
				ShowTableStatement::recognize(aTokenizer) ||
				DescribeTableStatement::recognize(aTokenizer) ||
				InsertTableStatement::recognize(aTokenizer) ||
				SelectTableStatement::recognize(aTokenizer) ||
				UpdateStatement::recognize(aTokenizer) ||
				DeleteStatement::recognize(aTokenizer) ||
				ShowIndexStatement::recognize(aTokenizer) ||
				ShowIndexFromStatement::recognize(aTokenizer) ||
				AlterAddStatement::recognize(aTokenizer) ||
				AlterDropStatement::recognize(aTokenizer) ? this : nullptr;
	}

	void SQLProcessor::setActiveDB(Database* aDB) {
		activeDB = aDB;
	}

	/*========================================================================================
									SQLStatement Factory
	========================================================================================*/

	Statement* createSQLStatement(Database* aDatabase, Tokenizer& aTokenizer) {
		return new CreateTableStatement(aDatabase, aTokenizer);
	}
	Statement* dropSQLStatement(Database* aDatabase, Tokenizer& aTokenizer) {
		return new DropTableStatement(aDatabase, aTokenizer);
	}
	Statement* showSQLStatement(Database* aDatabase, Tokenizer& aTokenizer) {
		auto& theToken = aTokenizer.peek(1);
		if (Keywords::tables_kw == theToken.keyword) {
			return new ShowTableStatement(aDatabase, aTokenizer);
		}
		else if (Keywords::indexes_kw == theToken.keyword) {
			return new ShowIndexStatement(aDatabase, aTokenizer);
		}
		else if (Keywords::index_kw == theToken.keyword) {
			return new ShowIndexFromStatement(aDatabase, aTokenizer);
		}
		return nullptr;
	}	
	Statement* describeSQLStatement(Database* aDatabase, Tokenizer& aTokenizer) {
		return new DescribeTableStatement(aDatabase, aTokenizer);
	}
	Statement* insertSQLStatement(Database* aDatabase, Tokenizer& aTokenizer) {
		return new InsertTableStatement(aDatabase, aTokenizer);
	}
	Statement* selectSQLStatement(Database* aDatabase, Tokenizer& aTokenizer) {
		return new SelectTableStatement(aDatabase, aTokenizer);
	}
	Statement* updateSQLStatement(Database* aDatabase, Tokenizer& aTokenizer) {
		return new UpdateStatement(aDatabase, aTokenizer);
	}
	Statement* deleteSQLStatement(Database* aDatabase, Tokenizer& aTokenizer) {
		return new DeleteStatement(aDatabase, aTokenizer);
	}
	Statement* alterTableStatement(Database* aDatabase, Tokenizer& aTokenizer) {
		auto& theToken = aTokenizer.peek(3);
		if (Keywords::add_kw == theToken.keyword) {
			return new AlterAddStatement(aDatabase, aTokenizer);
		}
		else if (Keywords::drop_kw == theToken.keyword) {
			return new AlterDropStatement(aDatabase, aTokenizer);
		}		
		return nullptr;
		
	}

	Statement* SQLProcessor::makeStatement(Tokenizer& aTokenizer,
		StatusResult& aResult) {
		if (!activeDB) {
			aResult = noDatabaseSpecified;
			return nullptr;
		}

		std::map<Keywords, StmtFactory> stmtMap = {
			{Keywords::create_kw,   createSQLStatement},
			{Keywords::drop_kw,	    dropSQLStatement},
			{Keywords::show_kw,		showSQLStatement },
			{Keywords::describe_kw,	describeSQLStatement},
			{Keywords::insert_kw,	insertSQLStatement},
			{Keywords::select_kw,	selectSQLStatement},
			{Keywords::update_kw,	updateSQLStatement},
			{Keywords::delete_kw,	deleteSQLStatement},
			{Keywords::alter_kw,    alterTableStatement}
		};
		if (stmtMap.count(aTokenizer.current().keyword)) {
			auto theStmt = stmtMap[aTokenizer.current().keyword](activeDB, aTokenizer);
			aResult = theStmt->parse(aTokenizer);
			if (aResult) {
				return theStmt;
			}
		}
		//aResult = invalidCommand;
		return nullptr;
	}

	StatusResult  SQLProcessor::run(Statement* aStmt) {
		return aStmt->run(output);
	}

}
