# Industrial Robotic Arm Control with Glove-based Interface

Welcome to the GitHub repository for the Industrial Robotic Arm Control project! This capstone project focuses on real-time control of an industrial robotic arm using a glove-based interface that captures hand movements. Additionally, it involves the development of a soft tipped gripper for object manipulation. The project aims to enable intuitive control and manipulation of the robot arm by mapping the user's hand gestures to corresponding robotic actions, while also incorporating a soft gripper for object manipulation. By utilizing a glove that records hand movements and leveraging animatronic principles.

## Project Overview

The Industrial Robotic Arm Control project combines hardware and software components to achieve seamless control and manipulation of the Mitsubishi Rv-2f industrial robot arm and cr751d controller. The system utilizes a glove embedded with sensors, including two flex resistors, an ADS1015 ADC, and an MPU6050 IMU, to capture hand movements accurately. The code is written in Node.js and includes a GUI interface for controlling the robot arm on the XYZ plane. Additionally, a modified 3D gripper design for a small DC motor is available, providing flexibility for object manipulation.

## Features

- Real-time control of an industrial robotic arm using a glove-based interface
- Mapping of user's hand gestures to corresponding robotic actions
- Incorporation of a soft robotic gripper for object manipulation
- Intuitive GUI interface for controlling the robot arm on the XYZ plane
- Utilization of animatronic principles for improved control and dexterity
- Modified 3D gripper design for a small DC motor

## Hardware

![General Schematic](https://github.com/atakankartli/Industrial-Robotic-Arm-Control-with-Glove-based-Interface/assets/33790735/9afd3be7-1c65-439a-8dfa-00adf05dd0b2)

To run this project, you need to have the following hardware:

- Any platform with I2C support (We used Raspberry Pi Compute Module 4)
- [Mitsubishi Rv-2f industrial robot arm](https://www.mitsubishielectric.com/fa/products/rbt/robot/index.html) and CR751d controller
- Glove
  - ADS1015 ADC
  - MPU6050 IMU
  - 2xFlex resistor
  - Voltage divider for the flex resistors
  - Button
- Gripper, DC, Servo or Step motor for the gripper  

## Controlling the arm Movement


![Mapping](https://github.com/atakankartli/Industrial-Robotic-Arm-Control-with-Glove-based-Interface/assets/33790735/29dcbc07-dee5-4a27-a661-d8d11a573ac3)

## Dependencies for Node.js

To run the Industrial Robotic Arm Control project in Node.js, you need to have the following additional dependencies installed:

- **pigpio**: A library for controlling GPIO (General Purpose Input/Output) pins on the Raspberry Pi. It provides access to the GPIO pins for controlling hardware components.

- **express**: A fast and minimalist web application framework for Node.js. It can be used to create a GUI interface for controlling the robotic arm.

- **socket.io**: A library that enables real-time, bidirectional communication between web clients and servers. It can be used to establish a connection between the GUI interface and the server.

- **dgram**: A module that provides UDP (User Datagram Protocol) communication capabilities in Node.js. It can be used for transmitting data between devices in a network.

- **i2c-bus**: A library for accessing I2C (Inter-Integrated Circuit) devices in Node.js. It allows communication with devices connected via the I2C protocol.

- **ads1x15**: A library for interacting with ADS1015/ADS1115 analog-to-digital converters. It enables reading analog sensor values connected to the ADC.

To install these dependencies, you can use the npm package manager by running the following command:

```bash
npm install pigpio express socket.io dgram i2c-bus ads1x15
```

Make sure to include these dependencies in your package.json file or install them individually as required.

Please note that specific versions of these libraries may be required depending on compatibility and project needs.


## Gripper Design

![gripper](https://github.com/atakankartli/Industrial-Robotic-Arm-Control-with-Glove-based-Interface/assets/33790735/21d6086f-57fe-4ff5-8c28-6d35aecf755a)

For this project, a modified 3D gripper design was implemented for a small DC motor. The gripper design enables the robotic arm to manipulate objects effectively. You can find the initial gripper design at the following link:

[Modified 3D Gripper Design](https://www.example.com)

Please note that the gripper design can be modified and customized according to your specific requirements. Feel free to make any adjustments or enhancements to optimize it for your application.


