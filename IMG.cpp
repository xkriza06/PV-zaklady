#include "head.hpp"
#include <cmath>
using namespace std;

const auto REDcomponent = 0.3;                  // ratio of red component in white light
const auto GREENcomponent = 0.6;                // ratio of green component in white light
const auto BLUEcomponent = 0.1;                 // ratio of blue component in white light

const int EDGE_INTENSITY_THRESHOLD = 80;        // <0,255>, threshold to consider pixel as edge in IMG_sobel
const double ANGLE_RADIUS = 0.785;              // radians <0,3.14>, range of gradient angle to search lines in hough transform
const double ANGLE_STEP = 0.001;                // radians <0,3.14>, interval of creating lines in hough transform
const double RAD_TO_DEG = 57.2957795;           // convert radians to degrees
const double DEG_TO_RAD = 0.0174532925;         // convert degrees to radians

const int MAXIMA_THRESHOLD = 80;                // <0,255>, minimal intensity of pixel to be considered as local maxima of hough transformed image  
const int MAXIMA_NEIGHBOURHOOD = 10;            // square of (2*MAXIMA_NEIGHBOURHOOD+1) * (2*MAXIMA_NEIGHBOURHOOD+1) to search for local maxima in hough transformed image


/* 
    constructor from cv::Mat to IMG
*/
IMG::IMG(cv::Mat &cvImg):collumns{cvImg.cols}, rows{cvImg.rows}, channels{cvImg.channels()}
{
    for(uint i=0; i<collumns*rows*channels;++i)
    {
        this->data.push_back(cvImg.data[i]);
    }
}

/* 
    print IMG
*/
void IMG::print()
{
    auto cvType = CV_MAKETYPE(CV_8U, channels);
    cv::Mat frame(rows, collumns, cvType, &(data[0]));
    cv::imshow("Display Image", frame);
    cv::waitKey(0);
}

/* 
     print IMG in given size
*/
void IMG::print(int w, int h)
{
    auto cvType = CV_MAKETYPE(CV_8U, channels);
    cv::Mat frame(rows, collumns, cvType, &(data[0]));
    cv::resize(frame, frame, cv::Size(w, h));
    cv::imshow("Display Image", frame);
    cv::waitKey(0);
}

/* 
    print IMG with found lines
*/
void IMG::print(vector <pair <pair<int,int>,pair<int,int>>> lines)
{
    auto cvType = CV_MAKETYPE(CV_8U, channels);
    cv::Mat frame(rows, collumns, cvType, &(data[0]));
    for(auto l : lines)  
    {  
        cv::line(frame, cv::Point(l.first.first, l.first.second), cv::Point(l.second.first, l.second.second), cv::Scalar( 0, 0, 255));  
    }  
    cv::imshow("Display Image", frame);
    cv::waitKey(0);
}

/* 
    print IMG in given size with found lines
*/
void IMG::print(int w, int h, vector <pair <pair<int,int>,pair<int,int>>> lines)
{
    auto cvType = CV_MAKETYPE(CV_8U, channels);
    cv::Mat frame(rows, collumns, cvType, &(data[0]));
    for(auto l : lines)  
    {  
        cv::line(frame, cv::Point(l.first.first, l.first.second), cv::Point(l.second.first, l.second.second), cv::Scalar( 0, 0, 255));  
    }  
    cv::resize(frame, frame, cv::Size(w, h));
    cv::imshow("Display Image", frame);
    cv::waitKey(0);
}

/* 
     return value of pixel[x,y]
*/
const uchar IMG::pixelData(int horizontalAxis, int verticalAxis)
{
    if(horizontalAxis>=collumns || verticalAxis>=rows)
    {
        throw "out of range in pixelData";
    }
    else
    {
        return data[collumns*verticalAxis+horizontalAxis];
    }
}

/* 
    convert rgb IMG to grayscale
*/
IMG IMG::rgbToGray() const
{
    vector<uchar>grayData;
    for (int i=0; i<collumns*rows*channels; i+=channels) 
    {
        grayData.push_back(BLUEcomponent*data[i]+
                            GREENcomponent*data[i+1]+
                            REDcomponent*data[i+2]);
    }
    return {grayData,collumns,rows,1};
}

/* 
    return img transformed by sobel, applicable only on 1-channel-IMG
*/
IMG_sobel IMG::graySobelFilter()
{
    if(channels!=1)
    {
        throw "image must be grayscale";
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
            angles.push_back( convolutionX*convolutionY!=0 ? atan(convolutionY/convolutionX) : 0);   // to evade division by 0
            
        }
    }
    return {sobelData,collumns,rows,channels,angles};
}

/* 
    do 3x3 convolution of pixel neighbourhood and kernel values
*/
int IMG::convolution3x3(int x, int y, vector<int> &kernel)
{
    int conv = 0;
    int pixelIntensity;
    int kernelPointIntensity;

    for(int verticalAxis = (y-1); verticalAxis<=(y+1); ++verticalAxis)
    {
        for(int horizontalAxis = (x-1); horizontalAxis<=(x+1); ++horizontalAxis)
        {
            if(verticalAxis<0 || verticalAxis>=rows || horizontalAxis<0 || horizontalAxis>=collumns)
            {
                continue;
            }
            else
            {
                pixelIntensity = pixelData(horizontalAxis, verticalAxis);
                kernelPointIntensity = kernel[3*(horizontalAxis-(x-1))+(verticalAxis-(y-1))];
                conv+= pixelIntensity * kernelPointIntensity;
            }  
        }
    }
    return conv;
}

