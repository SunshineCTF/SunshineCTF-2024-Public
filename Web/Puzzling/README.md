# Puzzling

## Web (Medium)

A CTFd compatible web challenge for SunshineCTF 2024.

## Deployment

This challenge can be built using docker:

```sh
docker build -t sunshinectf-web-puzzling .
```

The apache server inside the container is running on port 80. It can be deployed with:


```sh
docker run -p 1337:80 sunshinectf-web-puzzling
```

And then accessed at [http://localhost:1337](http://localhost:1337)

This process is automated by running the shell script included in the files:

```sh
./build-docker.sh
```

## Technical Details

The challenge is a PHP-based site using Composer and Codeigniter 4 MVC framework. All application logic can be found in the src/app folder in the Controllers, Models, View, etc folders. It stores data in a local SQLite database file.
