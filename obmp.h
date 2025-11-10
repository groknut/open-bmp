
#ifndef _OBMP_H_
#define _OBMP_H_

#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
using namespace std;

#pragma pack(push,1)

struct BITMAP_FILEHEADER
{
    uint16_t bfTybe;
    uint32_t bfSize;
    uint16_t bfreserved1;
    uint16_t bfreserved2;
    uint32_t bfOffBits;
};

struct BITMAP_INFOHEADER
{
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bitCount;
	uint32_t compression;
	uint32_t imageSize;
	uint32_t XpixelsPerM;
	uint32_t YpixelsPerM;
	uint32_t colorsUsed;
	uint32_t colorsImportant;
};

struct BITMAP_COLORTABLE
{
	uint8_t blue;
	uint8_t green;
	uint8_t red;	
};

#pragma pack(pop)
struct PresentSepia{
    enum class Style{
        CLASSIC, WARM, COOL, URANIUM, PHOTO, ANTIQ
    };

float red_red,red_green,red_blue;
float green_red,green_green,green_blue;
float blue_red,blue_green,blue_blue;

PresentSepia(Style style){
    switch (style){
        case Style::CLASSIC:
            red_red=0.393f; red_green=0.769f; red_blue=0.189f;
            green_red=0.349f; green_green=0.686f; green_blue=0.168f;
            blue_red=0.272f; blue_green=0.534f; blue_blue=0.131f;
            break;
        case Style::WARM:
            red_red=0.500f; red_green=0.700f; red_blue=0.100f;
            green_red=0.450f; green_green=0.600f; green_blue=0.080f;
            blue_red=0.350f; blue_green=0.400f; blue_blue=0.050f;
            break;
        case Style::COOL:
            red_red=0.343f; red_green=0.669f; red_blue=0.239f;
            green_red=0.303f; green_green=0.606f; green_blue=0.218f;
            blue_red=0.242f; blue_green=0.464f; blue_blue=0.171f;
            break;
        case Style::URANIUM:
            red_red=0.493f; red_green=0.869f; red_blue=0.089f;
            green_red=0.449f; green_green=0.786f; green_blue=0.078f;
            blue_red=0.372f; blue_green=0.634f; blue_blue=0.061f;
            break;
        case Style::PHOTO:
            red_red=0.359f; red_green=0.709f; red_blue=0.239f;
            green_red=0.319f; green_green=0.639f; green_blue=0.218f;
            blue_red=0.258f; blue_green=0.494f; blue_blue=0.171f;
            break;
        case Style::ANTIQ:
            red_red=0.333f; red_green=0.667f; red_blue=0.167f;
            green_red=0.294f; green_green=0.608f; green_blue=0.148f;
            blue_red=0.235f; blue_green=0.471f; blue_blue=0.118f;
            break;

        }

    }

};

class OpenBMP
{
private:
	BITMAP_FILEHEADER fileHeader;
	BITMAP_INFOHEADER infoHeader;
    vector<BITMAP_COLORTABLE> pixels;
    float AForRed = 0.299;
    float BForGreen = 0.587;
    float CForBlue = 0.114;
    int clamp(int value,int min, int max){
        if(value<min){
            return min;
        }
        if(value>max){
            return max;
        }
        return value;
    }
public:
    
	// class constructor
	OpenBMP(const std::string& filename);

	// info .bmp file
	std::pair<int, int> shape();

	// invert methods
	void invert(const std::string& method);
	OpenBMP arith_invert();
	OpenBMP bitwise_not();

	// grayscale
	void grayscale();
	OpenBMP rgb2gray();

	// mirror method
	void mirror(const std::string& method);
    
    void sepia(const std::string& method);
    void Redimg();
    
	void save(const std::string& filename);
    void checkBits();
};

class OpenError {};
class FormatError {};
class NotFoundMethodError {};

ostream& operator << (ostream& out, const std::pair<int ,int>& shape);

#endif
