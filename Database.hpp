//
//  Database.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <fstream> 
#include "Storage.hpp"
#include "Entity.hpp"
#include "TableDescriptionView.hpp"
#include "EntityView.hpp"
#include "QueryView.hpp"
#include "Query.hpp"
#include "Joins.hpp"
#include "IndexView.hpp"

namespace ECE141 {
	using TableList = std::vector<Entity>;
	using tableOpt = std::optional<Entity*>;
	using EntityForIndexOpt = std::optional<std::string>;


	class Database : public Storable, public Storage{
	public:

		Database(const std::string aPath, std::ostream& anOutput);
		//Database(const std::string aPath, OpenDB);
		virtual ~Database();

		StatusResult    encode(std::ostream& anOutput) override;
		StatusResult    decode(std::istream& anInput) override;
		StatusResult    decode(std::istream& anInput, Entity& anEntity);

		StatusResult    createTable(Entity& anEntity);
		StatusResult    describeTable(const std::string& aName, std::ostream& anOutput);
		StatusResult    dropTable(const std::string& aName);
		StatusResult    dumpTable(const std::string& aName);
		StatusResult    showTables(std::ostream& anOutput);
		StatusResult	showIndexes(std::ostream& anOutput);
		StatusResult	showIndexFrom(std::ostream& anOutput);
		StatusResult    dump(std::ostream& anOutput);
		StatusResult    insertTable(Entity& anEntity);
		StatusResult 	selectRows(const Query& aQuery);
		StatusResult	showQuery(const Query& aQuery, std::ostream& anOutput, const JoinList& aJoinList);
		StatusResult    updateRows(const Query& aQuery);
		StatusResult    updateRowsHelper(Row& aRow);
		StatusResult	deleteRows(const Query& aQuery);

		StatusResult	removeTable(const std::string aName, size_t& aNumOfDropBlock);
		StatusResult	findSelectedEntityBlock(const Query& aQuery, std::stringstream& anInput, std::string& aString);
		void			addSelectedRow(const Row aRow) { rowCollection.push_back(aRow); }
		std::string		getName();
		std::string		getPath();
		TableList		getTables();
		tableOpt		getTable(std::string aName);
		void			updateShowIndexes(Entity& anEntity);
		void			updateIndexBlockMap(Entity& anEntity, Row& aRow, bool isShowIndexes);
		ShowIndexesMap& getShowIndexesMap() { return showIndexesMap; }
		void			setEntityForIndex(std::string& aName) { entityForIndex = aName; }
		EntityForIndexOpt& getEntityForIndex() { return entityForIndex; }
		StatusResult	alterAdd(std::string aTableName);
		StatusResult	alterDrop(std::string aTableName);


		std::ostream&   output;

	protected:

		TableList		tables;				// Vector of tables ;
		RowList			rowCollection;		// Collection(vector) of rows
		std::string     name = "noName";
		std::string     path;
		std::fstream    stream;				// low level stream used by storage...
		bool            changed;			// might be helpful, or ignore if you prefer.
		bool            initialized;
		ShowIndexesMap  showIndexesMap;
		EntityForIndexOpt entityForIndex = std::nullopt;
	};

}
#endif /* Database_hpp */
