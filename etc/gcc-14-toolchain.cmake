include_guard(GLOBAL)

set(CMAKE_C_COMPILER gcc-14)
set(CMAKE_CXX_COMPILER g++-14)

set(CMAKE_CXX_FLAGS
  "-std=c++23 \
   -Wall -Wextra "
CACHE STRING "CXX_FLAGS" FORCE)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3" CACHE STRING "C++ DEBUG Flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -DNDEBUG" CACHE STRING "C++ Release Flags" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -DNDEBUG" CACHE STRING "C++ RelWithDebInfo Flags" FORCE)
set(CMAKE_CXX_FLAGS_TSAN "-O3 -g -fsanitize=thread" CACHE STRING "C++ TSAN Flags" FORCE)
set(CMAKE_CXX_FLAGS_ASAN "-O3 -g -fsanitize=address,undefined,leak" CACHE STRING "C++ ASAN Flags" FORCE)
set(CMAKE_CXX_FLAGS_MSAN "-O3 -g -fsanitize=memory" CACHE STRING "C++ MSAN Flags" FORCE)
