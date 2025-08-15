
glew_dep = dependency('glew', required : true)
glut_dep = dependency('glut', required : true)
glu_dep = dependency('glu', required : true)

nsae_inc = include_directories('.')
z80emu_inc = include_directories('z80emu')

nsae_bin = executable('nsae', [
    'main.c',
    'mmu.c',
    'nsae.c',
    'z80emu/z80emu.c',
    advprom_h,
  ],
  include_directories : [
    nsae_inc,
    z80emu_inc,
  ],
  dependencies : [
    glew_dep,
    glut_dep,
    glu_dep,
  ],
  install : true,
)

# end of file
