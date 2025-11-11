cfg-debug:
    cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

cfg-release:
    cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

build:
    ninja -C build

build-debug:
    just cfg-debug
    just build

build-release:
    just cfg-release
    just build

build-all:
    just build-debug
    just build-release

clean:
    #!/usr/bin/env bash
    rm -rf build

clean-win:
    #!/usr/bin/env pwsh
    if (Test-Path build) { Remove-Item -Recurse -Force build }