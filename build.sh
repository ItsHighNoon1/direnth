#!/bin/bash

GCC="/bin/gcc"
OUTPUT="direnth"

# compile
source_files=()
while IFS= read -r line; do
    source_files+=("${line#src/}")
done < <(find "src" -type f -name "*.c")

object_files=()
for source in "${source_files[@]}"; do
    object="obj/${source%.*}.o"
    object_files+=("$object")
    if [[ "$object" -ot "src/$source" ]]; then
        echo need to build $source
        dir="${object%/*}"
        mkdir -p $dir
        # glfw, stb, cglm all live in my /usr/include
        $GCC -g -c -o "$object" "src/$source" -I./include -I./src
    fi
done

# link
$GCC -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm -o "$OUTPUT" $(IFS=$'\n'; echo "${object_files[*]}") 