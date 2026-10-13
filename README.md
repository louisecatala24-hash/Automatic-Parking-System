# Automatic Parking System

An Arduino-based four-slot parking system with an automatic gate, LCD slot counter, and automatic lights.

## Hardware Needed

- 1 Arduino Uno
- 2 IR obstacle sensors
- 1 SG90 positional servo
- 1 16×2 LCD with I2C module
- 1 LDR
- 1 10kΩ resistor
- 3 LEDs
- 3 330Ω resistors
- Breadboard and jumper wires
- USB cable
- Suitable regulated 5V servo supply
- Model gate and parking base

## Connections

| Component | Arduino Pin |
|---|---|
| IR1 OUT | D2 |
| IR2 OUT | D3 |
| Servo signal | D4 |
| LED 1, 2, 3 | D8, D9, D10 |
| LDR divider | A0 |
| LCD SDA / SCL | A4 / A5 |

Each LED needs a series 330Ω resistor.

LDR wiring: 5V → LDR → A0 → 10kΩ resistor → GND.

Connect all grounds together.

## How to Use

1. Connect the hardware.
2. Install the Servo and LiquidCrystal_I2C libraries.
3. Open `Parking_System.ino` in Arduino IDE.
4. Select Arduino Uno and the correct port.
5. Upload and test.

## Features

- Opens the gate for entering and leaving cars.
- Displays available parking slots.
- Turns lights on when dark.

## Notes

Adjust servo angles, LCD address, and light thresholds as needed. Slots reset to four after restarting. Designed for one car at a time.
