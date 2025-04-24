project('three_hundred', 'cpp', default_options : ['cpp_std=c++17'])

subproject('openvr')
openvr_dep = dependency('openvr', static: true)
thread_dep = dependency('threads')

subdir('./src')
