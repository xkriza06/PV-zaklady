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
    IMG_sobel imgSobel{imgGray.graySobelFilter()};
    IMG imgHough{imgSobel.houghCreate()};
  
    img1.print(1280,720);
    imgGray.print(1280,720);
    imgSobel.print(1280,720);
    imgHough.print(1280,720);
    img1.print(1280,720, imgHough.HougheEnumerateLines(img1));

    return 0;
}
