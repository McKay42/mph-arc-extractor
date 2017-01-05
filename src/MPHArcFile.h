//============== Copyright lololol, All rights reserved. ==============//
//
// Purpose:		metroid prime hunters ARC file extractor
//
// $NoKeywords: $mpharc
//=====================================================================//

#ifndef MPHARCFILE_H
#define MPHARCFILE_H

#include <string>
#include <vector>
#include <stddef.h>


//************//
//  Typedefs  //
//************//

typedef unsigned char	u8;
typedef unsigned int	u32;


//*********************//
//   Helper functions  //
//*********************//

inline u32 endianSwapU32(u32 x)
{
	return ((x >> 24) & 0xff) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | ((x << 24) & 0xff000000);
}


//***********************************//
//	The main header of the ARC file  //
//***********************************//

struct MPHArcHeader
{
	u32 magicnumber[2]; 	// should contain 'SNDFILE'

	u32 numfiles;			// the number of files this ARChive contains
	u32 filesize;			// the size of the entire arc file in bytes

	u32 padding[4];			// empty, contains only zeroes
};


//*****************************************************************************//
//	Right after the main header, there are multiple instances of file headers  //
//*****************************************************************************//

struct MPHArcSubFileHeader
{
	u8 filename[32];		// a string containing the filename
	u32 offset;				// offset into the arc file where this file starts
	u32 filesize1;			// actual filesize
	u32 filesize2;			// padded filesize

	u32 padding[5];			// empty
};


class MPHArcFile
{
public:
	MPHArcFile();
	~MPHArcFile();

	bool load(std::string filename);
	bool extract(std::string folderToExtractTo);

	inline int getNumFiles() const { return m_subFileHeaders.size(); }
	inline int getFileSize() const { return m_header != NULL ? m_header->filesize : -1; }

private:
	void clear();

	bool m_bLoaded;
	u8 *m_fileBuffer;

	MPHArcHeader *m_header;
	std::vector<MPHArcSubFileHeader*> m_subFileHeaders;
};

#endif
