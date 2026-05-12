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
        package_path + "/configs/lqiPitch1.json";

    controllerPitch = std::make_unique<TurretController>(config_path.c_str());

    r.setZero(controllerPitch->getOutputNum());
    y.setZero(controllerPitch->getOutputNum());

    first_state_received = false;
    state_subscription_ = this->create_subscription<std_msgs::msg::Float32MultiArray>("turret/state", 10, std::bind(&TurretControlNode::state_callback, this, std::placeholders::_1));
    cmd_subscription = this->create_subscription<custom_msgs::msg::TurretCmd>("turret/cmd_setpoint", 10, std::bind(&TurretControlNode::cmd_callback, this, std::placeholders::_1));
    velocity_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turret/cmd_vel", 10);

    timer_ = this->create_wall_timer(std::chrono::milliseconds(static_cast<int>(controllerPitch->getDt() * 1000)), std::bind(&TurretControlNode::controllerPitch_callback, this));
  }

private:
  void state_callback(const std_msgs::msg::Float32MultiArray &msg)
  { 
    //, msg.data[5];
    this->y << msg.data[5];
    first_state_received=true;
    //RCLCPP_INFO(this->get_logger(), "State y: [%f, %f]", y[0], y[1]);
  }

  void cmd_callback(const custom_msgs::msg::TurretCmd &msg)
  
  {//msg.yaw, 
    this->r << msg.pitch;
    controllerPitch->updateReference(this->r);
    RCLCPP_INFO(this->get_logger(), "Reference r: [%f, %f]", r[0], 0.0);
  }

  void controllerPitch_callback()
  {
    if (first_state_received)
    {
      auto msg = geometry_msgs::msg::Twist();
      Eigen::VectorXd commands = controllerPitch->run(y);
      msg.angular.z = -commands[0];
      msg.angular.y = 0.0;
      this->velocity_publisher_->publish(msg);
      RCLCPP_INFO(this->get_logger(), "Commands u: [%f, %f, %f, %f]", commands[0], y[0], 0.0, 0.0);
    }
    return;
  }

  std::unique_ptr<TurretController> controllerPitch;
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
