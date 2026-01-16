# 2-DOF Vision Turret

This project develops a 2-degree-of-freedom (2-DOF) vision turret that combines embedded systems, advanced control algorithms, and computer vision. It integrates ROS2, low-level ESP32 motor control, reinforcement learning, and vision-based techniques for aiming, target tracking, and trajectory prediction. The project serves as a hands-on platform for learning software engineering, robotics, and control systems.

## Current Baseline Implementation
**ESP FOC + ROS2 Interface with Offline Control Design**

The current implementation focuses on the low-level control and communication stack:

- ESP32-based field-oriented motor control (FOC)
- Velocity command interface via ROS2
- Serial-based feedback of joint states (logging to CSV)
- System identification using MATLAB
- Offline-designed LQR controller with Luenberger observer

This baseline lays the foundation for future enhancements such as reinforcement learning-based aiming, computer vision target tracking, and trajectory prediction.

## Features
- Real-time embedded motor control with ESP32 FOC
- ROS2 nodes for velocity commands and joint-state feedback
- Modular architecture suitable for extending to vision and learning algorithms
- CSV logging for system analysis and performance evaluation
- Offline LQR controller with state observer for precise control

## Project Structure
