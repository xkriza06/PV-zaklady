#include "head.hpp"
using namespace std;
using namespace cv;

const auto REDcomponent = 0.3;
const auto GREENcomponent = 0.6;
const auto BLUEcomponent = 0.1;

/* some text
*/
IMG::IMG(Mat cvImg):numOfPixels{cvImg.total()}, collumns{cvImg.cols}, rows{cvImg.rows}, channels{cvImg.channels()}
{
    auto size = numOfPixels*cvImg.elemSize();
    for(uint i=0; i<size;++i)
    {
        this->data.push_back(cvImg.data[i]);
    }
}

void IMG::print()
{
    auto cvType = CV_MAKETYPE(CV_8U, channels);
    cv::Mat frame(rows, collumns, cvType, &(data[0]));
    imshow("Display Image", frame);
}

void IMG::print(int w, int h)
{
    auto cvType = CV_MAKETYPE(CV_8U, channels);
    cv::Mat frame(rows, collumns, cvType, &(data[0]));
    cv::resize(frame, frame, cv::Size(w, h));
    imshow("Display Image", frame);
}

const pair<uchar*,int> IMG::pixel(uint x, uint y)
{
    if(x<collumns && y<rows)
    {
        return {&(this->data[rows*collumns*y+x]),channels};
    }
    else
    {
        throw 1;
    }
}

void IMG::rgbToGray()
{
    vector<uchar>grayData;
    for (int i=0; i<numOfPixels*channels; i+=channels) 
    {
        grayData.push_back(BLUEcomponent*data[i]+
                            GREENcomponent*data[i+1]+
                            REDcomponent*data[i+2]);
    }
    data=grayData;
    channels=1;
}