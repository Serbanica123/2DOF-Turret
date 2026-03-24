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

class TurretControlNode : public rclcpp::Node
{
public:
  TurretControlNode() : Node("turret_control")
  {
    controler= std::make_unique<TurretController>("/home/alex/Desktop/2DOF-Turret/2dof_ros2_ws/src/turret_control/lqr_observer_config.json");
    reference.setZero(controler->getOutputNum());
    state_subscription_ = this->create_subscription<std_msgs::msg::Float32MultiArray>("turret/state", 10, std::bind(&TurretControlNode::state_callback, this, std::placeholders::_1));
    velocity_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turret/cmd_vel", 10);
    timer_ = this->create_wall_timer(std::chrono::milliseconds(static_cast<int>(controler->getDt()*1000)), std::bind(&TurretControlNode::controller_callback, this));
  }

private:
  void state_callback(const std_msgs::msg::Float32MultiArray &msg)
  { // To be added, message feedback processing
    this->turret_state = msg;
  }

  void cmd_callback(const custom_msgs::msg::TurretCmd &msg)
  {
    this->reference << msg.yaw, msg.pitch;
  }

  void controller_callback()
  {
    Eigen::VectorXd commands=controler->run(reference);
    
  }

  std::unique_ptr<TurretController> controler;
  rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr state_subscription_;
  rclcpp::Subscription<custom_msgs::msg::TurretCmd>::SharedPtr cmd_subscription;

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr velocity_publisher_;

  std_msgs::msg::Float32MultiArray turret_state;

  Eigen::VectorXd reference;
};

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  // rclcpp::init(argc, argv);
  // rclcpp::spin(std::make_shared<turret_control>());
  // rclcpp::shutdown();

  return 0;
}
