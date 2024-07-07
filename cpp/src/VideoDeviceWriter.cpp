/*
 * VideoDeviceWriter.cpp
 *
 *  Created on: Jul. 6, 2024
 *      Author: liam
 */

#include "VideoDeviceWriter.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <iostream>

VideoDeviceWriter::VideoDeviceWriter() : _isInitialized(false), _videoFileDescriptor(-1)
{

}

VideoDeviceWriter::~VideoDeviceWriter()
{
    this->Close();
}

bool
VideoDeviceWriter::Initialize(const int width, const int height, const std::string &videoDeviceName)
{
    if(videoDeviceName.empty())
    {
       std::cout << "[ERROR] No video device provided" << std::endl;
       return false;
    }
    if(access(videoDeviceName.c_str(), F_OK | W_OK) == -1)
    {
       std::cout << "[ERROR] Video Output: " << videoDeviceName << " does not exist, errno:" << errno << std::endl;
       return false;
    }
    if(this->_isInitialized)
    {
       close(this->_videoFileDescriptor);
       this->_isInitialized = false;
    }
    this->_videoFileDescriptor = open(videoDeviceName.c_str(), O_WRONLY);

    if(_videoFileDescriptor < 0)
    {
       std::cout << "[ERROR] Could not open video device: " << videoDeviceName << std::endl;
       return false;
    }
    else
    {
       struct v4l2_format v4l2Format;
       int formatSuccess = -1;
       v4l2Format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
       formatSuccess = ioctl(this->_videoFileDescriptor, VIDIOC_G_FMT, &v4l2Format);
       if(formatSuccess < 0)
       {
          std::cout << "[ERROR] Could not initialize video device" << std::endl;
          close(this->_videoFileDescriptor);
          this->_videoFileDescriptor = -1;
          return false;
       }
       v4l2Format.fmt.pix.width = width;
       v4l2Format.fmt.pix.height = height;
       v4l2Format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
       v4l2Format.fmt.pix.sizeimage = width * height * 3; // 3 colour channels
       formatSuccess = ioctl(this->_videoFileDescriptor, VIDIOC_S_FMT, &v4l2Format);
       if(formatSuccess < 0)
       {
          std::cout << "[ERROR] Could not initialize video device" << std::endl;
          close(this->_videoFileDescriptor);
          this->_videoFileDescriptor = -1;
          return false;
       }
    }

    this->_isInitialized = true;
    return true;
}

void
VideoDeviceWriter::Close()
{
    if(this->_isInitialized)
    {
       close(this->_videoFileDescriptor);
       this->_isInitialized = false;
    }
}

bool
VideoDeviceWriter::WriteVideoFrame(const cv::Mat &image) const
{
    const uint imgSize = image.total() * image.elemSize();
    const int written = write(this->_videoFileDescriptor, image.data, imgSize);
    if(written < 0)
    {
       std::cout << "[ERROR] Could not write frame" << std::endl;
       return false;
    }
    return true;
}

