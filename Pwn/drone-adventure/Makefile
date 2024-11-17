CFLAGS = -fno-stack-protector -no-pie -Wl,-z,norelro -fvisibility=hidden
LDFLAGS = -Wl,--retain-symbols-file=keep_symbols.txt -s
CHAL   = drone
FUZZ   = $(CHAL)-fuzz.c
SRC    = $(CHAL).c
BIN    = $(CHAL).bin

example:
	/opt/tigress/3.1/tigress --Environment=x86_64:Linux:Gcc:4.6 --Transform=EncodeLiterals --EncodeLiteralsKinds=string --Transform=Flatten --FlattenDispatch=goto --Functions=main,executeCommand,handleRECD,handleMOVE,handleSTOP,handleTURN,handleLIGH,handleCAMO,handleSIGN,handleSENS,handleARMX,handleCLAW,handleSCAN,handleSAMP,handleTRNS,handleBRKS,handleDEPL,handleSAFE,check_filename,log_reason,bug_report --out=$(FUZZ) $(SRC)
	echo "deprecated_feedback" > keep_symbols.txt
	gcc $(LDFLAGS) $(CFLAGS) -o $(BIN) $(FUZZ)
	rm keep_symbols.txt
	rm a.out
	rm drone-fuzz.c

clean:
	rm -rf $(BIN)
	rm -rf $(FUZZ)
	rm -rf keep_symbols.txt
	rm -rf .gdb_history
	rm -rf drone
	rm -rf a.out
	rm -rf core
