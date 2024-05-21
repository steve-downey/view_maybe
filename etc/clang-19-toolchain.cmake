include_guard(GLOBAL)

set(CMAKE_C_COMPILER clang-19)
set(CMAKE_CXX_COMPILER clang++-19)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS
  "-Wall -Wextra \
   -stdlib=libc++ --gcc-toolchain=/home/sdowney/.local"
CACHE STRING "CXX_FLAGS" FORCE)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3" CACHE STRING "C++ DEBUG Flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0" CACHE STRING "C++ Release Flags" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g" CACHE STRING "C++ RelWithDebInfo Flags" FORCE)
set(CMAKE_CXX_FLAGS_TSAN "-O3 -g -fsanitize=thread" CACHE STRING "C++ TSAN Flags" FORCE)
set(CMAKE_CXX_FLAGS_ASAN "-O3 -g -fsanitize=address,undefined,leak" CACHE STRING "C++ ASAN Flags" FORCE)
set(CMAKE_CXX_FLAGS_MSAN "-O3 -g -fsanitize=memory" CACHE STRING "C++ MSAN Flags" FORCE)
