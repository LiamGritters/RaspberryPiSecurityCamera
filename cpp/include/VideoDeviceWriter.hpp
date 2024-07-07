/*
 * VideoDeviceWriter.hpp
 *
 *  Created on: Jul. 6, 2024
 *      Author: liam
 */

#ifndef CPP_INCLUDE_VIDEODEVICEWRITER_HPP_
#define CPP_INCLUDE_VIDEODEVICEWRITER_HPP_

#include <opencv2/highgui.hpp>

#include <string>

class VideoDeviceWriter
{

public:
    VideoDeviceWriter();
    ~VideoDeviceWriter();

    bool Initialize(const int width, const int height, const std::string &videoDeviceName);

    void Close();

    bool WriteVideoFrame(const cv::Mat &image) const;

private:

    bool _isInitialized;

    int _videoFileDescriptor;

};

#endif /* CPP_INCLUDE_VIDEODEVICEWRITER_HPP_ */
