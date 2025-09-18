
glew_dep = dependency('glew', required : true)
glu_dep  = dependency('glu', required : true)
glfw_dep = dependency('libglfw', required : true)

nsae_inc = include_directories('.')
z80emu_inc = include_directories('z80emu')

nsae_bin = executable('nsae', [
    'adv.c',
    'breakpoints.c',
    'cpu.c',
    'crt.c',
    'fdc.c',
    'io.c',
    'kb.c',
    'main.c',
    'mmu.c',
    'nsae.c',
    'pio.c',
    'ram.c',
    'server.c',
    'sio.c',
    'speaker.c',
    'watchpoints.c',
    'z80emu/z80emu.c',
    advprom_h,
  ],
  include_directories : [
    nsae_inc,
    z80emu_inc,
  ],
  dependencies : [
    glew_dep,
    glfw_dep,
    glu_dep,
    log_dep,
    nsaecmd_dep,
    nsaeipc_dep,
  ],
  install : true,
)

# end of file
