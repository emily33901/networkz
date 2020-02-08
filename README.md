# Networkz

Riverbed replacement.

# Building

```sh

> git submodule update --init --recursive

# First - copy tools/build/emscripten-reference.txt -> tools/build/emscripten.txt
# and modify it so that it points at your emscripten install

# only needs to be done once
> build/tools/generate-projects.(sh/bat)

> cd build

# is the only command you need to run repeatedly
# it automatically reconfigures the needed files
> ninja
```