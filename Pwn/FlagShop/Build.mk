TARGET := flagshop

NX := 1
ASLR := 1
# RELRO := 1
# CANARY := 1
# STRIP := 1
# DEBUG := 1

PUBLISH_BUILD := $(TARGET)

# PwnableHarness 2.2 (used during the competition) defaulted to Ubuntu 24.04
UBUNTU_VERSION := 24.04

DOCKER_IMAGE := sun24-flagshop
DOCKER_PORTS := 24001
DOCKER_TIMELIMIT := 30

# `pwnmake check`: run the exploit and verify it prints the flag
$(call ctf_check,$(DIR),$(DOCKER_PORTS),python3 solver.py)
