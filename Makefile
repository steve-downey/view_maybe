#! /usr/bin/make -f
# -*-makefile-*-
INSTALL_PREFIX?=/home/sdowney/install
BUILD_DIR?=../cmake.bld/$(shell basename $(CURDIR))
BUILD_TYPE?=RelWithDebInfo
DEST?=../install
CMAKE_FLAGS?=

ifeq (,$(VER))
	_ver_suffix=
else
	_ver_suffix=-$(VER)
endif

ifeq (,$(COMPILER))
	_build_name=build
else ifeq (clang,$(COMPILER))
	_build_name=build-clang${_ver_suffix}
	_cmake_args=-DCMAKE_C_COMPILER=clang$(_vers_suffix) -DCMAKE_CXX_COMPILER=clang++${_ver_suffix}
else ifeq (gcc,$(COMPILER))
	_build_name=build-gcc${_ver_suffix}
	_cmake_args=-DCMAKE_C_COMPILER=gcc${_ver_suffix} -DCMAKE_CXX_COMPILER=g++${_ver_suffix}
else ifeq (clang-master,$(COMPILER))
	_build_name=build-clang-master
	_cmake_args=-DCMAKE_C_COMPILER=/home/sdowney/install/llvm-master/bin/clang \
		-DCMAKE_CXX_COMPILER=/home/sdowney/install/llvm-master/bin/clang++
else ifeq (clang-concepts,$(COMPILER))
	_build_name=build-clang-concepts
	_cmake_args=-DCMAKE_C_COMPILER=/home/sdowney/install/llvm-concepts/bin/clang \
		-DCMAKE_CXX_COMPILER=/home/sdowney/install/llvm-concepts/bin/clang++
endif


_build_path?=$(BUILD_DIR)/$(_build_name)

define run_cmake =
	cmake \
	-G "Ninja" \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-DCMAKE_INSTALL_PREFIX=$(abspath $(INSTALL_PREFIX)) \
	$(_cmake_args) \
	$(CMAKE_FLAGS) \
	$(CURDIR)
endef

default: build

$(_build_path):
	mkdir -p $(_build_path)

$(_build_path)/CMakeCache.txt: | $(_build_path)
	cd $(_build_path) && $(run_cmake)

build: $(_build_path)/CMakeCache.txt
	ninja -C $(_build_path) -k 0

install: $(_build_path)/CMakeCache.txt
	DESTDIR=$(abspath $(DEST)) ninja -C $(_build_path) -k 0  install

ctest: $(_build_path)/CMakeCache.txt
	cd $(_build_path) && ctest

ctest_ : build
	cd $(_build_path) && ctest

test: ctest_

cmake: $(_build_path)
	echo ${run_cmake}
	cd $(_build_path) && ${run_cmake}

clean: $(_build_path)
	ninja -C $(_build_path) clean

realclean:
	rm -rf $(_build_path)

.PHONY : default build install ctest cmake clean realclean
