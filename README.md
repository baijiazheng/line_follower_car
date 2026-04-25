# line_follower_car
**A seed is planted here**  
This is the first project written by a freshman majoring in Mechatronic Engineering.
This code represents my first real encounter with the gap between simulation and reality. The logic is sound — the hardware environment was not. I'm working to close that gap.

If you're working on a similar line follower or battling hardware gremlins — may this repo save you some time.

## Project Introduction
This is an Arduino-based smart car project that supports two modes: **Infrared Line Tracking** and **Bluetooth Remote Control**.
- Line Tracking Mode: 5-channel（yet 4） infrared sensors + PID algorithm for automatic line following
- Remote Control Mode: Bluetooth serial communication, supporting 6-direction control and real-time speed adjustment
- Heartbeat Protection: Automatic stop when Bluetooth connection is lost

## Hardware List
- Arduino UNO
- LM2596 7.4V to 5V Step-down Module
- 5-Channel（or 4-channel） Infrared Line Tracking Module
- HC-06 Bluetooth Module (or other serial Bluetooth modules) and 2K ohm resistor and 1K ohm resistor
- DC Geared Motor × 4
- 7.4V Lithium Battery
- TB6612 Motor Driver Module *2

## Wiring Instructions
[智能小车完整接线手册.xlsx](https://github.com/user-attachments/files/27086095/default.xlsx)
- **Every safety rule in this manual stems from a burned module, a track runaway, or a whole night of futile debugging.**
- **Please check each item carefully and do not repeat my mistakes.**
- This wiring manual is the hardware counterpart to our code.
- It's designed so that a new teammate with zero experience can safely power up the car within 30 minutes.
- Every rule in it was learned the hard way.







请再提供一些关于安全规则的具体例子。
如何确保所有团队成员都理解并遵守这些安全规则？
有没有相关的培训资料或教程来帮助我们更好地理解和应用这些安全规则？
**

## Key Design
- **Non-blocking architecture**: All sensor reading and PID calculation are based on millis() without using delay().
- **Adjustable PID parameters**: Kp and steering gain can be modified via macro definitions.
- **Safety mechanism**: Automatic stop on heartbeat timeout and forced stop during mode switching.
- **Debug interface**: Real-time deviation output through serial port for convenient parameter tuning.

## Existing Problems / Improvements to Be Made
- The vehicle tends to deviate on tracks with large curvature (lack of derivative term in PID).
- Speed fluctuates as battery voltage drops (no voltage compensation).
- Unoptimized sensor layout with limited coverage of 5-channel sensors.

## Photos / Videos
- <img width="1706" height="1279" alt="d8c6af9fdecceb6f29c73553ba6e5c37" src="https://github.com/user-attachments/assets/bb990f81-f393-4661-bcc5-3611da78af51" />
- <img width="1706" height="1279" alt="3838b09edfb6fc69f96b186bac28c7de" src="https://github.com/user-attachments/assets/d2ff8660-b83d-4f3b-a42f-11e5fde491cc" />
- MANU https://github.com/user-attachments/assets/2019864d-9a6c-4393-a8fa-ca62f66507d2
- AUTO https://github.com/user-attachments/assets/2357c0ef-0793-441a-891f-d9a13de56d9a



## Author
Bai Jiazheng __ North China Institute of Aerospace Engineering(NCIAE) __ Mechatronic Engineering
