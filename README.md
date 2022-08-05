# ESP32-LIB library components

esp32-lib libraryies is a ESP32 tools, peripherals drivers and components drivers for hardwares usage.

## Using esp32-lib library

### Cloning

Clone this repository somewhere, e.g.:

```Shell
cd ~/esp
git clone https://github.com/MuriloAM/esp32-lib.git
```

### Adding to your project

Add path to components in your [project makefile](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/build-system-legacy.html#minimal-example-makefile),
e.g:

```Makefile
PROJECT_NAME := my-esp-project
EXTRA_COMPONENT_DIRS := /home/user/esp/esp32-lib/components
include $(IDF_PATH)/make/project.mk
```

or in [CMakeLists.txt](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#minimal-example-cmakelists):

```CMake
cmake_minimum_required(VERSION 3.5)
set(EXTRA_COMPONENT_DIRS /home/user/esp/esp32-lib/components)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(my-esp-project)
```

## Library list

| Component                | Description                                                                      | License | Supported on       | Thread safety
|--------------------------|----------------------------------------------------------------------------------|---------|--------------------|--------------
| **new_component**              |            | MIT   | `esp32`, `esp32s2`, `esp32c3` | Yes

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)

## Credits