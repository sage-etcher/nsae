
# nsaectl_version
nsaectl_conf = configuration_data({
  'PROJ_NAME'    : 'nsaectl',
  'PROJ_VERSION' : meson.project_version(),
})
nsaectl_version = configure_file(
  output : 'nsaectl_version',
  input  : 'nsaectl_version.in',
  configuration : nsaectl_conf,
)
nsaectl_version_h = custom_target(
  'nsaectl_version.h',
  output  : 'nsaectl_version.h',
  input   : nsaectl_version,
  command : xxd_command,
)

# nsaectl_help
nsaectl_help_h = custom_target(
  'nsaectl_help.h',
  output  : 'nsaectl_help.h',
  input   : 'nsaectl_help',
  command : xxd_command,
)

nsaectl = executable('nsaectl', [
    'nsaectl.c',
    nsaectl_help_h,
    nsaectl_version_h,
  ],
  dependencies : [
    log_dep,
    nsaecmd_dep,
    nsaeipc_dep,
    stb_ds_dep,
  ],
  install : true,
)

debug_server = executable('debug_server', [
    'debug_server.c',
  ],
  dependencies: [
      nsaecmd_dep,
      nsaeipc_dep,
  ],
)

# end of file
