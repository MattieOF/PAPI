#!/bin/sh
echo "Generating projects for linux..."
cd ..
./Scripts/Bin/premake5 gmake2 --os=linux
read -rsp $'Press any key to continue...\n' -n 1 key
