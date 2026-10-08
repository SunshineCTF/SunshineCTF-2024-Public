# BotNet (flag 1) Writeup

- Easy: A challenge that shows you how to read ActivityPub Actors.
- Medium: A challenge that forces you to implement authenticated fetch for a custom Actor type.

## Autosolvers

This challenge has a lot of moving parts, so there are auto-solver scripts.

(note: ensure the MIME for the Easy auto-solve is `ld+json`, with the +)

- Easy: `curl --request GET --url https://__DOMAIN_GOES_HERE__/users/rin --header 'Accept: application/ld+json; profile="https://www.w3.org/ns/activitystreams"' --header 'Content-Type: application/ld+json; profile="https://www.w3.org/ns/activitystreams"'`
- Medium: `GET /autosolver` while the `debug` variable in `index.py` is asserted will auto-solve the challenge.
