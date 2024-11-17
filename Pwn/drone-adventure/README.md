### Notes from kcolley

Deployed using PwnableHarness (see [Build.mk](Build.mk)) using the pre-built
binary. Author notes below:

# Drone Adventure

## Deploying

Should be fairly easy to deploy. There is a ~~`Dockerfile`~~ included in the
~~`docker/`~~ directory. It just uses `tjoconnor/bin-hosting` as the base image
and copies over the [drone.bin](attachments/drone.bin) and echoes the
[flag.txt](flag.txt). Feel free to change to a different container if its easier
on you. Should make no impact on the challenge. Does not require libc.

```sh
cd docker/
docker build -t drone .
docker run -p31337:31337 drone
```

## Distributing

You can provide competitions the following files from the [attachments](attachments/) directory

Files

- [drone.bin](attachments/drone.bin)
