cmake_minimum_required(VERSION 3.8)
project(turret_control)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(custom_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(position_controller src/position_controller.cpp src/jsonRead.cpp src/Control.cpp)

ament_target_dependencies(position_controller
  rclcpp
  custom_msgs
  geometry_msgs
  std_msgs
)

target_link_libraries(position_controller
  Eigen3::Eigen
)

target_include_directories(position_controller PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(position_controller PUBLIC cxx_std_17)

install(TARGETS position_controller
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
