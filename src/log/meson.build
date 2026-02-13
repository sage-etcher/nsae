
log_inc = include_directories('.')
log_lib = library('nsae_log', [
    'log.c',
  ],
  dependencies : [
    darray_dep,
  ],
  install : true,
)

log_dep = declare_dependency(
  link_with : log_lib,
  include_directories : log_inc,
)

install_headers('log.h', subdir : 'nsae')

# end of file
