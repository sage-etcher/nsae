
stb_dep = dependency('stb', required: true)

stb_image_lib = static_library(
  'stb_image', 
  'stb_image.c', 
  dependencies: [
    stb_dep,
  ],
)

stb_image_dep = declare_dependency(
  link_with: stb_image_lib,
  dependencies: stb_dep,
)

# end of file
