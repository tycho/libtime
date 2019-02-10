project('time', 'c')

cc = meson.get_compiler('c')

threads = dependency('threads')
libm = cc.find_library('m')

subdir('src')
subdir('tests')
