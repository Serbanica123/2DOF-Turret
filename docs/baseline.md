**Tag:** turret-v0.1-foc-control-design

## Description
This baseline establishes reliable embedded motor control and host communication
for the 2-DOF turret system.

## Included
- ESP32 FOC motor control
- ESP32 Task scheduling with freeRTOS
- Velocity command interface
- Serial feedback to ROS2
- MATLAB-based system identification
- LQR and Luenberger observer design (offline)

## Excluded
- High Level controllers for positioning and tracking
- Vision and targeting
- Trajectory planning

## Purpose
Provides a stable foundation for future embedded control and perception layers with ROS2 integration.
