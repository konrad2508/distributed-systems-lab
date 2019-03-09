project(lab01)
set(CMAKE_CXX_STANDARD 17)

add_executable(main src/main.cpp)

target_link_libraries(main libws2_32.a)