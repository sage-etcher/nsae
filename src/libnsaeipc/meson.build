
zmq_dep = dependency('libzmq', required : true)

nsaeipc_inc = include_directories([
  '.',
])

nsaeipc_lib = library(
  'nsaeipc', 
  'nsaeipc.c',
  dependencies : [
    zmq_dep,
  ],
  install : true,
)

nsaeipc_dep = declare_dependency(
  link_with : nsaeipc_lib,
  include_directories : nsaeipc_inc,
  dependencies : [
    zmq_dep,
  ],
)

# end of file
