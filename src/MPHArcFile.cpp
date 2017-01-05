//============== Copyright lololol, All rights reserved. ==============//
//
// Purpose:		metroid prime hunters ARC file extractor
//
// $NoKeywords: $mpharc
//=====================================================================//

#include "MPHArcFile.h"

#include <fstream>
#include <iostream>
#include <direct.h>

#define MPHARC_MAX_SUBFILES 64

// recursive directory creation function (c) Nico Golde @ http://nion.modprobe.de/blog/archives/357-Recursive-directory-creation.html
static void _mkdir_tree(const char *dir)
{
	char tmp[256];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s", dir);
	len = strlen(tmp);
	if (tmp[len-1] == '/')
		tmp[len-1] = 0;
	for (p=tmp + 1; *p; p++)
		if (*p == '/')
		{
			*p = 0;
			_mkdir(tmp);
			*p = '/';
		}
	_mkdir(tmp);
}

MPHArcFile::MPHArcFile()
{
	m_header = NULL;
	m_fileBuffer = NULL;
	m_bLoaded = false;
}

MPHArcFile::~MPHArcFile()
{
	clear();
}

void MPHArcFile::clear()
{
	m_bLoaded = false;
	m_header = NULL;

	if (m_fileBuffer != NULL)
	{
		delete[] m_fileBuffer;
		m_fileBuffer = NULL;
	}
}

bool MPHArcFile::load(std::string filename)
{
	if (m_bLoaded)
		clear();

	// check if the file exists
	/*
	FILE *file = NULL;
	fopen_s(&file, filename.c_str(), "rb");
	*/
	FILE *file = fopen(filename.c_str(), "rb");
	if (!file)
	{
		printf("ERROR: %s does not exist!\n", filename.c_str());
		return false;
	}

	// get file size
	fseek(file, 0, SEEK_END);
	int filesize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// filesize error checking
	if (filesize < (sizeof(MPHArcHeader)+sizeof(MPHArcSubFileHeader)))
	{
		printf("ERROR: Invalid filesize (%i)\n", filesize);
		fclose(file);
		return false;
	}

	// create buffer
	m_fileBuffer = new u8[filesize];

	// read entire file, then close it
	fread(m_fileBuffer, sizeof(u8), filesize, file);
	fclose(file);


	// now get everything we want
	m_header = (MPHArcHeader*)m_fileBuffer;

	// fix endianness
	m_header->numfiles = endianSwapU32(m_header->numfiles);
	m_header->filesize = endianSwapU32(m_header->filesize);

	if (m_header->numfiles < 1 || m_header->numfiles > MPHARC_MAX_SUBFILES)
	{
		printf("ERROR: Invalid numfiles in header (%i)\n", m_header->numfiles);
		return false;
	}

	// load all sub file headers
	for (int i=0; i<m_header->numfiles; i++)
	{
		MPHArcSubFileHeader *subFileHeader = (MPHArcSubFileHeader*)(m_fileBuffer + sizeof(MPHArcHeader) + i*sizeof(MPHArcSubFileHeader));

		// fix endianness
		subFileHeader->offset = endianSwapU32(subFileHeader->offset);
		subFileHeader->filesize1 = endianSwapU32(subFileHeader->filesize1);
		subFileHeader->filesize2 = endianSwapU32(subFileHeader->filesize2);

		// error checking
		if (subFileHeader->offset > filesize || subFileHeader->offset < sizeof(MPHArcHeader))
			printf("WARNING: Potentially invalid subfile offset (%i)\n", subFileHeader->offset);
		if (subFileHeader->filesize1 > filesize || subFileHeader->filesize1 < 1 || subFileHeader->filesize2 > filesize || subFileHeader->filesize2 < 1)
			printf("WARNING: Potentially invalid subfile size (%i, %i)\n", subFileHeader->filesize1, subFileHeader->filesize2);

		m_subFileHeaders.push_back(subFileHeader);

		//printf("subFile #%i: name = %s, filesize1 = %i, filesize2 = %i\n", i, subFileHeader->filename, subFileHeader->filesize1, subFileHeader->filesize2);
	}

	m_bLoaded = true;
	return true;
}

bool MPHArcFile::extract(std::string folderToExtractTo)
{
	if (!m_bLoaded)
	{
		printf("ERROR: load() a file before trying to extract it.\n");
		return false;
	}

	printf("Starting extraction for %i subfiles:\n", m_subFileHeaders.size());

	// create folder if it doesn't exist already
	_mkdir_tree(folderToExtractTo.c_str());

	for (int i=0; i<m_subFileHeaders.size(); i++)
	{
		std::string outputFileName = folderToExtractTo;
		outputFileName.append((char*)m_subFileHeaders[i]->filename);

		printf("  Writing %s ...\n", outputFileName.c_str());
		std::ofstream output(outputFileName.c_str(), std::ios::out | std::ios::binary);
		if (output.is_open())
		{
			output.write(reinterpret_cast<char*>(&m_fileBuffer[m_subFileHeaders[i]->offset]), m_subFileHeaders[i]->filesize2);
		}
		else
			printf("ERROR: Couldn't write %s!\n", outputFileName.c_str());
	}

	printf("Done.\n");

	return true;
}


