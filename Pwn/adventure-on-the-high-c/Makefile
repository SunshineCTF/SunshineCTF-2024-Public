CFLAGS = -fno-stack-protector -pie -Wl,-z,relro,-z,now -fvisibility=hidden
LDFLAGS = -Wl,--retain-symbols-file=keep_symbols.txt -s
CHAL   = ship
SRC    = $(CHAL).c
BIN    = $(CHAL).bin


example:
	echo "main" > keep_symbols.txt
	gcc $(LDFLAGS) $(CFLAGS) -o $(BIN) $(SRC)
	rm keep_symbols.txt


clean:
	rm -rf $(BIN)
