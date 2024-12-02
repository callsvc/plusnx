cmake_minimum_required(VERSION 3.29)

add_executable(telemetry telemetry/main.cpp)
add_executable(encode encode/main.cpp)
add_executable(binary2hex binary2hex/main.cpp)
target_link_libraries(telemetry PRIVATE mbedcrypto Boost::endian)
target_link_libraries(binary2hex PRIVATE mbedcrypto Boost::program_options)
