TARGET := secure_flag_terminal

NX := 1
ASLR := 1
CANARY := 1
RELRO := 1
STRIP := 1

UBUNTU_VERSION := 18.04
DOCKER_IMAGE := sun24-secure-flag-terminal
DOCKER_PORTS := 24002
DOCKER_TIMELIMIT := 30

PUBLISH_BUILD := $(TARGET)
PUBLISH_LIBC := libc.so.6
PUBLISH_LD := ld-linux-x86-64.so.2

# `pwnmake check`: run the exploit and verify it prints the flag
$(call ctf_check,$(DIR),$(DOCKER_PORTS),python3 solve.py)
