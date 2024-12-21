Simple E131 client for ESP32 based device
Compile and flash to device, solder pigtail to ESP32 device.
Red to VIN pin, White to GND pin, Green to D13

To configure, connect Micro USB to ESP32 and plug in to PC - connect serial terminal 115200 baud
First time through, will try to connect to non existent wifi, you'll either see a stream of * or . every half second.
Pres the space bar to enter the configuration mode.  Follow the prompts to configure the device.
Configuration will allow you to set up Start Universe, Universe count, Count of LEDs, static IP address of the device, Gateway address, Subnet and DNS1 and DNS2 addresses.
For the IP addresses, enter only 1 number at a time as each entry in the address will be prompted.

Configuration will be saved in onboard flash on subsequent reboots.
If you wish to reconfigure, you can plug in to serial again and press space bar during the first 5 seconds
