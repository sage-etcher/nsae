
project(
  'sc',
  'c',
  version : '1.0.4',
  license : 'BSD-3-Clause',
  default_options : [
    'warning_level=0',
    'c_std=c99',
  ],
)

sc_time_inc = include_directories('./time')
sc_time_a = static_library('sc_time', 'time/sc_time.c')
sc_time_dep = declare_dependency(link_with : sc_time_a, include_directories : sc_time_inc)

sc_map_inc = include_directories('./map')
sc_map_a = static_library('sc_map', 'map/sc_map.c')
sc_map_dep = declare_dependency(link_with : sc_map_a, include_directories : sc_map_inc)

# end of file
