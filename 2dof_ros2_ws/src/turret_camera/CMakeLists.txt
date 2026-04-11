cmake_minimum_required(VERSION 3.8)
project(turret_camera)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(OpenCV REQUIRED)
find_package(sensor_msgs REQUIRED)
add_executable(turret_camera src/turret_camera.cpp src/Camera.cpp)
ament_target_dependencies(turret_camera rclcpp std_msgs sensor_msgs)

target_include_directories(turret_camera PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(turret_camera ${OpenCV_LIBS})

install(DIRECTORY include/
  DESTINATION include
)

install(TARGETS
  turret_camera
  DESTINATION lib/${PROJECT_NAME})

ament_package()

