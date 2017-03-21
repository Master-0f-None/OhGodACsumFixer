// Copyright (c) 2017 OhGodACompany - OhGodAGirl & OhGodAPet

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ATOM_CSUM_OFFSET		0x21

int main(int argc, char **argv)
{
	FILE *VBIOSFile;
	uint8_t *VBIOS, CSum = 0;
	size_t VBIOSSize, BytesRead;
	
	if(argc != 2)
	{
		printf("Usage: %s <VBIOS file>\n", argv[0]);
		return(1);
	}
	
	VBIOSFile = fopen(argv[1], "rb+");
	
	if(!VBIOSFile)
	{
		printf("Cannot open VBIOS image %s (does it exist?)\n", argv[1]);
		return(1);
	}
	
	fseek(VBIOSFile, 0UL, SEEK_END);
	VBIOSSize = ftell(VBIOSFile);
	rewind(VBIOSFile);
	
	VBIOS = (uint8_t *)malloc(sizeof(uint8_t) * VBIOSSize);
	
	BytesRead = fread(VBIOS, sizeof(uint8_t), VBIOSSize, VBIOSFile);
	
	if(BytesRead != VBIOSSize)
	{
		printf("Could not read entire VBIOS image.\n");
		fclose(VBIOSFile);
		free(VBIOS);
		return(1);
	}
		
	for(int i = 0; i < (VBIOS[0x02] * 512); ++i) CSum += VBIOS[i];
		
	if(!CSum)
	{
		printf("Checksum already valid (0x%02X), nothing to fix.\n", VBIOS[ATOM_CSUM_OFFSET]);
		fclose(VBIOSFile);
		free(VBIOS);
		return(0);
	}
	
	printf("Checksum invalid, fixing...\n");
		
	VBIOS[ATOM_CSUM_OFFSET] -= CSum;
	
	fseek(VBIOSFile, ATOM_CSUM_OFFSET, SEEK_SET);
	
	if(fwrite(VBIOS + ATOM_CSUM_OFFSET, sizeof(uint8_t), 1, VBIOSFile) != 1)
	{
		printf("Error writing to VBIOS image.\n");
		fclose(VBIOSFile);
		free(VBIOS);
		return(1);
	}
	
	fclose(VBIOSFile);
	free(VBIOS);
	return(0);
}
