
[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://opensource.org/licenses/)


# V-Spooler Motor Mod

An addon for the [V-Spooler](https://www.printables.com/model/684376-v-spooler) project to enable automation via an ESP32 and a stepper motor.

Should be fairly easy to port over to Arduino if anybody is so inclined, I think I've kept the project layout pretty sane and readable.


## Features

- Uses a filament runout sensor to automatically stop when spooling is complete.
- Indicates status via a WS2812B (NeoPixel) LED.


## Required Hardware
- **V-Spooler**
- **ESP32** - I used an ESP32C3 Super Mini for its small form factor, but any should work.
    - **Buck converter (optional)** - depends whether your MCU can handle the same voltage as the motor. Mine can't.
- **TMC2209** - or any motor driver compatible with your microcontroller.
- **Stepper motor** - any which physically fits the project and can be run by your hardware. I used an LDO Nema 17 Speedy Power.
- **Runout sensor** - any which outputs a signal your MCU can interpret. I used an old one from a dead Ender 3 with a 3D printed case to allow PTFE tubing to be inserted. You may need to alter the code depending on whether your runout sensor outputs a high or low signal when filament is detected.
- **WS2812B LED** - others may work, but will need adjusting in code.
  - **Level shifter (or equivalent)** - if you use a microcontroller with 3.3V logic, you either need to shift the logic level to at least 3.5V, or lower Vdd (WS2812B logic level is Vdd*0.7). I just connected the supply voltage through a diode (1N4007) to drop Vdd to ~4.2V, which allows 3.3V logic to control the LED.
- **Button & switch** - any toggle switch and momentary push button. The switch engages/disengages the motor, the button starts/stops respooling.
- **40mm fan (optional-ish)** - The TMC2209 and stepper motor can get quite hot if you run it at high RPM for a long time, so adding a small fan to the project is recommended. I used a Noctua 40mm fan that I had lying around, but any low-profile 40mm fan will work. Just make sure you connect it to the right power rail depending on whether it's a 5V or 12V fan.
- **PSU** - whatever is suitable for your chosen motor, plus however you want to connect it to the unit (i.e. hard-wired vs detachable plug, etc).
- 
## Acknowledgements

 - [esp_ws28xx](https://github.com/okhsunrog/esp_ws28xx) library by @okhsunrog


## License

[GPL v3](https://choosealicense.com/licenses/gpl-3.0/)

