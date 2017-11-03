# arduino_sorting_hat
Arduino powered talking sorting hat project

## Written by Richard Gowen (@alt_bier)
  
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

The audio directory has the mp3 files that I used.
These should be copied to the root directory 
of the SD card in the mp3 shield.

