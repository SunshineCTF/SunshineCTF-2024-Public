FROM golang:1.22.4 as builder

WORKDIR /app

COPY go.mod go.sum ./

RUN go mod download

COPY . .

RUN go build -o /app/app

CMD ["/app/app"]

# FROM alpine:3.13

# COPY --from=builder /app/app /app/app
# COPY --from=builder /app/html /app/html

# CMD ["/app/app"]
