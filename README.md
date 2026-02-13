
# NorthStar Advantage Emulator

NOTE: this is verry much a WIP

Tested support for AlmaLinux 9, FreeBSD 13, and Windows 11.

## Examples

![Example of CP/M running in the Emulator](/readme-resources/example-2026-02-12.png)

Scripting NSAE to load and run CP/M on Windows: <https://www.youtube.com/watch?v=-CBy6uQ1ej8>

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

## Usage

Provided is a non-exclusive list of common commands and use cases, for a full 
list of abilities please view the tooling's help menus

    nsae -h         # emulator
    nsaectl -h      # emulator controller, sends commands to the emulator

### Floppy Disks

There are 2 interfaces for loading floppy disks, you can either load them when
starting `nsae` or at runtime using `nsaectl`.

    nsae -A <diskfile> -B <diskfile>
    nsaectl load <drive number> <diskfile>

also of note, every disk operation is atomic, nothing is written to disk until
you specify:

    nsaectl save <drive nubmer> <diskfile>

### Memory

Memory support is essential for easily loading files in/out of some operating 
systems, like NorthStar DOS. We have a few interfaces for managing memory.

note: each of the following use MMU virtual addresses

    nsaectl info mmu                        # list which pages are loaded
    nsaectl set mmu slot<n> <page>          # load a page into an mmu slot

    nsaectl read <addr> <n>                 # read _n_ bytes from _addr_
    nsaectl write <addr> <byte0> [byten...] # write _n_ bytes of data to _addr_
    nsaectl load mmu <addr> <file>          # load a file into memory at _addr_
    nsaectl save mmu <addr> <file> <n>      # save _n_ bytes into _file_

### Speaker

The speakers may be quite loud on some systems, there is basic volume control 
built into the emulator

    nsaectl set speaker volume <percentage>%

### Display

The display colors are controllable at runtime, you can change the background, 
foreground, and the emulated inverted flag

    nsaectl set display foreground '#hexcolor'
    nsaectl set display background '#hexcolor'
    nsaectl set display inverted <true/false>

### Debugging Features

The emulator provides many debugging features, a few noteable ones are as 
follows

    nsaectl pause               # pause the emulator
    nsaectl run                 # resume
    nsaectl step                # execute 1 instruction, then pause
    nsaectl next                # step over next instruction, then pause

    nsaectl reset               # call the PROM coldboot procedure

    nsaectl info <category>     # print info about a category: cpu, mmu, etc.

    nsaectl break <pc>                  # set a breakpoint at _pc_
    nsaectl info breakpoints            # list all set breakpoints by index
    nsaectl delete breakpoint <index>   # delete a breakpoint by index

    nsaectl press <asciilist> string    # type a list of keys from ascii
    nsaectl press <keycode>             # type a key from keycode

# Licensing

Copyright (c) 2025 Sage I. Hendricks  
MIT License
