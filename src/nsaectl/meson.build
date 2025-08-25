
debug_server = executable('debug_server', 'debug_server.c', dependencies: nsaeipc_dep)

nsaectl = executable('nsaectl', [
    'nsaectl.c',
  ],
  dependencies : [
    nsaeipc_dep,
  ],
  install : true,
)

# end of file
