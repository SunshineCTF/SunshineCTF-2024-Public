# \[Web] BotNet

A series of ActivityPub challenges (flag 1 is the easy one, flag 2 the medium one).

## Setup
- Populate the `flag#.txt` files.
- Populate `DOMAIN` in `docker-compose.yml` with the public facing domain of the application.
    - e.g. botnet1.web.2024.sunshinectf.org
- Deploy using `docker compose up`

## Health check

At any time when the challenge is deployed, there is a secret `GET /sunshinectf_health` that will auto-test the challenge and return what `domain` is set. This is the best way to ensure the challenge is solvable, since it works by solving the challenge automatically.

See [writeup.md](writeup.md) for the solution.
