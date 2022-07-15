//
//  BlockIO.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef BlockIO_hpp
#define BlockIO_hpp

#include <stdio.h>
#include <iostream>
#include <functional>
#include <fstream>
#include "Config.hpp"
#include "Errors.hpp"
#include "LRUCache.hpp"

namespace ECE141 {
		
	class LRUCache;

	enum class BlockType {
		data_block = 'D',
		free_block = 'F',
		meta_block = 'M',
		entity_block = 'E',
		index_block = 'I',
		//other types?
		unknown_block = 'U',
	};

	//a small header that describes the block...
	struct BlockHeader {

		BlockHeader(BlockType aType = BlockType::data_block)
			: type(static_cast<char>(aType)) {}

		BlockHeader(const BlockHeader& aCopy) {
			*this = aCopy;
		}

		void empty() {
			type = static_cast<char>(BlockType::free_block);
		}

		BlockHeader& operator=(const BlockHeader& aCopy) {
			type = aCopy.type;
			return *this;
		}

		char      type;     //char version of block type
		//other properties?
	};

	const size_t kBlockSize = 1024;
	const size_t kPayloadSize = kBlockSize - sizeof(BlockHeader);

	//block .................
	class Block {
	public:
		Block(BlockType aType = BlockType::data_block);
		Block(const Block& aCopy);

		Block& operator=(const Block& aCopy);

		StatusResult write(std::ostream& aStream);

		BlockHeader   header;
		char          payload[kPayloadSize] = {0};
	};

	//------------------------------

	class BlockIO {
	public:

		BlockIO(std::fstream& aStream);

		uint32_t              getBlockCount();
		std::fstream&		  getStream();
		virtual StatusResult  readBlock(uint32_t aBlockNumber, Block& aBlock);
		virtual StatusResult  writeBlock(uint32_t aBlockNumber, Block& aBlock);

	protected:
		//std::iostream& stream;
		std::fstream& stream;
	};

	class CacheBlockIO: public BlockIO {
	public:

		CacheBlockIO(std::fstream& aStream, LRUCache& aCache);

		virtual StatusResult  readBlock(uint32_t aBlockNumber, Block& aBlock);
		virtual StatusResult  writeBlock(uint32_t aBlockNumber, Block& aBlock);

	protected:
		//std::iostream& stream;
		LRUCache&	  cache;
	};

}


#endif /* BlockIO_hpp */
