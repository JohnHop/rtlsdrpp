# rtlsdrpp

C++ wrapper for the [rtl-sdr](https://github.com/osmocom/rtl-sdr) library developed by [Osmocom](https://osmocom.org/).

## Feature

- RAII support for the original `rtlsdr_dev_t` struct.

- Exception support.

- The library is well scoped inside the `rtlsdrpp` namespace.

- `fc32` sample type support.

- Some utilities included.

## Build instructions

If you want to use this library in a project, use [git submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules). Assuming that *pwd* is the root of you project and you use a `lib/` directory for external libraries

```bash
git submodule add https://github.com/JohnHop/rtlsdrpp.git lib/rtlsdrpp
```

then add `[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)` to your top-level `CMakeLists.txt` to consume the library

```cmake
FetchContent_Declare(rtlsdrpp
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/rtlsdrpp
)
FetchContent_MakeAvailable(rtlsdrpp)
```

Or use directly `FetchContent` (slows the CMake configure stage)

```cmake
FetchContent_Declare(rtlsdrpp
  GIT_SOURCE ###
  GIT_TAG main
)
FetchContent_MakeAvailable(rtlsdrpp)
```

(just remember to add `include(FetchContent)` in your `CMakeLists.txt` in order to use `FetchContent`)

## How to use

Create a rtlsdr device and adjust some turner settings. Then remember to call the `reset_buffer()` method before streaming

```c++
rtlsdrpp::dev_t device;
device.set_center_frequency(97e6);
device.set_sample_rate(2.4e6);
device.set_turner_bandwidth(250e3);
device.set_turner_gain_mode(rtlsdrpp::turner_gain_mode_e::automatic);

device.reset_buffer();
```

The easiest way to read I/Q samples is using an synchronous call

```c++
auto const nsamples = rtlsdrpp::nsamples(100ms);
std::vector<rtlsdrpp::cu8_t> samples_cu8(nsamples);

int read_count;
device.read_sync(samples_cu8, read_count);
```

Or you can read continuously passing a callback as parameter to a asynchronous call

```c++
device.read_async(callback, context);
```

until the method `cancel_async()` on the device object is called.

## Examples

- `rx_samples_to_file.cpp` - inspired by the homonymous [uhd example program](https://github.com/EttusResearch/uhd/blob/master/host/examples/rx_samples_to_file.cpp).
