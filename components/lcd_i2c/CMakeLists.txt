cmake_minimum_required(VERSION 3.5)

# get target device
idf_build_get_property(target IDF_TARGET)

# set component source files
set(srcs "lcd_i2c.c")

# set component include directories
set(include_dirs include)

# set other required component files
set(required i2cbus)

# register component
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${include_dirs}
                    REQUIRES ${required})