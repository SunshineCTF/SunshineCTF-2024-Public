TARGET := heap01

RELRO := default
CANARY := 1
NX := 1

GLIBC_VERSION := 2.35
DOCKER_IMAGE := sun24-heap01
DOCKER_PORTS := 24006
DOCKER_TIMELIMIT := 30

PUBLISH_BUILD := $(TARGET)
PUBLISH_LIBC := libc.so.6

# `pwnmake check`: run the exploit and verify it prints the flag
$(call ctf_check,$(DIR),$(DOCKER_PORTS),python3 exploit.py)
