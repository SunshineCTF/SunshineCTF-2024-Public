# Secure Flag Terminal

A secure flag terminal to store your most secretive flags.

# Deployment

`pwnmake docker-start publish`

## Running locally

- Requirements: GLIBC_2.27 and `ld-linux-x86-64.so.2`
- Run the command: `patchelf secure_flag_terminal --set-interpreter ld-linux-x86-64.so.2 --set-rpath ./`
    - This will set the GLIBC and interpreter to the right version.
