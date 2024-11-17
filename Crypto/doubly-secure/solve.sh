#!/usr/bin/env sh
#
# Run the solver
# Note: this script assumes that the server is running on localhost:8080

ADDRESS=${1:-localhost}
PORT=${2:-8080}

docker build -t solver ./solver

docker run -it \
  --network="host" \
  -e ADDRESS="$ADDRESS"\
  -e PORT="$PORT"\
  solver
