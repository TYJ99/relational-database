#include"LRUCache.hpp"

namespace ECE141 {

	bool LRUCache::contains(uint32_t aBlockNum) {
		return blockMap.find(aBlockNum) != blockMap.end();
	}

	StatusResult LRUCache::updateCache(uint32_t aBlockNum, Block& aBlock) {
		if (maxSize > 0) {
			// not present in cache
			if (blockMap.find(aBlockNum) == blockMap.end()) {

				// cache is full
				if (blockNumList.size() == maxSize) {
					uint32_t theLast = blockNumList.back(); // delete least recently used element					
					blockNumList.pop_back();		   // Pops the last element					
					blockMap.erase(theLast);		   // Erase the last
				}
			}
			// present in cache
			else {
				blockNumList.erase(dqPosMap[aBlockNum]);
			}
				
			// update reference
			blockNumList.push_front(aBlockNum);
			dqPosMap[aBlockNum] = blockNumList.begin();
			blockMap[aBlockNum] = aBlock;

			return StatusResult{ noError };
		}

		return StatusResult{ cacheError };
		
	}

	Block& LRUCache::getBlock(uint32_t aBlockNum) {
		return blockMap[aBlockNum];
	}



}
