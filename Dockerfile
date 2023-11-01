FROM alpine:latest

RUN apk update && \
	apk add --no-cache apk-tools coreutils gcc libc-dev build-base

WORKDIR /app
COPY . .

RUN make
EXPOSE 8080

ENTRYPOINT ["./bin/main"]
