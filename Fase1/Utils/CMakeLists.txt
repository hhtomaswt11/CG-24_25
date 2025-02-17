# Set the library name
add_library(Utils STATIC
        primitive.cpp
        point.cpp
        XMLDataFormat.cpp
)

# If you want to include header files globally for this directory, you can add:
target_include_directories(Utils PUBLIC .)
