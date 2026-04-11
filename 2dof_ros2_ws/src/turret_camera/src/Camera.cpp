#include "turret_camera/Camera.h"

Camera::Camera(int id, double width, double height)
{
    this->cap = cv::VideoCapture(id);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Cannot open camera\n";
    }
    running = true;
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap.set(cv::CAP_PROP_FPS, 30);

    captureThread = std::thread(&Camera::captureLoop, this);
}

void Camera::captureLoop()
{
    cv::Mat frame;

    while (running)
    {
        cap >> frame;
        if (!frame.empty())
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            latestFrame = frame.clone();
        }
    }
}
const cv::Mat Camera::getLatestFrame()
{
    {
        std::lock_guard<std::mutex> lock(frameMutex);
        return latestFrame;
    }
}
Camera::~Camera()
{
    running = false;

    if (captureThread.joinable())
        captureThread.join();

    cap.release();

    std::cout << "Camera Released \n";
}