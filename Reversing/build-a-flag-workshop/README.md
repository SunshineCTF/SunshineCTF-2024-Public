# Build-A-Flag-Workshop
x64 ELF reversing challenge intended to require static and/or dynamic analysis to find the correct flag

## Building
### Requirements
* openssl - `sudo apt install openssl libssl-dev`

### Build Procedures
`gcc main.c -lssl -lcrypto -g0 -O2 -s -o attachments/build-a-flag-workshop`
