#ifndef LRUCache_hpp
#define LRUCache_hpp

#include <list>
#include <map>
#include "BlockIO.hpp"

namespace ECE141 {

	class BlockIO;
	class Block;

	class LRUCache {
	public:
		LRUCache():maxSize(0) {}
		LRUCache(size_t aMaxSize) :maxSize(aMaxSize) {}
		bool	contains(uint32_t aBlockNum);
		StatusResult	updateCache(uint32_t aBlockNum, Block& aBlock);
		Block&  getBlock(uint32_t aBlockNum);
		size_t  size() { return blockNumList.size(); }
		void    setMaxSize(size_t aSize) { maxSize = aSize; }
	protected:
		std::list<uint32_t>	  blockNumList;
		std::map<uint32_t, Block> blockMap;
		std::map<uint32_t, std::list<uint32_t>::iterator> dqPosMap;
		size_t					  maxSize;

	};


}




#endif // !LRUCache_hpp

