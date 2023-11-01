FROM alpine:latest

RUN apk update
RUN apk add --no-cache apk-tools coreutils
RUN apk add --no-cache gcc libc-dev build-base

COPY . .

RUN make
EXPOSE 8080

ENTRYPOINT ["./bin/main"]
