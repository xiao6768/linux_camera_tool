#!/bin/bash
echo "waitting for the camera."
if [[ "$1" = "install" ]] ; then
	sudo chmod 777 ./install/install.sh
	sudo ./install/install.sh
elif [[ $1 = "uninstall" ]] ; then
	sudo chmod 777 ./install/uninstall.sh
	sudo ./install/uninstall.sh
elif [[ $1 = "update_firmware" ]] ; then
	sudo chmod 777 ./bin/download_fx3 # change mode so you can execute following
	sleep 1
	# flash the RAM on FX3, this is needed to recogonize cypress device after erase firmware
	sudo LD_LIBRARY_PATH=./bin ./bin/download_fx3 -t RAM -i ./bin/cyfxflashprog.img 
        if [ $? -ne 0 ]
        then
                echo "update the firmware failed, please check if camera inserted";
		exit -1;
		
        fi
	sleep 1
	# flash the FLASH on FX3 board
	echo $2
	sudo LD_LIBRARY_PATH=./bin ./bin/download_fx3 -t SPI -i $2
        if [ $? -ne 0 ]
        then
                echo "update the firmware failed, please check if camera inserted";
                exit -1;

        fi
	sleep 1
	echo "rebooting system..."
	LD_LIBRARY_PATH=./bin ./bin/download_fx3 reboot
	sleep 1
	
	echo "update sucessfully"


elif [[ $1 = "--help" ]] ; then
	echo "./firmware.sh install					 		  -----------first time, run installation"
	echo "./firmware.sh update_firmware given_img_file.img -----------update update firmware file"
	echo "./firmware.sh --help                            --------------------------help message"
	echo "./firmware.sh uninstall						  ----------no longer need them, unintall"
else
	echo "./firmware.sh install					 		  -----------first time, run installation"
	echo "./firmware.sh update_firmware given_img_file.img -----------update update firmware file"
	echo "./firmware.sh uninstall						  ----------no longer need them, unintall"
fi

