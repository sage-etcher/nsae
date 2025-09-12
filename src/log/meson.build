
log_inc = include_directories('.')
log_lib = library('log', [
    'log.c',
  ],
  install : true,
)

log_dep = declare_dependency(
  link_with : log_lib,
  include_directories : log_inc,
)

# end of file
