cmake_minimum_required(VERSION 3.24)
project(Cloud-Doba2000 VERSION 2.0.0 DESCRIPTION "HinaPE is not a Physics Engine" LANGUAGES CXX)

add_executable(cloud-doba2000)
file(GLOB headers include/*.h)
file(GLOB sources src/*.cpp)
target_sources(
        cloud-doba2000
        PRIVATE
        ${sources}

        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../
        FILES
        ${headers}
)
#set_target_properties(cloud-doba2000 PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS ON)
target_include_directories(cloud-doba2000 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} include)

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
target_include_directories(cloud-doba2000 PUBLIC ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
target_link_libraries(cloud-doba2000 PUBLIC ${OPENGL_LIBRARIES} )
target_link_options(cloud-doba2000 PUBLIC -framework GLUT)