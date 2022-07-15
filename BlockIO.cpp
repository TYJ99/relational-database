//
//  BlockIO.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include "BlockIO.hpp"
#include <cstring>

namespace ECE141 {

	Block::Block(BlockType aType): header(aType) {}
	

	Block::Block(const Block& aCopy) {
		*this = aCopy;
	}

	Block& Block::operator=(const Block& aCopy) {
		std::memcpy(payload, aCopy.payload, kPayloadSize);
		header = aCopy.header;
		return *this;
	}

	StatusResult Block::write(std::ostream& aStream) {
		return StatusResult{ Errors::noError };
	}

	//---------------------------------------------------

	BlockIO::BlockIO(std::fstream& aStream) : stream(aStream) {}
	
	// USE: write data a given block (after seek) ---------------------------------------
	StatusResult BlockIO::writeBlock(uint32_t aBlockNum, Block& aBlock) {
		stream.clear();
		stream.seekp(aBlockNum * kBlockSize);
		stream.write((char*)&aBlock, sizeof(aBlock));
		stream.flush();
		stream.clear();
		return StatusResult{ noError };		
	}

	// USE: write data a given block (after seek) ---------------------------------------
	StatusResult BlockIO::readBlock(uint32_t aBlockNum, Block& aBlock) {
		stream.clear();
		stream.seekg(aBlockNum * kBlockSize);
		stream.read((char*)&aBlock, sizeof(aBlock));
		stream.clear();
		return StatusResult{ noError };
	}

	// USE: count blocks in file ---------------------------------------
	uint32_t BlockIO::getBlockCount() {
		stream.clear();
		stream.seekg(0, std::ios::end);
		size_t theFileSize = stream.tellg();
		stream.seekg(0, std::ios::beg);
		stream.clear();
		uint32_t theNumOfBlock = theFileSize / kBlockSize;
		return theNumOfBlock; //What should this be?
	}

	std::fstream& BlockIO::getStream() {
		return stream;
	}

	// ------------------------------------------------------------------------------------------

	CacheBlockIO::CacheBlockIO(std::fstream& aStream, LRUCache& aCache):BlockIO(aStream), cache(aCache) {}
	
	// USE: write data a given block (after seek) ---------------------------------------
	StatusResult CacheBlockIO::writeBlock(uint32_t aBlockNum, Block& aBlock) {	
		StatusResult theResult;
		theResult = BlockIO::writeBlock(aBlockNum, aBlock);
		if (theResult) {
			theResult = cache.updateCache(aBlockNum, aBlock);
		}		
		return theResult;
	}

	// USE: write data a given block (after seek) ---------------------------------------
	StatusResult CacheBlockIO::readBlock(uint32_t aBlockNum, Block& aBlock) {
		StatusResult theResult;
		if (!cache.contains(aBlockNum)) {
			theResult = BlockIO::readBlock(aBlockNum, aBlock);
		}
		else {
			aBlock = cache.getBlock(aBlockNum);
		}
		return theResult;
	}

}
