

#include "obmp.h"

PresentSepia::PresentSepia(Style style)
{
    switch (style)
    {
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

int OpenBMP::clamp(const int& val, const int& min, const int& max)
{
	return val < min ? min : ( val > max ? max : val );
}

int OpenBMP::map(int val, int from_min, int from_max, int to_min,  int to_max)
{
	return from_max == from_min ? to_min : ( val - from_min ) * (to_max - to_min) / (from_max - from_min) + to_min;
}

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

vector<vector<char>> OpenBMP::to_ascii(const int& ascii_width) 
{
	vector<vector<char>> ascii_map;
	auto [height, width] = shape();

	int ascii_height = (ascii_width * height) / width / 2;

    ascii_map.resize(ascii_height, vector<char>(ascii_width));

	for (int dy = 0; dy < ascii_height; dy++)
	{
		for (int dx = 0; dx < ascii_width; dx++)
		{
			int img_x = (dx * width) / ascii_width;
			int img_y = height - 1 - (dy * height) / ascii_height;
			int img_idx = img_y * width + img_x;
			auto& pxl = pixels[img_idx];
            int brightness= (pxl.red + pxl.green + pxl.blue) / 3;
            int char_index = map(brightness, 0, 255, 0, ASCII.length()-1 );
            ascii_map[dy][dx] = ASCII[char_index];
		}
	}
    
	return ascii_map;
}

void OpenBMP::print_ascii(const int& ascii_width)
{
	vector<vector<char>> ascii_map = to_ascii(ascii_width);

	for (size_t y = 0; y < ascii_map.size(); y++)
	{
		for (size_t x = 0; x < ascii_map[y].size(); x++)
			std::cout << ascii_map[y][x];
		std::cout << std::endl;
	}
	
}


void OpenBMP::sepia(const string& method)
{
    PresentSepia::Style style=PresentSepia::Style::CLASSIC;
    
    if (method == "classic")
        style=PresentSepia::Style::CLASSIC;
        
    else if (method == "warm")
        style=PresentSepia::Style::WARM;
    
    else if (method == "cold")
        style=PresentSepia::Style::COOL;

    else if (method == "uranium")
        style=PresentSepia::Style::URANIUM;
    
    else  if (method == "photo")
        style=PresentSepia::Style::PHOTO;
    
    else if (method == "antiquar")
		style=PresentSepia::Style::ANTIQ;
    
    else
        throw NotFoundMethodError();

    PresentSepia present(style);

	for (auto& pxl: pixels)
	{
		int changeR = (int)(
			present.red_red * pxl.red + present.red_green * pxl.green + present.red_blue * pxl.blue
		);
		int changeG = (int)(
					present.green_red * pxl.red + present.green_green * pxl.green + present.green_red * pxl.blue
		);
		int changeB = (int)(
					present.blue_red * pxl.red + present.blue_green * pxl.green + present.blue_blue * pxl.blue
		);

		pxl.red = (uint8_t) clamp(changeR, 0, 255);
		pxl.green = (uint8_t) clamp(changeG, 0, 255);
		pxl.blue = (uint8_t) clamp(changeB, 0, 255);
		
	}
}


void OpenBMP::sharpen()
{
	OpenBMP temp = *this;

	auto [height, width] = shape();

	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			int sum_r = 0, sum_g = 0, sum_b = 0;
			for (int dy = -1; dy <= 1; dy++)
			{
				for (int dx = -1; dx <= 1; dx++)
				{
					BITMAP_COLORTABLE& neighbour = temp.pixels[(y+dy)*width+(x+dx)];
                    int weh = CONTRAST_MATRIX[dy+1][dx+1];
                    sum_r += neighbour.red * weh;
                    sum_g += neighbour.green * weh;
                    sum_b += neighbour.blue * weh;
				}
			}
			pixels[y*width+x] = {
               (uint8_t) clamp(sum_b, 0, 255),
               (uint8_t) clamp(sum_g, 0, 255),
               (uint8_t) clamp(sum_r, 0, 255)
           };
		}
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

std::pair<int, int> OpenBMP::shape() const
{
	return std::make_pair(infoHeader.height, infoHeader.width);
}

ostream& operator << (ostream& out, const std::pair<int ,int>& shape)
{
	out << "(" << shape.first << ", " << shape.second << ")";
	return out;
}


void OpenBMP::bitwise_and(const OpenBMP& other)
{
	auto [height, width] = shape();
	auto [other_height, other_weight] = other.shape();

	if (height != other_height && width != other_weight)
		throw ShapeError();

	vector<BITMAP_COLORTABLE> pxls = other.get_pxl();

	for (size_t i = 0; i < pxls.size(); i++)
	{
		pixels[i].red &= pxls[i].red;
		pixels[i].blue &= pxls[i].green;
		pixels[i].blue &= pxls[i].blue;
	}	
}
