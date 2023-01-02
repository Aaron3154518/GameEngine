#!/bin/bash
find src/ -name "*.h" -o -name "*.cpp" | xargs wc -l
