////////////////////////////////////////////////////////////////////////
// 
// Filename: convert_bmp_to_verilog.c
//
// Description: The purpose of this program is to transform a graphics image 
//   in BMP format into a Verilog ROM model.  This program is narrowly constrained
//   to accept BMPs that:
//    - are constrained to 64 x 64 pixels
//    - are coded with RGB pixels, 8-bits each, for 24 bits per pixel
//    - no compression or other fancy options.
//   This program is a concatenation of various code fragments posted on newsgroups.
//
//   Note: if you don't like this program, write your own.
//
//   Modified: P. Athanas, Virginia Tech, April 2015.
//   Modified: Danny Dutton, Virginia Tech, Nov 2015
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
 
 
typedef struct {
		unsigned short type;             /* Magic identifier            */
		unsigned int size;               /* File size in bytes          */
		unsigned int reserved;
		unsigned int offset;             /* Offset to image data, bytes */
} HEADER;
typedef struct {
		unsigned int size;               /* Header size in bytes      */
		int width,height;                /* Width and height of image */
		unsigned short planes;           /* Number of colour planes   */
		unsigned short bits;             /* Bits per pixel            */
		unsigned int compression;        /* Compression type          */
		unsigned int imagesize;          /* Image size in bytes       */
		int xresolution,yresolution;     /* Pixels per meter          */
		unsigned int ncolours;           /* Number of colours         */
		unsigned int importantcolours;   /* Important colours         */
} INFOHEADER;
typedef struct {
		unsigned char r,g,b,junk;
} COLOURINDEX;
 
 
int main(int argc, char *argv[]){
	int i,j,rd;
	int gotindex = 0;
	unsigned char grey,r,g,b;
	double ampl;
	short _2byte[2];
	HEADER header;
	INFOHEADER infoheader;
	COLOURINDEX colourindex[256];
	FILE *fptr;    
	char module[128];
	char vhfile[128];
	FILE *fvh;
   
	void (*bmpread)();
	
	// 8bit images
	void _eightbit()
	{
		if(fread(&grey, sizeof(unsigned char), 1, fptr) != 1)
		{
			fprintf(stderr,"Image read failed\n");
			exit(-1);
		}

		if (gotindex)
		{
			ampl =  colourindex[grey].r * 64. +
					colourindex[grey].g * 128.+
					colourindex[grey].b * 64.;
		}

		else
		{
			ampl = grey * 256. - 32768.;
		}
		// printf("%.2f\n", ampl);
	}
	
	// 24bit images
	void _twentyfourbit()
	{
		do{
			if((rd = fread(&b, sizeof(unsigned char), 1, fptr)) != 1) break;
			if((rd = fread(&g, sizeof(unsigned char), 1, fptr)) != 1) break;
			if((rd = fread(&r, sizeof(unsigned char), 1, fptr)) != 1) break;
		}while(0);
		
		if(rd != 1)
		{   
			fprintf(stderr,"Image read failed\n");
			exit(-1);
		}
		// printf("%.2f\n", ampl);
	}
	
	// Try to open input file
	printf("Input file: %s\n", argv[1]);
	if((fptr = fopen(argv[1],"r")) == NULL)
	{
		fprintf(stderr,"Unable to open BMP file \"%s\"\n",argv[1]);
		exit(-1);
	}
			/* Read and check BMP header */
	if(fread(&header.type, 2, 1, fptr) != 1)
	{
		fprintf(stderr, "Failed to read BMP header\n");
		exit(-1);
	}
	if(header.type != 'M'*256+'B')
	{
		fprintf(stderr, "File is not bmp type\n");
		exit(-1);
	}
	
	do{
		if((rd = fread(&header.size, 4, 1, fptr)) != 1) break;
		printf("File size: %d bytes\n", header.size);
		if((rd = fread(&header.reserved, 4, 1, fptr)) != 1) break;
		if((rd = fread(&header.offset, 4, 1, fptr)) != 1) break;
		printf("Offset to image data is %d bytes\n", header.offset);
	}while(0);
	

	if(rd != 1)
	{
		fprintf(stderr, "Error reading file\n");
		exit(-1);
	}
	
	/* Read and check the information header */
	if (fread(&infoheader, sizeof(INFOHEADER), 1, fptr) != 1)
	{
		fprintf(stderr,"Failed to read BMP info header\n");
		exit(-1);
	}
	
	// Print out some stats
	printf("Image size = %d x %d\n", infoheader.width, infoheader.height);
	printf("Number of colour planes is %d\n", infoheader.planes);
	printf("Bits per pixel is %d\n", infoheader.bits);
	printf("Compression type is %d\n", infoheader.compression);
	printf("Number of colours is %d\n", infoheader.ncolours);
	printf("Number of required colours is %d\n", infoheader.importantcolours);
	
	/* Read the lookup table if there is one */
	for (i=0; i<255; i++)
	{
		colourindex[i].r = rand() % 256;
		colourindex[i].g = rand() % 256;
		colourindex[i].b = rand() % 256;
		colourindex[i].junk = rand() % 256;
	}
	
	if (infoheader.ncolours > 0)
	{
		for (i=0; i<infoheader.ncolours; i++)
		{
			do{
				if ((rd = fread(&colourindex[i].b, sizeof(unsigned char),1,fptr)) != 1)
					break;
				if ((rd = fread(&colourindex[i].g, sizeof(unsigned char),1,fptr)) != 1)
					break;
				if ((rd = fread(&colourindex[i].r, sizeof(unsigned char),1,fptr)) != 1)
					break;
				if ((rd = fread(&colourindex[i].junk, sizeof(unsigned char),1,fptr)) != 1)
					break;
			}while(0);
			
			if(rd != 1)
			{
				fprintf(stderr,"Image read failed\n");
				exit(-1);
			}                      
			
			printf("%3d\t%3d\t%3d\t%3d\n", i,
			colourindex[i].r, colourindex[i].g, colourindex[i].b);
		}
		gotindex = 1;
	}
	
	if(infoheader.bits < 8)
	{
		printf("Too small image map depth (%d < 8)\n", infoheader.bits);
		exit(-1);
	}
	
	/* Seek to the start of the image data */
	fseek(fptr, header.offset, SEEK_SET);
	bmpread = _eightbit;
	if(infoheader.bits == 24)
	{
		bmpread = _twentyfourbit;
	}
	
	else
	{
	   fprintf(stderr,"Error, this BMP is not 24-bit color.  Convert before running this program.\n");
	   exit(-1);
	}
   
	/* Read the image */
	for (i=0; i<strlen(argv[1]); i++)
	{
		vhfile[i] = argv[1][i];
		if (argv[1][i] == '.') break;
	}
	
	/* Create Verilog ROM file */
	/* Note that according to the BMP spec, the first pixel (0,0) is on the lower left
	 * corner.  Do the transformation so that (0,0) is at the upper left corner. */
	vhfile[i] = 0;
	strcpy(module, vhfile);
	strcat(vhfile, ".v");
	printf("Output file: %s\n", vhfile);
	fvh = fopen(vhfile, "w");
	fprintf(fvh, "/////////////////////////////////////////////////////////////////////////////\n");
	fprintf(fvh, "//\n");
	fprintf(fvh, "// FILENAME: %s\n", vhfile);
	fprintf(fvh, "//\n");
	fprintf(fvh, "// CREATED BY: %s\n", argv[0]);
	fprintf(fvh, "//\n");
	fprintf(fvh, "// DESCRIPTION: This is a distilation of a BMP file with 24-bit pixels, converted\n");
	fprintf(fvh, "//    into a Verilog header file in ROM format.  Each line consistes of a 24-bit\n");
	fprintf(fvh, "//    (r,g,b) point.\n");
	fprintf(fvh, "//\n");
	fprintf(fvh, "// IMAGE SOURCE: %s, 24-bit pixels (R,G,B), size = %d x %d\n",argv[1], 
		infoheader.width, infoheader.height);
	fprintf(fvh, "//\n");
	fprintf(fvh, "/////////////////////////////////////////////////////////////////////////////\n");
	fprintf(fvh, "module %s( input clk, input [11:0] addr, output reg [23:0] rom);\n", module);
	fprintf(fvh, "   always@(posedge clk) begin\n");
	fprintf(fvh, "      case (addr)\n");

	for (j=0;j<infoheader.height;j++)
	{
		fprintf(fvh, "         // ROW %d\n",j);
		
		for (i=0;i<infoheader.width;i++)
		{
			bmpread();
			fprintf(fvh, "         12'h%04x:  rom = 24'h",i+infoheader.height*(infoheader.height-j -1));
			fprintf(fvh, "%02x%02x%02x;\n",r,g,b);
		} // i
	} // j
	fprintf(fvh, "      endcase\n   end\nendmodule\n");
	
	fclose(fvh);
	fclose(fptr);
}
