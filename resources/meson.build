
xxd = find_program('xxd')
xxd_command = [
  xxd, '-C', '-c', '0x10', '-i', '@INPUT@', '@OUTPUT@',
]

advprom_h = custom_target(
  'advprom.h',
  output  : 'advprom.h',
  input   : 'advprom.com',
  command : xxd_command,
)

icon_embed_h = custom_target(
  'icon_embed.h',
  output  : 'icon_embed.h',
  input   : 'icon_128x128.png',
  command : xxd_command,
)

install_data(
  'icon_16x16.png', 
  rename      : 'nsae.png',
  install_dir : '/usr/share/icons/hicolor/16x16/apps/',
)
install_data(
  'icon_32x32.png', 
  rename      : 'nsae.png',
  install_dir : '/usr/share/icons/hicolor/32x32/apps/',
)
install_data(
  'icon_48x48.png', 
  rename      : 'nsae.png',
  install_dir : '/usr/share/icons/hicolor/48x48/apps/',
)
install_data(
  'icon_64x64.png', 
  rename      : 'nsae.png',
  install_dir : '/usr/share/icons/hicolor/64x64/apps/',
)
install_data(
  'icon_128x128.png', 
  rename      : 'nsae.png',
  install_dir : '/usr/share/icons/hicolor/128x128/apps/',
)

install_data(
  'nsae.desktop',
  install_dir : '/usr/share/applications/',
)

# end of file
