#include<Arduino.h>
#include"DHT.h"
#include"FS.h"
#include <SPI.h>
#include <SD.h>
#include<string.h>
#include <RTClib.h>
#define SD_MISO     21
#define SD_MOSI     22
#define SD_SCLK     23
#define SD_CS       25
#define A0 32
#define A1 33
#define A2 34
#define A3 35
#define A4 36
#define A5 39
#define muxs0 26
#define muxs1 27
#define muxs2 14
#define muxs3 12
#define signal 13
SPIClass sdSPI(VSPI);
const int maxi =97*120;
String dataMessage;
int counter=0;
int check=0;
void logSDCard() {
  // dataMessage = String(readingID) + "," + String(dayStamp) + "," + String(timeStamp) + "," + 
  //              String(temperature) + "\r\n";
  dataMessage = "Hello World \n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  //appendFile(SD, "/data1.txt", dataMessage.c_str());
}

// Modify the writeFile function to accept an array of integers as input
void writeFile(fs::FS &fs, const char *path, int *intArray, size_t arraySize) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  for (size_t i = 0; i < arraySize; i++) {
    file.print(intArray[i]);
    if (i < arraySize - 1) {
      file.print(","); // Add a comma between values
    }
  }
  file.println(); // Move to the next line after writing the array

  Serial.println("Array written to file");
  file.close();
}


// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const int* intArray, size_t arraySize) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  for (size_t i = 0; i < arraySize; i++) {
    file.print(intArray[i]);
    if (i < arraySize - 1) {
      file.print(","); // Add a comma between values
    }
  }
  file.println(); // Move to the next line after writing the array

  Serial.println("Array written to file");
  file.close();
}
void setmuxchannel(int channel)
{
    digitalWrite(muxs0,bitRead(channel,0));
    digitalWrite(muxs1,bitRead(channel,1));
    digitalWrite(muxs2,bitRead(channel,2));
    digitalWrite(muxs3,bitRead(channel,3));
}
int pin[6]={A0,A1,A2,A3,A4,A5};
int value1[maxi];
int setupvalue[16][6];
size_t arraySize = sizeof(value1) / sizeof(value1[0]);
void setup() {
   Serial.begin(115200);
  SD.begin(SD_CS);  
  check++;
  sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  check++;
 while(!SD.begin(SD_CS, sdSPI, 1000000)) {
   Serial.println("Card Mount Failed");
}
  check++;
  Serial.println("1");
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  File file = SD.open("/datasis2_16.txt");
  Serial.print("File opened");
  Serial.println("2");
  pinMode(muxs0,OUTPUT);
  pinMode(muxs1,OUTPUT);
  pinMode(muxs2,OUTPUT);
  pinMode(muxs3,OUTPUT);
  pinMode(signal,OUTPUT);
  for(int j=0;j<16;j++)
  {
    
    setmuxchannel(j);
    digitalWrite(signal,HIGH);
    for(int i=0;i<6;i++)
    {
         setupvalue[j][i]=analogRead(pin[i]);
    }
    digitalWrite(signal,LOW);
  }
  check++;
}
void loop()
{
      //   for(int j=0;j<16;j++)
      // {
      //   setmuxchannel(j);
      //   digitalWrite(signal,HIGH);
      //   for(int i=0;i<6;i++)
      //   {
      //     value[j][i]=analogRead(pin[i]);
      //   }
      //   digitalWrite(signal,LOW);
      // }
      for(int k=0;k<16;k++)
      {
        setmuxchannel(k);
        digitalWrite(signal,HIGH);
        for(int l=0;l<6;l++)
        {
          if(analogRead(pin[l])-setupvalue[k][l]<0)
          {
              value1[counter]=0;
          }
          else
          {
            value1[counter]=analogRead(pin[l])-setupvalue[k][l];
          }
          // Serial.print(value1[counter]);
          counter++;
          
          // Serial.print(" ");
        }
        digitalWrite(signal,LOW);
        // Serial.println();
      }
      value1[counter]=-100;
      counter++;
      if(counter==97*120)
      {
          counter=0;
      // int start=micros();
        appendFile(SD, "/datasis2_16.txt", value1, arraySize);
        logSDCard();
      }
      Serial.println(check);
}