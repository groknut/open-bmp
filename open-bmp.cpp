#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
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
	uint8_t red;
	uint8_t green;
	uint8_t blue;	
};
#pragma pack(pop)


bool ReadFile(const string& filename,BITMAP_FILEHEADER& fileheader,BITMAP_INFOHEADER& infoheader){
    ifstream InImage(filename,ios::binary);
    if(!InImage){
        cout<<"error to open BMP \n";
        return false;
    }
    InImage.read(reinterpret_cast<char*>(&fileheader),sizeof(fileheader));
    InImage.read(reinterpret_cast<char*>(&infoheader),sizeof(infoheader));
    if(fileheader.bfTybe!=0x4D42){
        cout<<"Only BMP file example \n";
        return false;
    }
    cout<<"completed \n";
    cout<<infoheader.width<<endl<<infoheader.height<<endl;
    return true;
}


