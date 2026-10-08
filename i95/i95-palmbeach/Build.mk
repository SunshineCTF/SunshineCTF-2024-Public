TARGET := palmbeach

RELRO := 1
ASLR := 1

UBUNTU_VERSION := 22.04
DOCKER_IMAGE := sun24-i95-palmbeach
DOCKER_PORTS := 24603
DOCKER_TIMELIMIT := 30

PUBLISH_BUILD := $(TARGET)

# `pwnmake check`: run the exploit and verify it prints the flag
$(call ctf_check,$(DIR),$(DOCKER_PORTS),python3 exploit.py)
