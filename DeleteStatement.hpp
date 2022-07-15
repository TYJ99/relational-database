#ifndef DeleteStatement_hpp
#define DeleteStatement_hpp

#include "SelectTableStatement.hpp"

namespace ECE141 {
	class DeleteStatement : public SelectTableStatement {
	public:
		DeleteStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SelectTableStatement(aDatabase, aTokenizer) {}
		static StatusResult  recognize(Tokenizer& aTokenizer);
		StatusResult		 run(std::ostream& aStream) const override;
		StatusResult		 parse(Tokenizer& aTokenizer) override;
		StatusResult		 parseHeader(KWList& aKWList, Tokenizer& aTokenizer) override;

	protected:



	};
}

#endif // !DeleteStatement_hpp

