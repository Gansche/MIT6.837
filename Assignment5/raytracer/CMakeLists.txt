cmake_minimum_required(VERSION 3.25)
project(raytracer)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
link_directories(lib/x64)

aux_source_directory(src SRC)

add_executable(raytracer ${SRC}
        src/rayTracer.h src/rayTracer.cpp
        src/Imglib/image.cpp src/Imglib/image.h
        src/LAlib/matrix.cpp src/LAlib/matrix.h src/LAlib/vectors.h
        src/rayTree.cpp src/rayTree.h
        src/boundingbox.cpp src/boundingbox.h
        src/marchinginfo.h)
target_link_libraries(raytracer libfreeglut.a opengl32.dll libglu32.a)