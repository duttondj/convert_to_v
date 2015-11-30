CC = gcc

CFLAGS = -m32 -g -Wall

all:	text2v bmp2v

text2v: convert_text_to_verilog.c
	$(CC) $(CFLAGS) -o text2v convert_text_to_verilog.c

bmp2v: convert_bmp_to_verilog.c
	$(CC) $(CFLAGS) -o bmp2v convert_bmp_to_verilog.c

clean:
	$(RM) text2v bmp2v
