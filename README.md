# Bluetooth-System-Monitor
 ![alt text](/assets/main_image.jpg "Bluetooth-System-Monitor")

 A handy little system information monitor using and ESP32 + ILI9488 TFT. Receives data over Serial Bluetooth and thus giving you the option to have it far away from your actual computer. On the computer side you can write your own program for writing over Serial BT, but I have provided python scripts for both Windows and MacOS. Instructions on how to install and use these are below.

## Connecting the screen

Use the image below to connect the screen to the ESP32.

 ![alt text](/assets/connecting_ili9488.png "ESP32 TFT connections")

  ![alt text](/assets/ESP_TFT_WIRING.png "ESP32 TFT Wiring")

## Using the ESP32 TouchDown

For this project you can also use the ESP32 TouchDown (which I used in the image above). You can buy it here: https://www.esp32touchdown.com/

## ESP32 side installation

To use this sketch, you will need the Arduino IDE (1.8.10 or higher). You will also need to add the ESP32-Arduino core (https://github.com/espressif/arduino-esp32) to your board manager.

This sketch uses the latest version of TFT_eSPI, available here: https://github.com/Bodmer/TFT_eSPI

Before uploading the sketch, configure the TFT_eSPI library to use the correct pins. You will need to edit the User_Setup.h that comes with the TFT_eSPI library. You can find examples user setups for this project here: [https://github.com/DustinWatts/Bluetooth-System-Monitor/tree/main/user_setup.h Examples](https://github.com/DustinWatts/Bluetooth-System-Monitor/tree/main/user_setup.h%20Examples)

In the sketch, you can set some warning levels. This will turn the text read if above a certain value. In case of RAM, it will alert when it is under the warning value.

 ![alt text](/assets/warning_levels.png "Warning leves")

Open the sketch in the Arduino IDE. Select the right board, the right port and hit 'Upload'. After this, you will also need to upload the `data` folder. Because the background image also needs to be uploaded. You can use the "ESP Sketch Data Upload" tool that can be found here: https://github.com/me-no-dev/arduino-esp32fs-plugin
![ESP Sketch Data](http://dustinwatts.nl/freetouchdeck/images/ftd_esp_sketch_data.png)

## Computer side installation on Windows:

On Windows the python script uses PySerial to communicate with the ESP32 and it uses psutil to get the the system information. CPU, GPU and fan speed are retreived using `OpenHardwareMonitorLib.dll`. [More information about OpenHardwareMonitorLib.dll](https://openhardwaremonitor.org/)

Make sure `OpenHardwareMonitorLib.dll` is in the same folder as the Python script!

`pip install pyserial`
Docs on PySerial: https://pythonhosted.org/pyserial/

`pip install psutil`
Docs on psutil: https://psutil.readthedocs.io/en/latest/

`pip install pythonnet`
Docs on psutil: http://pythonnet.github.io/

Look in the script for the place to change the COM port to match the COM port of the ESP32:

 ![alt text](/assets/windows_comport.png "COM Port on Windows")
 
 _**Running the script**_
 
 **This script needs administrator rights to get system info. And should be run from the command line as an Aministrator**
 To run: **`python windows_host.py`** or **`python3 windows_host.py`**
 
 Follow this tutorial if if you get an error because Python is not in your PATH variables:
 [Add Python to PATH variables](https://www.makeuseof.com/python-windows-path/).
 
 
## Computer side installation on MacOs:

On MacOS the python script uses `PySerial` to communicate with the ESP32 and it uses `psutil` to get the the system information. Temperature details are retreived using `powermetrics` and `smc` which does not need installation.

`pip install pyserial`
Docs on PySerial: https://pythonhosted.org/pyserial/

`pip install psutil`
Docs on psutil: https://psutil.readthedocs.io/en/latest/

_**Running the script**_

**This script needs administrator rights to get system info. So to run:
`sudo python macos_host.py`** or **`sudo python3 macos_host.py`**

On MacOS connection to a COM port is done using it's name. If you have not changed the name, this python script already had the correct COM port in it, so there is no need to change this.

 ![alt text](/assets/macos_comport.png "COM Port on MacOS")


## Support Me

If you like what I am doing, there are a number of ways you can support me. 

| Platform | Link|
|:-----:|:-----|
| [<img src="https://github.com/DustinWatts/small_logos/blob/main/twitter_logo.png" alt="Twtter" width="24"/>](https://twitter.com/dustinwattsnl "Follow me on Twitter") | You can follow me on Twitter: [@dustinwattsnl](https://twitter.com/dustinwattsnl "Follow me on Twitter")|
| [<img src="https://github.com/DustinWatts/small_logos/blob/main/youtube_logo.png" alt="YouTube" width="32"/>](https://www.youtube.com/dustinwatts "Subscrive to my YouTube channel") | You can subscribe to my channel on Youtube: [/dustinWatts](https://www.youtube.com/dustinwatts "Subscribe to my YouTube channel") |
| [<img src="https://github.com/DustinWatts/small_logos/blob/main/patreon_logo.png" alt="Patreon" width="32"/>](https://www.patreon.com/dustinwatts) | You can support me by becoming a patron on Patreon: https://www.patreon.com/dustinwatts |
| [<img src="https://github.com/DustinWatts/small_logos/blob/main/paypalme_logo.png" alt="PayPal.me" width="32"/>](https://www.paypal.me/dustinwattsnl) | You can make a one time donation using PayPal.me: https://www.paypal.me/dustinwattsnl |

## Get help

For quick access to help you can join my Discord server: https://discord.gg/RE3XevS
