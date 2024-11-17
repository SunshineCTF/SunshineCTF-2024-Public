# \[Web] BotNet

A series of ActivityPub challenges.

- Easy: A challenge that shows you how to read ActivityPub Actors.
- Medium: A challenge that forces you to implement authenticated fetch for a custom Actor type.

## Setup
- Populate the `flag#.txt` files.
- Populate `DOMAIN` in `docker-compose.yml` with the public facing domain of the application.
    - e.g. botnet1.web.2024.sunshinectf.org
- Deploy using `docker compose up`

## Autosolvers

This challenge has a lot of moving parts, so there are auto-solver scripts.

(note: ensure the MIME for the Easy auto-solve is `ld+json`, with the +)

- Easy: `curl --request GET --url https://__DOMAIN_GOES_HERE__/users/rin --header 'Accept: application/ld+json; profile="https://www.w3.org/ns/activitystreams"' --header 'Content-Type: application/ld+json; profile="https://www.w3.org/ns/activitystreams"'`
- Medium: `GET /autosolver` while the `debug` variable in `index.py` is asserted will auto-solve the challenge.

At any time when the challenge is deployed, there is a secret `GET /sunshinectf_health` that will auto-test the challenge and return what `domain` is set. This is the best way to ensure the challenge is solvable, since it works by solving the challenge automatically.
