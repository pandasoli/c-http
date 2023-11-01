FROM alpine:latest

RUN apk add --no-cache gcc libc-dev make

COPY . .

RUN make
EXPOSE 8080

ENTRYPOINT ["./main"]
