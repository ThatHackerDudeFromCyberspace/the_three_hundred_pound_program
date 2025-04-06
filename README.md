# Three Hundred Pound Program
A SteamVR input fuzzer driver designed for researching how bindings and input translations are handled (you don't want to know)  

<a href='https://ko-fi.com/hackerdude' target='_blank'><img height='35' style='border:0px;height:46px;' src='https://storage.ko-fi.com/cdn/brandasset/v2/support_me_on_kofi_dark.png' border='0' alt='Buy Me a Coffee at ko-fi.com' />

## How to compile
1. Install Meson
2. ```sh
meson setup builddir
cd builddir
meson compile
```

## How do I use this
1. Compile it
2. Copy the `driver_three_hundred_fuzzer.so` file to the `three_hundred_fuzzer` folder
3. Copy the `three_hundred_fuzzer` folder to ` ~/.steam/steam/steamapps/common/SteamVR/drivers/`
4. Run SteamVR
5. Look in `test/test.py` on how to use it once it is running