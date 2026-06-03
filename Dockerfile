# --- этап сборки -------------------------------------------------------------
FROM debian:bookworm-slim AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends g++ cmake make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY CMakeLists.txt ./
COPY third_party ./third_party
COPY src ./src

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j"$(nproc)"

# --- этап запуска ------------------------------------------------------------
FROM debian:bookworm-slim AS runtime

# libstdc++ уже есть в базовом образе; ставить ничего не нужно.
WORKDIR /app
COPY --from=build /app/build/ToyFactoryAssets ./ToyFactoryAssets
COPY web ./web
COPY data ./data

EXPOSE 8080
CMD ["./ToyFactoryAssets"]
