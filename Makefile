INSTALL_PREFIX?=../install

ifeq (clang,$(TOOLCHAIN))
	BUILD_NAME?=build-clang
	BUILD_DIR?=../cmake.bld/$(shell basename $(CURDIR))
	BUILD_PATH?=$(BUILD_DIR)/$(BUILD_NAME)
	BUILD_TYPE?=RelWithDebInfo
	CMAKE_ARGS=-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
else ifeq (clang-master,$(TOOLCHAIN))
	BUILD_NAME?=build-clang-master
	BUILD_DIR?=../cmake.bld/$(shell basename $(CURDIR))
	BUILD_PATH?=$(BUILD_DIR)/$(BUILD_NAME)
	BUILD_TYPE?=RelWithDebInfo
	export LLVM_ROOT?=~/install/llvm-master
	CMAKE_ARGS=-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/etc/llvm-master-toolchain.cmake
else
	BUILD_NAME?=build
	BUILD_DIR?=../cmake.bld/$(shell basename $(CURDIR))
	BUILD_PATH?=$(BUILD_DIR)/$(BUILD_NAME)
	BUILD_TYPE?=RelWithDebInfo
endif

define run_cmake =
	cmake \
	-G "Unix Makefiles" \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-DCMAKE_INSTALL_PREFIX=$(abspath $(INSTALL_PREFIX)) \
	$(CMAKE_ARGS) \
	$(CURDIR)
endef

default: build

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

$(BUILD_PATH)/CMakeCache.txt: | $(BUILD_PATH)
	cd $(BUILD_PATH) && $(run_cmake)

build: $(BUILD_PATH)/CMakeCache.txt
	cd $(BUILD_PATH) && make -k

install: $(BUILD_PATH)/CMakeCache.txt
	cd $(BUILD_PATH) && make install

ctest: $(BUILD_PATH)/CMakeCache.txt
	cd $(BUILD_PATH) && ctest

ctest_ : build
	cd $(BUILD_PATH) && ctest

test: ctest_

cmake: | $(BUILD_PATH)
	cd $(BUILD_PATH) && $(run-cmake)

clean: $(BUILD_PATH)/CMakeCache.txt
	cd $(BUILD_PATH) && make clean

realclean:
	rm -rf $(BUILD_PATH)
