# Original CMake SFML Project Template

This project was initially scaffolded from [SFML's CMake Project Template](https://github.com/SFML/cmake-sfml-project). Below is the original template documentation.

---

# CMake SFML Project Template

This repository template allows for a fast and hassle-free kick start of SFML projects using CMake. Thanks to [GitHub's nature of templates](https://docs.github.com/en/repositories/creating-and-managing-repositories/creating-a-repository-from-a-template), the template repo can be forked without inheriting its Git history.

## Features

- Basic CMake script to build projects and link SFML on any operating system
- Basic [GitHub Actions](https://github.com/features/actions) script for all major platforms

## How to Use

1. Install [Git](https://git-scm.com/downloads) and [CMake](https://cmake.org/download/). Use your system's package manager if available.
2. Follow [GitHub's instructions](https://docs.github.com/en/repositories/creating-and-managing-repositories/creating-a-repository-from-a-template) for how to use their project template feature to create your own project.
3. Clone your new GitHub repo and open the repo in your text editor of choice.
4. Open [CMakeLists.txt](CMakeLists.txt). Rename the project and the target name of the executable to whatever name you want. Make sure to change all occurrences.
5. If you want to add or remove any .cpp files, change the source files listed in the `add_executable` call in CMakeLists.txt to match the source files your project requires.
6. If your code uses the Audio or Network modules then add `sfml-audio` or `sfml-network` to the `target_link_libraries` call alongside the existing `sfml-graphics` library.
7. If you use Linux, install SFML's dependencies using your system package manager:
   ```bash
   sudo apt update
   sudo apt install \
       libxrandr-dev \
       libxcursor-dev \
       libxi-dev \
       libudev-dev \
       libfreetype-dev \
       libflac-dev \
       libvorbis-dev \
       libgl1-mesa-dev \
       libegl1-mesa-dev
   ```
8. Configure and build your project:
   ```bash
   cmake -B build
   cmake --build build
   ```

## Upgrading SFML

SFML is found via CMake's [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) module. FetchContent automatically downloads SFML from GitHub and builds it alongside your own code. This ensures ABI compatibility and simplifies specifying static versus shared libraries.

Modify the SFML version by changing the `GIT_TAG` argument in [CMakeLists.txt](CMakeLists.txt). Currently uses SFML 2.6.x.

## Configuration Options

### Change Compilers
See the variety of [`CMAKE_<LANG>_COMPILER`](https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER.html) options. Modify `CMAKE_CXX_COMPILER` to point to your desired C++ compiler.

### Change Compiler Optimizations
CMake abstracts compiler flags through the [`CMAKE_BUILD_TYPE`](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html) option:
- `Release` builds enable optimizations (default)
- `Debug` builds enable debug symbols but disable optimizations

For multi-configuration generators on Windows, modify [`CMAKE_CONFIGURATION_TYPES`](https://cmake.org/cmake/help/latest/variable/CMAKE_CONFIGURATION_TYPES.html).

### Change Generators
To modify the generator:
```bash
cmake -G "Ninja" -B build
cmake --build build
```

See [CMake generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) for a complete list.

## Resources

- [Official CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/)
- [How to Use CMake Without the Agonizing Pain - Part 1](https://alexreinking.com/blog/how-to-use-cmake-without-the-agonizing-pain-part-1.html)
- [How to Use CMake Without the Agonizing Pain - Part 2](https://alexreinking.com/blog/how-to-use-cmake-without-the-agonizing-pain-part-2.html)
- [Better CMake YouTube series by Jefferon Amstutz](https://www.youtube.com/playlist?list=PL8i3OhJb4FNV10aIZ8oF0AA46HgA2ed8g)

## License

The template source code is dual licensed under Public Domain and MIT -- choose whichever you prefer.
