#include "BluetoothSerial.h"
#include <TFT_eSPI.h>
#include <SPIFFS.h>
#include <FS.h>

// Choose one of these depending on your setup!
// ESP32 TouchDown uses CAP_TOUCH
// DevKitC with ILI9488 uses RES_TOUCH
// Use NO_TOUCH when your screen does not have touch

//-------------- SELECT --------------------
//#define ENABLE_CAP_TOUCH
#define ENABLE_RES_TOUCH
//#define NO_TOUCH 
//------------------------------------------

#ifdef ENABLE_CAP_TOUCH
#include <FT6236.h>
FT6236 ts = FT6236();
#endif

// Change this to match the script on the host
int redrawtime = 4000;

// The scale of the Y-axis per graph
int ymax_cpu = 110; // In Celcius
int ymax_fan = 2000; // In rpm
int ymax_ram = 32000;  // In MB
int ymax_hdd = 250;  // In GB
int ymax_gpu = 110;  // In Celcius
int ymax_procs = 500;  // In # (numbers)

// Define warning levels, change this as you like
int warn_cpu = 75; // Higher then, in C
int warn_rpm = 7000; // Higher then, in RPM
int warn_ram = 2000; // Lower then, in MB
int warn_hdd = 35; // Lower then, in GB
int warn_gpu = 70; // Higher then, in Celcius
int warn_procs = 800; // Higher then, in #

