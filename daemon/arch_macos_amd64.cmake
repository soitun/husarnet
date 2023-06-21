set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

set(triple x86_64-macos-none)

set(CMAKE_C_COMPILER "zig" cc -target ${triple})
set(CMAKE_CXX_COMPILER "zig" c++ -target ${triple})

set(CMAKE_AR "${CMAKE_CURRENT_LIST_DIR}/zig-ar.sh")
set(CMAKE_RANLIB "${CMAKE_CURRENT_LIST_DIR}/zig-ranlib.sh")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
