
project('NorthStar Advantage Emulator', 'c',
  version : '0.1.0',
  license : 'MIT',
  default_options : [
    'c_std=c99',
    'warning_level=3',
    #'b_pie=true',
    #'b_lundef=false',
    #'b_sanitize=address,undefined',
  ],
  meson_version : '>= 1.1',
)

add_project_arguments([
    '-D_POSIX_C_SOURCE=200809L',
    '-Wno-gnu-binary-literal',
    '-Wno-unused-parameter',
  ], 
  language : 'c',
)


subdir('resources')
subdir('src')

if get_option('tests')
    subdir('tests')
endif

# end of file
