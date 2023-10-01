// Copyright (C) 2021 Theo Niessink <theo@taletn.com>
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.

#include <windows.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

int error(const char* const msg, const int exitCode, FILE* const bin, FILE* const obj)
{
	fprintf(stderr, "Error: %s\n", msg);

	if (bin) fclose(bin);
	if (obj) fclose(obj);

	return exitCode;
}

int main(const int argc, const char* const* argv)
{
	if (argc != 5)
	{
		printf("Usage: %s <name> <alignment> <binary file> <OBJ file>\n", argv[0]);
		return 1;
	}

	const char* const name = argv[1];
	const int align = atoi(argv[2]);
	const char* const binFile = argv[3];
	const char* const objFile = argv[4];

	FILE* const bin = fopen(binFile, "rb");
	FILE* const obj = fopen(objFile, "wb");

	if (!bin || fseek(bin, 0, SEEK_END))
	{
		readErr: return error("Can't read binary file", 2, bin, obj);
	}

	const DWORD binSize = ftell(bin);
	if ((long int)binSize == -1L || fseek(bin, 0, SEEK_SET))
	{
		goto readErr;
	}

	if (!obj)
	{
		writeErr: return error("Can't write OBJ file", 3, bin, obj);
	}

	IMAGE_FILE_HEADER imgFile;
	memset(&imgFile, 0, sizeof(imgFile));

	imgFile.Machine = IMAGE_FILE_MACHINE_UNKNOWN;
	imgFile.NumberOfSections = 1;
	imgFile.TimeDateStamp = (DWORD)time(NULL);
	imgFile.PointerToSymbolTable = sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_SECTION_HEADER) + binSize;
	imgFile.NumberOfSymbols = 2;

	IMAGE_SECTION_HEADER imgSection;
	memset(&imgSection, 0, sizeof(imgSection));

	static const BYTE rdata[IMAGE_SIZEOF_SHORT_NAME] = ".rdata";
	memcpy(imgSection.Name, rdata, IMAGE_SIZEOF_SHORT_NAME);

	imgSection.SizeOfRawData = binSize;
	imgSection.PointerToRawData = sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_SECTION_HEADER);

	switch (align)
	{
		default:
		case 1: imgSection.Characteristics = IMAGE_SCN_ALIGN_1BYTES; break;
		case 2: imgSection.Characteristics = IMAGE_SCN_ALIGN_2BYTES; break;
		case 4: imgSection.Characteristics = IMAGE_SCN_ALIGN_4BYTES; break;
		case 8: imgSection.Characteristics = IMAGE_SCN_ALIGN_8BYTES; break;
		case 16: imgSection.Characteristics = IMAGE_SCN_ALIGN_16BYTES; break;
		case 32: imgSection.Characteristics = IMAGE_SCN_ALIGN_32BYTES; break;
		case 64: imgSection.Characteristics = IMAGE_SCN_ALIGN_64BYTES; break;
		case 128: imgSection.Characteristics = IMAGE_SCN_ALIGN_128BYTES; break;
		case 256: imgSection.Characteristics = IMAGE_SCN_ALIGN_256BYTES; break;
		case 512: imgSection.Characteristics = IMAGE_SCN_ALIGN_512BYTES; break;
		case 1024: imgSection.Characteristics = IMAGE_SCN_ALIGN_1024BYTES; break;
		case 2048: imgSection.Characteristics = IMAGE_SCN_ALIGN_2048BYTES; break;
		case 4096: imgSection.Characteristics = IMAGE_SCN_ALIGN_4096BYTES; break;
		case 8192: imgSection.Characteristics = IMAGE_SCN_ALIGN_8192BYTES; break;
	}

	imgSection.Characteristics |= IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;

	IMAGE_SYMBOL imgSymbol[2];
	memset(&imgSymbol, 0, sizeof(imgSymbol));

	memcpy(imgSymbol[0].N.ShortName, rdata, IMAGE_SIZEOF_SHORT_NAME);
	imgSymbol[0].SectionNumber = 1;
	imgSymbol[0].StorageClass = IMAGE_SYM_CLASS_STATIC;

	DWORD strTblSize = sizeof(DWORD);
	size_t len = strlen(name);

	if (len <= IMAGE_SIZEOF_SHORT_NAME)
	{
		memcpy(imgSymbol[1].N.ShortName, name, len);
	}
	else
	{
		imgSymbol[1].N.Name.Long = strTblSize;
		strTblSize += (DWORD)++len;
	}

	imgSymbol[1].SectionNumber = 1;
	imgSymbol[1].StorageClass = IMAGE_SYM_CLASS_EXTERNAL;

	unsigned char buf[4*1024];

	if (!(fwrite(&imgFile, sizeof(IMAGE_FILE_HEADER), 1, obj) == 1 &&
	      fwrite(&imgSection, sizeof(IMAGE_SECTION_HEADER), 1, obj) == 1))
	{
		goto writeErr;
	}

	for (size_t i = binSize; i > 0;)
	{
		size_t n = sizeof(buf);
		n = i < n ? i : n;

		if (fread(buf, 1, n, bin) != n) goto readErr;
		if (fwrite(buf, 1, n, obj) != n) goto writeErr;

		i -= n;
	}

	if (!(fwrite(&imgSymbol, sizeof(IMAGE_SYMBOL), 2, obj) == 2 &&
	      fwrite(&strTblSize, sizeof(DWORD), 1, obj) == 1 && (strTblSize <= sizeof(DWORD) ||
	      fwrite(name, 1, len, obj) == len)))
	{
		goto writeErr;
	}

	fclose(obj);
	fclose(bin);

	return 0;
}
