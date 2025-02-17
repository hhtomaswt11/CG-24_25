# This tells CMake to compile the files in the tinyXML directory into a library
add_library(tinyXML STATIC
        tinyxml.cpp  # Replace with actual files in tinyXML
)

# Include directories if necessary (e.g., for headers)
target_include_directories(tinyXML PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
