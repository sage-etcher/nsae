
nsaeipc_inc = include_directories([
  '.',
])

os = host_machine.system() 
if os == 'linux' or os.endswith('bsd') or os == 'dragonfly' or os == 'cygwin'
  nsaeipc_src = files('nsaeipc_unix.c')

elif os == 'windows'
  nsaeipc_src = files('nsaeipc_win.c')

else
  error('unsupported operating system')
endif



nsaeipc_lib = library(
  'nsaeipc', 
  nsaeipc_src,
  install : true,
)

nsaeipc_dep = declare_dependency(
  link_with : nsaeipc_lib,
  include_directories : nsaeipc_inc,
)

# end of file
