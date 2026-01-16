# 2-DOF Turret Control System

This project implements the low-level control and communication stack for a
2-degree-of-freedom turret.

## Current Baseline
**ESP32 FOC + ROS2 Interface with Offline Control Design**

- ESP-based field-oriented motor control (FOC)
- Velocity command interface via ROS2
- Serial-based feedback of joint states
- System identification in MATLAB
- LQR controller and Luenberger observer designed offline

See `docs/system_overview.md` for details.