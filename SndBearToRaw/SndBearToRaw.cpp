#include <stdio.h>
#include <algorithm>
#include <windows.h>
#include <shlwapi.h>


#pragma comment(lib,"shlwapi.lib")


static int g_major = 1;
static int g_minor = 0;


int wmain(int argc, wchar_t* argv[])
{
	try
	{
		printf("Sound bear to raw converter %d.%d\n", g_major, g_minor);
		printf("Written by Gabor Somogyi\n");
		printf("Compile date %s %s\n\n", __DATE__, __TIME__);

		if (argc != 2)
		{
			printf("Usage : SndBearToRaw.exe input.bear\n");
			return -1;
		}

		printf("Opening input file: %ws...\n", argv[1]);
		FILE* inFile = NULL;
		_wfopen_s(&inFile, argv[1], L"rb");
		if (!inFile)
		{
			printf("Failed to open file for read.\n");
			return 1;
		}
		printf("OK\n");

		printf("Getting file size...\n");
		fseek(inFile, 0, SEEK_END);
		long fileSize = ftell(inFile);
		fseek(inFile, 0, SEEK_SET);
		printf("OK: %d\n", fileSize);

		printf("Reading content...\n");
		unsigned char* rawData = new unsigned char[fileSize];
		if (!rawData)
		{
			printf("Unable to allocate space for file content\n");
			fclose(inFile);
			return 1;
		}
		memset(rawData, 0xDEADCAFE, fileSize);
		if (!fread(rawData, 1, fileSize, inFile))
		{
			printf("Unable to read file content\n");
			fclose(inFile);
			return 1;
		}
		printf("OK\n");

		fclose(inFile);

		printf("Converting data...\n");
		unsigned char* outData = new unsigned char[fileSize];
		if (!outData)
		{
			printf("Unable to allocate space for file content\n");
			fclose(inFile);
			return 1;
		}
		memset(outData, 0xDEADCAFE, fileSize);
		unsigned char accomulator = 0;
		for (long i = 0; i < fileSize; ++i)
		{
			if (rawData[i] < 0x80)
				accomulator += rawData[i];
			else
				accomulator -= (rawData[i] ^ 0x80);
			outData[i] = (accomulator ^ 0x80);
		}
		printf("OK\n");

		wchar_t* fileName = _wcsdup(argv[1]);
		size_t fileNameLen = wcslen(fileName);
		PathRemoveExtension(fileName);
		wcscat_s(fileName, fileNameLen, L".raw");

		printf("Opening output file: %ws...\n", fileName);
		FILE* outFile = NULL;
		_wfopen_s(&outFile, fileName, L"wb");
		if (!outFile)
		{
			printf("Failed to open file for write.\n");
			return 1;
		}
		printf("OK\n");

		printf("Writing content...\n");
		if (!fwrite(outData, 1, fileSize, outFile))
		{
			printf("Unable to write file content\n");
			fclose(outFile);
			return 1;
		}
		printf("OK\n");

		free(fileName);

		delete[] outData;
		outData = NULL;

		delete[] rawData;
		rawData = NULL;
	}
	catch(std::exception* e)
	{
		printf("Exception caught: %s\n", e->what());
	}

	return 0;
}
