

#include "obmp.h"

OpenBMP::OpenBMP(const std::string& filename)
{
	std::ifstream in(filename, ios::binary);

	if (!in.is_open())
		throw OpenError();

	in.read(
		reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader)		
	);
	
	in.read(
		reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader) 
	);

	if (fileHeader.bfTybe != 0x4D42)
		throw OpenError();

	if (infoHeader.bitCount != 24)
		throw FormatError();
        int width=infoHeader.width;
        int height=infoHeader.height;
        int rowSize=(width*3+3)& ~3;
        int padding=rowSize-width*3;
        pixels.resize(width*height);
        in.seekg(fileHeader.bfOffBits,ios::beg);
        for(int y=0;y<height;y++){
            for(int x=0;x<width;x++){
                in.read(reinterpret_cast<char*>(&pixels[y*width+x]),sizeof(BITMAP_COLORTABLE));
            }
            in.seekg(padding,ios::cur);
        }
}

void OpenBMP::invert(const std::string& method){

	if (method == "arithmetic")
	    for(auto& pixel:pixels){
	        pixel.red=255-pixel.red;
	        pixel.green=255-pixel.green;
	        pixel.blue=255-pixel.blue;
		}
	else if (method == "bitwise_not")
		for (auto& pxl: pixels)
		{
			pxl.red = ~pxl.red & 0xff;
			pxl.green = ~pxl.green & 0xff;
			pxl.blue = ~pxl.blue & 0xff;
		}
	else
		throw NotFoundMethodError();
}

OpenBMP OpenBMP::arith_invert(){
	OpenBMP obmp = *this;
	obmp.invert("arithmetic");
	return obmp;
}

OpenBMP OpenBMP::bitwise_not()
{
	OpenBMP obmp = *this;
	obmp.invert("bitwise_not");
	return obmp;
}

void OpenBMP::negativeGrayImage(){
    for(auto& pixel:pixels){
        uint8_t transGray=AForRed*pixel.red+BForGreen*pixel.green+CForBlue*pixel.blue;
        pixel.red=transGray;
        pixel.green=transGray;
        pixel.blue=transGray;
    }
}
void OpenBMP::Mirrorvertical(){
    int width=infoHeader.width;
    int height=infoHeader.height;
    for(int y=0;y<height;y++){
        for(int x=0;x<width/2;x++){
            int left_idx=y*width+x;
            int right_idx=y*width+(width-1-x);
            std::swap(pixels[left_idx],pixels[right_idx]);
        }
    }
}
void OpenBMP::saveImage(const string& filename){
    std::ofstream out(filename,std::ios::binary);
    if(!out.is_open()){
        throw OpenError();
    }
    out.write(reinterpret_cast<char*>(&fileHeader),sizeof(fileHeader));
    out.write(reinterpret_cast<char*>(&infoHeader),sizeof(infoHeader));
    int width=infoHeader.width;
    int height=infoHeader.height;
    int rowSize=(width*3+3)&~3;
    int padding=rowSize-width*3;
    out.seekp(fileHeader.bfOffBits,std::ios::beg);
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            out.write(reinterpret_cast<char*>(&pixels[y*width+x]),sizeof(BITMAP_COLORTABLE));
        }
        for(int i=0;i<padding;i++){
            out.put(0);
        }
    }
}
std::pair<int, int> OpenBMP::shape()
{
	return std::make_pair(infoHeader.height, infoHeader.width);
}

ostream& operator << (ostream& out, const std::pair<int ,int>& shape)
{
	out << "(" << shape.first << ", " << shape.second << ")";
	return out;
}
