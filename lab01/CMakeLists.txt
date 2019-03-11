project(lab01)
set(CMAKE_CXX_STANDARD 17)

add_executable(main src/main.cpp)
add_executable(client src/testUdpClient.cpp)
add_executable(server src/testUdpServer.cpp)

target_link_libraries(main libws2_32.a)
target_link_libraries(client libws2_32.a)
target_link_libraries(server libws2_32.a)