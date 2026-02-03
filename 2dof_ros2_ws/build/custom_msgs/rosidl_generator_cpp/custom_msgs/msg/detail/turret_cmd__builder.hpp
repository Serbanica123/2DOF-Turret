// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from custom_msgs:msg/TurretCmd.idl
// generated code does not contain a copyright notice

#ifndef CUSTOM_MSGS__MSG__DETAIL__TURRET_CMD__BUILDER_HPP_
#define CUSTOM_MSGS__MSG__DETAIL__TURRET_CMD__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "custom_msgs/msg/detail/turret_cmd__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace custom_msgs
{

namespace msg
{

namespace builder
{

class Init_TurretCmd_fire
{
public:
  explicit Init_TurretCmd_fire(::custom_msgs::msg::TurretCmd & msg)
  : msg_(msg)
  {}
  ::custom_msgs::msg::TurretCmd fire(::custom_msgs::msg::TurretCmd::_fire_type arg)
  {
    msg_.fire = std::move(arg);
    return std::move(msg_);
  }

private:
  ::custom_msgs::msg::TurretCmd msg_;
};

class Init_TurretCmd_pitch
{
public:
  explicit Init_TurretCmd_pitch(::custom_msgs::msg::TurretCmd & msg)
  : msg_(msg)
  {}
  Init_TurretCmd_fire pitch(::custom_msgs::msg::TurretCmd::_pitch_type arg)
  {
    msg_.pitch = std::move(arg);
    return Init_TurretCmd_fire(msg_);
  }

private:
  ::custom_msgs::msg::TurretCmd msg_;
};

class Init_TurretCmd_yaw
{
public:
  Init_TurretCmd_yaw()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_TurretCmd_pitch yaw(::custom_msgs::msg::TurretCmd::_yaw_type arg)
  {
    msg_.yaw = std::move(arg);
    return Init_TurretCmd_pitch(msg_);
  }

private:
  ::custom_msgs::msg::TurretCmd msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::custom_msgs::msg::TurretCmd>()
{
  return custom_msgs::msg::builder::Init_TurretCmd_yaw();
}

}  // namespace custom_msgs

#endif  // CUSTOM_MSGS__MSG__DETAIL__TURRET_CMD__BUILDER_HPP_
