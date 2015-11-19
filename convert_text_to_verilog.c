/////////////////////////////////////////////////////////////////////////////////////
//
// Filename: convert__to_verilog.c
//
// Description: The purpose of this program is to transform a text document, containing
//   normal ASCII characters, into a Verilog ROM model.
//
//   Note that characters such as <CR> \n <newline> are not interpreted.  Characters in
//   the input text file are directly mapped to the characters in the character_rom.v
//   file.
//
//   If you don't like this program, go write your own.
//
//   Created: P. Athanas, Virginia Tech, April 2015.
//
/////////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 512

main(int argc, char **argv) {
   char c;
   FILE *fin, *fout;
   int idx = 0;

   if (argc != 3) {
      fprintf(stderr, "Usage: %s input_text_file  output_rom_file\n",argv[0]);
      exit(-1);
   }
   fin = fopen(argv[1], "r");
   if (fin == NULL) {
      fprintf(stderr, "Error, cannot open input file %s.  Bye.\n", argv[1]);
      exit(-1);
   }
   fout = fopen(argv[2], "w");
   if (fin == NULL) {
      fprintf(stderr, "Error, cannot open output file %s.  Bye.\n", argv[2]);
      exit(-1);
   }
   fprintf(fout, "/////////////////////////////////////////////////////////////////////////////\n");
   fprintf(fout, "//\n");
   fprintf(fout, "// FILENAME: %s\n", argv[2]);
   fprintf(fout, "//\n");
   fprintf(fout, "// CREATED BY: %s\n", argv[0]);
   fprintf(fout, "//\n");
   fprintf(fout, "// DESCRIPTION: This is a distilation an ASCII text file converted to a\n");
   fprintf(fout, "//    Verilog ROM.  This program is not perfect.\n");
   fprintf(fout, "//\n");
   fprintf(fout, "/////////////////////////////////////////////////////////////////////////////\n");
   fprintf(fout, "module textrom ( input clk, input [11:0] addr, output reg [23:0] rom);\n");
   fprintf(fout, "   always@(posedge clk) begin \n");
   fprintf(fout, "     case (addr) \n");

   while (!feof(fin)) {
      c = getc(fin);
      fprintf(fout,"      9'd%03d:  rom = 8'h%02x;\n", idx++, c);
      if (idx > MAXSIZE-1) {
         fprintf(stderr,"Warning: input text file greater than 1024 characters. Truncating.\n");
         break;
      }
   }
   fprintf(fout, "      default: rom = 8'h00;\n");
   fprintf(fout, "    endcase \n");
   fprintf(fout, "  end \n");
   fprintf(fout, "endmodule\n");
   fclose(fin);
   fclose(fout);
}
