//
//  Storage.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//


#include <sstream>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <cstring>
#include "Storage.hpp"
#include "Config.hpp"

namespace ECE141 {

	
	// USE: ctor ---------------------------------------
	Storage::Storage(std::fstream& aStream) {
		size_t theCacheSize = Config::getCacheSize(CacheType::block);
		cache.setMaxSize(theCacheSize);

		if (Config::useCache(CacheType::block)) {
			blockIO = std::make_unique<CacheBlockIO>(aStream, cache);
		}
		else {
			blockIO = std::make_unique<BlockIO>(aStream);
		}

	}

	// USE: dtor ---------------------------------------
	Storage::~Storage() {
	}

	bool Storage::each(const BlockVisitor& aVisitor) {
		size_t theCount = blockIO->getBlockCount();
		Block theBlock;
		for (uint32_t i = 0; i < theCount; i++) {
			if (blockIO->readBlock(i, theBlock)) {
				if (!aVisitor(theBlock, i)) break;
			}
		}
		return true;
	}

	uint32_t Storage::getFreeBlock() {
		if (available.size()) {
			uint32_t theResult = available.front();
			available.pop_front();
			return theResult;
		}
		return blockIO->getBlockCount();
	}

	StatusResult Storage::updateAvailableBlock() {
		uint32_t theBlockCount = blockIO->getBlockCount();
		char theType {0};
		for (uint32_t i = 0; i < theBlockCount; ++i) {
			blockIO->getStream().clear();
			blockIO->getStream().seekg(i * kBlockSize);
			blockIO->getStream().read((char*)&theType, sizeof(theType));
			blockIO->getStream().clear();
			if ('F' == theType) {
				available.push_back(i);
			}
		}
		return StatusResult{ noError };
	}

	StatusResult Storage::markBlockAsFree(uint32_t aPos) {
		Block theBlock(BlockType::free_block);
		//std::memset(&theBlock.header, 0, sizeof(theBlock.header));
		available.push_back(aPos);
		return blockIO->writeBlock(aPos, theBlock);
	}

	// USE: for use with storable API...
	StatusResult Storage::releaseBlocks(uint32_t aPos, bool aInclusive) {
		Block theBlock;

		StatusResult theResult{ Errors::noError };
		while ((theResult = blockIO->readBlock(aPos, theBlock))) {
			if (aInclusive) {
				markBlockAsFree(aPos);
			}
			//aPos=theBlock.header.next;
			aInclusive = true;
			if (!aPos) break;
		}

		return theResult;
	}

	StatusResult Storage::save(std::iostream& aStream,
		StorageInfo& anInfo) {

		uint32_t theStartBlock = anInfo.start;
		if (anInfo.start != kNewBlock) {
			releaseBlocks(anInfo.start, false); //free prior chain...
		}
		else theStartBlock = getFreeBlock();

		uint32_t theBlockNum = theStartBlock;
		StatusResult theResult{ Errors::noError };
		size_t theCount = ceil((anInfo.size / kPayloadSize) + 0.5);

		size_t thePart = 0;
		Block theBlock;

		while (theResult && (anInfo.size > 0)) {
			size_t theBufSize = std::min(anInfo.size, kPayloadSize);
			std::memset(&theBlock.payload, 0, kPayloadSize);
			aStream.read((char*)&theBlock.payload, theBufSize);
			anInfo.size -= theBufSize;
			theBlock.header.type = (char)anInfo.type;
			//theBlock.header.count=theCount;
			//theBlock.header.pos=++thePart;
			//theBlock.header.next=0;
			if (anInfo.size) {
				//theBlock.header.next=getFreeBlock();
			}
			theResult = blockIO->writeBlock(theBlockNum, theBlock);
		}
		theResult.value = theStartBlock;
		return theResult;
	}

	StatusResult Storage::load(std::iostream& anOut, uint32_t aBlockNum) {
		StatusResult theResult{ Errors::seekError };
		if (blockIO->getBlockCount() > aBlockNum) {
			Block theBlock;
			theResult.error = Errors::noError;
			if ((theResult = blockIO->readBlock(aBlockNum, theBlock))) {
				anOut.write((char*)&theBlock, kBlockSize);
				//aBlockNum=theBlock.header.next;
			}
			return StatusResult{ Errors::noError };
		}
		return theResult;
	}

	BlockIO* Storage::getBlockIO() {
		return blockIO.get();
	}

}

