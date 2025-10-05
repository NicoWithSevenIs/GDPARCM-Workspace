#include "RTImage.h"

RTImage::RTImage(const int imageWidth, const int imageHeight): imageWidth(imageWidth), imageHeight(imageHeight)
{
	this->pixels = std::make_unique<cv::Mat>(cv::Mat::zeros(this->imageHeight, this->imageWidth, CV_8UC3));

}

void RTImage::setPixel(int x, int y, float r, float g, float b, int samplesPerPixel)
{

    /*
    // gamma correction
    float scale = 1.0f / samplesPerPixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);
    */
   
    int rInt = int(255.999 * r);
    int gInt = int(255.999 * g);
    int bInt = int(255.999 * b);

    cv::Mat imgChannels[3];
    cv::split(*this->pixels, imgChannels);

    imgChannels[0].at<uchar>(this->imageHeight - 1 - y, x) = bInt;
    imgChannels[1].at<uchar>(this->imageHeight - 1 - y, x) = gInt;
    imgChannels[2].at<uchar>(this->imageHeight - 1 - y, x) = rInt;

    cv::merge(imgChannels, 3, *this->pixels);
}


void RTImage::saveImage(cv::String& fileName) const
{
	cv::imwrite(fileName, *this->pixels);
}
