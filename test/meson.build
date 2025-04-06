include_dirs = include_directories('./include', '../src/include')
sources = files(
    'main.cpp',
    '../src/pipeHandler.cpp',
    '../src/inputParser.cpp'
)


executable('three_hundred_test',
    sources,
    include_directories: include_dirs,
    dependencies: [thread_dep, openvr_dep]
)