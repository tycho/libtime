project('time', 'c')

cc = meson.get_compiler('c')

global_deps = [dependency('threads')]
if target_machine.system() != 'windows'
  global_deps += [cc.find_library('m')]
endif
if target_machine.system() == 'windows'
  global_deps += [cc.find_library('winmm')]
endif

subdir('src')
if not meson.is_subproject()
  subdir('tests')
endif
