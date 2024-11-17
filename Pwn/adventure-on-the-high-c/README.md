### Notes from kcolley

This challenge was deployed using the pre-built binary but hosted with
PwnableHarness (see [Build.mk](Build.mk)). Below are the original deployment
notes provided to us:


# Adventure on the High C

## Deploying

Should be fairly easy to deploy. There is a [Dockerfile](attachments/Dockerfile)
included. It just uses ``tjoconnor/bin-hosting`` as the base image and copies
over the [ship.bin](attachments/ship.bin) and [flag.txt](flag.txt).

```sh
cd docker/
docker build -t high-c .
docker run -p31337:31337 high-c
```

## Distributing

You can provide competitions the following files from the
[attachments](attachments/) directory:

Files

- [ship.bin](attachments/ship.bin)
- [Dockerfile](attachments/Dockerfile)

## Additional Note

I commented some lines out to the Dockerfile. Please leave them in the case the
competitors wanted to uncomment them and debug inside the container. The
uncommented lines will install gdb, pwndbg, and pwntools which should be enough.
