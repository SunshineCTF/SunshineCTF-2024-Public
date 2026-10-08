TARGET := jacksonville

RELRO := default
NX := 1

GLIBC_VERSION := 2.38
DOCKER_IMAGE := sun24-i95-jacksonville
DOCKER_PORTS := 24608
DOCKER_TIMELIMIT := 30

PUBLISH_BUILD := $(TARGET)
PUBLISH_LIBC := libc.so.6

# `pwnmake check`: run the exploit and verify it prints the flag
$(call ctf_check,$(DIR),$(DOCKER_PORTS),python3 exploit.py)
