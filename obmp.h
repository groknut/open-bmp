
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

struct PresentSepia
{
	enum class Style
	{
		CLASSIC,
		WARM,
		COOL,
		URANIUM,
		PHOTO,
		ANTIQ
	};

	float red_red, red_green, red_blue;
	float green_red, green_green, green_blue;
	float blue_red, blue_green, blue_blue;

	PresentSepia(Style style);
	
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

    const std::string ASCII=".:!@#$%^&*+";

    int clamp(const int& val, const int&  min, const int& max);

    const int CONTRAST_MATRIX[3][3]={
        {0,-1,0},
        {-1,5,-1},
        {0,-1,0}
    };

	int map(int val, int from_min, int from_max, int to_min,  int to_max);

public:
    
	// class constructor
	OpenBMP(const std::string& filename);
	OpenBMP() {};

	vector<BITMAP_COLORTABLE> get_pxl() const { return pixels; }

	// info .bmp file
	std::pair<int, int> shape() const;

	// invert methods
	void invert(const std::string& method);
	OpenBMP arith_invert();
	OpenBMP bitwise_not();

	// grayscale
	void grayscale();
	OpenBMP rgb2gray();

	// bitwise_methods
	void bitwise_and(const OpenBMP& other);
	void bitwise_or(const OpenBMP& other);

	// mirror method
	void mirror(const std::string& method);

	// void to_ascii(const int& ascii_width);
	vector<vector<char>> to_ascii(const int& ascii_width);
	void print_ascii(const int& width);
    
    void sepia(const std::string& method);
    void Redimg();
    void sharpen();    
	void save(const std::string& filename);
    void checkBits();
    void Image_to_ascii(int out_width);
};

class OpenError {};
class FormatError {};
class NotFoundMethodError {};
class ShapeError {};

ostream& operator << (ostream& out, const std::pair<int ,int>& shape);

#endif
