
[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://www.gnu.org/licenses/gpl-3.0.html)


# V-Spooler Motor Mod

An addon for the [V-Spooler](https://www.printables.com/model/684376-v-spooler) project to enable automation via an ESP32 and a stepper motor.

Should be fairly easy to port over to the Arduino framework if anybody is so inclined; I’ve kept dependencies minimal and the project layout clean and easy to follow.

## Features
- Filament detection with automatic motor control to start and stop respooling
- Visual status indication using a WS2812B (NeoPixel) LED

## Getting started
See [BUILD_GUIDE.md](./BUILD_GUIDE.md) for detailed assembly instructions, circuit schematics and setup tips.

## Planned improvements
While the main functionality is done, there are still a couple of things that can be improved;
- **Non-linear motor ramp profiles** — currently the motor ramps up and down linearly. Non-linear acceleration could reduce mechanical stress and help avoid skipped teeth, although I haven’t observed missed steps in my tests.
- **TMC2209 UART control or current monitoring** — some filament ends have a small bend that prevents clearing the PTFE tube and runout sensor, causing the motor to stall. This could be addressed by enabling UART mode for stall detection or by monitoring motor current via a shunt resistor and the ESP32 ADC.


## Required Hardware
- **V-Spooler** - only tested with the base model, not any of the extended versions like V-Spooler X. Untested with the desk mount, but it will almost certainly interfere since it uses the same mounting holes. A modified case might be possible to get around this, but I don't use the desk mount so haven't looked into it.
- **ESP32** - I used an ESP32C3 Super Mini for its small form factor, but any should work. I recommend something with 5V logic though, or you'll need to do some level shifting (see below)
    - **Buck converter (optional)** - depends whether your MCU can handle the same voltage as the motor. Some can handle up to 12V or 15V. Mine can't. RIP.
- **TMC2209** - or any motor driver compatible with your microcontroller.
- **Stepper motor** - any which physically fits the project and can be run by your hardware. I used an LDO Nema 17 Speedy Power (the models are based around this, so it's the largest motor which will physically fit the enclosure).
- **Runout sensor** - any which outputs a signal your MCU can interpret. I used an old one from a dead Ender 3 with a 3D printed case to allow PTFE tubing to be inserted. You may need to alter the code depending on whether your runout sensor is active-high or active-low when filament is loaded.
- **WS2812B LED** - others may work, but will need adjusting in code.
- **Button & switch** - any toggle switch and momentary push button. The switch engages/disengages the motor, the button starts/stops respooling.
- **Pulleys & belt** - I use a 3:1 reduction for better torque, but that's probably not necessary for a 1kg spool. Spacing between the pulleys is ~58mm.
- **PSU** - depends on your motor's specs. A 12V 2.5A supply works well with the motor I used here.
- **Misc hardware:**
  - **Veroboard** - the project is designed to use two 25x64mm strips of veroboard for the electronics, with 2.54mm hole pitch. You could also use a single larger strip (or even have a PCB made), I just designed it around what I had on hand. The holes in opposite corners need to be drilled out for the M2 mounting screws. 
  - **Capacitors** - optional, but useful for decoupling/bulk capacitance. I use a small 0.1μF ceramic cap for the LED and TMC2209, 10μF electrolytic bulk cap for the 5V rail and a 470μF electrolytic cap for the 12V rail.
  - **40mm low-profile fan** - I'm using a Noctua 40mm fan to cool the TMC2209 and motor, but any low-profile 40mm fan should work. Just be sure to connect it to the right voltage rail (5V or 12V)
  - **Screws**:
    - 6x M2*4 for the electronics and LED retainer
    - 4x M3*8 + washers to attach the case to the main assembly
    - 8x M3*6 + washers to attach the lid and motor
    - 2x M3*4 to mount the buck converter
    - 4x fan screws (I didn't have any and ended up using M4 screws + nuts, but fan screws would be ideal)
  - **Power passthrough** - depends how you want to get power into the enclosure. I used a panel-mount barrel jack, but you could just use a grommet and hard-wire the power in - you'll need to add some internal strain relief if you go that route though.
  - **JST Connectors (optional)** - I wanted the project to be easily disassembled if needed, so I opted to use JST connectors to connect everything to the veroboard. This is completely optional, you can just solder everything if you want to save a lot of time crimping connectors.

## Tips & Caveats
- **ESP32 Logic Level**: The ESP32 has a logic level of 3.3V, but the WS2812B is expecting a Vih of `Vdd * 0.7`. For a 5V Vdd, that corresponds to a Vih of 3.5V. To account for this, you can either use a level shifter to raise the output from the ESP32 to 5V, or drop the voltage supplied to the LED. I connected the LED's Vdd through a 1N4007 diode, dropping Vdd to ~4.2V. This in turn lowers the expected Vih to ~3V, allowing the ESP32's 3.3V output to properly drive the LED.

## Acknowledgements

 - [esp_ws28xx](https://github.com/okhsunrog/esp_ws28xx) library by @okhsunrog

## Datasheets
- [TMC2209](https://www.analog.com/media/en/technical-documentation/data-sheets/tmc2209_datasheet_rev1.09.pdf)
- [ESP32-C3 Super Mini](https://dl.artronshop.co.th/ESP32-C3%20SuperMini%20datasheet.pdf)
- [LDO NEMA17 Speedy Power Motor](https://www.onetwo3d.co.uk/wp-content/uploads/2022/04/LDO-42STH48-2504AC_RevA.pdf)

## License

[GPL v3](https://www.gnu.org/licenses/gpl-3.0.html)

