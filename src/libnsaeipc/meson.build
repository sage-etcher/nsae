
nsaeipc_inc = include_directories([
  '.',
])

nsaeipc_lib = library('nsaeipc', [
    'nsaeipc.c',
  ],
  install : true,
)

nsaeipc_dep = declare_dependency(
  link_with : nsaeipc_lib,
  include_directories : nsaeipc_inc,
)

# end of file
