# Bluetooth-System-Monitor
 A simple system information monitor using and ESP32 + ILI9488 TFT. Receives data over Serial Bluetooth.

 ![alt text](/assets/main_image.jpg "Bluetooth-System-Monitor")

 To send data to the screen you can use the provided python scripts for Windows or MacOS. Instruction on how to install and use this are below.

# Connecting the screen

Use the image below to connect the screen to the ESP32.

 ![alt text](/assets/connecting_ili9488.png "ESP32 TFT connections")

  ![alt text](/assets/ESP_TFT_WIRING.png "ESP32 TFT Wiring")

# Using the ESP32 TouchDown

For this project you can also use the ESP32 TouchDown (which I used in the image above). You can buy it here: https://www.esp32touchdown.com/

# ESP32 side installation

To use this sketch, you will need the Arduino IDE (1.8.10 or higher). You will also need to add the ESP32-Arduino core (https://github.com/espressif/arduino-esp32) to your board manager.

This sketch uses the latest version of TFT_eSPI, available here: https://github.com/Bodmer/TFT_eSPI

Before uploading the sketch, configure the TFT_eSPI library to use the correct pins. You will need to edit the User_Setup.h that comes with the TFT_eSPI library. You can find examples user setups for this project here: [https://github.com/DustinWatts/Bluetooth-System-Monitor/user_setup.h Examples](https://github.com/DustinWatts/Bluetooth-System-Monitor/user_setup.h%20Examples)



Open the sketch in the Arduino IDE. Select the right board, the right port and hit 'Upload'

# Computer side installation

Windows:




MacOs:

On MacOS the python script uses PySerial to communicate with the ESP32 and it uses psutil to get the the system information. Temperature details are retreived using "powermetrics" which does not need installation.

`pip install pyserial`
Docs on PySerial: https://pythonhosted.org/pyserial/

`pip install psutil`
Docs on psutil: https://psutil.readthedocs.io/en/latest/

This script needs administrator right to get system info. So to run:
`sudo python macos_host.py`

# Help

You can join my Discord server where I can help you out. https://discord.gg/RE3XevS