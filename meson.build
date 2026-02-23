
project('NorthStar Advantage Emulator', 'c',
  version : '0.2.0',
  license : 'MIT',
  default_options : [
    'c_std=c99',
    'warning_level=3',
    #'b_pie=true',
    #'b_lundef=false',
    #'b_sanitize=address,undefined',
  ],
  meson_version : '>= 0.55.0',
)

cc = meson.get_compiler('c')

#imports
cmake = import('cmake')

#subprojects
sc_proj = subproject('sc')
sc_time_dep = sc_proj.get_variable('sc_time_dep')
sc_map_dep  = sc_proj.get_variable('sc_map_dep')

#arguments
add_project_arguments([
    '-D_POSIX_C_SOURCE=200809L',
    '-Wno-gnu-binary-literal',
    '-Wno-unused-parameter',
  ], 
  language : 'c',
)

if get_option('debug')
  add_project_arguments(['-D_DEBUG'], language : 'c')
endif

# only allow enable-serial=True when on system that provides the following
# serial support is variable but mostly POSIX only (NO WINDOWS SUPPORT)
if get_option('enable-serial')
  serial_support_systems = [
    'linux',
  ]

  if not serial_support_systems.contains(host_machine.system())
    warning(
      'The host_machine does not have tested support for serial emulation.',
      'If compilation issues occur, you may disable NSAE\'s serial emulation',
      'using the meson setup flag, \'-Denable-serial=false\'.',
    )
  endif

  cc.has_header('fcntl.h',   required : true)
  cc.has_header('termios.h', required : true)
  cc.has_header('unistd.h',  required : true)
endif

#subdirectories
subdir('resources')
subdir('src')

if get_option('enable-tools')
  subdir('tools')
endif

if get_option('tests')
    subdir('tests')
endif

# end of file
