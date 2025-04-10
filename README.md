# Drawing_Robot

Components:

ESP32 Development Board
L298N Motor Driver (or similar dual-channel driver)
4 DC Motors (2 for X-axis, 2 for Y-axis)
Servo Motor (e.g., SG90 for pen control)
Power Supply :
    5V-12V battery/power adapter for motors.
    5V USB or external power for ESP32.

Wiring Connection:

A. L298N Motor Driver to ESP32

L298N PIN                 ESP32 PIN                   PURPOSE

IN1                       GPIO 25                     Control X-axis motor direction
IN2                       GPIO 26                     Control X-axis motor direction
ENA                       GPIO 33                     PWM speed control for X-axis
IN3                       GPIO 27                     Control Y-axis motor direction
IN4                       GPIO 14                     Control Y-axis motor direction
ENB                       GPIO 32                     PWM speed control for Y-axis

B. L298N Power Supply

Connect a 5V-12V battery to the L298N:
      Positive terminal → +12V on L298N.
      Negative terminal → GND on L298N.
Ensure the L298N GND is connected to the ESP32 GND for a common ground.

C. DC Motors to L298N

X-Axis Motors :
      Left motor → OUT1 and OUT2 on L298N.
      Right motor → OUT3 and OUT4 on L298N.
Y-Axis Motors :
      Front motor → OUT1 and OUT2 on L298N.
      Back motor → OUT3 and OUT4 on L298N.
      
D. Servo Motor to ESP32

SERVO WIRE             ESP32 PIN             PURPOSE

Brown (GND)            GND                   Ground
Red (VCC)              5V                    Power (use external 5V supply)
Yellow (Signal)        GPIO 13               PWM signal for pen control

  If the servo requires more current than the ESP32’s 5V pin can provide, use an external 5V power supply for the servo and connect its GND to the ESP32’s GND .
  
3. Power Supply Connections
   
ESP32 Power :
    Use a USB cable or a 3.3V regulator to power the ESP32.
    Alternatively, use a 5V regulator with a common ground.
Motor Power :
    Use a 5V-12V battery to power the L298N motor driver.
    Ensure the battery’s GND is connected to both the ESP32 GND and L298N GND .

5. Final Setup
   
Here’s how everything connects:

ESP32 :
    Controls the L298N motor driver via GPIO pins.
    Sends PWM signals to control motor speed.
    Communicates with the servo motor via GPIO 13.
L298N Motor Driver :
    Drives the DC motors for X/Y movement.
    Receives control signals from the ESP32.
Servo Motor :
    Connected to GPIO 13 for pen up/down control.
    Powered by a 5V source (either ESP32 or external).
Power Supply :
    Separate power sources for ESP32 (3.3V/5V) and motors (5V-12V).
    Common ground between all components.
