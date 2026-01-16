#2-DOF Vision Turret

This project aims to develop a 2-degree-of-freedom (2-DOF) vision turret that combines computer vision, advanced control algorithms, and embedded systems. It integrates ROS2, low-level ESP32 motor control, reinforcement learning, and vision-based techniques for aiming, target tracking, and trajectory prediction. The project serves as a comprehensive learning platform for software engineering, robotics, and control systems.

## Current Baseline Implementation
**ESP FOC + ROS2 Interface with Offline Control Design**

The current implementation focuses on the low-level control and communication stack for the turret:

- ESP32-based field-oriented motor control (FOC)
- Velocity command interface via ROS2
- Serial-based feedback of joint state (logging to CSV)
- System identification using MATLAB
- Offline-designed LQR controller with Luenberger observer

Future work will extend this baseline with reinforcement learning, computer vision, and trajectory prediction for autonomous target tracking.

## Features
- Real-time motor control with ESP32 FOC
- ROS2 nodes for velocity command and joint-state feedback
- CSV logging for system identification and analysis
- Offline LQR controller with state observer
- Learning-focused platform for control, vision, and robotics

## Project Structure