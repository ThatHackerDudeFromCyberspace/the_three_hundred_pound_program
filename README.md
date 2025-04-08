# Three Hundred Pound Program
A SteamVR input fuzzer driver designed for researching how bindings and input translations are handled (you don't want to know)  

<a href='https://ko-fi.com/hackerdude' target='_blank'><img height='35' style='border:0px;height:46px;' src='https://storage.ko-fi.com/cdn/brandasset/v2/support_me_on_kofi_dark.png' border='0' alt='Buy Me a Coffee at ko-fi.com' />

## How to compile
1. Install Meson
2. 
```sh
meson setup builddir
cd builddir
meson compile
```

## How do I use this
1. Compile it
2. Copy the `driver_three_hundred.so` file to the `three_hundred/bin/linux64` folder
3. Add the `three_hundred` folder to the `external_drivers` section in `~/.config/openvr/openvrpaths.vrpath`
4. Run SteamVR
5. Look in `test/test.py` on how to use it once it is running

### Configuration
`default.vrsettings` contains some config info:
* `input_profile_path` is the input profile path passed to SteamVR
* `input_definition_path` must be set to the full path to the `input_definition.json` file which is used to instantiate the controller's inputs.

### Log
Log output is prefixed with `[ThreeHundred]` for convenience
