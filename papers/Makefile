#! /usr/bin/make -f
# -*-makefile-*-

CMAKE_FLAGS?=
PROJECT?=$(shell basename $(CURDIR))
USE_DOCKER_FILE:=.use-docker
DOCKER_CMD := docker volume create cmake.bld; docker-compose run --rm dev
LOCAL_MAKE_CMD := make -e -f targets.mk
MAKE_CMD := $(LOCAL_MAKE_CMD)
REQS_MARKER := $(VENV)/bin/.pip-sync
# These targets are only run locally
LOCAL_ONLY_TARGETS :=
DEPS_DIR = .deps
MAKEFLAGS += --no-builtin-rules

.DEFAULT_GOAL := help

export

# If .lcldev/use-docker exists, then set `USE_DOCKER` to True
ifneq ("$(wildcard $(USE_DOCKER_FILE))","")
	USE_DOCKER := True
endif

ifdef USE_DOCKER
	MAKE_CMD := $(DOCKER_CMD) $(MAKE_CMD)
endif

.DEFAULT:
	$(MAKE_CMD) $@


.SUFFIXES:

%.pdf : %.tex
	$(MAKE_CMD) $@

# These targets that should only be run locally.
$(LOCAL_ONLY_TARGETS):
	$(LOCAL_MAKE_CMD) $@

.update-submodules:
	git submodule update --init --recursive
	touch .update-submodules

.gitmodules: .update-submodules

.PHONY: use-docker
use-docker: ## Create docker switch file so that subsequent `make` commands run inside docker container.
	touch $(USE_DOCKER_FILE)

.PHONY: remove-docker
remove-docker: ## Remove docker switch file so that subsequent `make` commands run locally.
	$(RM) $(USE_DOCKER_FILE)

.PHONY: docker-rebuild
docker-rebuild: ## Rebuilds the docker file using the latest base image.
	docker-compose build

.PHONY: docker-clean
docker-clean: ## Clean up the docker volumes and rebuilds the image from scratch.
	docker-compose down -v
	docker-compose build

.PHONY: docker-shell
docker-shell: ## Shell in container
	docker-compose run --rm dev


# Help target
help: ## Show this help.
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'  $(MAKEFILE_LIST) targets.mk | sort

wg21.bib:
	curl https://wg21.link/index.bib > wg21.bib

# Include dependencies
$(foreach file,$(TARGET),$(eval -include $(DEPS_DIR)/$(file).d))
