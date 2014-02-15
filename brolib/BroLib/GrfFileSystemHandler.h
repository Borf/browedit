#pragma once

#include <blib/util/FileSystem.h>
#include "grflib/grf.h"


class GrfFileSystemHandler : public blib::util::FileSystemHandler
{
	Grf* grf;
public:
	GrfFileSystemHandler(const std::string &grfFile);

	virtual blib::util::StreamInFile* openRead( const std::string &fileName );
	virtual void getFileList( const std::string &path, std::vector<std::string> &files );

};