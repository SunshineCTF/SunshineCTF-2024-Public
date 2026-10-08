# BotNet (flag 2) Writeup

- Easy: A challenge that shows you where
- Medium: A challenge that forces you to implement authenticated fetch for a custom user type

## Autosolvers

This challenge has a lot of moving parts, so there are auto-solver scripts.

- Easy: `curl --request GET --url https://f16a-132-170-209-199.ngrok-free.app/users/miku --header 'Accept: application/ld+json; profile="https://www.w3.org/ns/activitystreams"' --header 'Content-Type: application/ld+json; profile="https://www.w3.org/ns/activitystreams"'`
- Medium: `GET /autosolver` while the `debug` variable in `index.py` is asserted will auto-solve the challenge.

## Additional notes

The local `solver.py` implements authenticated fetch itself: it stands up a key server that the challenge calls back to (via the docker bridge gateway), sends a signed GET for miku's Actor, and reads `flag_2` from the response.
