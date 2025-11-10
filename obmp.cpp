

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

	int width = infoHeader.width;
	int height = infoHeader.height;

	int rowSize = (width * 3 + 3) & ~3;
	int padding = rowSize - width * 3;

	pixels.resize(width * height);

    in.seekg(fileHeader.bfOffBits, ios::beg);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
			in.read(reinterpret_cast<char*>(&pixels[y*width+x]),sizeof(BITMAP_COLORTABLE));
		in.seekg(padding, ios::cur);
	}
}
void OpenBMP::sepia(const string& method){
    PresentSepia::Style style=PresentSepia::Style::CLASSIC;
    if(method=="classic"){
        style=PresentSepia::Style::CLASSIC;
        cout<<"selected classic";
    }
    else if(method=="warm"){
        style=PresentSepia::Style::WARM;
    }
    else if(method=="cold"){
        style=PresentSepia::Style::COOL;
    }
    else if(method=="uranium"){
        style=PresentSepia::Style::URANIUM;
    }
    else  if(method=="photo"){
        style=PresentSepia::Style::PHOTO;
    }
    else if(method=="antiquar"){
        style=PresentSepia::Style::ANTIQ;
    }
    else
        throw NotFoundMethodError();

    PresentSepia present(style);
    for(auto& pxl:pixels){
        int changeR=(int)(present.red_red*pxl.red+present.red_green*pxl.green+present.red_blue*pxl.blue);
        int changeG=(int)(present.green_red*pxl.red+present.green_green*pxl.green+present.green_blue*pxl.blue);
        int changeB=(int)(present.blue_red*pxl.red+present.blue_green*pxl.green+present.blue_blue*pxl.blue);
        pxl.red=(uint8_t) clamp(changeR,0,255);
        pxl.green=(uint8_t) clamp(changeG,0,255);
        pxl.blue=(uint8_t) clamp(changeB,0,255);
        
     
        
    }
}
void OpenBMP::invert(const std::string& method){

	if (method == "arithmetic")
	    for(auto& pxl: pixels)
	    {
	        pxl.red=255-pxl.red;
	        pxl.green=255-pxl.green;
	        pxl.blue=255-pxl.blue;
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
void OpenBMP::Redimg(){
    for(auto& pxl:pixels){
        pxl.red=255;
        pxl.green=0;
        pxl.blue=0;
    }
}

void OpenBMP::grayscale()
{
    for(auto& pxl: pixels)
    {
        uint8_t transGray = AForRed * pxl.red + BForGreen * pxl.green + CForBlue * pxl.blue;

        pxl.red = transGray;
        pxl.green = transGray;
        pxl.blue = transGray;
        
    }
}

OpenBMP OpenBMP::rgb2gray()
{
	OpenBMP obmp = *this;
	obmp.grayscale();
	return obmp;
}

void OpenBMP::mirror(const std::string& method)
{
	int width = infoHeader.width;
	int height = infoHeader.height;
	if (method == "vertical")
	{
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width / 2; x++)
			{
				std::swap(
					pixels[y * width + x], pixels[y * width + (width - 1 - x)]
				);
			}
	}
	else
		throw NotFoundMethodError();
}

void OpenBMP::save(const string& filename)
{
    std::ofstream out(filename, std::ios::binary);
    
    if(!out.is_open())
        throw OpenError();
    
    out.write(reinterpret_cast<char*>(&fileHeader),sizeof(fileHeader));
    out.write(reinterpret_cast<char*>(&infoHeader),sizeof(infoHeader));

    int width = infoHeader.width;
    int height = infoHeader.height;
    int rowSize = (width * 3 + 3) & ~3;
    int padding = rowSize - width * 3;

    out.seekp(fileHeader.bfOffBits,std::ios::beg);

    for(int y = 0; y < height; y++)
    {
        for(int x = 0;x < width; x++)
            out.write(reinterpret_cast<char*>(&pixels[y*width+x]),sizeof(BITMAP_COLORTABLE));
            
        for(int i = 0;i < padding; i++)
            out.put(0);
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
