#!/bin/bash

mkdir .bins
pushd .bins

wget https://github.com/premake/premake-core/releases/download/v5.0.0-alpha6/premake-5.0.0-alpha6-src.zip
unzip premake-5.0.0-alpha6-src.zip
if [ "$TRAVIS_OS_NAME" == "linux" ]; then
  pushd premake-5.0.0-alpha6/build/gmake.unix/
  make
  popd
elif [ "$TRAVIS_OS_NAME" == "osx" ]; then
  pushd premake-5.0.0-alpha6/build/gmake.macosx/
  make
  popd
fi
cp premake-5.0.0-alpha6/bin/release/premake5 premake5

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
  wget https://github.com/martine/ninja/releases/download/v1.6.0/ninja-linux.zip
  unzip ninja-linux.zip
elif [ "$TRAVIS_OS_NAME" == "osx" ]; then
  wget https://github.com/martine/ninja/releases/download/v1.6.0/ninja-mac.zip
  unzip ninja-mac.zip
fi

chmod +x premake5
chmod +x ninja
ls -al

popd
