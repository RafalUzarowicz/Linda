# -----------------------------------------------------------------------------
# CMake project wrapper Makefile ----------------------------------------------
# -----------------------------------------------------------------------------

SHELL := /bin/bash
BUILDDIR := build
TESTDIR := test
BINDIR := bin

all: ./build/Makefile
	@ $(MAKE) --silent -C build

./build/Makefile:
	@ mkdir -p build > /dev/null
	@ (cd build > /dev/null 2>&1 && cmake ..)

clean:
	$(RM) -r $(BUILDDIR) $(BINDIR) $(TESTDIR)

.PHONY: all clean