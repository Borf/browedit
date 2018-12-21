#include "GrfFileSystemHandler.h"
#include <blib/util/Log.h>
using blib::util::Log;

#include <algorithm>
#include <cctype>


std::string GrfFileSystemHandler::sanitizeFileName(std::string fileName)
{
	std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
	std::replace(fileName.begin(), fileName.end(), '/', '\\');

	int index;
	while((index = fileName.find("\\\\")) != -1)
		fileName = fileName.substr(0, index) + fileName.substr(index + 1);
	return fileName;
}


GrfFileSystemHandler::GrfFileSystemHandler( const std::string &grfFile ) : blib::util::FileSystemHandler("Grf")
{
	grf = NULL;

	GrfError error;
	grf = grf_open(grfFile.c_str(), "rb", &error);
	if (grf == NULL)
	{
		Log::err<<"Error opening GRF file: "<<grfFile<<Log::newline;
		return;
	}
	Log::out<<"Loaded GRF file "<<grfFile<<Log::newline;
	for(unsigned int i = 0; i < grf->nfiles; i++)
		lookup[sanitizeFileName(grf->files[i].name)] = i;
	Log::out << lookup.size() << " files loaded"<< Log::newline;
}

GrfFileSystemHandler::~GrfFileSystemHandler()
{
	grf_close(grf);
}


blib::util::StreamInFile* GrfFileSystemHandler::openRead( const std::string &fileName )
{
	if(!grf)
		return NULL;
	auto it = lookup.find(sanitizeFileName(fileName));
	if(it == lookup.end())
		return NULL;

	GrfError error;
	unsigned int size = 0;
	char* data = (char*)grf_index_get(grf, it->second, &size, &error);


	blib::util::StreamInFile* f = new blib::util::MemoryFile(data, size, true);
	return f;
}

void GrfFileSystemHandler::getFileList( const std::string &path, std::vector<std::string> &files )
{
	std::string directory = sanitizeFileName(path);
	for (std::map<std::string, int>::iterator it = lookup.begin(); it != lookup.end(); it++)
	{
		if (it->first.substr(0, directory.length()) == directory && it->first.find("\\", directory.size() + 1) == -1)
			files.push_back(it->first);
	}
}

void GrfFileSystemHandler::getFileList(const std::function<bool(const std::string&)> &filter, std::vector<std::string> &files)
{
	for (std::map<std::string, int>::iterator it = lookup.begin(); it != lookup.end(); it++)
	{
		if (filter(it->first))
			files.push_back(it->first);
	}
}
