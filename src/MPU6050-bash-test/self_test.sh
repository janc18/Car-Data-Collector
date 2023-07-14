#!/bin/bash
i2cPackage="i2c-tools"
BGreen='\033[1;32m'
BRed='\033[1;31m'
NC='\033[0m' # No Color

# To run this script it needed the "i2c-tools" package
# It can be downloaded with the following command:
# sudo apt install i2c-tools
# or
# download the package automatically

if [ $(dpkg-query -W -f='${Status}' $i2cPackage 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  	read -p "i2c-tools don't found, You want to install? (Y/N): " confirm && [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]] || exit 1
	sudo apt install $i2cPackage;
fi


# To check if the mpu6050 is connected, it can be done with the next steps
# 1. Read the 0x75(WHO_AM_I) register with 0x68 chip address
# 2. Compare to 0x68 because this is the chip address


echo "Searching MPU6050 at 0x68 address"
value_at_0x75=`i2cget -y 1 0x68 0x75`

if [ $value_at_0x75=0x68 ]
then
    echo -e "${BGreen}MPU6050 found $NC"
else
    echo -e "${BRed}MPU6050 not found $NC"
    return 1
fi
#----------------------------------------------

PWR_MGMT_1=0x6B
GYRO_CONFIG=0x1B
ACCEL_CONFIG=0x1C

CLKSEL=0x01
FS_SEL=0x00
AFS_SEL=0x00
SLEEP=0x00

ACCEL_XOUT_H=59
GYRO_XOUT_H=67

echo -e "${BGreen}Writing $((CLKSEL<<2 | SLEEP<<6)) to 0x6B(PWR_MGMT) register $NC"

i2cset -y -r 1 0x68 $PWR_MGMT_1 $((CLKSEL<<2 | SLEEP<<6)) b

i2cset -y -r 1 0x68 $GYRO_CONFIG $((FS_SEL<<3)) b

i2cset -y -r 1 0x68 $ACCEL_CONFIG $((AFS_SEL<<3)) b

#------------Reading ACCELEROMETER--------------------------
for ((j=0;j<10;j++));
do
    array=[]
    indice=1
for ((i=ACCEL_XOUT_H;i<=64;i++));
do
    array[$indice]=$(i2cget -y 1 0x68 $i)
    ((indice++))
done

printf "ACCEL_X: %d ACCEL_Y: %d ACCEL_Z: %d\r" \
  $((${array[1]}<<8 | ${array[2]})) \
  $((${array[3]}<<8 | ${array[4]})) \
  $((${array[5]}<<8 | ${array[6]}))
sleep 0.5
done
printf "\n"
#---------------------------------------------------


#------------Reading Gyroscope----------------------
for ((j=0;j<10;j++));
do  
    array=[]
    indice=1
for ((i=GYRO_XOUT_H;i<=72;i++));
do  
    array[$indice]=$(i2cget -y 1 0x68 $i)
    ((indice++))
done

printf "GYRO_X: %d GYRO_Y: %d GYRO_Z: %d\r" \
  $((${array[1]}<<8 | ${array[2]})) \
  $((${array[3]}<<8 | ${array[4]})) \
  $((${array[5]}<<8 | ${array[6]}))  
sleep 0.5
done
