#include "BluetoothSerial.h"
#include <TFT_eSPI.h>
#include <SPIFFS.h>
#include <FS.h>

// Define warning levels, change this as you like
int warn_cpu = 75; // Higher then, in C
int warn_rpm = 7000; // Higher then, in RPM
int warn_ram = 2000; // Lower then, in MB
int warn_hdd = 224; // Higher then, in GB
int warn_gpu = 70; // Higher then, in Celcius
int warn_procs = 800; // Higher then, in #

// Define the filesystem we are using
#define FILESYSTEM SPIFFS

// Creating objects
TFT_eSPI tft = TFT_eSPI();
BluetoothSerial BTSerial;

void setup() {
  // Begin BT Serial
  BTSerial.begin("MyDisplay");

  // Begin regular Serial for debugging
  Serial.begin(115200);

  // Begin our filesystem
  FILESYSTEM.begin();

  // Initialise the TFT stuff
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.setTextColor(TFT_SKYBLUE);
  tft.setTextSize(1);

  // Draw Background
  drawBmp("/bg.bmp", 0, 0);

  // Draw n/a until data is available
  tft.drawCentreString("n/a", 75, 135, 1);
  tft.drawCentreString("n/a", 244, 135, 1);
  tft.drawCentreString("n/a", 403, 115, 1);

  tft.drawCentreString("n/a", 75, 280, 1);
  tft.drawCentreString("n/a", 244, 280, 1);
  tft.drawCentreString("n/a", 403, 280, 1);

}

void loop() {

  // Check for incoming Serial Data
  if (BTSerial.available()) {

    // We expect incoming data like this: "33,428,8343,16000,68,371" (temp, rpm, mem_use, free_disk, gpu, procs)
    String temp = BTSerial.readStringUntil(',');
    String rpm = BTSerial.readStringUntil(',');
    String ram = BTSerial.readStringUntil(',');
    String hdd = BTSerial.readStringUntil(',');
    String gpu = BTSerial.readStringUntil(',');
    String procs = BTSerial.readStringUntil('/');
    
    // Clear values by draw a black rectangle
    tft.fillRect(40,130,440,30,TFT_BLACK);
    tft.fillRect(355,100,440,30,TFT_BLACK);
    tft.fillRect(25,280,440,30,TFT_BLACK);
    
    // Make some nice strings for us to print
    String temp_data = String(temp.toInt()) + " C";
    String rpm_data = String(rpm.toInt()) + " rpm";
    String ram_data = String(ram.toInt()) + " MB";
    
    String hdd_data = String(hdd.toInt()) + " GB";
    String gpu_data = String(gpu.toInt()) + " C";
    String procs_data = String(procs.toInt());
    
    // Print all the values
    if(temp.toInt() > warn_cpu){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(temp_data, 75, 135, 1);     
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(temp_data, 75, 135, 1);    
    }

    if(rpm.toInt() > warn_rpm){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(rpm_data, 244, 135, 1);     
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(rpm_data, 244, 135, 1);    
    }

    if(ram.toInt() < warn_ram){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(ram_data, 403, 115, 1);;     
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(ram_data, 403, 115, 1);    
    }

    if(hdd.toInt() > warn_hdd){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(hdd_data, 75, 280, 1);    
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(hdd_data, 75, 280, 1);    
    }

    if(gpu.toInt() > warn_gpu){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(gpu_data, 244, 280, 1);     
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(gpu_data, 244, 280, 1);    
    }

    if(procs.toInt() > warn_procs){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(procs_data, 403, 280, 1);     
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(procs_data, 403, 280, 1);    
    }
        
  }
}


// Functions use to draw a BMP image not important for you
void drawBmp(const char *filename, int16_t x, int16_t y)
{

  if ((x >= tft.width()) || (y >= tft.height()))
    return;

  fs::File bmpFS;

  bmpFS = FILESYSTEM.open(filename, "r");

  if (!bmpFS)
  {

    Serial.print("File not found:");
    Serial.println(filename);
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row;
  uint8_t r, g, b;

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++)
      {

        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t *bptr = lineBuffer;
        uint16_t *tptr = (uint16_t *)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t *)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
    }
    else
      Serial.println("[WARNING]: BMP format not recognized.");
  }
  bmpFS.close();
}

uint16_t read16(fs::File &f)
{
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f)
{
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
