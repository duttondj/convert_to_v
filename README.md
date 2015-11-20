# convert_to_v
Tools for converting text and bitmaps into Verilog ROM files. There were some compilation issues with the original files so I updated them to fix the errors and warnings along with adding some comments so things make some sense. I also tried to reduce some future Quartus warnings by making sure nothing would be truncated.

## Download
Change to the directory where you wish to store the files and then run:
	
	git clone https://github.com/duttondj/convert_to_v.git
or download and unzip:

	https://github.com/duttondj/convert_to_v/archive/master.zip

  
## Compile
Run:
	
	make
Binaries will be called bmp2v and text2v

## Usage
	./bmp2v input.bmp output.v
	./text2v input.txt output.v
