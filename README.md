# TGit
TGit is a GUI `git` client with the primary design goal of being fast and responsive.

## Dependencies
* `libgit2 (>= 0.27)`
* `Qt5 (>= 5.12)`

## Building
### CMake
Supported build configurations (`CMAKE_BUILD_TYPE`/`--config` argument in case of multi-config generators):
* `Release`
* `Debug`

Supported cache variables:
* `Qt5_DIR`: This variable should point to `<Qt5 Installation>/<Qt5 Kit>/lib/cmake/Qt5`
* `libgit2_ROOT`: This variable should point to the installtion of `libgit2`

### Linux
```bash
mkdir _builds/default
cd _builds/default
cmake -DCMAKE_BUILD_TYPE=<...> -DQt5_Dir=<...> -Dlibgit2_ROOT=<...> ../..
make
```

### Windows
```batch
mkdir _builds
cd _builds
cmake -G"Visual Studio 15 2017" "-DQt5_Dir=<...>" "-Dlibgit2_ROOT=<...>" ..
cmake --build . --config <...>
rem To create an installer also run
cmake --build . --config <...> --target package
```
