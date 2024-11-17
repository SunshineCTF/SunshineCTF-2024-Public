### Notes from kcolley

Deployed using PwnableHarness with the pre-built binary. Author notes below:

# Jungle Adventure

## Deploying

Should be fairly easy to deploy. There is a ~~`Dockerfile`~~ included in the
~~`docker/`~~ directory. It just uses `tjoconnor/bin-hosting` as the base
image and copies over the [jungle.bin](attachments/jungle.bin) and copies
[flag.txt](flag.txt). I copied out the libc version and placed it in
~~`attachments/libc.so.6`~~.

```sh
cd docker/
docker build -t jungle .
docker run -p31337:31337 jungle
```

## Distributing

You can provide competitions the following files from the
[attachments](attachments/) directory:

Files:

- [jungle.bin](attachments/jungle.bin)
- `libc.so.6`
