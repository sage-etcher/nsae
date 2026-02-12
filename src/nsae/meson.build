
# version and help files
nsae_conf = configuration_data({
  'PROJ_NAME'    : 'nsae',
  'PROJ_VERSION' : meson.project_version(),
})

nsae_version = configure_file(
  output : 'nsae_version',
  input  : 'nsae_version.in',
  configuration : nsae_conf,
)

nsae_version_h = custom_target(
  'nsae_version.h',
  output  : 'nsae_version.h',
  input   : nsae_version,
  command : xxd_command,
)

nsae_help_h = custom_target(
  'nsae_help.h',
  output  : 'nsae_help.h',
  input   : 'nsae_help',
  command : xxd_command,
)

# build the binaries
cc = meson.get_compiler('c')

glew_dep = dependency('glew', required : true)
glfw_dep = dependency('glfw3', required : true)
glu_dep  = dependency('glu', required : true)
portaudio_dep = dependency('portaudio-2.0', required : true)
m_dep = cc.find_library('m', required : false)

nsae_inc = include_directories('.')
z80emu_inc = include_directories('z80emu')

nsae_bin = executable('nsae', [
    'adv.c',
    'audio.c',
    'breakpoints.c',
    'cpu.c',
    'crt.c',
    'fdc.c',
    'io.c',
    'kb.c',
    'kb_decode.c',
    'main.c',
    'mmu.c',
    'nsae.c',
    'pio.c',
    'ram.c',
    'server.c',
    'sio.c',
    'speaker.c',
    #'timer.c',
    'watchpoints.c',
    'z80emu/z80emu.c',
    advprom_h,
    nsae_help_h,
    nsae_version_h,
  ],
  include_directories : [
    nsae_inc,
    z80emu_inc,
  ],
  dependencies : [
    darray_dep,
    glew_dep,
    glfw_dep,
    glu_dep,
    log_dep,
    portaudio_dep,
    m_dep,
    nsaecmd_dep,
    nsaeipc_dep,
    sc_map_dep,
    sc_time_dep,
  ],
  install : true,
)

# end of file
