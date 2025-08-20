
glew_dep = dependency('glew', required : true)
glut_dep = dependency('glut', required : true)
glu_dep = dependency('glu', required : true)

nsae_bin = executable('nsae-foo', [
    'main.c',
    'ltexture.c',
    'lutil.c',
  ],
  dependencies : [
    glew_dep,
    glut_dep,
    glu_dep,
  ],
  install : true,
)

# end of file
