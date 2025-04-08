include_dirs = include_directories('./include')
sources = files(
    'main.cpp',
    'trackedDeviceProvider.cpp',
    'pipeHandler.cpp',
    'controllerDevice.cpp',
    'inputParser.cpp'
)


shared_library('three_hundred',
    sources,
    include_directories: include_dirs,
    dependencies: [openvr_dep, thread_dep],
    name_prefix: 'driver_'
)
