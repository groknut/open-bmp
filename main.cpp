

#include <iostream>
#include "obmp.h"

int main(int argc, char** argv)
{

	OpenBMP obmp = OpenBMP("./sample/object.bmp");
    std::cout << "Shape(y, x): " << obmp.shape() << std::endl;

    obmp.invert("bitwise_not");
    obmp.save("./sample/invert_object.bmp");

    obmp.invert("bitwise_not");
    obmp.grayscale();
    obmp.save("./sample/grayscale_object.bmp");

	return 0;
}
