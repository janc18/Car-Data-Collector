import subprocess
import json
import os
from dotenv import load_dotenv

load_dotenv()
#It is nedded a .env file in the root directory, with the following structure
"""
IP=Raspberry Pi IP #user@ip
MDH=mounting directory host #path
MDR=mounting directory rp #path
"""

def create_env_file ():
    with open ("../../.env","w") as env_file:
        env_file.write("IP=<Raspberry_Pi_IP>\nMDH=<mounting_directory_host>\nMDR=<mounting_directory_rp>\n")
    print("¡Env file created in ../../.env !")
IP = os.getenv("IP")
MDH= os.getenv("MDH")
MDR= os.getenv("MDR")

if any(var is None or var == "" for var in [IP, MDH, MDR]):
    print(".env file don't found or some variable isn't declared")
    print("Do you want to create a new file Y/n")
    option=input()
    if option=="y" or "Y":
        create_env_file()
    else:
        exit()


config_data = {

    "Raspberry Pi IP": IP,
	"mounting directory host":MDH,
	"mounting directory rp":MDR
}

with open("config.json", "w") as config_file:
    json.dump(config_data, config_file, indent=4)
with open("config.json","r")as read_json:
    data=json.load(read_json)
    subprocess.call(["sudo","sshfs","-o","allow_other",data["Raspberry Pi IP"]+":"+data["mounting directory rp"],data["mounting directory host"]])
