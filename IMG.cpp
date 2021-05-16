#include "head.hpp"
using namespace std;
using namespace cv;

const auto REDcomponent = 0.3;
const auto GREENcomponent = 0.6;
const auto BLUEcomponent = 0.1;


/* some text
*/
IMG::IMG(Mat &cvImg):numOfPixels{cvImg.total()}, collumns{cvImg.cols}, rows{cvImg.rows}, channels{cvImg.channels()}
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

const pair<uchar*,int> IMG::pixel(uint horizontalAx, uint verticalAx)
{
    if(horizontalAx>collumns || verticalAx>rows)
    {
        cout<<horizontalAx<<" "<<verticalAx<<"\n";
        cout<<collumns<<" "<<rows<<"\n";
        throw 1;
    }
    else
    {
        return {&(data[collumns*verticalAx+horizontalAx]),channels};
    }
}

IMG IMG::rgbToGray() const
{
    vector<uchar>grayData;
    for (int i=0; i<numOfPixels*channels; i+=channels) 
    {
        grayData.push_back(BLUEcomponent*data[i]+
                            GREENcomponent*data[i+1]+
                            REDcomponent*data[i+2]);
    }
    return {grayData,numOfPixels,collumns,rows,1};
}

int IMG::convolution3x3(int x, int y, vector<int> &kernel)
{
    int conv = 0;
    int kernelIndex=0;
    for(int verticalAx = (y-1); verticalAx<=(y+1); ++verticalAx)
    {
        for(int horizontalAx = (x-1); horizontalAx<=(x+1); ++horizontalAx)
        {
            if(verticalAx<0 || verticalAx>rows || horizontalAx<0 || horizontalAx>collumns)
            {
                continue;
            }
            else
            {
                kernelIndex= 3*(horizontalAx-(x-1))+(verticalAx-(y-1));
                conv+=(*(pixel(horizontalAx,verticalAx).first))*kernel[kernelIndex];
            }  
        }
    }
    return conv;
}


IMG IMG::graySobelFilter()
{
    if(channels!=1)
    {
        throw 1;
    }

    vector<uchar>sobelData;
    vector<int> kernelX{1,2,1,0,0,0,-1,-2,-1};
    vector<int> kernelY{1,0,-1,2,0,-2,1,0,-1};

    int convolutionX;
    int convolutionY;
    uchar convolutionFinal;

    for(int y=0; y<rows; ++y)
    {
        for(int x=0; x<collumns; ++x)
        {
           convolutionX = convolution3x3(x,y,kernelX); 
           convolutionY = convolution3x3(x,y,kernelY); 
           convolutionFinal = unsigned(sqrt(convolutionX*convolutionX + convolutionY*convolutionY));
         //  cout<<int(convolutionFinal)<<"\n";
           sobelData.push_back(convolutionFinal);

        // sobelData.push_back(convolutionY);
        }
    }
    return {sobelData,numOfPixels,collumns,rows,channels};
}