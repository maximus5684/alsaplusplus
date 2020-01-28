cmake_minimum_required(VERSION 3.1)
project(AlsaPlusPlus)

set(AlsaPlusPlus_VERSION_MAJOR 0)
set(AlsaPlusPlus_VERSION_MINOR 1)
set(AlsaPlusPlus_VERSION_PATCH 1)
set(AlsaPlusPlus_VERSION ${AlsaPlusPlus_VERSION_MAJOR}.${AlsaPlusPlus_VERSION_MINOR}.${AlsaPlusPlus_VERSION_PATCH})

option(WITH_EXAMPLES "Build and install example programs" OFF)
option(INSTALL_HEADERS "Install library headers" ON)

set(CMAKE_CXX_STANDARD 14)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADERS
  ${HEADER_DIR}/alsaplusplus/common.hpp;
  ${HEADER_DIR}/alsaplusplus/error.hpp;
  ${HEADER_DIR}/alsaplusplus/mixer.hpp;
  ${HEADER_DIR}/alsaplusplus/pcm.hpp;
)

include_directories(${HEADER_DIR})

add_library(
  ${PROJECT_NAME} SHARED
  src/error.cpp
  src/mixer.cpp
  src/pcm.cpp
)

set_target_properties(
  ${PROJECT_NAME} PROPERTIES
  VERSION ${AlsaPlusPlus_VERSION}
  SOVERSION ${AlsaPlusPlus_VERSION_MAJOR}
)
target_link_libraries(${PROJECT_NAME} asound)

if(WITH_EXAMPLES)
  add_executable(set_volume examples/set_volume.cpp)
  target_link_libraries(set_volume ${PROJECT_NAME})

  add_executable(wav_player examples/wav_player.cpp)
  target_link_libraries(wav_player ${PROJECT_NAME})

  install(
    TARGETS set_volume wav_player
    DESTINATION share/alsaplusplus/examples
  )
endif(WITH_EXAMPLES)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)

if(INSTALL_HEADERS)
  install(
    FILES ${HEADERS}
    DESTINATION include/alsaplusplus
  )
endif(INSTALL_HEADERS)

if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
  )
  add_custom_target(
    uninstall COMMAND
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  )
endif(NOT TARGET uninstall)
