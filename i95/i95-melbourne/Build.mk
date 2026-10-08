TARGET := melbourne

NX := 1
ASLR := 1
RELRO := 1
CANARY := 1

UBUNTU_VERSION := 22.04
DOCKER_IMAGE := sun24-i95-melbourne
DOCKER_PORTS := 24601
DOCKER_TIMELIMIT := 30

PUBLISH_BUILD := $(TARGET)

# `pwnmake check`: run the exploit and verify it prints the flag
$(call ctf_check,$(DIR),$(DOCKER_PORTS),python3 exploit.py)
