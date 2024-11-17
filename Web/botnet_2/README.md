# \[Web] BotNet

A series of ActivityPub challenges.

- Easy: A challenge that shows you where
- Medium: A challenge that forces you to implement authenticated fetch for a custom user type

## Setup
- Populate the `flag#.txt` files.
- Populate the env variables in `src/Dockerfile`
- Deploy using `docker compose up`

## Autosolvers

This challenge has a lot of moving parts, so there are auto-solver scripts.

- Easy: `curl --request GET --url https://f16a-132-170-209-199.ngrok-free.app/users/miku --header 'Accept: application/ld+json; profile="https://www.w3.org/ns/activitystreams"' --header 'Content-Type: application/ld+json; profile="https://www.w3.org/ns/activitystreams"'`
- Medium: `GET /autosolver` while the `debug` variable in `index.py` is asserted will auto-solve the challenge.
