
[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://opensource.org/licenses/)


# V-Spooler Motor Mod

An addon for the [V-Spooler](https://www.printables.com/model/684376-v-spooler) project to enable automation via an ESP32 and a stepper motor.

Should be fairly easy to port over to Arduino if anybody is so inclined, I think I've kept the project layout pretty sane and readable.


## Features

- Uses a filament runout sensor to automatically stop when spooling is complete.
- Indicates status via a WS2812B (NeoPixel) LED.


## Required Hardware
- **V-Spooler** - only tested with the base model, not any of the extended versions like V-Spooler X. Untested with the desk mount, but it will almost certainly interfere since it uses the same mounting holes. A modified case might be possible to get around this, but I don't use the desk mount so haven't looked into it.
- **ESP32** - I used an ESP32C3 Super Mini for its small form factor, but any should work. I recommend something with 5V logic though, or you'll need to do some level shifting (see below)
    - **Buck converter (optional)** - depends whether your MCU can handle the same voltage as the motor. Some can handle up to 12V or 15V. Mine can't. RIP.
- **TMC2209** - or any motor driver compatible with your microcontroller.
- **Stepper motor** - any which physically fits the project and can be run by your hardware. I used an LDO Nema 17 Speedy Power.
- **Runout sensor** - any which outputs a signal your MCU can interpret. I used an old one from a dead Ender 3 with a 3D printed case to allow PTFE tubing to be inserted. You may need to alter the code depending on whether your runout sensor outputs a high or low signal when filament is detected.
- **WS2812B LED** - others may work, but will need adjusting in code.
  - **Level shifter (or equivalent)** - if you use a microcontroller with 3.3V logic, you either need to shift the logic level to at least 3.5V, or lower Vdd (WS2812B logic level is Vdd*0.7). I just connected the supply voltage through a diode (1N4007) to drop Vdd to ~4.2V, which allows 3.3V logic to control the LED.
- **Button & switch** - any toggle switch and momentary push button. The switch engages/disengages the motor, the button starts/stops respooling.
- **Pulleys & belt** - I use a 3:1 reduction for better torque, but that's probably not necessary for a 1kg spool. Spacing between the pulleys is ~58mm.
- **PSU** - whatever is suitable for your chosen motor - 12V 2.5A in my case.
- **Misc hardware:**
  - **Veroboard** - the project is designed to use two 25x64mm strips of veroboard for the electronics, with 2.54mm hole pitch. You could also use a single larger strip (or even have a PCB made), I just designed it around what I had on hand. The holes in opposite corners need to be drilled out for the M2 mounting screws. 
  - **Capacitors** - optional, but useful for decoupling/bulk capacitance. I use a small 0.1μF ceramic cap for the LED and TMC2209, 10μF electrolytic bulk cap for the 5V rail and a 470μF electrolytic cap for the 12V rail.
  - **40mm fan (optional-ish)** - The TMC2209 and stepper motor can get quite hot if you run it at high RPM for a long time, so adding a small fan to the project is recommended. I used a Noctua 40mm fan that I had lying around, but any low-profile 40mm fan will work. Just make sure you connect it to the right power rail depending on whether it's a 5V or 12V fan.
  - **Screws**:
    - 6x M2*4 for the electronics and LED retainer
    - 4x M3*8 + washers to attach the case to the main assembly
    - 8x M3*6 + washers to attach the lid and motor
    - 2x M3*4 to mount the buck converter
    - 4x fan screws (I didn't have any and ended up using M4 screws + nuts, but fan screws would be ideal)
  - **Power passthrough** - depends how you want to get power into the enclosure. I used a panel-mount barrel jack, but you could just use a grommet and hard-wire the power in - you'll need to add some internal strain relief if you go that route though.
  - **JST Connectors (optional)** - I wanted the project to be easily disassembled if needed, so I opted to use JST connectors to connect everything to the veroboard. This is completely optional, you can just solder everything if you want to save a lot of time crimping connectors.


## Acknowledgements

 - [esp_ws28xx](https://github.com/okhsunrog/esp_ws28xx) library by @okhsunrog


## License

[GPL v3](https://choosealicense.com/licenses/gpl-3.0/)

