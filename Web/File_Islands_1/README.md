# \[Web] File Islands

An easy aventure-themed LFI.

- Easy: A challenge that abuses poor programming practices to read the environment variable of flag stored at `/proc/self/environ`

## Setup
- Populate the `FLAG` environment variable to your choosing or keep the one I made
- Populate `DOMAIN` in `docker-compose.yml` with the public facing domain of the application.
    - e.g. islands.2024.sunshinectf.games
- Deploy using `docker compose build --no-cache` then `docker compose up`

## Autosolvers
- TODO: need to program one + a simple health checker to follow consistency from previous web challenges.
