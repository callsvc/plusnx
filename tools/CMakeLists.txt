cmake_minimum_required(VERSION 3.29)

add_executable(sysdtl sysdtl/main.cpp)
target_link_libraries(sysdtl PRIVATE mbedcrypto Boost::endian)
