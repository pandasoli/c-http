FROM alpine:latest

RUN apk add --no-cache gcc libc-dev build-base

COPY . .

RUN make
EXPOSE 8080

ENTRYPOINT ["./bin/main"]
