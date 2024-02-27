# main
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential g++ python-dev autotools-dev cmake wget git libicu-dev libbz2-dev libboost-all-dev openssl libssl-dev libeigen3-dev

#gcc
wget https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.xz
tar -zxvf gcc-13.2.0.tar.xz
cd gcc-13.2.0
wget https://gcc.gnu.org/pub/gcc/infrastructure/gmp-6.2.1.tar.bz2
wget https://gcc.gnu.org/pub/gcc/infrastructure/isl-0.24.tar.bz2
wget https://gcc.gnu.org/pub/gcc/infrastructure/mpfr-4.1.0.tar.bz2
wget https://gcc.gnu.org/pub/gcc/infrastructure/mpc-1.2.1.tar.gz
tar -jvxf gmp-6.2.1.tar.bz2
tar -jvxf isl-0.24.tar.bz2
tar -jvxf mpfr-4.1.0.tar.bz2
tar -zvxf mpc-1.2.1.tar.xz
ln -sf gmp-6.2.1 gmp
ln -sf isl-0.24 isl
ln -sf mpfr-4.1.0 mpfr
ln -sf mpc-1.2.1 mpc
mkdir build
cd build
../configure -enable-checking=release -enable-languages=c,c++ -disable-multilib
make
sudo make install
mv /usr/bin/gcc /usr/bin/gcc_bak
mv /usr/bin/g++ /usr/bin/g++_bak
mv /usr/bin/c++ /usr/bin/c++_bak
mv /usr/bin/cc /usr/bin/cc_bak
ln -s /usr/local/bin/gcc /usr/bin/gcc
ln -s /usr/local/bin/g++ /usr/bin/g++
ln -s /usr/local/bin/c++ /usr/bin/c++
ln -s /usr/local/bin/gcc /usr/bin/cc
sudo cp /usr/local/lib64/libstdc++.so.6.0.32 /usr/lib/x86_64-linux-gnu/
sudo rm /usr/lib/x86_64-linux-gnu/libstdc++.so.6
ln -s /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.32 /usr/lib/x86_64-linux-gnu/libstdc++.so.6
cd ..
cd ..

# boost
wget https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.tar.gz
tar -zvxf boost_1_80_0.tar,gz
cd boost_1_80_0
./bootstrap.sh
./b2
sudo ./b2 install
cd ..

# fmt
git clone https://github.com/fmtlib/fmt.git
cd fmt
mkdir build
cd build
cmake ..
make -j3
sudo make install
cd ..
cd ..

# clean
rm -rf gcc-13.2.0
rm -rf boost_1_80_0
rm -rf fmt
