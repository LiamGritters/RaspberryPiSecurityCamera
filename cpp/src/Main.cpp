/*
 * Main.cpp
 *
 *  Created on: Jan. 17, 2021
 *      Author: liam
 */

#include "VideoDeviceWriter.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <csignal>
#include <chrono>

constexpr int MaxNumberOfVideos = 10;
constexpr int DetectionTimeoutPeriod = 5; //seconds
const std::string filepath = "/var/www/html/savedVideo/";
const std::string liveVideoPath = "/dev/video9";

bool stopProgram = false;

void
signal_handler( int signal_num )
{
   std::cout << "The interrupt signal is (" << signal_num << ")" << std::endl;;

   stopProgram = true;
}

bool
doesFileExist(const std::string &filename)
{
    std::ifstream ifile;
    ifile.open(filename);
    if(ifile)
    {
       return true;
    }
    return false;
}

std::string
getFilenameFromSeed(int seed, const std::string &filePath)
{
    return (filePath + "video" + std::to_string(seed) + ".avi");
}

std::string
getNewFilename(int &seed, const std::string &filePath)
{
    ++seed;
    if(seed > MaxNumberOfVideos)
    {
        seed = 1;
    }

    const std::string file = getFilenameFromSeed(seed, filePath);
    if(doesFileExist(file))
    {
        (void)std::remove(file.c_str());
    }

    return file;
}

bool
writeVideoToFile(cv::VideoWriter &writer, cv::Mat &frame, int width, int height, int fps, const std::string &filename)
{
    if(writer.isOpened() ||
       writer.open(filename, cv::VideoWriter::fourcc('M','J','P','G'), fps, cv::Size(width,height)))
    {
        writer.write(frame);
        return true;
    }

    std::cout << "[ERROR]: Failed to write video to file: " << filename << std::endl;
    return false;
}

double
getTime()
{
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    return (double)(ms.count() / 1000.0);
}

bool
hasDetectionTimedOut(double time)
{
    if((getTime() - time) > DetectionTimeoutPeriod)
    {
        return true;
    }
    return false;
}

std::vector<std::vector<cv::Point> > contours;
std::vector<cv::Vec4i> hierarchy;

int main(int argc, char* argv[])
{
    // register signal SIGABRT and signal handler
    signal(SIGINT, signal_handler);

    //create Background Subtractor objects
    cv::Ptr<cv::BackgroundSubtractor> pBackSub;
    pBackSub = cv::createBackgroundSubtractorMOG2();

    cv::VideoCapture capture(0);
    if (!capture.isOpened())
    {
        //error in opening the video input
        std::cerr << "Unable to open: " << std::endl;
        return -1;
    }
    const int frameWidth = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    const int frameHeight = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    const int videoFPS = 12; // capture.get(cv::CAP_PROP_FPS); runs too slow for 30 fps, 12 was calculated

    VideoDeviceWriter deviceWriter;
    if(!deviceWriter.Initialize(frameWidth, frameHeight, liveVideoPath))
    {
        std::cerr << "Failed to initialize device writer with path: " << liveVideoPath << std::endl;
        return -1;
    }

    cv::VideoWriter writer;
    std::string filename;
    int videoSeed = 0;
    int framesWritten = 0;
    double timeOfDetection = getTime();

    cv::Mat frame, fgMask;
    while (!stopProgram)
    {
        capture >> frame;
        if (frame.empty()) break;

        if(!deviceWriter.WriteVideoFrame(frame))
        {
            std::cout << "[WARNING]: failed to write video frame" << std::endl;
        }

        //update the background model
        pBackSub->apply(frame, fgMask);

        cv::RNG rng(12345);
        cv::findContours(fgMask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

        std::vector<cv::Rect>boundRect (contours.size());
        std::vector<std::vector<cv::Point> > contours_poly( contours.size() );

        bool writeVideo = false;
        for (int i = 0; i < (int)contours.size();i++)
        {
            if(cv::contourArea(contours[i]) > 500)
            {
                writeVideo = true;
                timeOfDetection = getTime();
                break;
            }
        }

        if(writeVideo || !hasDetectionTimedOut(timeOfDetection))
        {
            if(filename.empty())
            {
                filename = getNewFilename(videoSeed, filepath);
            }
            stopProgram = !writeVideoToFile(writer, frame, frameWidth, frameHeight, videoFPS, filename);
            framesWritten++;
        }
        else if(!writeVideo && writer.isOpened())
        {
            std::cout << "Video: " << filename << " has finished with " << framesWritten << " frames" << std::endl;
            framesWritten = 0;
            writer.release();
            filename.clear();
        }
    }

    if(writer.isOpened()) writer.release();
    capture.release();
    deviceWriter.Close();

    return 0;
}
