set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(${CMAKE_CURRENT_LIST_DIR}/GNU.CXX.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Intel.CXX.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Clang.CXX.cmake)
