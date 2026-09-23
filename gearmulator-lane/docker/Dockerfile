# Reproducible headless multisample renders (Vst3OfflineRender + grid script).
# Build from repo root:
#   docker build -f gearmulator-lane/docker/Dockerfile -t instruments-multisample .
#
# ROM and plugins are mounted at run time — never COPY firmware into the image.

FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -qq && apt-get install -y -qq \
    build-essential g++-12 gcc-12 cmake git \
    libstdc++-12-dev \
    libasound2-dev libfreetype6-dev libfontconfig1-dev \
    libgl1-mesa-dev libx11-dev libxrandr-dev libxcursor-dev \
    libxinerama-dev libxext-dev libcurl4-openssl-dev \
    libxcomposite-dev freeglut3-dev pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12 \
    && cmake --build build -j"$(nproc)" --target Vst3OfflineRender

FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -qq && apt-get install -y -qq --no-install-recommends \
    libasound2 libfreetype6 libfontconfig1 libgl1 libx11-6 libxrandr2 \
    libxcursor1 libxinerama1 libxext6 libcurl4 libxcomposite1 freeglut3 \
    libstdc++6 ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /src/build/Vst3OfflineRender /usr/local/bin/Vst3OfflineRender
COPY --from=builder /src/scripts/gearmulator/render_multisample_grid.sh /usr/local/bin/render_multisample_grid.sh
COPY gearmulator-lane/docker/entrypoint.sh /entrypoint.sh

RUN chmod +x /usr/local/bin/render_multisample_grid.sh /entrypoint.sh

ENV VST3_OFFLINE_RENDER_BIN=/usr/local/bin/Vst3OfflineRender
ENV GEARMULATOR_ROM_DIR=/rom

VOLUME ["/plugins", "/rom", "/out"]
WORKDIR /out

ENTRYPOINT ["/entrypoint.sh"]
CMD ["--help"]
