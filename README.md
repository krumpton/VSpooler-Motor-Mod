
[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://opensource.org/licenses/)![GitHub top language](https://img.shields.io/github/languages/top/krumpton/VSpooler-Motor-Mod)




# V-Spooler Motor Mod

An addon for the [V-Spooler](https://www.printables.com/model/684376-v-spooler) project to enable automation via an ESP32 and a stepper motor.

Should be fairly easy to port over to Arduino if anybody is so inclined, I think I've kept the project layout pretty sane and readable.


## Features

- Uses a filament runout sensor to automatically stop when spooling is complete.
- Indicates status via a WS2812B (NeoPixel) LED.


## Required Hardware
- **V-Spooler**
- **ESP32** - I used an ESP32C3 for its small form factor, but any should work.
    - **Buck converter (optional)** - depends whether your MCU can handle the same voltage as the motor. Mine can't.
- **TMC2209** - or any motor driver compatible with your microcontroller.
- **Stepper motor** - any which physically fits the project and can be run by your hardware. I used an LDO Nema 17 Speedy Power.
- **Runout sensor** - any which outputs a signal your MCU can interpret. I used an old one from a dead Ender 3.
- **WS2812B LED** - others may work, but will need adjusting in code.
- **Button & switch** - any toggle switch and momentary push button. The switch engages/disengages the motor, the button starts/stops respooling.
- **PSU** - whatever is suitable for your chosen motor, plus however you want to connect it to the unit (i.e. hard-wired vs detachable plug, etc).
## Acknowledgements

 - [esp_ws28xx](https://github.com/okhsunrog/esp_ws28xx) library by @okhsunrog


## License

[GPL v3](https://choosealicense.com/licenses/gpl-3.0/)

