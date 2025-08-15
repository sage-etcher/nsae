
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

# end of file
