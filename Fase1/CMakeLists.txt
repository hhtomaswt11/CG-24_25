cmake_minimum_required(VERSION 3.5)

# Project Name
PROJECT(fase1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Define os executáveis a serem construídos neste projecto
add_executable(engine ./src/Engine/engine.cpp)
add_executable(generator ./src/Generator/generator.cpp)


# Biblioteca das figuras, dos pontos e user-interface
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
)

# Biblioteca de parsing de XML
add_library(
        tinyXML
        ./tinyXML/tinystr.cpp
        ./tinyXML/tinyxmlerror.cpp
        ./tinyXML/tinyxmlparser.cpp
        ./tinyXML/tinystr.h
        ./tinyXML/tinyxml.h
        ./tinyXML/tinyxml.cpp
)

target_include_directories(engine PRIVATE ./include/Utils)
target_include_directories(generator PRIVATE ./include/Utils)


target_link_libraries(tinyXML) # remover no futuro 

# Associa a biblioteca das figuras e dos pontos aos executáveis
target_link_libraries(generator Utils_n_Build_lib)
target_link_libraries(engine Utils_n_Build_lib tinyXML )


set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

if  (WIN32)

    message(STATUS "Toolkits_DIR set to: " ${TOOLKITS_FOLDER})
    set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")

    if (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(ERROR ": GLUT not found")
    endif (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")


    include_directories(${TOOLKITS_FOLDER}/glut )
    #target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES}
    #									  ${TOOLKITS_FOLDER}/glut/glut32.lib)
    target_link_libraries(generator ${OPENGL_LIBRARIES} ${TOOLKITS_FOLDER}/glut/glut32.lib)

    target_link_libraries(engine ${OPENGL_LIBRARIES} ${TOOLKITS_FOLDER}/glut/glut32.lib)

    if (EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll" )
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll" )

    #set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
else (WIN32) #Linux and Mac

    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR})
    link_directories(${GLUT_LIBRARY_DIRS})
    add_definitions(${GLUT_DEFINITIONS})

    target_link_libraries(generator ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} )
    target_link_libraries(engine ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} )
    if(NOT GLUT_FOUND)
        message(ERROR ": GLUT not found!")
    endif(NOT GLUT_FOUND)

endif(WIN32)