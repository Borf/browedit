#pragma once

#include <list>
#include <string>
#include <fstream>

#include <json/value.h>

class File;
class FileLoader;
class InFile;
class OutFile;


class File
{
	static std::list<FileLoader*> fileLoaders;
public:
	//File();


	static void registerFileLoader(FileLoader* loader);
	static InFile* openRead(std::string fileName);
	static OutFile* openWrite(std::string fileName);
	static std::string getData(std::string fileName);
	static Json::Value getJson(std::string fileName);
};



class InFile : public File
{
public:
	enum SeekPos { BEG, CUR, END };

	virtual bool eof() = 0;
	virtual void seek(int pos, SeekPos relTo) = 0;
	virtual int read(char* data, int len) = 0;
	virtual char get() = 0;
};

class OutFile : public File
{
public:
	virtual void write(char* data, int len) = 0;
	virtual void put(char data) = 0;
};


class PhysicalInFile : public InFile
{
	std::ifstream &file;
public:
	PhysicalInFile(std::ifstream& file);
	virtual bool eof();
	virtual void seek( int pos, SeekPos relTo );
	virtual int read( char* data, int len );
	virtual char get();
};

class PhysicalOutFile : public OutFile
{
public:
	virtual void write( char* data, int len );
	virtual void put( char data );
};



class FileLoader
{
public:
	virtual void isFile(std::string fileName) = 0;
	virtual InFile* openRead(std::string fileName) = 0;
	virtual OutFile* openWrite(std::string fileName) = 0;
};

class PhysicalFileLoader : public FileLoader
{
public:
	virtual void isFile( std::string fileName );
	virtual InFile* openRead( std::string fileName );
	virtual OutFile* openWrite( std::string fileName );

};
