project('openvr')

cpp = meson.get_compiler('cpp')

search_directory = meson.current_source_dir() / 'lib' / host_machine.system()
cpu_family = host_machine.cpu_family()

if cpu_family == 'x86_64'
    search_directory += '64'
elif cpu_family == 'x86'
    search_directory += '32'
elif cpu_family == 'aarch64'
    search_directory += 'arm64'
endif


openvr_dep = declare_dependency(
    dependencies: cpp.find_library('openvr_api', dirs: search_directory),
    include_directories: include_directories('headers')
)

meson.override_dependency('openvr', openvr_dep, static: true)