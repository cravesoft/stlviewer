# STLViewer

A free, cross-platform viewer for STL files built with Qt6 and OpenGL 3.3.

## Screenshots

![Screenshot](https://cravesoft.github.io/stlviewer/images/screenshot1.png)

## Building

### Dependencies

| Dependency | Minimum version |
|------------|----------------|
| CMake      | 3.16            |
| Qt         | 6.0             |
| OpenGL     | 3.3 core        |

On Ubuntu/Debian:

```bash
sudo apt install cmake libgl-dev qt6-base-dev qt6-base-dev-tools
```

On Fedora/RHEL:

```bash
sudo dnf install cmake mesa-libGL-devel \
    qt6-qtbase-devel qt6-qtopengl-devel
```

On macOS (Homebrew):

```bash
brew install cmake qt6
```

On Windows, install [Qt 6](https://www.qt.io/download) via the online installer and [CMake](https://cmake.org/download/). Make sure the Qt `bin/` directory is on your `PATH`.

### Compile

```bash
git clone https://github.com/cravesoft/stlviewer.git
cd stlviewer

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The executable is placed at `build/stlviewer` (Linux/macOS) or `build\Release\stlviewer.exe` (Windows).

#### Optional: install system-wide

```bash
sudo cmake --install build
```

### Packaging

After a successful build, run `cpack` from the build directory. The package format is auto-selected based on the platform.

#### .deb (Ubuntu/Debian)

```bash
cd build
cpack -G DEB
# produces stlviewer-x.x.x-Linux.deb
sudo dpkg -i stlviewer-x.x.x-Linux.deb
```

#### Windows installer (.exe, NSIS)

Build on Windows (or cross-compile), then:

```bash
cd build
cpack -G NSIS
# produces stlviewer-x.x.x-win64.exe
```

You can override the generator on any platform with `-G <generator>`. Run `cpack --help` for the full list.

#### Debug build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

## Usage

```bash
# Open one or more files from the command line
stlviewer model.stl

# Or launch with no arguments and use File → Open
stlviewer
```

### Keyboard shortcuts

| Key | Action |
|-----|--------|
| `Ctrl+O` | Open file |
| `Ctrl+S` | Save |
| `Ctrl+Shift+S` | Save As |
| `Ctrl+I` | Save image |
| `Ctrl+Q` | Quit |
| `R` | Toggle rotate mode |
| `P` | Toggle pan mode |
| `W` | Toggle wireframe |
| `+` | Zoom in |
| `-` | Zoom out |
| `1` | Reset zoom |

## License

MIT — see [LICENSE](LICENSE).