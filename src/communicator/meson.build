
zmq_dep = dependency('libzmq', required : true)

communicator_inc = include_directories([
  '.',
])

communicator_lib = library(
  'nsae_communicator', 
  'communicator.c',
  dependencies : [
    zmq_dep,
  ],
  install : true,
)

communicator_dep = declare_dependency(
  link_with : communicator_lib,
  include_directories : communicator_inc,
  dependencies : [
    zmq_dep,
  ],
)

# end of file
