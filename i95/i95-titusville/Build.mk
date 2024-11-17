TARGET := titusville

LDFLAGS := -static
CFLAGS := -fno-stack-protector

RELRO := default
NX := 1

UBUNTU_VERSION := 22.04
DOCKER_IMAGE := i95-titusville
DOCKER_PORTS := 24607
DOCKER_TIMELIMIT := 30

PUBLISH_BUILD := $(TARGET)
