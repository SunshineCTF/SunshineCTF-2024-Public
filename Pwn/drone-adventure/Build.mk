# Re-use existing binary
PUBLISH_TOP := $(DIR)/attachments/drone.bin
DOCKER_CHALLENGE_NAME := drone
DOCKER_CHALLENGE_PATH := $(DIR)/attachments/drone.bin

UBUNTU_VERSION := 24.04

DOCKER_IMAGE := drone
DOCKER_PORTS := 24004
DOCKER_TIMELIMIT := 30
