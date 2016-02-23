//============== Copyright lololol, All rights reserved. ==============//
//
// Purpose:		main
//
// $NoKeywords: $
//=====================================================================//

#include "MPHArcFile.h"

int main(int argc, const char* argv[])
{
	if (argc < 2 || argc > 3)
	{
		printf("Invalid arguments.\n");
		printf("	Usage: mpharc.exe <filename.arc>\n");
		printf("	       mpharc.exe <filename.arc> <path/to/output/directory/>\n");
		printf("\nExample: mpharc.exe unit2_C0.arc mph/extractedModels/\n");
		return 0;
	}

	MPHArcFile arc;
	arc.load(argv[1]);
	arc.extract(argc == 3 ? argv[2] : "");

	return 0;
}
