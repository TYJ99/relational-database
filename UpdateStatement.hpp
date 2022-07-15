#ifndef UpdateStatement_hpp
#define UpdateStatement_hpp

#include "SelectTableStatement.hpp"

namespace ECE141 {
	class UpdateStatement : public SelectTableStatement {
	public:
		UpdateStatement(Database* aDatabase, Tokenizer& aTokenizer) :
			SelectTableStatement(aDatabase, aTokenizer) {}
		StatusResult		 run(std::ostream& aStream) const override;
		static StatusResult	 recognize(Tokenizer& aTokenizer);
		StatusResult		 parse(Tokenizer& aTokenizer) override;
		StatusResult		 parseHeader(KWList& aKWList, Tokenizer& aTokenizer) override;

	protected:



	};
}



#endif // !UpdateStatement_hpp

