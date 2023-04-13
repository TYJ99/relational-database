# use g++ with C++17 support
CXX=g++
CXXFLAGS?=-Wall -pedantic -g -O0 -std=c++17
OUTFILES=tyjSQL
OBJS = Application.o Attribute.o BlockIO.o CmdProcessor.o Database.o DBProcessor.o DBStatement.o DeleteStatement.o Entity.o Faked.o Filters.o LRUCache.o main.o ParseClauseHelper.o ParseCreateTableHelper.o ParseHelper.o ParseInsertHelper.o Row.o SelectTableStatement.o SQLProcessor.o SQLStatement.o Statement.o Storage.o Tokenizer.o UpdateStatement.o
# OBJS = Application.o Attribute.o BasicTypes.o BlockIO.o CmdProcessor.o Compare.o Config.o Database.o DBProcessor.o DBStatement.o DeleteStatement.o Entity.o EntityView.o Errors.o Faked.o Filters.o FolderReader.o FolderView.o Helpers.o IndexView.o Joins.o keywords.o LRUCache.o main.o ParseClauseHelper.o ParseCreateTableHelper.o ParseHelper.o Query.o QueryView.o Row.o Scanner.o ScriptRunner.o SelectTableStatement.o SQLProcessor.o SQLStatement.o Statement.o Storage.o TableDescriptionView.o TestAutomatic.o TestManually.o TestSequencer.o Timer.o Tokenizer.o UpdateStatement.o View.o

all: $(OUTFILES)

$(OUTFILES): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o tyjSQL

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $^

# %.o : %.hpp
# 	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -f $(OUTFILES) *.o
