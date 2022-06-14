rm -rf build
mkdir -p build
cd build
if [ "$1" = "xilinx" ]; then
    echo " --------- build target xilinx --------- "
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_BUILD_TARGET=xilinx .. -DMY_VARIABLE=True    
else
    echo " --------- build target x86 --------- "
    cmake -DCMAKE_BUILD_TYPE=Debug .. -DMY_VARIABLE=True
fi
make -j8
make