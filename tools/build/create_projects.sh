echo creating projects...

./tools/build/premake5 "gmake2"
./tools/build/premake5 "export-compile-commands"

echo finished.
