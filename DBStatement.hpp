
#ifndef DBStatement_hpp
#define DBStatement_hpp

#include "Application.hpp"
#include "Statement.hpp"
#include "Helpers.hpp"

namespace ECE141 {
	class Application;

	class DBStatement : public Statement {	
	public:
		DBStatement(Application* anApp, Keywords aStatementType = Keywords::unknown_kw):
			Statement(aStatementType), app(anApp) {}
		const char* getStatementName() const override { return "DBStatement"; }		
		//static bool recognize(Tokenizer& aTokenizer) { return false; }
		
		Application *app;
		inline static std::string name = "noName";
	};

	class CreateDBStatement : public DBStatement {
	public:
		CreateDBStatement(Application* anApp, Tokenizer& aTokenizer): DBStatement(anApp, Keywords::create_kw) {}
		const char* getStatementName() const override { return "CreateDBStatement"; }
		static StatusResult recognize(Tokenizer &aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
		StatusResult  run(std::ostream& aStream) const override;
	};

	class DropDBStatement : public DBStatement {
	public:
		DropDBStatement(Application* anApp, Tokenizer& aTokenizer) : DBStatement(anApp, Keywords::drop_kw) {}
		const char* getStatementName() const override { return "DropDBStatement"; }
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
		StatusResult  run(std::ostream& aStream) const override;
	};

	class ShowDBStatement : public DBStatement {
	public:
		ShowDBStatement(Application* anApp, Tokenizer& aTokenizer) : DBStatement(anApp, Keywords::show_kw) {}
		const char* getStatementName() const override { return "ShowDBStatement"; }
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
		StatusResult  run(std::ostream& aStream) const override;
	};

	class UseDBStatement : public DBStatement {
	public:
		UseDBStatement(Application* anApp, Tokenizer& aTokenizer) : DBStatement(anApp, Keywords::use_kw) {}
		const char* getStatementName() const override { return "UseDBStatement"; }
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
		StatusResult  run(std::ostream& aStream) const override;
	};

	class DumpDBStatement : public DBStatement {
	public:
		DumpDBStatement(Application* anApp, Tokenizer& aTokenizer) : DBStatement(anApp, Keywords::dump_kw) {}
		const char* getStatementName() const override { return "DumpDBStatement"; }
		static StatusResult recognize(Tokenizer& aTokenizer);
		StatusResult  parse(Tokenizer& aTokenizer) override;
		StatusResult  run(std::ostream& aStream) const override;
	};


}
#endif