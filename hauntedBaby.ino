/*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are the pins used by the haunted baby
#define RelayEYEs A1 // eyes on the baby
#define RelayEXTEND A3 // extend the baby
#define RelayRETRACT A4 // shrink
#define MotionDETECT A0 // PIR



// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);


////

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit VS1053 Library Test");

  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files
  printDirectory(SD.open("/"), 0);

  // SETUP for baby
  
  pinMode(RelayEYEs, OUTPUT);
  digitalWrite(RelayEYEs, HIGH);  // relays are OFF when HIGH
  
  pinMode(RelayEXTEND, OUTPUT);
  digitalWrite(RelayEXTEND, HIGH);
  
  pinMode(RelayRETRACT, OUTPUT);
  digitalWrite(RelayRETRACT, HIGH);
  
  pinMode(MotionDETECT, INPUT);
  

  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(15,15);

  /***** Two interrupt options! *******/ 
  // This option uses timer0, this means timer1 & t2 are not required
  // (so you can use 'em for Servos, etc) BUT millis() can lose time
  // since we're hitchhiking on top of the millis() tracker
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  // *** This method is preferred
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
}


void loop() {  
  // Alternately, we can just play an entire file at once
  // This doesn't happen in the background, instead, the entire
  // file is played and the program will continue when it's done!
  // musicPlayer.playFullFile("scream.mp3");

  // Start playing a file, then we can do stuff while waiting for it to finish
  // if (! musicPlayer.startPlayingFile("giggles1.wav")) {
    // Serial.println("Could not open file giggles1.wav");
    // while (1);
  // }
  // Serial.println(F("Started playing"));

  //while (musicPlayer.playingMusic) {
    // file is now playing in the 'background' so now's a good time
    // to do something else like handling LEDs or buttons :)
    //Serial.print(".");
    //delay(1000);
 // }
  //Serial.println("Done playing music");

  while(digitalRead(MotionDETECT) == LOW){
    delay(1000); // do nothing for 1s
  }
  Serial.println("Motion Found!");

  Serial.println("Playing Giggles");
  musicPlayer.playFullFile("giggles1.wav");

  Serial.println("Playing Scream!");
  musicPlayer.startPlayingFile("scream.mp3");

  Serial.println("Extending Cylinder");
  digitalWrite(RelayEXTEND, LOW);
  delay(100);
  digitalWrite(RelayEXTEND, HIGH);
  delay(100);
  Serial.println("Turning on Eyes");
  digitalWrite(RelayEYEs, LOW);
  while(musicPlayer.playingMusic){
    delay(100);
  }
  delay(5000);
  Serial.println("Scare Complete!");
  digitalWrite(RelayEYEs, HIGH);
  digitalWrite(RelayRETRACT, LOW);
  delay(100);
  digitalWrite(RelayRETRACT, HIGH);
  
  Serial.println("Entering reset delay");
  delay(20000);
  Serial.println("Finished with reset delay.");
}


/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

