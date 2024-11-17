# Never ended up building with PwnableHarness, the binary was just different
# enough that the existing exploit didn't work (and I didn't have time to fix
# the exploit to work with the new binary). -kcolley
ifdef BUILD_WITH_PWNABLEHARNESS

TARGET := adventure-on-the-high-c

NX := default
ASLR := 1
RELRO := 1
CANARY := 0

# Not needed when passing --retain-symbols-file
# STRIP := 1

# Don't strip ALL symbols
LDFLAGS := -Wl,--retain-symbols-file=$(BUILD_DIR)/keep_symbols.txt
$(BUILD_DIR)/$(TARGET): $(BUILD_DIR)/keep_symbols.txt
$(BUILD_DIR)/keep_symbols.txt:
	$(_v)echo "main" > $@

PUBLISH_BUILD := $(TARGET)

else #!BUILD_WITH_PWNABLEHARNESS

# Re-use existing binary
PUBLISH_TOP := $(wildcard $(DIR)/attachments/*)
DOCKER_CHALLENGE_NAME := ship
DOCKER_CHALLENGE_PATH := $(DIR)/attachments/ship.bin

endif #!BUILD_WITH_PWNABLEHARNESS

UBUNTU_VERSION := 24.04

DOCKER_IMAGE := ship
DOCKER_PORTS := 24003

# kcolley: When testing the author exploit against the remote challenge server,
# I observed that it took somewhere around 2 minutes to complete. To account for
# teams in remote parts of the world with higher round-trip latency, I decided
# to increase the time limit for this challenge to 5 minutes.
DOCKER_TIMELIMIT := 300
