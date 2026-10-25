#!/usr/bin/env bash
set -euo pipefail
repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
compiler="${CXX:-i686-w64-mingw32-clang++}"
if [[ -n "${LLVM_MINGW:-}" ]]; then
    compiler="$LLVM_MINGW/bin/i686-w64-mingw32-clang++"
fi
output="${1:-$repo_dir/build/version.dll}"
mkdir -p -- "$(dirname -- "$output")"
"$compiler" -shared -static -std=c++17 -fms-extensions -fasm-blocks \
    -Wno-microsoft -Wno-deprecated-declarations -Wno-deprecated -O1 -g \
    -I"$repo_dir/src" -include "$repo_dir/src/BuildCompat.h" \
    "$repo_dir"/src/*.cpp "$repo_dir"/src/tiny-json.c \
    "$repo_dir"/src/minhook/*.cpp "$repo_dir"/src/minhook/hde/*.cpp \
    "$repo_dir/src/version.def" -lpsapi -lopengl32 -o "$output"
printf 'Built %s\n' "$output"
