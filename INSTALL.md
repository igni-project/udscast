# Building and Installation

## Prerequisites

Specialised software is required in order to compile this source code. If one or more prerequisites are not installed, the final executable may not build or lack certain features.

### Autotools

This software uses the GNU Autotools system.

[GNU Autotools](https://en.wikipedia.org/wiki/GNU_Autotools) is readily
available on most linux systems package managers.

- Arch Linux: `sudo pacman -Syu autoconf automake libtool`
- Debian and Ubuntu: `sudo apt-get install -y autotools-dev autoconf`
- Fedora: `sudo dnf install autoconf automake`

## Initialisation

The repository itself, as cloned, is not quite ready for compilation. Autotools must download some additional files to function properly.

Execute the `./bootstrap` script in the repository root to bootstrap Autotools.

## Configuration

Autotools creates a script named `configure` during the bootstrap process.
Run `./configure` to change build settings and generate usable makefiles for compilation.

## Compilation

Build all binaries from source by using the `make` command.

## Installation

The command `sudo make install` shall install the final compiled program(s) onto your system.

