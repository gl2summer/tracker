#!/bin/bash


rm -rf tracker.app

cp -rf ../output/build-tracker-Desktop_Qt_5_12_7_clang_64bit-Release/tracker.app ./
mkdir -p tracker.app/Contents/MacOS/map
cp -rf ../map/ ./tracker.app/Contents/MacOS/map/

macdeployqt tracker.app/
