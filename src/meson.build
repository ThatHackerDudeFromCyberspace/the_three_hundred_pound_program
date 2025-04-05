include_dirs = include_directories('./include')
sources = files(
    'main.cpp',
    'trackedDeviceProvider.cpp',
    'hmdDevice.cpp',
    'globalState.cpp',
    'displayComponent.cpp'
)


shared_library('three_hundred',
    sources,
    include_directories: include_dirs,
    dependencies: [openvr_dep, thread_dep],
    name_prefix: '',
    name_suffix: ''
)