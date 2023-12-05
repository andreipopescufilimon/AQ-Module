/*
* Air Quality Monitoring System
* 
* Sensor data is read with high precision
* 
* The program can be configured to print data on the serial monitor or LCD screen, or both.
* 
* Author: Popescu Filimon Andrei Cosmin
* Last update: March 6, 2023
*/

// Include necessary libraries
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MQUnifiedsensor.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <DHT.h>



// Define constants and variables
#define lcdprint 0     //lcd print = 0 - printing data on lcd / lcdprint = 1 - not printing data on serial
#define serialprint 0  //serialprint = 0 - printing data on serial /serialprint = 1 - not printing data on serial

// Pins
#define Board ("Arduino Mega 2560")
#define PinMQ2 (A0)            // MQ2
#define PinMQ3 (A1)            // MQ3
#define PinMQ4 (A2)            // MQ4
#define PinMQ5 (A3)            // MQ5
#define PinMQ6 (A4)            // MQ6
#define PinMQ7 (A5)            // MQ7
#define PinMQ8 (A6)            // MQ8
#define PinMQ9 (A7)            // MQ9
#define PinMQ135 (A8)          // MQ135
#define DHTPIN 3               // Temperature & Humidity
#define DHTTYPE DHT11          // Temperature & Humidity sensor type
#define ADC_Bit_Resolution 10  // 10 bit ADC
#define Voltage_Resolution 5   // volt resolution to calculate the voltage

// Data
#define RatioMQ2CleanAir (9.83)   // RS / R0 = 9.83 ppm
#define RatioMQ3CleanAir (60)     // RS / R0 = 60 ppm
#define RatioMQ4CleanAir (4.4)    // RS / R0 = 4.4 ppm
#define RatioMQ5CleanAir (6.5)    // RS / R0 = 6.5 ppm
#define RatioMQ6CleanAir (10)     // RS / R0 = 10 ppm
#define RatioMQ7CleanAir (27.5)   // RS / R0 = 27.5 ppm
#define RatioMQ8CleanAir (70)     // RS / R0 = 70 ppm
#define RatioMQ9CleanAir (9.6)    // RS / R0 = 9.6 ppm
#define RatioMQ135CleanAir (3.6)  // RS / R0   = 10 ppm
#define Type ("Arduino Mega 2560")

// SD Card
/*Sd2Card card;
SdVolume volume;
SdFile root;
const int chipSelect = 53;*/
File myFile;

// LCD
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

// GPS
static const int RXPin = 15, TXPin = 14;
static const uint32_t GPSBaud = 4800;
TinyGPSPlus gps;

// Sensor Declare
MQUnifiedsensor MQ2(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ2, Type);
MQUnifiedsensor MQ3(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ3, Type);
MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ4, Type);
MQUnifiedsensor MQ5(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ5, Type);
MQUnifiedsensor MQ6(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ6, Type);
MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ7, Type);
MQUnifiedsensor MQ8(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ8, Type);
MQUnifiedsensor MQ9(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ9, Type);
MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ135, Type);
DHT dht(DHTPIN, DHTTYPE);
int read_no = 1;

