# AnimatronicEye

This repository holds all the software and hardware files for my animatronic eye project.

![animatronic_eye_joystick_demo.gif](media/animatronic_eye_joystick_demo.gif)

You can find a video on its design in [here](https://youtu.be/aWKz5cGIKcs).

## 3D Models

You can find the STL files ready for 3D printing in the `3D` directory.

The eyelid linkage is the same for the upper and lower eyelid so just print that one twice.

I also suggest printing every model that has a sphere vertically so that the spheres don't
require support which will make them rounder and more uniform.

## Software

This project is built with `platformio` so make sure you have it installed so that
its easy to build and upload the code.

The code is written for the `esp32` microcontroller development board.

For now there is only the basic firmware to control the ye with a basic joystick and also run
hardcoded animations that could probably be improved.

## License

- **Software Code**: Licensed under the GNU General Public License (GPL). See [LICENSE_CODE](./LICENSE_CODE) for details.
- **3D Models (STL Files)**: Licensed under the Creative Commons Attribution-NonCommercial (CC BY-NC) license. See [LICENSE_MODELS](./LICENSE_MODELS) for details.
