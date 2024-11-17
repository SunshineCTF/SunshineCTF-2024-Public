#! /bin/sh

docker build -t sunshinectf-web-puzzling .
docker run -p 1337:80 --rm sunshinectf-web-puzzling