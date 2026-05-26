# cool-s Docker image
# Multi-stage: build in full gcc image, ship in minimal image
#
# Build:   docker build -t cool-s .
# Run:     docker run --rm -it cool-s
# Flags:   docker run --rm -it cool-s cool-s -o -r -s 4

FROM alpine:3.19 AS builder

RUN apk add --no-cache gcc musl-dev make

WORKDIR /build
COPY src/cool-s.c src/
COPY Makefile VERSION ./

RUN make

# ── Runtime image ──────────────────────────────────────────────────────────────
FROM alpine:3.19

LABEL org.opencontainers.image.title="cool-s"
LABEL org.opencontainers.image.description="Draw the legendary Cool S in your terminal"
LABEL org.opencontainers.image.source="https://github.com/fourzerosix/cool-s"
LABEL org.opencontainers.image.licenses="GPL-3.0-only"

COPY --from=builder /build/cool-s /usr/local/bin/cool-s

# Default: run the animation. Override CMD for flags, e.g.:
#   docker run --rm -it cool-s cool-s -o
ENTRYPOINT ["cool-s"]
CMD []
