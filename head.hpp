#include <iostream>
#include <stdio.h>
#include "opencv2/opencv.hpp"

using namespace std;
class IMG
{
    friend class IMG_sobel;
    private:
        vector<uchar> data;         // pixel datas, access to individual pixels - data[y*collumns+x]
        int collumns;
        int rows;
        int channels;
        
    public:
        IMG(cv::Mat &cvImg);  
        IMG(vector<uchar> &data, int collumns, int rows, int channels):                         //copy constructor   
                data{data}, collumns{collumns}, rows{rows}, channels{channels} {};                  
        const uchar pixelData(int horizontalAxis, int verticalAxis);                             
        void print();                                                                           
        void print(int w, int h);                                                                 
        void print(vector <pair <pair<int,int>,pair<int,int>>> lines);                           
        void print(int w, int h, vector <pair <pair<int,int>,pair<int,int>>> lines);           
        IMG rgbToGray()const;                                                                    
        int convolution3x3(int x, int y, vector<int> &kernel);                                   
        class IMG_sobel graySobelFilter();                                                       
        vector<pair<int,int>> findLocalMaximaPoints();
        bool isPointLocMax(int x, int y);
        vector<pair<pair<int, int>,pair<int, int>>> HougheEnumerateLines(IMG originalImage);
};

class IMG_sobel: public IMG
{
    private:
        vector<double> gradientAngle;     // sequences of 2 vals - angle and weight

    public:
        IMG_sobel(vector<uchar> &data, int collumns, int rows, int channels, vector<double> gradientAngle):
                IMG{data, collumns, rows, channels}, gradientAngle{gradientAngle} {};
        IMG houghCreate();
        void houghTransformPoint(int x, int y, int houghColls, int houghRows, vector<ulong> &houghSpacel);
        static vector<uchar> houghSpaceNormalization(vector<ulong> &space);              
};





