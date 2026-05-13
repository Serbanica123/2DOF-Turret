#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include <cstdio>
#include <rclcpp/rclcpp.hpp>
#include <custom_msgs/msg/turret_cmd.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/float32_multi_array.hpp>
#include <turret_control/Control.h>
#include "ament_index_cpp/get_package_share_directory.hpp"

class TurretControlNode : public rclcpp::Node
{
public:
  TurretControlNode() : Node("turret_control")
  {
    std::string package_path =
    ament_index_cpp::get_package_share_directory("turret_control");

    std::string config_path =
        package_path + "/configs/lqiPRBSR200.json";

    controller = std::make_unique<TurretController>(config_path.c_str());

    r.setZero(controller->getOutputNum());
    y.setZero(controller->getOutputNum());

    first_state_received = false;
    state_subscription_ = this->create_subscription<std_msgs::msg::Float32MultiArray>("turret/state", 10, std::bind(&TurretControlNode::state_callback, this, std::placeholders::_1));
    cmd_subscription = this->create_subscription<custom_msgs::msg::TurretCmd>("turret/cmd_setpoint", 10, std::bind(&TurretControlNode::cmd_callback, this, std::placeholders::_1));
    velocity_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turret/cmd_vel", 10);

    timer_ = this->create_wall_timer(std::chrono::milliseconds(static_cast<int>(controller->getDt() * 1000)), std::bind(&TurretControlNode::controller_callback, this));
  }

private:
  void state_callback(const std_msgs::msg::Float32MultiArray &msg)
  { 
    this->y << msg.data[2], msg.data[5];
    first_state_received=true;
    //RCLCPP_INFO(this->get_logger(), "State y: [%f, %f]", y[0], y[1]);
  }

  void cmd_callback(const custom_msgs::msg::TurretCmd &msg)
  {
    this->r << msg.yaw, msg.pitch;
    controller->updateReference(this->r);
    RCLCPP_INFO(this->get_logger(), "Reference r: [%f, %f]", r[0], r[1]);
  }

  void controller_callback()
  {
    if (first_state_received)
    {
      auto msg = geometry_msgs::msg::Twist();
      Eigen::VectorXd commands = controller->run(y);
      msg.angular.z = commands[1];
      msg.angular.y = commands[0];
      this->velocity_publisher_->publish(msg);
      RCLCPP_INFO(this->get_logger(), "Commands u: [%f, %f, %f, %f]", commands[0], y[0], commands[1], y[1]);
    }
    return;
  }

  std::unique_ptr<TurretController> controller;
  rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr state_subscription_;
  rclcpp::Subscription<custom_msgs::msg::TurretCmd>::SharedPtr cmd_subscription;

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr velocity_publisher_;

  Eigen::VectorXd r;
  Eigen::VectorXd y;
  bool first_state_received;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TurretControlNode>());
  rclcpp::shutdown();

  return 0;
}
