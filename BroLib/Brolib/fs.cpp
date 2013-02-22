#include "fs.h"
#include "Log.h"

#include <json/reader.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>


std::list<FileLoader*> File::fileLoaders;
void File::registerFileLoader(FileLoader* loader)
{
	File::fileLoaders.push_back(loader);
}



InFile* File::openRead(std::string fileName)
{
	std::transform(fileName.begin(), fileName.end(), fileName.begin(), std::tolower);
	std::replace(fileName.begin(), fileName.end(), '/', '\\');

	for(std::list<FileLoader*>::iterator it = fileLoaders.begin(); it != fileLoaders.end(); it++)
	{
		InFile* file = (*it)->openRead(fileName);
		if(file)
			return file;
	}
	return NULL;
}





Json::Value File::getJson(std::string fileName)
{
	Json::Reader reader;
	Json::Value root;
	std::string data = File::getData(fileName);
	if(!reader.parse(data, root))
		logger<<"Error parsing json file "<<fileName<<" : "<<reader.getFormattedErrorMessages()<<Log::newline;
	return root;
}

std::string File::getData(std::string fileName)
{
	char buf[1024];
	std::string data;
	InFile* file = File::openRead(fileName);
	if(!file)
		return "";

	while(!file->eof())
	{
		int read = file->read(buf, 1024);
		data += std::string(buf, read);
	}
	return data;
}








#include <Windows.h>
void PhysicalFileLoader::isFile( std::string fileName )
{
	throw std::exception("The method or operation is not implemented.");
}

InFile* PhysicalFileLoader::openRead( std::string fileName )
{
	std::ifstream& file = *(new std::ifstream());
	file.open(fileName.c_str(), std::ios::in | std::ios::binary);
	if(!file.is_open())
	{
		delete file;
		return NULL;
	}
	return new PhysicalInFile(file);
}

OutFile* PhysicalFileLoader::openWrite( std::string fileName )
{
	throw std::exception("The method or operation is not implemented.");
}

PhysicalInFile::PhysicalInFile(std::ifstream& file) : file(file)
{
}

bool PhysicalInFile::eof()
{
	return file.eof();
}

void PhysicalInFile::seek( int pos, SeekPos relTo )
{
	throw std::exception("The method or operation is not implemented.");
}

int PhysicalInFile::read( char* data, int len )
{
	file.read(data, len);
	return (int)file.gcount();
}

char PhysicalInFile::get()
{
	throw std::exception("The method or operation is not implemented.");
}

void PhysicalOutFile::write( char* data, int len )
{
	throw std::exception("The method or operation is not implemented.");
}

void PhysicalOutFile::put( char data )
{
	throw std::exception("The method or operation is not implemented.");
}
