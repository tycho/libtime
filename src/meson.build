sources = ['cpu.c', 'libtime.c', 'sleep.c', 'wall_darwin.c', 'wall_posix.c', 'wall_windows.c']
incdirs = include_directories('../include', '../private', '.')
public_incdirs = include_directories('../include')

lib = static_library(
  'time',
  sources,
  c_args: ['-DLIBTIME_STATIC'],
  dependencies: global_deps,
  include_directories: incdirs
)

libtime = declare_dependency(
  include_directories: public_incdirs,
  link_with: lib)
