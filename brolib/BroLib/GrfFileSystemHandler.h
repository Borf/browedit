#pragma once

#include <blib/util/FileSystem.h>

#include "grflib/grf.h"
#include <map>



class GrfFileSystemHandler : public blib::util::FileSystemHandler
{
	Grf* grf;
	std::map<std::string, int> lookup;
public:
	GrfFileSystemHandler(const std::string &grfFile);
	~GrfFileSystemHandler();

	virtual blib::util::StreamInFile* openRead( const std::string &fileName );
	virtual void getFileList( const std::string &path, std::vector<std::string> &files );
	virtual void getFileList(const std::function<bool(const std::string&)> &filter, std::vector<std::string> &files);


private:
	std::string sanitizeFileName(std::string fileName);
};