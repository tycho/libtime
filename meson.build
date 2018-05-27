project('time', 'c')

sources = ['src/cpu.c', 'src/libtime.c', 'src/sleep.c', 'src/wall_darwin.c', 'src/wall_posix.c', 'src/wall_windows.c']
incdirs = include_directories('include', 'private', 'src')
public_incdirs = include_directories('include')

lib = static_library(
  'time',
  sources,
  c_args: ['-DLIBTIME_STATIC'],
  include_directories: incdirs
)

time_dep = declare_dependency(
  include_directories: public_incdirs,
  link_with: lib)
