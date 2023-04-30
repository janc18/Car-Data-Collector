#!/bin/bash
#Note: The packages used for this script have the Raspberrypi zero as a target
kernel_version="rpi-6.1.y"
number_of_cores_to_compile=$(expr $(nproc)/2)
# Install necessary packages
echo -e "\e[32mInstalling necessary packages\e[0m"
sudo apt-get install gcc-arm-linux-gnueabihf build-essential git

# Creating kernel directory
echo -e "\e[32mCreating kernel directory\e[0m"
mkdir -p arm-kernel

echo -e "\e[32mEnter directory\e[0m"
cd arm-kernel

echo -e "\e[32mDownloading $kernel_version kernel version\e[0m"
git clone --depth=1 --branch=$kernel_version https://github.com/raspberrypi/linux

cd linux
echo -e "\e[32mCompiling kernel modules\e[0m"
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcmrpi_defconfig modules -j$number_of_cores_to_compile

echo -e "\e[32mReady to compile new kernel modules\e[0m"
