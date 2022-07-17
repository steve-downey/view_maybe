#include wg21/Makefile




wg21.bib:
	curl https://wg21.link/index.bib > wg21.bib

VENV := .venv
SOURCE_VENV := . $(VENV)/bin/activate;
PYEXEC := $(SOURCE_VENV) python
PIP_SYNC := $(PYEXEC) -m piptools sync
PYEXECPATH ?= $(shell which python3.9 || which python3.9 || which python3.8 || which python3.7 || which python3)
PYTHON ?= $(shell basename $(PYEXECPATH))
REQS_MARKER := $(VENV)/bin/.pip-sync
PIP := $(PYEXEC) -m pip

override DEPS := $(VENV) wg21.bib

$(VENV):
	$(PYTHON) -m venv $(VENV)
	$(PIP) install setuptools pip pip-tools wheel
	$(PIP_SYNC) requirements.txt

.PHONY: create-venv
create-venv:
	rm -rf $(VENV)
	make $(VENV)


override PDF := $(SRC:.tex=.pdf)
.PHONY: pdf
pdf: $(PDF)

%.pdf: %.tex wg21.bib | $(VENV)
	$(SOURCE_VENV) latexmk --shell-escape $<
