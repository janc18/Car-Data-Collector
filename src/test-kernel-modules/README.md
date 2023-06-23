# Test module kernel

Here is where all the principal components will be tested,
for that, a kernel module will be created for each feature

For example source code to establish connection between the SBC and Mpu6050 (i2c) to get data and 
another one for gps communication (UART)

To use your preffer editor you can use sshfs and here is a little [Python script](./setup_env_sshfs.py).

It's just needed a .env file with the nexts contents:
```env
IP=Raspberry Pi IP 
MDH=mounting directory host 
MDR=mounting directory rp 
```
