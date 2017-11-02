/*************************************************** 
  Written by Richard Gowen (@alt_bier)
  
  This is my Sorting Hat code to control an Arduino Uno
  with an MP3 player shield and four buttons connected to
  analog pins 0 to 3 to for house selection.

  It will randomly choose some sound clips to play for
  the sorting hat to make its choice before playing the
  sound clip of the announcment house choice.

  This could easily be changed to randomly choose a house.
  But for my purposes I wanted to select the house assignment.

  This is based on the Adafruit VS1053 Codec Breakout example
  and requires either the Adafruit Music Maker MP3 Shield
  ----> https://www.adafruit.com/products/1788
  OR the the Adafruit VS1053 Codec Breakout board
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Example was Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// define the pins used
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

// Setup Buttons
#define BUTT_ONE 14
#define BUTT_TWO 15
#define BUTT_THR 16
#define BUTT_FOU 17
int ButtonOne = 0;
int ButtonTwo = 0;
int ButtonThr = 0;
int ButtonFou = 0;

// Setup Audio File Name Arrays
// Array of Stalling Audio Clips
 char* audio_s[] = {
  "s-ahrigh.mp3", // "Ahh... Right then."
  "s-diffic.mp3", // "Hmm... Difficult, very difficult."
  "s-itsall.mp3", // "You could be great, you know.  It's all here, in your head." -- break into two clips put great to attributes
  "s-ohyes1.mp3", // "Oh, Yes."
  "s-righto.mp3", // "Hm, right. Ok."
  "s-wheret.mp3"  // "But where to put you?"
};
// Array of Attribute Audio Clips
char* audio_t[] = {
  "t-courag.mp3", // "Plenty of courage, I see."
  "t-notaba.mp3", // "Not a bad mind a..." -- needs work, either clip or expand or replace with great clip above
  "t-readym.mp3", // "Ahhh yes. A ready mind."
  "t-talent.mp3"  // "There's talent."
};
// Array of Determined House Audio Clips
char* audio_k[] = {
  "k-better.mp3", // "Better be..."
  "k-iknow1.mp3", // "I Know!"
  "k-iknowj.mp3", // "I know just what to do with you."
  "k-youbel.mp3"  // "You belong in..."
};
// Array of House Announcement Audio Clips
char* audio_h[] = {
  "h-gryff1.mp3", // "Gryffindor!"
  "h-huffl1.mp3", // "Hufflepuff!"
  "h-raven1.mp3", // "Ravenclaw!"
  "h-slyth1.mp3"  // "Slytherin!"
};

void setup() {
  // Setup Buttons
  pinMode(BUTT_ONE, INPUT);
  pinMode(BUTT_TWO, INPUT);
  pinMode(BUTT_THR, INPUT);
  pinMode(BUTT_FOU, INPUT);

  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Simple Test");

  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
   if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(5,5);

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

  // Original Example Code Commented Out
  //
  // Play one file, don't return until complete
  //  Serial.println(F("Playing track 001"));
  //  musicPlayer.playFullFile("track001.mp3");
  // Play another file in the background, REQUIRES interrupts!
  //  Serial.println(F("Playing track 002"));
  //  musicPlayer.startPlayingFile("track002.mp3");
}

void loop() {
  // read the state of the buttons
  ButtonOne = digitalRead(BUTT_ONE);
  ButtonTwo = digitalRead(BUTT_TWO);
  ButtonThr = digitalRead(BUTT_THR);
  ButtonFou = digitalRead(BUTT_FOU);  

  // determine random audio clip numbers
  int stall1 = (random(0,6));
  int stall2 = (random(0,6));
  if (stall2 == stall1) {
    if (stall1 == 0) {
      stall2 = 5;
    } else {
      stall2 = stall1-1; 
    }
  }
  int attr1 = (random(0,4));
  int know1 = (random(0,4));

  // check if a button is pushed (HIGH) and then execute code.
  if (ButtonOne == HIGH) {
    // Execute One - Gryffindor
    Serial.println(F("Button One HIGH - Gryffindor"));
    // Play Random Clips
    playRandom(stall1, stall2, attr1, know1);
    // Play House Announcement
    musicPlayer.playFullFile(audio_h[0]);
    
  } else if (ButtonTwo == HIGH) {
    // Execute Two - Hufflepuff
    Serial.println(F("Button Two HIGH - Hufflepuff"));
    // Play Random Clips
    playRandom(stall1, stall2, attr1, know1);
    // Play House Announcement
    musicPlayer.playFullFile(audio_h[1]);
    
  } else if (ButtonThr == HIGH) {
    // Execute Three - Ravenclaw
    Serial.println(F("Button Three HIGH - Ravenclaw"));
    // Play Random Clips
    playRandom(stall1, stall2, attr1, know1);
    // Play House Announcement
    musicPlayer.playFullFile(audio_h[2]);

  } else if (ButtonFou == HIGH) {
    // Execute Three - Slytherin
    Serial.println(F("Button Four HIGH - Slytherin"));
    // Play Random Clips
    playRandom(stall1, stall2, attr1, know1);
    // Play House Announcement
    musicPlayer.playFullFile(audio_h[3]);

  }

  // Original Example Code Commented Out
  //
  // File is playing in the background
  //if (musicPlayer.stopped()) {
  //  Serial.println("Done playing music");
  //  while (1);
  //}
  //if (Serial.available()) {
  //  char c = Serial.read();
  //  
    // if we get an 's' on the serial console, stop!
  //  if (c == 's') {
  //    musicPlayer.stopPlaying();
  //  }
  //  
    // if we get an 'p' on the serial console, pause/unpause!
  //  if (c == 'p') {
  //    if (! musicPlayer.paused()) {
  //      Serial.println("Paused");
  //      musicPlayer.pausePlaying(true);
  //    } else { 
  //      Serial.println("Resumed");
  //      musicPlayer.pausePlaying(false);
  //    }
  //  }
  //}
  
  delay(100);
  
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

void playRandom(int stall1, int stall2, int attr1, int know1) {
    // Stall
    musicPlayer.playFullFile(audio_s[stall1]);
    delay(8);
    // Mention Attribute
    musicPlayer.playFullFile(audio_t[attr1]);
    delay(8);
    // Stall More
    musicPlayer.playFullFile(audio_s[stall2]);
    delay(8);
    // Made Choice
    musicPlayer.playFullFile(audio_k[know1]);
    delay(8);
}

