Industrial Robotic Arm Control with Glove-based Interface
Welcome to the GitHub repository for the Industrial Robotic Arm Control project! This capstone project focuses on real-time control of an industrial robotic arm using a glove-based interface that captures hand movements. Additionally, it involves the development of a soft robotic gripper for object manipulation. The project aims to enable intuitive control and manipulation of the robot arm by mapping the user's hand gestures to corresponding robotic actions, while also incorporating a soft gripper for object manipulation. By utilizing a glove that records hand movements and leveraging animatronic principles.

Project Overview
The Industrial Robotic Arm Control project combines hardware and software components to achieve seamless control and manipulation of the Mitsubishi Rv-2f industrial robot arm and cr751d controller. The system utilizes a glove embedded with sensors, including two flex resistors, an ADS1015 ADC, and an MPU6050 IMU, to capture hand movements accurately. The code is written in Node.js and includes a GUI interface for controlling the robot arm on the XYZ plane. Additionally, a modified 3D gripper design for a small DC motor is available, providing flexibility for object manipulation.

Features
Real-time control of an industrial robotic arm using a glove-based interface
Mapping of user's hand gestures to corresponding robotic actions
Incorporation of a soft robotic gripper for object manipulation
Intuitive GUI interface for controlling the robot arm on the XYZ plane
Utilization of animatronic principles for improved control and dexterity
Modified 3D gripper design for a small DC motor
Dependencies
To run this project, you need to have the following dependencies installed:

Node.js (version X.X.X)
Any platform with I2C support (e.g., Raspberry Pi Compute Module 4)
Mitsubishi Rv-2f industrial robot arm and cr751d controller
ADS1015 ADC library
MPU6050 IMU library
Graphical User Interface (GUI) library (e.g., Electron, React, or similar)
Installation
Clone this repository to your local machine using the following command:

bash
Copy code
git clone https://github.com/your-username/repository-name.git
Install the project dependencies by navigating to the project's root directory and running the following command:

Copy code
npm install
Connect the glove to the platform (e.g., Raspberry Pi Compute Module 4) and ensure that the necessary drivers are installed.

Make sure the Mitsubishi Rv-2f industrial robot arm and cr751d controller are properly connected and powered.

Configuration
Before running the code, you need to configure the project by following these steps:

Modify the configuration file config.js with the appropriate settings for your setup. Update the communication settings, such as IP addresses and port numbers, to establish a connection between the software and the Mitsubishi Rv-2f industrial robot arm.

Calibrate the glove sensors by running the calibration script provided (calibration.js).

Usage
To start the Industrial Robotic Arm Control system, execute the following command:

Copy code
node app.js
Make sure all the hardware components are connected and powered on before running the code. The GUI interface will launch, providing controls to manipulate the robot arm on the XYZ plane. The glove-based interface will also be active, allowing you to control the robot arm using hand movements.

Gripper Design
For this project, a modified 3D gripper design was implemented to work with a small DC motor. The gripper design allows for object manipulation and is customizable to fit different requirements. You can find the initial gripper design at the following link:

Modified 3D Gripper Design

Please note that you have the flexibility to modify and adapt the gripper design according to your specific needs. Feel free to make any changes to the design to optimize it for your application.

License
This project is licensed under the MIT License.

Acknowledgements
We would like to express our gratitude to the following resources for their contribution to this project:

Mitsubishi Rv-2f industrial robot arm documentation: link
ADS1015 ADC library: link
MPU6050 IMU library: link
Contributing
Contributions are welcome! If you'd like to contribute to this project, please follow these steps:

Fork the repository.
Create a new branch for your feature or bug fix.
Make your changes and commit them with descriptive messages.
Push your changes to your forked repository.
Submit a pull request explaining your changes.
Contact
If you have any questions or feedback regarding this project, please feel free to contact us.
