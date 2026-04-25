# line_follower_car
// A seed is planted here  
This is the first project written by a sophomore majoring in Mechatronic Engineering.
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


## Author
Bai Jiazheng __ North China Institute of Aerospace Engineering(NCIAE) __ Mechatronic Engineering
