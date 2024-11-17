# Re-use existing binary
PUBLISH_TOP := $(DIR)/attachments/jungle.bin
PUBLISH_LIBC := libc.so.6
DOCKER_CHALLENGE_NAME := jungle
DOCKER_CHALLENGE_PATH := $(DIR)/attachments/jungle.bin

UBUNTU_VERSION := 24.04

DOCKER_IMAGE := jungle
DOCKER_PORTS := 24005
DOCKER_TIMELIMIT := 30
