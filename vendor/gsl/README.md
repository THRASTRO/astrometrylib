# GSL

From https://www.gnu.org/software/gsl

The GNU Scientific Library (GSL) is a numerical library for C and C++ programmers. It is free software under the GNU General Public License.
The library provides a wide range of mathematical routines such as random number generators, special functions and least-squares fitting. There are over 1000 functions in total with an extensive test suite.
Unlike the licenses of proprietary numerical libraries the license of GSL does not restrict scientific cooperation. It allows you to share your programs freely with others.

## License

GSL is licensed under GPL v3 (see LICENSE.txt). I've included the headers and compiled libraries for MSVC in order to ease compilation on windows.
This was done with best intentions and is hopefully compatible with the dual license of astrometry.net (which also includes GPL v3).

## MSVC libraries for GSL

Extract `gsl-libs-msvc.7z` inside the directory.
You should get a folder structure like this:

- vendor/gsl/x64
- vendor/gsl/x64/Debug
- vendor/gsl/x64/Release
- vendor/gsl/x86
- vendor/gsl/x86/Debug
- vendor/gsl/x86/Release

### How it was built

https://www.gnu.org/software/gsl/extras/native_win_builds.html