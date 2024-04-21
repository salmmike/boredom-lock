# Boredom-lock
This repository provides libboredomlock.

The Boredom-lock applications goal is to make sure you stay bored at least sometimes, and prevent you from frying your brain.

The Boredom-lock asks for your device to be plugged in to the machine running the application, and makes noisy alerts if you try to disconnect and use your device without a good reason.

For example, you can configure Boredom-lock to demand your device to be plugged in from 8PM to 6AM every day, and all the time during weekends.

## Configuration files style
The libboredomlock expects INI configuration files to operate.

The configfile contains the device USB vid:pid and lists of time periods when configured device should be plugged in.

Example configuration:
```ini
[My Device]
usb_id = babe:cafe
weekend = 00:00 - 24:00
weekdays = 00:00-04:00, 20:00 - 24:00
```

## Building
Built with CMake.

Init the `simpleini` submodule. `git submodule init & git submodule update`.

Set the CMAKE\_PREFIX\_PATH variable when running cmake, e.g.
`cmake -B build/ -S ./ && cd build && make -j8`

## Style
Use `clang-format -style="{BasedOnStyle: Mozilla, IndentWidth: 4}"`

