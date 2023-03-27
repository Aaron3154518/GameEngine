#!/bin/sh
echo "Compiling..."
./node_modules/.bin/tsc
echo "Compiled!"
echo "Launching Application"
electron ./dist/app.js
