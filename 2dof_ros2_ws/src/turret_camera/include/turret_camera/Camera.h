#ifndef CAMERA_H
#define CAMERA_H
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

class Camera
{
public:
    Camera(int id, double width, double height);
    ~Camera();
    const cv::Mat getLatestFrame();

private:
    void captureLoop();

    cv::VideoCapture cap;
    cv::Mat latestFrame;

    std::thread captureThread;
    std::mutex frameMutex;

    std::atomic<bool> running;
};

#endif