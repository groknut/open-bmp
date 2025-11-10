

#include <iostream>
#include "obmp.h"

int main(int argc, char** argv)
{
	if (argc < 2)
		return 0;
		
	if (argc == 2)
	{
		OpenBMP obmp = OpenBMP(argv[1]);
		std::cout << "Shape(y, x): " << obmp.shape();

//		obmp.mirror("vertical");
//		OpenBMP other = obmp.rgb2gray();
        obmp.invert("arithmetic");
		obmp.save("red_object.bmp");
//		other.save("other.bmp");

	}
	
	return 0;
}
