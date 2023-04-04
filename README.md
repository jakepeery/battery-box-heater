# battery-box-heater
Controls 4 individual battery heaters in the same heating box, each with their own temp sensor.  Setup to limit the max temp of the heaters themselves to not over-heat and melt the plastic.  Also controls up to two cooling fans and two relay outputs to turn off the solar panels and the charge controler input.


Temp sensors for batteries (4 sensors to be mounted under each battery)
Temp sensor for battery box/top of batteries (one sensor on top of the batteries to measure the box temp)

OLED pins 21 and 22

Input: one-wire DS18B20 sensors GPIO 4 (all 5 sensors to be wired in parallel with one terminating resistor. 

Outputs
1-Heater 1                   27
2-Heater 2                   26
3-Heater 3                   25
4-Heater 4                   33
5-Fan 1 - normal             19
6-Fan 2 - dual               18
7-Relay for solar            13
8-Relay for 120v charger     23

Un-used GPIO:
GPIO 32, 16(RX2), 17(TX2), 
GPI 35, 34, 39, 36 input only available (maybe for voltage some day)