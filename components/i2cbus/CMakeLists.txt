cmake_minimum_required(VERSION 3.5)

# get target device
idf_build_get_property(target IDF_TARGET)

# set component source files
set(srcs "i2cbus.c")

# set component include directories
set(include_dirs include)

# register component
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "${include_dirs}")
