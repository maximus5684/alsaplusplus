cmake_minimum_required(VERSION 2.8.3)
project(AlsaPlusPlus)

add_definitions(-std=c++11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(AlsaPlusPlus_VERSION_MAJOR 0)
set(AlsaPlusPlus_VERSION_MINOR 1)
set(AlsaPlusPlus_VERSION_PATCH 0)
set(AlsaPlusPlus_VERSION ${AlsaPlusPlus_VERSION_MAJOR}.${AlsaPlusPlus_VERSION_MINOR}.${AlsaPlusPlus_VERSION_PATCH})

set(HEADER_DIR ${CMAKE_SOURCE_DIR}/include)

set(HEADERS
  ${HEADER_DIR}/common.hpp;
  ${HEADER_DIR}/error.hpp;
  ${HEADER_DIR}/mixer.hpp;
  ${HEADER_DIR}/pcm.hpp;
)

include_directories (
  ${HEADER_DIR}
)

add_library(
  ${PROJECT_NAME} SHARED
  src/error.cpp
  src/mixer.cpp
  src/pcm.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${AlsaPlusPlus_VERSION} SOVERSION ${AlsaPlusPlus_VERSION_MAJOR})

target_link_libraries(
  ${PROJECT_NAME}
  asound
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include)

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(
  wav_player
  examples/wav_player.cpp
)

target_link_libraries(
  wav_player
  ${PROJECT_NAME}
)
