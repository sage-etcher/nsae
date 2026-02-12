
# NorthStar Advantage Emulator

NOTE: this is verry much a WIP

Tested support for AlmaLinux 9, FreeBSD 13, and Windows 11.

![Example of CP/M running in the Emulator](/readme-resources/example-2026-02-12.png)

## Runtime Dependencies

- GLEW
- GLFW3
- GLU
- OpenGL 1.0
- PortAudio 2.0
- ZeroMQ

## Build Dependencies

- meson
- xxd

## Build

    meson setup build
    cd build
    ninja
    meson install

# Licensing

Copyright (c) 2025 Sage I. Hendricks
MIT License