// Place to store the last 21 readings
int32_t cpu[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int32_t fan[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int32_t ram[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int32_t hdd[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int32_t gpu[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int32_t procs[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// Other variables
int margin = 40;
int screenwidth = 480;
int screenheight = 320;

boolean graphshowing = false;
boolean homescreen = true;
int currentgraph = 0;
int previousmillis;


// Define the filesystem we are using
#define FILESYSTEM SPIFFS

// Define the name of our calibration file
#define CALIBRATION_FILE "/calibrationData"

// Creating objects
TFT_eSPI tft = TFT_eSPI();
BluetoothSerial BTSerial;

void setup() {
  // Begin BT Serial
  BTSerial.begin("MyDisplay"); // you can change this name, but on MacOs make sure to macth your port to this name in the script

  // Begin regular Serial for debugging
  Serial.begin(115200);
  Serial.println("Normal Serial begun.");

  // Begin our filesystem
  if(FILESYSTEM.begin()){
    Serial.println("SPIFFS begun.");
  }else{
    Serial.println("SPIFFS failed!");
  }

  #ifdef ENABLE_CAP_TOUCH
  //Begin the touchscreen
  if(ts.begin(40)){
    Serial.println("Touchscreen begun.");
  }else{
    Serial.println("Touchscreen failed!");
  }
  #endif


  // Initialise the TFT stuff
  tft.begin();
  tft.setRotation(1);
    
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.setTextColor(TFT_SKYBLUE);
  tft.setTextSize(1);

  #ifdef ENABLE_RES_TOUCH
  touch_calibrate();
  #endif


  // Draw Background
  drawBmp("/bg.bmp", 0, 0);
  Serial.println("Background drawn.");

  // Draw n/a until data is available
  tft.drawCentreString("n/a", 75, 135, 1);
  tft.drawCentreString("n/a", 244, 135, 1);
  tft.drawCentreString("n/a", 403, 115, 1);

  tft.drawCentreString("n/a", 75, 280, 1);
  tft.drawCentreString("n/a", 244, 280, 1);
  tft.drawCentreString("n/a", 403, 280, 1);
  
  Serial.println("Setup done.");

}

void loop() {

  // Some variables we need
  int t_y, t_x;
  uint16_t x, y;
  bool pressed = false;

  // Make sure we not use last loop's touch
  pressed = false;

  // Check for incoming Serial Data
  if (BTSerial.available()) {

    // We expect incoming data like this: "33,428,8343,16000,68,371" (temp, rpm, mem_use, free_disk, gpu, procs)
    String cpu_string = BTSerial.readStringUntil(',');
    String fan_string = BTSerial.readStringUntil(',');
    String ram_string = BTSerial.readStringUntil(',');
    String hdd_string = BTSerial.readStringUntil(',');
    String gpu_string = BTSerial.readStringUntil(',');
    String procs_string = BTSerial.readStringUntil('/');

    // Add values we got to our arrays
    addToArray(cpu_string.toInt(), cpu);
    addToArray(fan_string.toInt(), fan);
    addToArray(ram_string.toInt(), ram);
    addToArray(hdd_string.toInt(), hdd);
    addToArray(gpu_string.toInt(), gpu);
    addToArray(procs_string.toInt(), procs);       
        
  }

#ifdef ENABLE_CAP_TOUCH

  if (ts.touched())
  {
  
    // Retrieve a point
    TS_Point p = ts.getPoint();
  
    //Flip things around so it matches our screen rotation
    p.x = map(p.x, 0, 320, 320, 0);
    t_y = p.x;
    t_x = p.y;
  
    pressed = true;
    
  }

#endif

#ifdef ENABLE_RES_TOUCH

  if (tft.getTouch(&x, &y)) {

    t_x = x;
    t_y = y;
    pressed = true;

  }



#endif


  // Process Touches
  if(pressed){

  if(graphshowing){

    drawBmp("/bg.bmp", 0, 0);
    updateHomeScreen();
    graphshowing = false;
    
  }else{

      if(t_y < screenheight/2){
    
      Serial.print("Upper ");
    
        if(t_x < screenwidth/3){
    
          Serial.println("Left");
          drawGraph(0, sizeof(cpu) / sizeof(cpu[0]), 0, ymax_cpu, cpu, "CPU Temp in Celcius", false);
          graphshowing = true;
          currentgraph = 1;
          
        }else if(t_x < (screenwidth/3)*2){
    
          Serial.println("Middle");
          drawGraph(0, sizeof(fan) / sizeof(fan[0]), 0, ymax_fan, fan, "Fan Speeds in rpm", false);
          graphshowing = true;
          currentgraph = 2;
          
        }else{
    
          Serial.println("Right");
          drawGraph(0, sizeof(ram) / sizeof(ram[0]), 0, ymax_ram, ram, "Free Memory in MB", false);
          graphshowing = true;
          currentgraph = 3;
          
        }
        
      }
    
      else
      {
    
        Serial.print("Lower ");
    
        if(t_x < screenwidth/3){
    
          Serial.println("Left");
          drawGraph(0, sizeof(hdd) / sizeof(hdd[0]), 0, ymax_hdd, hdd, "Free space in GB", false);
          graphshowing = true;
          currentgraph = 4;
          
        }else if(t_x < (screenwidth/3)*2){
    
          Serial.println("Middle");
           drawGraph(0, sizeof(gpu) / sizeof(gpu[0]), 0, ymax_gpu, gpu, "GPU Temperature in Celcius", false);
           graphshowing = true;
          currentgraph = 5;
          
        }else{
    
          Serial.println("Right");

          drawGraph(0, sizeof(procs) / sizeof(procs[0]), 0, ymax_procs, procs, "Number of active processes", false);
          graphshowing = true;
          currentgraph = 6;
          
        }
    
      }
    
      pressed = false;
    }
}

//Redraw graphs if needed

if(graphshowing && previousmillis+redrawtime <= millis()){

  previousmillis = millis();

  if(currentgraph == 1){

    drawGraph(0, sizeof(cpu) / sizeof(cpu[0]), 0, ymax_cpu, cpu, "CPU Temp in Celcius", true);
    
  }else if(currentgraph == 2){

    drawGraph(0, sizeof(fan) / sizeof(fan[0]), 0, ymax_fan, fan, "Fan Speeds in rpm", true);
    
  }else if(currentgraph == 3){

    drawGraph(0, sizeof(ram) / sizeof(ram[0]), 0, ymax_ram, ram, "Free Memory in MB", true);
   
  }else if(currentgraph == 4){

      drawGraph(0, sizeof(hdd) / sizeof(hdd[0]), 0, ymax_hdd, hdd, "Free space in GB", true);
      
    
  }else if(currentgraph == 5){

    drawGraph(0, sizeof(gpu) / sizeof(gpu[0]), 0, ymax_gpu, gpu, "GPU Temperature in Celcius", true);
    
    
  }else if(currentgraph == 6){

    drawGraph(0, sizeof(procs) / sizeof(procs[0]), 0, ymax_procs, procs, "Number of active processes", true);
      
  }
  
}


else if (previousmillis+redrawtime <= millis()){

    previousmillis = millis();
    updateHomeScreen();

}

}

// Function to shift all values in an array to the left and add a value
void addToArray(int32_t addvalue, int32_t *arr){

  for(int i=0; i<20; i++){

    arr[i] = arr[i+1];
    
  }

  arr[20] = addvalue;
  
}

// Draw the homescreen

void updateHomeScreen(){

  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.setTextColor(TFT_SKYBLUE);
  tft.setTextSize(1);
  
    // Make some nice strings for us to print
    String temp_data = String(cpu[20]) + " C";
    String rpm_data = String(fan[20]) + " rpm";
    String ram_data = String(ram[20]) + " MB";
    
    String hdd_data = String(hdd[20]) + " GB";
    String gpu_data = String(gpu[20]) + " C";
    String procs_data = String(procs[20]);
  
      // Clear values by draw a black rectangle
    tft.fillRect(40,130,440,30,TFT_BLACK);
    tft.fillRect(350,100,440,30,TFT_BLACK);
    tft.fillRect(25,280,440,30,TFT_BLACK);



    if(cpu[20] > warn_cpu){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(temp_data, 75, 135, 1);     
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(temp_data, 75, 135, 1);    
    }

    if(fan[20] > warn_rpm){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(rpm_data, 244, 135, 1);     
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(rpm_data, 244, 135, 1);    
    }

    if(ram[20] < warn_ram){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(ram_data, 403, 115, 1);;     
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(ram_data, 403, 115, 1);    
    }

    if(hdd[20] < warn_hdd){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(hdd_data, 75, 280, 1);    
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(hdd_data, 75, 280, 1);    
    }

    if(gpu[20] > warn_gpu){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(gpu_data, 244, 280, 1);     
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(gpu_data, 244, 280, 1);    
    }

    if(procs[20] > warn_procs){
      tft.setTextColor(TFT_RED);
      tft.drawCentreString(procs_data, 403, 280, 1);     
    }
    else
    {
      tft.setTextColor(TFT_SKYBLUE);
      tft.drawCentreString(procs_data, 403, 280, 1);    
    }
  
}

// Function to draw a graph
void drawGraph(int32_t xmin, int32_t xmax, int32_t ymin, int32_t ymax, int32_t *arr, String title, bool redraw){

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextFont(1); 

  int32_t arrlen = xmax;

  if(redraw){

    tft.fillRect(0+margin+1, 0+margin-1, screenwidth+5-(2*margin),screenheight-(2*margin)-1, TFT_BLACK);
    tft.drawLine(0+margin, 0+margin, 0+margin,screenheight-margin, TFT_SKYBLUE);
    tft.drawLine(0+margin, screenheight-margin, screenwidth-margin, screenheight-margin, TFT_SKYBLUE);
    
  }

  if(!redraw){ // if reDraw, do not draw the x/y axis again

  // If this is the first draw, black the screen first
  tft.fillScreen(TFT_BLACK);
    
  // Draw graph title
  tft.setTextDatum(ML_DATUM);
  tft.drawString(title, margin-10, margin-20, 1);

  // Draw empty graph
  tft.drawLine(0+margin, 0+margin, 0+margin,screenheight-margin, TFT_SKYBLUE);
  tft.drawLine(0+margin, screenheight-margin, screenwidth-margin, screenheight-margin, TFT_SKYBLUE);

  tft.setTextDatum(MR_DATUM);
  tft.drawString("0", margin-5, (screenheight-margin)+10, 1);
  }

  // Draw the Y-axis
  int32_t yrange = ymax - ymin;
  int32_t ysteps = (screenheight-2*margin)/5;

  if(!redraw){ // if reDraw, do not draw the x/y axis again
    
    for(int i = 1; i < 5; i++){
      tft.setTextDatum(MR_DATUM);
      tft.drawNumber(0+((yrange/5)*i), margin-5, (screenheight-margin)-ysteps*i, 1); 
    }
  

    tft.drawNumber(ymax, margin-5, (screenheight-margin)-ysteps*5, 1);
  }

  // Draw the X-axis
  int32_t xrange = xmax - xmin;
  int32_t xsteps = (screenwidth-2*margin)/(arrlen-1);

  if(!redraw){ // if reDraw, do not draw the x/y axis again
    for(int i = 1; i < arrlen-1; i++){
      tft.setTextDatum(MC_DATUM);
      tft.drawNumber(i, (0+margin)+xsteps*i, (screenheight-margin)+10, 1); 
    }
  
    tft.drawNumber(xmax-1, (0+margin)+xsteps*(arrlen-1), (screenheight-margin)+10,  1);
  
    }

  for(int i = 0; i < arrlen; i++){

    Serial.printf("%i, ", arr[i]);
    
  }

  Serial.println();
  
  for(int i = 0; i < arrlen-1; i++){

    int32_t point = map(arr[i], ymax, ymin, 0+margin, screenheight-margin);
    int32_t nextpoint = map(arr[i+1], ymax, ymin, 0+margin, screenheight-margin);
    
    //tft.fillCircle((0+margin)+xsteps*i, point, 1, TFT_WHITE);

    if(nextpoint < screenheight-(margin) && nextpoint > -1 && point < screenheight-(margin)){

      Serial.printf("%i, %i \n", point, nextpoint);

      tft.drawLine((2+margin)+(xsteps*i), point, (2+margin)+xsteps*(i+1), nextpoint, TFT_WHITE);
      
    }
  }

  tft.setTextColor(TFT_SKYBLUE);
  tft.setTextSize(3);
  tft.setFreeFont(&FreeSansBold12pt7b);
  
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

#ifdef ENABLE_RES_TOUCH
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check if calibration file exists and size is correct
  if (FILESYSTEM.exists(CALIBRATION_FILE))
  {
      fs::File f = FILESYSTEM.open(CALIBRATION_FILE, "r");
      if (f)
      {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
  }

  if (calDataOK)
  {
    // calibration data valid
    tft.setTouch(calData);
    Serial.println("Touch calibration loaded.");
  }
  else
  {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");
    Serial.println("Touch calibration loaded.");

    // store data
    fs::File f = FILESYSTEM.open(CALIBRATION_FILE, "w");
    if (f)
    {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}
#endif
