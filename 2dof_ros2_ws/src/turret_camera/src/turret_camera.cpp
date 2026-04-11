// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "turret_camera/Camera.h"

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
 * member function as a callback from the timer. */

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
