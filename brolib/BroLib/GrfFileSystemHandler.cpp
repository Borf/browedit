#include "GrfFileSystemHandler.h"
#include <blib/util/Log.h>
using blib::util::Log;

GrfFileSystemHandler::GrfFileSystemHandler( const std::string &grfFile ) : blib::util::FileSystemHandler("Grf")
{
	GrfError error;
	grf = grf_open(grfFile.c_str(), "rb", &error);
	if (grf == NULL)
	{
		Log::err<<"Error opening GRF file: "<<Log::newline;
		return;
	}
/*	for(unsigned int i = 0; i < grf->nfiles; i++)
	{
		lookup[File::sanitizeFileName(grf->files[i].name)] = i;
	}*/
}


blib::util::StreamInFile* GrfFileSystemHandler::openRead( const std::string &fileName )
{
	throw std::exception("The method or operation is not implemented.");
}

void GrfFileSystemHandler::getFileList( const std::string &path, std::vector<std::string> &files )
{
	throw std::exception("The method or operation is not implemented.");
}

