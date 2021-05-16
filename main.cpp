#include "head.hpp"

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("bad args\n");
        return -1;
    }

    cv::Mat image;
    image = cv::imread( argv[1], 1 );
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }



    IMG img1{image};
    IMG imgGray{img1.rgbToGray()};

    IMG imgSobel{imgGray.graySobelFilter()};
   // imgSobel.print(320,400);
   imgSobel.print();

    cv::waitKey(0);
    return 0;
}
