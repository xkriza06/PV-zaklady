#include <iostream>
#include <stdio.h>

#include "opencv2/opencv.hpp"


using namespace std;
class IMG
{
    private:
        vector<uchar> data;
        size_t numOfPixels;
        int collumns;
        int rows;
        int channels;


    public:
        IMG(cv::Mat cvImg);
        const pair<uchar*,int> pixel(uint x, uint y);
        void print();
        void print(int w, int h);
        void rgbToGray();
};