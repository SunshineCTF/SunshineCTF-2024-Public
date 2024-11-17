CFLAGS = -fstack-protector  -pie -Wl,-z,relro,-z,now -fvisibility=hidden
LDFLAGS = -Wl,--retain-symbols-file=keep_symbols.txt -s -Wstringop-overflow=0
CHAL   = jungle
SRC    = $(CHAL).c
BIN    = $(CHAL).bin


example:
	echo "main" > keep_symbols.txt
	echo "knapsack" >> keep_symbols.txt
	echo "used" >> keep_symbols.txt
	gcc $(LDFLAGS) $(CFLAGS) -o $(BIN) $(SRC)
	rm keep_symbols.txt


clean:
	rm -rf $(BIN)
