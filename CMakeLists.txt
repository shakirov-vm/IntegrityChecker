cmake_minimum_required(VERSION 3.1)

project(
  CRC32DAEMON
  VERSION 1.0
  LANGUAGES CXX)

add_executable(daemon main.cpp crc32.cpp daemon.cpp handler.cpp)

set_property(TARGET daemon PROPERTY CXX_STANDARD 17)
