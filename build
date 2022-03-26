#! /usr/bin/make -f
# -*-makefile-*-
INSTALL_PREFIX?=/home/sdowney/install
BUILD_DIR?=../cmake.bld/$(shell basename $(CURDIR))
BUILD_TYPE?=RelWithDebInfo
DEST?=../install
CMAKE_FLAGS?=

ifeq ($(strip $(TOOLCHAIN)),)
	_build_name?=build
	_build_dir?=../cmake.bld/$(shell basename $(CURDIR))
	_build_type?=RelWithDebInfo
else
	_build_name?=build-$(TOOLCHAIN)
	_build_dir?=../cmake.bld/$(shell basename $(CURDIR))
	_build_type?=RelWithDebInfo
	_cmake_args=-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/etc/$(TOOLCHAIN)-toolchain.cmake
endif


_build_path?=$(_build_dir)/$(_build_name)

define run_cmake =
	cmake \
	-G "Ninja" \
	-DCMAKE_BUILD_TYPE=$(_build_type) \
	-DCMAKE_INSTALL_PREFIX=$(abspath $(INSTALL_PREFIX)) \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
	$(_cmake_args) \
	$(CURDIR)
endef

default: compile

$(_build_path):
	mkdir -p $(_build_path)

$(_build_path)/CMakeCache.txt: | $(_build_path)
	cd $(_build_path) && $(run_cmake)
	-rm compile_commands.json
	ln -s $(_build_path)/compile_commands.json

compile: $(_build_path)/CMakeCache.txt
	ninja -C $(_build_path) -k 0

install: $(_build_path)/CMakeCache.txt
	DESTDIR=$(abspath $(DEST)) ninja -C $(_build_path) -k 0  install

ctest: $(_build_path)/CMakeCache.txt
	cd $(_build_path) && ctest

ctest_ : compile
	cd $(_build_path) && ctest

test: ctest_

cmake: |  $(_build_path)
	cd $(_build_path) && ${run_cmake}

clean: $(_build_path)/CMakeCache.txt
	ninja -C $(_build_path) clean

realclean:
	rm -rf $(_build_path)

.PHONY: install ctest cmake clean realclean
