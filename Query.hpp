#ifndef Query_hpp
#define Query_hpp

#include "Tokenizer.hpp"
#include "keywords.hpp"
#include "Filters.hpp"
#include <unordered_map>

namespace ECE141 {

    using StringList = std::vector<std::string>;
    using TableMap = std::unordered_map<std::string, Entity*>;

    class Filters;
    class Query {
    public:

        Query(const StringList aTableNameList, StringList afieldsSelected, Keywords aQueryType, bool aSelectType = 1)
            : tableNameList(aTableNameList), fieldsSelected(afieldsSelected), queryType(aQueryType), 
            selectAll(aSelectType), offset(0), limit(0), hasLimit(false) {
            filters = std::make_unique<Filters>();
        }
        Query():offset(0), limit(0), hasLimit(false) {
            filters = std::make_unique<Filters>(); 
        };
        ~Query() {};

        //Setters        
        void setQueryType(Keywords aQueryType) { queryType = aQueryType; }
        void setSelectAll() { selectAll = 1; }
        void setLimit(uint32_t aLimit) { limit = aLimit; }
        void setOffset(uint32_t aOffset) { offset = aOffset; }
        void setHasLimit(bool aHasLimit) { hasLimit = aHasLimit; }

        // Getters
        StringList            getTableNameList() const { return tableNameList; }
        StringList            getfieldsSelected() const { return fieldsSelected; }
        StringList            getOrderBy() const { return orderBy; }
        Keywords              getQueryType() { return queryType; }
        bool                  getSelectAll() const { return selectAll; }
        bool                  getHasLimit() const { return hasLimit; }
        Filters*              getFilters() const { return filters.get(); }
        uint32_t              getLimit() const { return limit; }
        uint32_t              getOffset() const { return offset; }
        TableMap              getTableMap() const{ return tableMap; }
        AttributeValueMap     getAttrValMap() const { return attributeValueMap; }
        StringList            getAllTableNameList() const { return allTableNameList; }
       
        // update lists
        void            addField(std::string& anAttr) { fieldsSelected.push_back(anAttr); }
        void            addTableName(const std::string aName) { tableNameList.push_back(aName); }
        void            addField(StringList& anAttrNameList) { fieldsSelected = anAttrNameList; }
        void            addTableName(StringList& aTableNameList) { tableNameList = aTableNameList; }
        void            addOrderBy(StringList& anAttrNameList) { orderBy = anAttrNameList; }
        void            addTableMap(TableMap& aTableMap) { tableMap = aTableMap; }
        void            addAttrValMap(AttributeValueMap& aMap) { attributeValueMap = aMap; }
        void            addALLTableName(const std::string aName) { allTableNameList.push_back(aName); }
        void            addALLTableName(StringList& aTableNameList) { allTableNameList = aTableNameList; }

    protected:
        // more need to added later on when queries become more complex
        StringList                      tableNameList;    //name of table being used excluding join table
        StringList                      allTableNameList;    //name of table being used including join table
        TableMap                        tableMap;
        bool                            selectAll = 0;  // Select all or not
        Keywords                        queryType;    //What kind of query (select, update, delete, etc...)
        StringList                      fieldsSelected;
        std::unique_ptr<Filters>        filters;
        bool                            hasLimit;
        uint32_t                        limit = 0;
        uint32_t                        offset = 0;
        StringList                      orderBy;
        AttributeValueMap               attributeValueMap;
    };

}
#endif /* Query_hpp */