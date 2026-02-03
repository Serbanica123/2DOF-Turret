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

class turret_control : public rclcpp::Node
{
public:
  turret_control() : Node("turret_control")
  {
    state_subscription_ = this->create_subscription<std_msgs::msg::Float32MultiArray>("turret/state", 10, std::bind(&turret_control::state_callback, this, std::placeholders::_1));
    velocity_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turret/cmd_vel", 10);
    timer_ = this->create_wall_timer(std::chrono::milliseconds(500), std::bind(&turret_control::controller_callback, this));
  }

private:
  void state_callback(const std_msgs::msg::Float32MultiArray &msg)
  { // To be added, message feedback processing
    this->turret_state = msg;
  }

  void cmd_callback(const custom_msgs::msg::TurretCmd &msg)
  {
    this->current_commands = msg;
  }

  void controller_callback()
  {
  }
  rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr state_subscription_;
  rclcpp::Subscription<custom_msgs::msg::TurretCmd>::SharedPtr cmd_subscription;

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr velocity_publisher_;

  std_msgs::msg::Float32MultiArray turret_state;
  custom_msgs::msg::TurretCmd current_commands;
};

int main(int argc, char **argv)
{
  // rclcpp::init(argc, argv);
  // rclcpp::spin(std::make_shared<turret_control>());
  // rclcpp::shutdown();

  TurretController test("/home/alex/Desktop/2DOF-Turret/2dof_ros2_ws/src/turret_control/lqr_observer_config.json");
  test.run();
  return 0;
}
