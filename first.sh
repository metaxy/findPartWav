git clone https://github.com/zsiciarz/aquila.git
cd aquila
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=g++-4.7 ..
make
sudo make install
cd ../..
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=g++-4.7 ..
make


