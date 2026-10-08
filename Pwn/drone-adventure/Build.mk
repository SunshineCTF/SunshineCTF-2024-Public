# Re-use existing binary
PUBLISH_TOP := $(DIR)/attachments/drone.bin
DOCKER_CHALLENGE_NAME := drone
DOCKER_CHALLENGE_PATH := $(DIR)/attachments/drone.bin

UBUNTU_VERSION := 24.04

DOCKER_IMAGE := sun24-drone
DOCKER_PORTS := 24004
DOCKER_TIMELIMIT := 30

# `pwnmake check`: run the exploit and verify it prints the flag
$(call ctf_check,$(DIR),$(DOCKER_PORTS),python3 solve/pwn-armx.py)
