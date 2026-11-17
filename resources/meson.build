
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

install_data(
  'nsaetesticon16.png', 
  rename      : 'nsae.png',
  install_dir : '/usr/share/icons/hicolor/16x16/apps/',
)
install_data(
  'nsaetesticon32.png', 
  rename      : 'nsae.png',
  install_dir : '/usr/share/icons/hicolor/32x32/apps/',
)
install_data(
  'nsaetesticon48.png', 
  rename      : 'nsae.png',
  install_dir : '/usr/share/icons/hicolor/48x48/apps/',
)
install_data(
  'nsaetesticon64.png', 
  rename      : 'nsae.png',
  install_dir : '/usr/share/icons/hicolor/64x64/apps/',
)
install_data(
  'nsaetesticon128.png', 
  rename      : 'nsae.png',
  install_dir : '/usr/share/icons/hicolor/128x128/apps/',
)

install_data(
  'nsae.desktop',
  install_dir : '/usr/share/applications/',
)

# end of file