void setup() {
  // put your setup code here, to run once:
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // GPS init
  Serial3.begin(9600);

  // Initialize the LCD
  if (lcdprint == 0) {
    lcd.begin();
  }

  if (serialprint == 0) {
    Serial.println("Air Quality ~ airqualityanalyzer.com");
    Serial.println("Initializating...");
  }
  // Turn on the blacklight and print a message.
  if (lcdprint == 0) {
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Air Quality");
    lcd.setCursor(2, 1);
    lcd.print("Initializating");
    delay(2000);
    lcd.setCursor(16, 1);
    lcd.print(".");
    delay(200);
    lcd.setCursor(17, 1);
    lcd.print(".");
    delay(200);
    lcd.setCursor(18, 1);
    lcd.print(".");
    delay(2000);
    lcd.clear();
  }
  // Boring thing
  if (serialprint == 0) {
    Serial.print("\nInitializing SD card...");
  }
  if (lcdprint == 0) {
    lcd.setCursor(1, 0);
    lcd.print("Initializing");
    lcd.setCursor(1, 1);
    lcd.print("SD card");
    delay(100);
    lcd.setCursor(8, 1);
    lcd.print(".");
    delay(100);
    lcd.setCursor(9, 1);
    lcd.print(".");
    delay(100);
    lcd.setCursor(10, 1);
    lcd.print(".");
  }
  // We'll use the initialization code from the utility libraries
  // Since we're just testing if the card is working!

  /*if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    if (lcdprint == 0) {
      lcd.setCursor(2, 3);
      lcd.print("failed.");
      delay(1000);
    }
    if (serialprint == 0) {
      Serial.println("initialization failed. Things to check:");
      Serial.println("* is a card inserted?");
      Serial.println("* is your wiring correct?");
      Serial.println("* did you change the chipSelect pin to match your shield or module?");
    }
    //  while (1)
    // ;
  } else {
    if (serialprint == 0) {
      Serial.println("Wiring is correct and a card is present.");
    }
    if (lcdprint == 0) {
      lcd.setCursor(2, 3);
      lcd.print("initializated.");
      delay(1000);
    }
  }

  if (serialprint == 0) {
    // Print the type of card
    Serial.println();
    Serial.print("Card type:         ");
  }
  if (lcdprint == 0) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Card type:");
    lcd.setCursor(12, 0);
  }
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      if (serialprint == 0) {
        Serial.println("SD1");
      }
      if (lcdprint == 0) {
        lcd.print("SD1");
      }
      break;
    case SD_CARD_TYPE_SD2:
      if (serialprint == 0) {
        Serial.println("SD2");
      }
      if (lcdprint == 0) {
        lcd.print("SD2");
      }
      break;
    case SD_CARD_TYPE_SDHC:
      if (serialprint == 0) {
        Serial.println("SDHC");
      }
      if (lcdprint == 0) {
        lcd.print("SDHC");
      }
      break;
    default:
      if (serialprint == 0) {
        Serial.println("Unknown");
      }
      if (lcdprint == 0) {
        lcd.print("Unknown");
      }
  }
  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    if (serialprint == 0) {
      Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    }
    if (lcdprint == 0) {
      lcd.setCursor(0, 1);
      lcd.print("FAT16/FAT32");
      lcd.setCursor(0, 2);
      lcd.print("partition not found");
      lcd.setCursor(0, 3);
      lcd.print("Format the card");
    }
    // while (1);
  }
  if (lcdprint == 0) {
    delay(2500);
    lcd.clear();
  }
  if (serialprint == 0) {
    Serial.print("Clusters:          ");
    Serial.println(volume.clusterCount());
  }
  if (lcdprint == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Clusters:");
    lcd.print(volume.clusterCount());
  }
  if (serialprint == 0) {
    Serial.print("Blocks x Cluster:  ");
    Serial.println(volume.blocksPerCluster());
  }
  if (lcdprint == 0) {
    lcd.setCursor(0, 1);
    lcd.print("Blocks x Cluster:");
    lcd.print(volume.blocksPerCluster());
  }
  if (serialprint == 0) {
    Serial.print("Total Blocks:      ");
    Serial.println(volume.blocksPerCluster() * volume.clusterCount());
    Serial.println();
  }
  if (lcdprint == 0) {
    lcd.setCursor(0, 2);
    lcd.print("Total Blocks:");
    lcd.print(volume.blocksPerCluster() * volume.clusterCount());
    delay(2000);
    lcd.clear();
  }

  // Print the type and size of the first FAT-type volume
  uint32_t volumesize;
  if (serialprint == 0) {
    Serial.print("Volume type is:    FAT");
    Serial.println(volume.fatType(), DEC);
  }
  if (lcdprint == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Volume type is: FAT");
    lcd.setCursor(0, 1);
    lcd.print(volume.fatType());
    delay(2000);
    lcd.clear();
  }

  volumesize = volume.blocksPerCluster();  // Clusters are collections of blocks
  volumesize *= volume.clusterCount();     // We'll have a lot of clusters
  volumesize /= 2;                         // SD card blocks are always 512 bytes (2 blocks are 1KB)
  if (serialprint == 0) {
    Serial.print("Volume size (Kb):  ");
    Serial.println(volumesize);
  }
  if (lcdprint == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Volume size (Kb):");
    lcd.print(volumesize);
  }
  if (serialprint == 0) {
    Serial.print("Volume size (Mb):  ");
    volumesize /= 1024;
    Serial.println(volumesize);
  }
  if (lcdprint == 0) {
    lcd.setCursor(0, 1);
    lcd.print("Volume size (Mb):");
    lcd.print(volumesize);
  }
  if (serialprint == 0) {
    Serial.print("Volume size (Gb):  ");
    Serial.println((float)volumesize / 1024.0);
  }
  if (lcdprint == 0) {
    lcd.setCursor(0, 2);
    lcd.print("Volume size (Gb):");
    lcd.print((float)volumesize / 1024.0);
    delay(2000);
    lcd.clear();
  }
  if (serialprint == 0) {
    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  }
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  */
  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1)
      ;
  }
  Serial.println("initialization done.");

  /*// open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("aqdata.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to aqdata.txt...");
    myFile.print("https://airqualityanalyzer.com \n");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening aqdata.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("aqdata.txt");
  if (myFile) {
    Serial.println("aqdata.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening aqdata.txt");
  }*/
  // SD END

  // init the sensor
  dht.begin();

  MQ2.init();
  MQ2.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ2.setA(574.25);
  MQ2.setB(-2.222);  // Configurate the ecuation values to get LPG concentration
  MQ2.setR0(9.659574468);

  MQ3.init();
  MQ3.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ3.setA(0.3934);
  MQ3.setB(-1.504);  // Configurate the ecuation values to get Alcohol concentration
  MQ3.setR0(3.86018237);

  MQ4.init();
  MQ4.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ4.setA(1012.7);
  MQ4.setB(-2.786);  // Configurate the ecuation values to get CH4 concentration
  MQ4.setR0(3.86018237);

  MQ5.init();
  MQ5.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ5.setA(97124);
  MQ5.setB(-4.918);  // Configurate the ecuation values to get Alcohol concentration
  MQ5.setR0(71.100304);

  MQ6.init();
  MQ6.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ6.setA(2127.2);
  MQ6.setB(-2.526);  // Configurate the ecuation values to get CH4 concentration
  MQ6.setR0(13.4285714);

  MQ7.init();
  MQ7.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ7.setA(99.042);
  MQ7.setB(-1.518);  // Configurate the ecuation values to get CO concentration
  MQ7.setR0(4);

  MQ8.init();
  MQ8.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ8.setA(976.97);
  MQ8.setB(-0.688);  // Configurate the ecuation values to get H2 concentration
  MQ8.setR0(1);

  MQ9.init();
  MQ9.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ9.setA(1000.5);
  MQ9.setB(-2.186);  // Configurate the ecuation values to get LPG concentration
  MQ9.setR0(9.42857143);

  MQ135.init();
  MQ135.setRegressionMethod(1);  //_PPM =  a*ratio^b

  /*****************************  MQ CAlibration ********************************************/
  if (serialprint == 0) {
    Serial.print("Calibrating please wait...");
  }
  if (lcdprint == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Calibrating");
    lcd.setCursor(0, 1);
    lcd.print(" please wait...");
  }
  float MQ2calcR0 = 0;
  float MQ3calcR0 = 0;
  float MQ4calcR0 = 0;
  float MQ5calcR0 = 0;
  float MQ6calcR0 = 0;
  float MQ7calcR0 = 0;
  float MQ8calcR0 = 0;
  float MQ9calcR0 = 0;
  float MQ135calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    // Update the voltage lectures
    MQ2.update();
    MQ3.update();
    MQ4.update();
    MQ5.update();
    MQ6.update();
    MQ7.update();
    MQ8.update();
    MQ9.update();
    MQ135.update();

    MQ2calcR0 += MQ2.calibrate(RatioMQ2CleanAir);
    MQ3calcR0 += MQ3.calibrate(RatioMQ3CleanAir);
    MQ4calcR0 += MQ4.calibrate(RatioMQ4CleanAir);
    MQ5calcR0 += MQ5.calibrate(RatioMQ5CleanAir);
    MQ6calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    MQ7calcR0 += MQ7.calibrate(RatioMQ7CleanAir);
    MQ8calcR0 += MQ8.calibrate(RatioMQ8CleanAir);
    MQ9calcR0 += MQ9.calibrate(RatioMQ9CleanAir);
    MQ135calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    if (serialprint == 0) {
      Serial.print("Calculating...");
    }
  }
  MQ2.setR0(MQ2calcR0 / 20);
  MQ3.setR0(MQ3calcR0 / 20);
  MQ4.setR0(MQ4calcR0 / 20);
  MQ5.setR0(MQ5calcR0 / 20);
  MQ6.setR0(MQ6calcR0 / 20);
  MQ7.setR0(MQ7calcR0 / 20);
  MQ8.setR0(MQ8calcR0 / 20);
  MQ9.setR0(MQ9calcR0 / 20);
  MQ135.setR0(MQ135calcR0 / 20);
  if (serialprint == 0) {
    Serial.println("  Done!");
  }
  if (lcdprint == 0) {
    lcd.setCursor(0, 3);
    lcd.print(" Done!");
  }
  if (serialprint == 0) {
    Serial.print("Values:");
    Serial.print(MQ2calcR0 / 10);
    Serial.print(" | ");
    Serial.print(MQ3calcR0 / 10);
    Serial.print(" | ");
    Serial.print(MQ4calcR0 / 10);
    Serial.print(" | ");
    Serial.print(MQ5calcR0 / 10);
    Serial.print(" | ");
    Serial.print(MQ6calcR0 / 10);
    Serial.print(" | ");
    Serial.print(MQ7calcR0 / 10);
    Serial.print(" | ");
    Serial.print(MQ8calcR0 / 10);
    Serial.print(" | ");
    Serial.print(MQ9calcR0 / 10);
    Serial.print(" | ");
    Serial.print(MQ135calcR0 / 10);
    Serial.println(" |");
  }

  delay(1000);
  if (lcdprint == 0) {
    lcd.clear();
  }
  if (isinf(MQ2calcR0) || isinf(MQ3calcR0) || isinf(MQ4calcR0) || isinf(MQ5calcR0) || isinf(MQ6calcR0) || isinf(MQ7calcR0) || isinf(MQ8calcR0) || isinf(MQ9calcR0) || isinf(MQ135calcR0)) {
    Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply");
    if (lcdprint == 0) {
      lcd.setCursor(0, 0);
      lcd.print("W: Connection issue");
      lcd.setCursor(1, 1);
      lcd.print("R0 is inf");
    }
    while (1)
      ;
  }
  if (MQ2calcR0 == 0 || MQ3calcR0 == 0 || MQ4calcR0 == 0 || MQ5calcR0 == 0 || MQ6calcR0 == 0 || MQ7calcR0 == 0 || MQ8calcR0 == 0 || MQ9calcR0 == 0 || MQ135calcR0 == 0) {
    if (serialprint == 0) {
      Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply");
    }
    if (lcdprint == 0) {
      lcd.setCursor(0, 0);
      lcd.print("W: Connection issue");
      lcd.setCursor(1, 0);
      lcd.print("R0 is zero");
    }
    while (1)
      ;
  }
  /*****************************  MQ CAlibration ********************************************/

  if (serialprint == 0) {
    // Print in serial monitor
    Serial.println("MQ2 to MQ9 - lecture");
    Serial.println("*************************** Lectures from MQ-board ***************************");
    Serial.println("     |     |    LPG   |  Alcohol |   CH4, Alcohol, LPG, Smoke    |  Alcohol |   CH4    |    CO    |    H2    |    LPG   | CO, CO2, Tolueno, NH4, Acetona");
    Serial.println("Temp | Hum |    MQ-2  |   MQ-3   |             MQ-4              |   MQ-5   |   MQ-6   |   MQ-7   |   MQ-8   |   MQ-9   |             MQ-135            ");
    // pinMode(calibration_button, INPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  float gpslat, gpslng;
  int year, month, day;
  int hour, min, sec, centi;

  // GPS Data
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }
  //  if (gps.location.isUpdated()) {
  if (serialprint == 0) {
    // Latitude in degrees (double
    Serial.print("Latitude= ");
    Serial.print(gps.location.lat(), 6);
    gpslat = gps.location.lat();
    // Longitude in degrees (double)
    Serial.print(" Longitude= ");
    Serial.println(gps.location.lng(), 6);
    gpslng = gps.location.lng();

    // Raw latitude in whole degrees
    Serial.print("Raw latitude = ");
    Serial.print(gps.location.rawLat().negative ? "-" : "+");
    Serial.println(gps.location.rawLat().deg);
    // ... and billionths (u16/u32)
    Serial.println(gps.location.rawLat().billionths);

    // Raw longitude in whole degrees
    Serial.print("Raw longitude = ");
    Serial.print(gps.location.rawLng().negative ? "-" : "+");
    Serial.println(gps.location.rawLng().deg);
    // ... and billionths (u16/u32)
    Serial.println(gps.location.rawLng().billionths);

    // Raw date in DDMMYY format (u32)
    Serial.print("Raw date DDMMYY = ");
    Serial.println(gps.date.value());

    // Year (2000+) (u16)
    Serial.print("Year = ");
    Serial.println(gps.date.year());
    year = gps.date.year();
    // Month (1-12) (u8)
    Serial.print("Month = ");
    Serial.println(gps.date.month());
    month = gps.date.month();
    // Day (1-31) (u8)
    Serial.print("Day = ");
    Serial.println(gps.date.day());
    day = gps.date.day();

    // Raw time in HHMMSerial1CC format (u32)
    Serial.print("Raw time in HHMMSerial1CC = ");
    Serial.println(gps.time.value());

    // Hour (0-23) (u8)
    Serial.print("Hour = ");
    Serial.println(gps.time.hour());
    hour = gps.time.hour();
    // Minute (0-59) (u8)
    Serial.print("Minute = ");
    Serial.println(gps.time.minute());
    min = gps.time.minute();
    // Second (0-59) (u8)
    Serial.print("Second = ");
    Serial.println(gps.time.second());
    sec = gps.time.second();
    // 100ths of a second (0-99) (u8)
    Serial.print("Centisecond = ");
    Serial.println(gps.time.centisecond());
    centi = gps.time.centisecond();

    // Raw speed in 100ths of a knot (i32)
    Serial.print("Raw speed in 100ths/knot = ");
    Serial.println(gps.speed.value());
    // Speed in knots (double)
    Serial.print("Speed in knots/h = ");
    Serial.println(gps.speed.knots());
    // Speed in miles per hour (double)
    Serial.print("Speed in miles/h = ");
    Serial.println(gps.speed.mph());
    // Speed in meters per second (double)
    Serial.print("Speed in m/s = ");
    Serial.println(gps.speed.mps());
    // Speed in kilometers per hour (double)
    Serial.print("Speed in km/h = ");
    Serial.println(gps.speed.kmph());

    // Raw course in 100ths of a degree (i32)
    Serial.print("Raw course in degrees = ");
    Serial.println(gps.course.value());
    // Course in degrees (double)
    Serial.print("Course in degrees = ");
    Serial.println(gps.course.deg());

    // Raw altitude in centimeters (i32)
    Serial.print("Raw altitude in centimeters = ");
    Serial.println(gps.altitude.value());
    // Altitude in meters (double)
    Serial.print("Altitude in meters = ");
    Serial.println(gps.altitude.meters());
    // Altitude in miles (double)
    Serial.print("Altitude in miles = ");
    Serial.println(gps.altitude.miles());
    // Altitude in kilometers (double)
    Serial.print("Altitude in kilometers = ");
    Serial.println(gps.altitude.kilometers());
    // Altitude in feet (double)
    Serial.print("Altitude in feet = ");
    Serial.println(gps.altitude.feet());

    // Number of satellites in use (u32)
    Serial.print("Number os satellites in use = ");
    Serial.println(gps.satellites.value());

    // Horizontal Dim. of Precision (100ths-i32)
    Serial.print("HDOP = ");
    Serial.println(gps.hdop.value());
  }
  //  }
  // GPS Data End

  // Sensors Data Read
  // Update the voltage lectures
  MQ2.update();
  MQ3.update();
  MQ4.update();
  MQ5.update();
  MQ6.update();
  MQ7.update();
  MQ8.update();
  MQ9.update();
  MQ135.update();
  // Read the sensor and print in serial port
  float MQ2Lecture = MQ2.readSensor();
  float MQ3Lecture = MQ3.readSensor();
  // MQ4
  MQ4.setA(3811.9);
  MQ4.setB(-3.113);              // Configurate the ecuation values to get CH4 concentration
  float LPG = MQ4.readSensor();  // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ4.setA(1012.7);
  MQ4.setB(-2.786);              // Configurate the ecuation values to get CH4 concentration
  float CH4 = MQ4.readSensor();  // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ4.setA(200000000000000);
  MQ4.setB(-19.05);                  // Configurate the ecuation values to get CH4 concentration
  float Alcohol = MQ4.readSensor();  // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ4.setA(30000000);
  MQ4.setB(-8.308);                // Configurate the ecuation values to get CH4 concentration
  float Smoke = MQ4.readSensor();  // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  float MQ5Lecture = MQ5.readSensor();
  float MQ6Lecture = MQ6.readSensor();
  float MQ7Lecture = MQ7.readSensor();
  float MQ8Lecture = MQ8.readSensor();
  float MQ9Lecture = MQ9.readSensor();
  // MQ135
  MQ135.setA(605.18);
  MQ135.setB(-3.937);             // Configurate the ecuation values to get CO concentration
  float CO = MQ135.readSensor();  // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ135.setA(77.255);
  MQ135.setB(-3.18);               // Configurate the ecuation values to get Alcohol concentration
  float CO2 = MQ135.readSensor();  // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ135.setA(44.947);
  MQ135.setB(-3.445);                  // Configurate the ecuation values to get Tolueno concentration
  float Tolueno = MQ135.readSensor();  // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ135.setA(102.2);
  MQ135.setB(-2.473);              // Configurate the ecuation values to get NH4 concentration
  float NH4 = MQ135.readSensor();  // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  MQ135.setA(34.668);
  MQ135.setB(-3.369);                  // Configurate the ecuation values to get Acetona concentration
  float Acetona = MQ135.readSensor();  // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    if (serialprint == 0) {
      Serial.println(F("Failed to read from DHT sensor!"));
    }
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  // Sensors Data Read End
  if (lcdprint == 0) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Analyzing");
    lcd.setCursor(10, 0);
    lcd.print(".");
    lcd.setCursor(11, 0);
    lcd.print(".");
    lcd.setCursor(12, 0);
    lcd.print(".");
    delay(1000);
  }

  // SD Card Data write
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File myFile = SD.open("aqdata.txt", FILE_WRITE);

  // If the file opened okay, write to it:
  if (myFile) {
    if (serialprint == 0) {
      Serial.print("Writing to aqdata.txt...");
    }
    if (lcdprint == 0) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Writing to SD");
      lcd.setCursor(14, 0);
      lcd.print(".");
      lcd.setCursor(15, 0);
      lcd.print(".");
      lcd.setCursor(16, 0);
      lcd.print(".");
      delay(1000);
    }
    // Write
    if (read_no == 999999) {
      myFile.print("Reading Set End\nNew Set \n\n ");
    }

    myFile.print("~Read ");
    myFile.print(read_no);
    myFile.print(": \n");

    myFile.print("Location:");
    myFile.print("\n");
    myFile.print("Latitude: ");
    myFile.print(gpslat);
    myFile.print("\n");
    myFile.print("Longitude: ");
    myFile.print(gpslng);
    myFile.print("\n");


    myFile.print("Date(day.month.year): ");
    myFile.print(day);
    myFile.print(".");
    myFile.print(month);
    myFile.print(".");
    myFile.print(year);
    myFile.print("\n");

    myFile.print("Time(hour:minute:second): ");
    myFile.print(hour);
    myFile.print(":");
    myFile.print(min);
    myFile.print(":");
    myFile.print(sec);
    myFile.print("\n");


    myFile.print("Temperature | Humidity ");
    myFile.print("\n");
    myFile.print(t);
    myFile.print(" C");
    myFile.print("        ");
    myFile.print(hic);
    myFile.print(" %");
    myFile.print("\n");



    myFile.print("MQ2: ");
    myFile.print(MQ2Lecture);
    myFile.print("\n");

    myFile.print("MQ3: ");
    myFile.print(MQ3Lecture);
    myFile.print("\n");

    myFile.print("LPG: ");
    myFile.print(LPG);
    myFile.print("\n");

    myFile.print("CH4: ");
    myFile.print(CH4);
    myFile.print("\n");

    myFile.print("Alcohol: ");
    myFile.print(Alcohol);
    myFile.print("\n");

    myFile.print("Smoke: ");
    myFile.print(Smoke);
    myFile.print("\n");

    myFile.print("MQ5: ");
    myFile.print(MQ5Lecture);
    myFile.print("\n");

    myFile.print("MQ6: ");
    myFile.print(MQ6Lecture);
    myFile.print("\n");

    myFile.print("MQ7: ");
    myFile.print(MQ7Lecture);
    myFile.print("\n");

    myFile.print("MQ8: ");
    myFile.print(MQ8Lecture);
    myFile.print("\n");

    myFile.print("MQ9: ");
    myFile.print(MQ9Lecture);
    myFile.print("\n");

    myFile.print("Acetona: ");
    myFile.print(Acetona);
    myFile.print("\n");

    myFile.print("CO: ");
    myFile.print(CO);
    myFile.print("\n");

    myFile.print("CO2: ");
    myFile.print(CO2);
    myFile.print("\n");

    myFile.print("Tolueno: ");
    myFile.print(Tolueno);
    myFile.print("\n");

    myFile.print("NH4: ");
    myFile.print(NH4);
    myFile.print("\n");

    myFile.print("~Read ");
    myFile.print(read_no);
    myFile.print(" end.");
    myFile.write("\n\n");  // new line
    // Close the file:
    myFile.close();
    if (serialprint == 0) {
      Serial.println("done.");
    }
    if (lcdprint == 0) {
      lcd.setCursor(1, 2);
      lcd.print("done.");
    }
  } else {
    // If the file didn't open, print an error:
    if (serialprint == 0) {
      Serial.println("error opening aqdata.txt");
    }
    if (lcdprint == 0) {
      lcd.setCursor(1, 0);
      lcd.print("error opening");
      lcd.setCursor(1, 1);
      lcd.print("aqdata.txt");
    }
  }
  if (lcdprint == 0) {
    delay(2500);
  }
  // SD Card Data write End


  // Sensors data LCD write
  if (lcdprint == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature: ");
    lcd.setCursor(13, 0);
    lcd.print(t);
    lcd.setCursor(18, 0);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("--------------------");
    lcd.setCursor(0, 2);
    lcd.print("Humidity: ");
    lcd.setCursor(13, 2);
    lcd.print(hic);
    lcd.setCursor(18, 2);
    lcd.print("%");
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MQ2");
    lcd.setCursor(10, 0);
    lcd.print(MQ2Lecture);
    lcd.setCursor(0, 1);
    lcd.print("MQ3");
    lcd.setCursor(10, 1);
    lcd.print(MQ3Lecture);
    lcd.setCursor(0, 2);
    lcd.print("LPG");
    lcd.setCursor(10, 2);
    lcd.print(LPG);
    lcd.setCursor(0, 3);
    lcd.print("CH4");
    lcd.setCursor(10, 3);
    lcd.print(CH4);
    delay(2000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Alcohol");
    lcd.setCursor(10, 0);
    lcd.print(Alcohol);
    lcd.setCursor(0, 1);
    lcd.print("Smoke");
    lcd.setCursor(10, 1);
    lcd.print(Smoke);
    lcd.setCursor(10, 1);
    lcd.setCursor(0, 2);
    lcd.print("MQ5");
    lcd.setCursor(10, 2);
    lcd.print(MQ5Lecture);
    lcd.setCursor(0, 3);
    lcd.print("MQ6");
    lcd.setCursor(10, 3);
    lcd.print(MQ6Lecture);
    delay(2000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("MQ7");
    lcd.setCursor(10, 0);
    lcd.print(MQ7Lecture);
    lcd.setCursor(0, 1);
    lcd.print("MQ8");
    lcd.setCursor(10, 1);
    lcd.print(MQ8Lecture);
    lcd.setCursor(0, 2);
    lcd.print("MQ9");
    lcd.setCursor(10, 2);
    lcd.print(MQ9Lecture);
    lcd.setCursor(0, 3);
    lcd.print("Acetona");
    lcd.setCursor(10, 3);
    lcd.print(Acetona);
    delay(2000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("CO");
    lcd.setCursor(10, 0);
    lcd.print(CO);
    lcd.setCursor(0, 1);
    lcd.print("CO2");
    lcd.setCursor(10, 1);
    lcd.print(CO2);
    lcd.setCursor(0, 2);
    lcd.print("Tolueno");
    lcd.setCursor(10, 2);
    lcd.print(Tolueno);
    lcd.setCursor(0, 3);
    lcd.print("NH4");
    lcd.setCursor(10, 3);
    lcd.print(NH4);
    delay(2000);
    lcd.clear();
  }

  if (serialprint == 0) {
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));
    Serial.print("| ");
    Serial.print(MQ2Lecture);
    Serial.print("   |   ");
    Serial.print(MQ3Lecture);
    Serial.print("   |   ");
    Serial.print(LPG);
    Serial.print("   |   ");
    Serial.print(CH4);
    Serial.print("   |   ");
    Serial.print(Alcohol);
    Serial.print("   |   ");
    Serial.print(Smoke);
    Serial.print("   |   ");
    Serial.print(MQ5Lecture);
    Serial.print("   |   ");
    Serial.print(MQ6Lecture);
    Serial.print("   |   ");
    Serial.print(MQ7Lecture);
    Serial.print("   |   ");
    Serial.print(MQ8Lecture);
    Serial.print("   |   ");
    Serial.print(MQ9Lecture);
    Serial.print("   |   ");
    Serial.print(Acetona);
    Serial.print("   |   ");
    Serial.print(CO);
    Serial.print("   |   ");
    Serial.print(CO2);
    Serial.print("   |   ");
    Serial.print(Tolueno);
    Serial.print("   |   ");
    Serial.print(NH4);
    Serial.println("|");
  }
  // Sensors data LCD write End
  read_no++;
  if (read_no > 1000000)
    read_no = 1;
}