//----------------------------------------Including Libraries.
#include "FS.h"
#include "SD_MMC.h"
//----------------------------------------

// Defines the SD Card Slot PIN (SDMMC).
#define SD_MMC_CLK_PIN    39
#define SD_MMC_CMD_PIN    38
#define SD_MMC_DATA_PIN   40



//________________________________________________________________________________ listDir(fs::FS &fs, const char * dirname, uint8_t levels)
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file){
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels -1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ readFile(fs::FS &fs, const char * path)
void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ writeFile(fs::FS &fs, const char * path, const char * message)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ deleteFile(fs::FS &fs, const char * path)
void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:

  delay(1000);
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  //---------------------------------------- SDMMC Settings.
  Serial.println();
  Serial.println("------SDMMC settings.");
  
  // Set SDMMC PIN (SDMMC 1-Bit).
  Serial.println("Set SDMMC PIN (SDMMC 1-Bit).");
  if (!SD_MMC.setPins(SD_MMC_CLK_PIN, SD_MMC_CMD_PIN, SD_MMC_DATA_PIN)) {
    Serial.println("Set SDMMC PIN failed !.");
    return;
  }
  Serial.println("Set SDMMC PIN (SDMMC 1-Bit) successfully.");

  Serial.println();
  
  // Initialize SD card with SDMMC (SDMMC 1-Bit).
  Serial.println("Initialize SD card with SDMMC (SDMMC 1-Bit).");
  for (byte i = 0; i < 3; i++) {
    if (!SD_MMC.begin("/sdcard", true)) {
      Serial.println("SD Card Mount Failed !.");
      Serial.println("Try again.");
    } else {
      break;
    }
    delay(1000);
  }
  Serial.println("SD Card Mount Successfully.");
  Serial.println("SD Card initialization with SDMMC (SDMMC 1-Bit) Successful.");

  Serial.println("------SDMMC settings complete.");
  //---------------------------------------- 

  delay(1000);

  //---------------------------------------- Get SD Card type.
  Serial.println();
  Serial.println("Get SD_MMC Card type.");
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD_MMC card attached");
    return;
  }

  Serial.print("SD_MMC Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  //---------------------------------------- 

  delay(1000);

  //---------------------------------------- Get SD_MMC Card size.
  Serial.println();
  Serial.println("Get SD_MMC Card size.");
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
  //---------------------------------------- 

  delay(1000);

  Serial.println();
  
  listDir(SD_MMC, "/", 0);
  writeFile(SD_MMC, "/test.txt", "Hello World!\n");
  readFile(SD_MMC, "/test.txt");
  deleteFile(SD_MMC, "/test.txt");
  Serial.println();
  Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));
}
//________________________________________________________________________________ 



//________________________________________________________________________________ VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:

  delay(100);
}
//________________________________________________________________________________ 