/* 
    return img transformed by hough, applicable on sobel transformed img
*/
IMG IMG_sobel::houghCreate()
{
    int longerSide = rows>collumns?rows:collumns;
    int houghRows = sqrt(2.0) * longerSide;         // pythagoras sqrt(longerSide^2 + longerSide^2), max possible r distance
    int houghColls = 180;                           // 180 degrees

    vector<ulong> houghSpaceL;
    for(int i =0; i < (houghRows * houghColls); ++i)    houghSpaceL.push_back(0);

    for(int y = 0; y<rows; ++y)
    {
        for(int x = 0; x<collumns; ++x)
        {
            houghTransformPoint(x, y, houghColls, houghRows, houghSpaceL);
        }
    }
    vector<uchar> houghSpaceUChar{houghSpaceNormalization(houghSpaceL)};
    return {houghSpaceUChar, houghColls, houghRows, channels};
}

/* 
    transform point from sobel img and insert it into a hough space
*/
void IMG_sobel::houghTransformPoint(int x, int y, int houghColls, int houghRows, vector<ulong> &houghSpaceL)
{
    double angle;
    double stopAngle;
    double r;
    double centerX = collumns/2;
    double centerY = rows/2;

    if(pixelData(x,y) > EDGE_INTENSITY_THRESHOLD )
    {
        angle = gradientAngle[y * collumns + x] - ANGLE_RADIUS/2;
        stopAngle = angle + ANGLE_RADIUS;
        while(angle < stopAngle)
        {
            r = ( (x - centerX) * cos(angle) + ((y - centerY) * sin(angle)));                                           //shifr origin to the centre
            houghSpaceL[static_cast<int>(round(r+houghRows/2) * houghColls + round(angle* RAD_TO_DEG))]++;           // r shifted by houghRows/2 because of <r,-r>
            angle+=ANGLE_STEP;
        }
    }
}

/* 
     normalize values from hough space to interval <0,255>
*/
vector<uchar> IMG_sobel::houghSpaceNormalization(vector<ulong> &space)
{
    ulong min=ULONG_LONG_MAX;
    ulong max=0;
    vector<uchar> spaceUchar;
    for(int i=0; i<=space.size(); ++i)
    {
        min= min<space[i]? min : space[i];
        max= max>space[i]? max : space[i];
    }
    for(int i=0; i<space.size(); ++i)
    {
        spaceUchar.push_back( 1.0 * (space[i]-min) / (max-min) * UCHAR_MAX);
    }
    return spaceUchar;
}

/* 
     return points that are local maximas in hough space
*/
vector<pair<int,int>> IMG::findLocalMaximaPoints()
{
    vector<pair<int,int>> localMaxima;
    bool isMax;
    for(int y = 0; y<rows; ++y)
    {
        for(int x = 0; x<collumns; ++x)
        {
            if(pixelData(x,y)>MAXIMA_THRESHOLD)
            {
                if(isPointLocMax(x,y))
                {
                    localMaxima.push_back({x,y});
                }
            } 
        }
    }
    return localMaxima;
}

/* 
     find out if point is local maxima in his neighbourhood
*/
bool IMG::isPointLocMax(int x, int y)
{
    for(int neighbourY = (y-MAXIMA_NEIGHBOURHOOD); neighbourY<=(y+MAXIMA_NEIGHBOURHOOD); ++neighbourY)
    {
        for(int neighbourX = (x-MAXIMA_NEIGHBOURHOOD); neighbourX<=(x+MAXIMA_NEIGHBOURHOOD); ++neighbourX)
        {
            if(neighbourY<0 || neighbourY>=rows || neighbourX<0 || neighbourX>=collumns)
            {
                continue;
            }
            if(neighbourX==x && neighbourY==y)
            {
                continue;
            }
            else
            {
                if(pixelData(x,y) <= pixelData(neighbourX,neighbourY))
                {
                    return false;
                }
            } 
        }
    }
    return true;
}

/* 
     return pairs of points determining line adjusted for given IMG
*/
vector <pair <pair<int,int>,pair<int,int>>>IMG::HougheEnumerateLines(IMG originalImage)
{
    int x1, y1, x2, y2;    
    int r_shiftedToVerticalCenter;
    double angleInRad;
    int origImgVerticalCentre   = originalImage.rows/2;
    int origImgHorizontalCentre = originalImage.collumns/2;
    vector <pair <pair<int,int>,pair<int,int>>> lines;

    vector<pair<int,int>> localMaximas = findLocalMaximaPoints();
    for(auto houghPoint : localMaximas)
    {
        r_shiftedToVerticalCenter = houghPoint.second - (rows/2);
        angleInRad          = houghPoint.first * DEG_TO_RAD;
        
        if(houghPoint.first >= 45 && houghPoint.first <= 135)                   // if line rises more in horizontal of vertical direction
        {  
            //y = (r - x cos(t)) / sin(t)  
            x1 = 0;  
            x2 = originalImage.collumns;  
            y1 = (r_shiftedToVerticalCenter - ((x1 - origImgHorizontalCentre) * cos(angleInRad))) / sin(angleInRad) + origImgVerticalCentre;  
            y2 = (r_shiftedToVerticalCenter - ((x2 - origImgHorizontalCentre) * cos(angleInRad))) / sin(angleInRad) + origImgVerticalCentre;  
        }  
        else  
        {  
            //x = (r - y sin(t)) / cos(t);  
            y1 = 0;  
            y2 = originalImage.rows;
            x1 = (r_shiftedToVerticalCenter - ((y1 - origImgVerticalCentre) * sin(angleInRad))) / cos(angleInRad) + origImgHorizontalCentre;  
            x2 = (r_shiftedToVerticalCenter - ((y2 - origImgVerticalCentre) * sin(angleInRad))) / cos(angleInRad) + origImgHorizontalCentre;  
        }  
        lines.push_back({pair<int, int>(x1,y1),pair<int, int>(x2,y2)});
    }
    return lines;  
}