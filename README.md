# Industrial Robotic Arm Control with Glove-based Interface

Welcome to the GitHub repository for the Industrial Robotic Arm Control project! This capstone project focuses on real-time control of an industrial robotic arm using a glove-based interface that captures hand movements. Additionally, it involves the development of a soft robotic gripper for object manipulation. The project aims to enable intuitive control and manipulation of the robot arm by mapping the user's hand gestures to corresponding robotic actions, while also incorporating a soft gripper for object manipulation. By utilizing a glove that records hand movements and leveraging animatronic principles.

## Project Overview

The Industrial Robotic Arm Control project combines hardware and software components to achieve seamless control and manipulation of the Mitsubishi Rv-2f industrial robot arm and cr751d controller. The system utilizes a glove embedded with sensors, including two flex resistors, an ADS1015 ADC, and an MPU6050 IMU, to capture hand movements accurately. The code is written in Node.js and includes a GUI interface for controlling the robot arm on the XYZ plane. Additionally, a modified 3D gripper design for a small DC motor is available, providing flexibility for object manipulation.

## Features

- Real-time control of an industrial robotic arm using a glove-based interface
- Mapping of user's hand gestures to corresponding robotic actions
- Incorporation of a soft robotic gripper for object manipulation
- Intuitive GUI interface for controlling the robot arm on the XYZ plane
- Utilization of animatronic principles for improved control and dexterity
- Modified 3D gripper design for a small DC motor

## Dependencies

To run this project, you need to have the following dependencies installed:

- Node.js (version X.X.X)
- Any platform with I2C support (e.g., Raspberry Pi Compute Module 4)
- [Mitsubishi Rv-2f industrial robot arm](https://www.mitsubishielectric.com/fa/products/rbt/robot/index.html) and cr751d controller
- ADS1015 ADC library
- MPU6050 IMU library
- Graphical User Interface (GUI) library (e.g., Electron, React, or similar)

## Installation

1. Clone this repository to your local machine using the following command:

