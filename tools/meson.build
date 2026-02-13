
# posix requirements
find_program('[',        required : false)
find_program('basename', required : false)
find_program('dd',       required : false)
find_program('dirname',  required : false)
find_program('echo',     required : false)
find_program('sh',       required : false)

# obscure requirements
find_program('mkfs.cpm', required : false)  # cpmtools
find_program('mkfs.ns',  required : false)  # github.com/sage-etcher/north_star_tools


nsae_tools = [
  'mkfd.ns',
  'mkfd.nscpm',
  'mkfd.nsdos',
  'nsaecli',
]

install_data(
  nsae_tools,
  install_dir : get_option('bindir'),
  install_mode : 'rwxr-xr-x',
)

# end of file
