#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "turret_camera/Camera.h"

using namespace std::chrono_literals;

class CameraPublisher : public rclcpp::Node
{
public:
  CameraPublisher()
      : Node("turret_camera"), count_(0)
  {

    rclcpp::QoS qos_profile(1);
    qos_profile.reliability(rclcpp::ReliabilityPolicy::BestEffort);
    qos_profile.durability(rclcpp::DurabilityPolicy::Volatile);

    this->cam = std::make_unique<Camera>(0, 1280, 960);

    publisher_ = this->create_publisher<sensor_msgs::msg::Image>("camera/img_raw", qos_profile);
    timer_ = this->create_wall_timer(
        16ms, std::bind(&CameraPublisher::timer_callback, this));
  }

private:
  void timer_callback()
  {
    cv::Mat frame = cam->getLatestFrame();

    if (frame.empty())
      return;

    sensor_msgs::msg::Image msg;

    msg.height = frame.rows;
    msg.width = frame.cols;
    msg.encoding = "bgr8";
    msg.step = frame.cols * frame.elemSize();

    msg.data.assign(
        frame.data,
        frame.data + frame.total() * frame.elemSize());

    msg.header.stamp = this->now();
    msg.header.frame_id = "camera";

    publisher_->publish(msg);
  }
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;

  std::unique_ptr<Camera> cam;

  size_t count_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CameraPublisher>());
  rclcpp::shutdown();

  return 0;
}
