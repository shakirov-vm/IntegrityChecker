cmake_minimum_required(VERSION 3.1)

project(
  CRC32DAEMON
  VERSION 1.0
  LANGUAGES CXX)

add_executable(daemon src/main.cpp src/crc32.cpp src/daemon.cpp src/handler.cpp)

set_property(TARGET daemon PROPERTY CXX_STANDARD 17)
