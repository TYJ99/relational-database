//
//  FolderReader.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderReader_h
#define FolderReader_h

#include <string>
#include <filesystem>
#include <fstream>
#include "Config.hpp"

namespace fs = std::filesystem;

namespace ECE141 {

	using FileVisitor = std::function<bool(const std::string&)>;

	class FolderReader {
	public:
		FolderReader(const char* aPath) : path(aPath) {}
		virtual ~FolderReader() {}

		virtual bool exists(const std::string& aFilename) {
			std::ifstream theStream(aFilename);
			return !theStream ? false : true;
		}

		virtual void each(const std::string& anExt, const FileVisitor& aVisitor) const {
			// The for(auto const& dir_entry : std::filesystem::directory_iterator{...}) loop will iterate through a folder 
			// given by fs::temp_directory_path().string(). The if statement will then check dir_entry.path().string().find(anExt)
			// to see if the file currently being inspected contains the given anExt (".txt" in this case)
			// If it finds a .txt file then it will call aVisitor which is the lambda function from doReaderTest() in TestAutomatic.hpp line 292
			// with dir_entry.path().string() (the current file being inspected) as the parameter &aName and will execute the lambda function
			for (auto const& dir_entry : std::filesystem::directory_iterator{ Config::getStoragePath() })
			{
				if (dir_entry.path().string().find(anExt) != std::string::npos) {
					aVisitor(dir_entry.path().string());
					// std::cout << "Entry found!\n" << dir_entry << '\n';
				}
			}
		};

		std::string path;
	};

}

#endif /* FolderReader_h */