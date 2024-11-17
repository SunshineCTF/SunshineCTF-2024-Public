TARGET := daytonabeach

RELRO := default
NX := 1

GLIBC_VERSION := 2.35
DOCKER_IMAGE := i95-daytonabeach
DOCKER_PORTS := 24605
DOCKER_TIMELIMIT := 30

PUBLISH_BUILD := $(TARGET)
PUBLISH_LIBC := libc.so.6
PUBLISH_LD := ld-linux-x86-64.so.2
