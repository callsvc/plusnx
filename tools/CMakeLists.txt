cmake_minimum_required(VERSION 3.29)

add_executable(telemetry telemetry/main.cpp)
target_link_libraries(telemetry PRIVATE mbedcrypto Boost::endian)
