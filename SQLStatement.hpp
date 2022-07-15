#ifndef SQLStatement_hpp
#define SQLStatement_hpp

#include "Statement.hpp"
#include "Database.hpp"
#include "TestSequencer.hpp"
#include "Entity.hpp"
#include "Helpers.hpp"
#include "Attribute.hpp"
#include "DBStatement.hpp"
#include <functional>
#include "Row.hpp"
#include "ParseCreateTableHelper.hpp"
#include "ParseInsertHelper.hpp"
#include "ParseHelper.hpp"


namespace ECE141 {
	class Database;
	class Entity;

	//using ParseMethod = StatusResult(*)(Tokenizer&);

	class SQLStatement : public Statement {
		
	public:
		SQLStatement(Database* aDatabase, Keywords aStatementType = Keywords::unknown_kw) :Statement(aStatementType),
			tableName("noName"), db(aDatabase) {}
		const char* getStatementName() const override { return "SQLStatement"; }
	
		bool checkTableName(std::string& aName);						
		virtual StatusResult  parseHeader(KWList& aKWList, Tokenizer& aTokenizer);		

		std::string tableName; // table name;
		Database*				  db;
		std::unique_ptr<Entity>   entity;
	};

	class CreateTableStatement : public SQLStatement {
	public:
		CreateTableStatement(Database* aDatabase, Tokenizer& aTokenizer):
			SQLStatement(aDatabase, Keywords::create_kw) {}
		StatusResult  run(std::ostream& aStream) const override;
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
		
	};

	class DropTableStatement : public SQLStatement {
	public:
		DropTableStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SQLStatement(aDatabase, Keywords::drop_kw) {}
		StatusResult  run(std::ostream& aStream) const override;
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
	};

	class ShowTableStatement : public SQLStatement {
	public:
		ShowTableStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SQLStatement(aDatabase, Keywords::show_kw) {}
		StatusResult  run(std::ostream& aStream) const override;
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
	};

	class DescribeTableStatement : public SQLStatement {
	public:
		DescribeTableStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SQLStatement(aDatabase, Keywords::create_kw) {}
		StatusResult  run(std::ostream& aStream) const override;
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
	};

	class InsertTableStatement : public SQLStatement {
	public:
		InsertTableStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SQLStatement(aDatabase, Keywords::insert_kw) {}
		StatusResult  run(std::ostream& aStream) const override;
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
	};

	class ShowIndexStatement : public SQLStatement {
	public:
		ShowIndexStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SQLStatement(aDatabase, Keywords::insert_kw) {}
		StatusResult  run(std::ostream& aStream) const override;
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
	};

	class ShowIndexFromStatement : public SQLStatement {
	public:
		ShowIndexFromStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SQLStatement(aDatabase, Keywords::insert_kw) {}
		StatusResult  run(std::ostream& aStream) const override;
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
	};

	class AlterAddStatement : public SQLStatement {
	public:
		AlterAddStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SQLStatement(aDatabase, Keywords::alter_kw) {}
		StatusResult  run(std::ostream& aStream) const override;
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
	};

	class AlterDropStatement : public SQLStatement {
	public:
		AlterDropStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SQLStatement(aDatabase, Keywords::alter_kw) {}
		StatusResult  run(std::ostream& aStream) const override;
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
	};

}


#endif // !SQLStatement_hpp

