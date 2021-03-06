# Include the parent directory of PyReaktoro
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Collect the header and source files of PyReaktoro
file(GLOB_RECURSE HEADER_FILES *.hpp)
file(GLOB_RECURSE SOURCE_FILES *.cpp)

# Include the local Eigen directory so that pybind11 can work
include_directories(${CMAKE_SOURCE_DIR}/Reaktoro/Math)

# Create the PyReaktoro library
pybind11_add_module(PyReaktoro ${SOURCE_FILES} NO_EXTRAS) # NO_EXTRAS used to decrease linking time, but causes larger module file size

# Link PyReaktoro against Boost.Python and Python libraries
target_link_libraries(PyReaktoro LINK_PRIVATE ReaktoroShared)

# Allow cotire to add Eigen header files to the generated prefix header file
set_target_properties(PyReaktoro PROPERTIES
    COTIRE_PREFIX_HEADER_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/Reaktoro/Math/Eigen")

# Apply cotire to PyReaktoro target to improve compilation time
cotire(PyReaktoro)
