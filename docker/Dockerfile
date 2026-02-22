FROM gcc:15 AS build

RUN set -ex; \
    apt-get update; \
    apt-get install -y \
    cmake \
    ninja-build \
    zip

COPY ./docker/vcpkg-install.sh /vcpkg-install.sh

RUN chmod +x /vcpkg-install.sh && /vcpkg-install.sh

WORKDIR /app

COPY . .

RUN cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -G Ninja && \
    cmake --build build --parallel $(nproc)

FROM scratch AS runtime

COPY --from=build /app/build/todo_api /todo_api

EXPOSE 18080

CMD ["/todo_api"]
