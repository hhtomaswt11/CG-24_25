cmake_minimum_required(VERSION 3.5)

# Project Name
PROJECT(fase3)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Define executables
add_executable(engine ./src/Engine/engine.cpp)
add_executable(generator ./src/Generator/generator.cpp)

# Library for figures, points, and user interface
add_library(
    Utils_n_Build_lib
    ./src/Utils/point.cpp
    ./include/Utils/point.h
    ./src/Utils/primitive.cpp
    ./include/Utils/primitive.h
    ./src/Utils/XMLDataFormat.cpp
    ./include/Utils/XMLDataFormat.h
    ./src/Generator/buildPrimitives.cpp
    ./include/Generator/buildPrimitives.h
    ./src/UI/ui.cpp 
    ./include/UI/ui.h 
    ./src/Generator/bezier.cpp
    ./include/Generator/bezier.h
)

# Library for XML parsing (tinyXML)
add_library(
    tinyXML
    ./tinyXML/tinystr.cpp
    ./tinyXML/tinyxmlerror.cpp
    ./tinyXML/tinyxmlparser.cpp
    ./tinyXML/tinystr.h
    ./tinyXML/tinyxml.h
    ./tinyXML/tinyxml.cpp
)

# Linking libraries
target_link_libraries(generator Utils_n_Build_lib)
target_link_libraries(engine Utils_n_Build_lib tinyXML)

# Find OpenGL package
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
if (NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

# Include OpenGL directories
target_include_directories(engine PRIVATE ${OPENGL_INCLUDE_DIRS})
target_include_directories(generator PRIVATE ${OPENGL_INCLUDE_DIRS})

# Find GLEW and GLUT
find_package(GLEW REQUIRED)
if (NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW not found!")
endif()

find_package(GLUT REQUIRED)
if (NOT GLUT_FOUND)
    message(FATAL_ERROR "GLUT not found!")
endif()

# Include GLEW and GLUT directories
target_include_directories(engine PRIVATE ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})
target_include_directories(generator PRIVATE ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})

# Linking GLEW and GLUT
target_link_libraries(generator ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})
target_link_libraries(engine ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})

# Handle platform-specific settings
if(WIN32)
    message(STATUS "Configuring for Windows")

    # Ensure correct path for GLUT on Windows
    set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")
    if (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(FATAL_ERROR "GLUT not found in the specified TOOLKITS_FOLDER")
    endif()

    include_directories(${TOOLKITS_FOLDER}/glut)
    target_link_libraries(generator ${TOOLKITS_FOLDER}/glut/glut32.lib)
    target_link_libraries(engine ${TOOLKITS_FOLDER}/glut/glut32.lib)

    if (EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif()

else()  # Linux and macOS
    message(STATUS "Configuring for Linux/macOS")

    # Add compile flags for Linux/macOS
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

    target_link_libraries(generator ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    target_link_libraries(engine ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
endif()

# Add the executable to the appropriate directories (for organization)
set_target_properties(engine PROPERTIES FOLDER "Executables")
set_target_properties(generator PROPERTIES FOLDER "Executables")
