#!/bin/bash
NDKPath=$(< "$PWD/NDKPath.txt")
echo "NDK located at: $NDKPath"

buildScript="$NDKPath/ndk-build"

echo "[BUILD] Starting NDK..."
"$buildScript" NDK_PROJECT_PATH="$PWD" APP_BUILD_SCRIPT="$PWD/Android.mk" NDK_APPLICATION_MK="$PWD/Application.mk" NDK_DEBUG=0 -j12
EXIT_CODE=$?
echo "[BUILD] Done!"

echo "[BUILD] Transfering lib..."
directory="$(cd "$(dirname "$0")" && pwd)"
folderName="$(basename "$directory")"
rm -rf /sdcard/CompiledLibs/$folderName
cp -r "$directory/libs" /sdcard/CompiledLibs/$folderName
echo "[BUILD] Lib located at: /sdcard/CompiledLibs/$folderName"

exit $EXIT_CODE