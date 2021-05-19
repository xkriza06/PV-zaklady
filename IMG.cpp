#include "head.hpp"
#include <cmath>
using namespace std;
using namespace cv;

const auto REDcomponent = 0.3;
const auto GREENcomponent = 0.6;
const auto BLUEcomponent = 0.1;

const int EDGE_INTENSITY_TRESHOLD = 150;
const double ANGLE_RADIUS = 0.785;
const double ANGLE_STEP = 0.001;
const double RAD_TO_DEG = 57.2957795;

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

const pair<uchar*,int> IMG::pixel(uint horizontalAxis, uint verticalAxis)
{
    if(horizontalAxis>collumns || verticalAxis>rows)
    {
        cout<<horizontalAxis<<" "<<verticalAxis<<"\n";
        cout<<collumns<<" "<<rows<<"\n";
        throw 1;
    }
    else
    {
        return {&(data[collumns*verticalAxis+horizontalAxis]),channels};
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
    for(int verticalAxis = (y-1); verticalAxis<=(y+1); ++verticalAxis)
    {
        for(int horizontalAxis = (x-1); horizontalAxis<=(x+1); ++horizontalAxis)
        {
            if(verticalAxis<0 || verticalAxis>rows || horizontalAxis<0 || horizontalAxis>collumns)
            {
                continue;
            }
            else
            {
                kernelIndex= 3*(horizontalAxis-(x-1))+(verticalAxis-(y-1));
                conv+= (*(pixel(horizontalAxis,verticalAxis).first)) * kernel[kernelIndex];
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
    vector<double>angles;
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

            sobelData.push_back(convolutionFinal);
            if(convolutionX*convolutionY != 0)
            {
              //  int ang = static_cast<int>(atan(convolutionY/convolutionX)*180/3.14);   //angle from radians to degrees
                angles.push_back(atan(convolutionY/convolutionX));
            }
            else
            {
                angles.push_back(0);
            }
            
        }
    }
    return {sobelData,numOfPixels,collumns,rows,channels,angles};
}


void IMG::hough(vector<uchar> &houghSpace)
	{
        int longerSide = rows>collumns?rows:collumns;
		int houghRows = sqrt(2.0) * longerSide;         // pythagoras sqrt(longerSide^2 + longerSide^2), max possible r distance
		int houghColls = 180;                           // 180 degrees

        double angle;
        double stopAngle;
        double r;

        houghSpace.clear();
        for(int i =0; i < (houghRows * houghColls);++i)
        {
            houghSpace.push_back(0);
        }

		double centerHorizontal = collumns/2;
		double centerVertical = rows/2;


		for(int vertical = 0; vertical<rows; ++vertical)
		{
			for(int horizontal = 0; horizontal<collumns; ++horizontal)
			{
				if( data[(vertical*collumns) + horizontal] > EDGE_INTENSITY_TRESHOLD )
				{
                    angle = gradientAngle[vertical * collumns + horizontal] - ANGLE_RADIUS/2;
                    stopAngle = angle + ANGLE_RADIUS;
                    while(angle < stopAngle)
					{
						r = ( (horizontal - centerHorizontal) * cos(angle) + ((vertical - centerVertical) * sin(angle)));   //shifr origin to the centre
						houghSpace[round(r + houghRows/2) * houghColls + angle * RAD_TO_DEG]++;           // r shifted by houghRows/2 because of <r,-r>
                        angle+=ANGLE_STEP;
					}
				}
			}
		}



        cv::Mat frame(houghRows, houghColls, CV_8UC1, &(houghSpace[0]));
        imshow("Display Image", frame);
        cv::waitKey(0);

	}

   