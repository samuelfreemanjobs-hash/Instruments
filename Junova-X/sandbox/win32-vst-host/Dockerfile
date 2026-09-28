# Optional reproducible Win32 VST sandbox (build on agent machine; not required for Junova CI).
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
      wine wine32:i386 winetricks xvfb firejail curl ca-certificates && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /sandbox
COPY setup.sh run_sandbox.sh run_vsthost.sh ./
RUN chmod +x setup.sh run_sandbox.sh run_vsthost.sh && ./setup.sh

# Mount plugins/ and host/ at runtime; write renders/ to a volume.
VOLUME ["/sandbox/plugins", "/sandbox/host", "/sandbox/renders"]

CMD ["./run_sandbox.sh", "./run_vsthost.sh"]
