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
        IMG(cv::Mat &cvImg);
        IMG(vector<uchar> &data, size_t numOfPixels, int collumns, int rows, int channels):
                data{data}, numOfPixels{numOfPixels}, collumns{collumns}, rows{rows}, channels{channels} {};
        const pair<uchar*,int> pixel(uint horizontalAx, uint verticalAx);
        void print();
        void print(int w, int h);
        
        IMG rgbToGray()const;
        int convolution3x3(int x, int y, vector<int> &kernel);
        IMG graySobelFilter();
};
