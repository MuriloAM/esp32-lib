# set component source files
set(srcs "main.c")

# set component include directories
set(include_dirs include)

# register component
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "${include_dirs}")