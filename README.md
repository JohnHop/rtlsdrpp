# rtlsdrpp

C++ wrapper for the [rtlsdr](https://github.com/osmocom/rtl-sdr) library developed by [Osmocom](https://osmocom.org/).

## Feature

- RAII support for the original `rtlsdr_dev_t` struct.

- Exception support.

- The library is well scoped inside the `rtlsdrpp` namespace.

- `fc32` sample type support.

- Some utilities included.

## Build instructions

If you want to use this library in a project, use [git submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules). Assuming *pwd* is the root of you project and you use a `lib/` directory for external libraries

```bash
git submodule add ### lib/rtlsdrpp
```

then add `[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)` to your top-level `CMakeLists.txt` to consume the library

```cmake
FetchContent_Declare(rtlsdrpp
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/rtlsdrpp
)
FetchContent_MakeAvailable(rtlsdrpp)
```

Or use directly `FetchContent` (slower)

```cmake
FetchContent_Declare(rtlsdrpp
  GIT_SOURCE ###
  GIT_TAG main
)
FetchContent_MakeAvailable(rtlsdrpp)
```

(just remember to add `include(FetchContent)` in your `CMakeLists.txt` in order to use `FetchContent`)

## How to use



## Examples

## License