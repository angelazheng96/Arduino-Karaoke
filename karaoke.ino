/*
  Angela Zheng
  TEJ3M-01
  January 24, 2025
  Summative Project - Karaoke Boogie Nights!

  VERSION CONTROL
  2025-01-13_Karaoke:
    Initialized LCD and Serial Monitor for user input.
  2025-01-15_Karaoke:
    Added buzzer code. Works with delay() but still working on using micros().
  2025-01-16_Karaoke:
    Buzzer notes play using micros(). Began writing header file for custom Song class.
  2025-01-17_Karaoke:
    Continued writing header file and source file for Song class.
    The reference to the method playSong() works and previous code was copied from this file to play a simple melody (still untested).
  2025-01-19_Karaoke:
    Decided not to continue with writing my own Song class.
    Polyphony works! Two buzzers play Twinkle Twinkle Little Star.
    Added LEDs (electronic organ); LEDs connected to pins 0, 1, 13 not turning on.
  2025-01-20_Karaoke_a:
    LEDs all working (connected to analog pins).
    Added pauses between notes (last 10% of each note duration).
    Added second song, Happy Birthday.
  2025-01-20_Karaoke_b:
    Created String arrays for storing lyrics.
  2025-01-21_Karaoke:
    Added lyrics scrolling per note.
  2025-01-22_Karaoke:
    Debugged lyrics not being read from PROGMEM properly after multiple plays - made the buffer char array static.
    Cleaned up code comments.
  angelaz_karaoke:
    Final code to be handed in.

  CREDITS
  Libraries:
    Note Frequencies: "pitches.h" by OSEPP
    Liquid Crystal Library: Arduino Example Files by Tom Igoe, Scott Fitzgerald, Arturo Guadalupi
    digitalWriteFast Library: by Watterott (https://github.com/ArminJo/digitalWriteFast)

  Sheet Music: (modified slightly)
    Twinkle Twinkle Little Star: by JuliaTheZhu (https://musescore.com/juliathezhu/twinkle-twinkle-little-star-easy)
    Happy Birthday: by unmarried_199 (https://musescore.com/user/32960140/scores/5847891)

  Other:
    Playing Tones Using digitalWrite(): inspired by cwduffy01 (https://forum.arduino.cc/t/playing-two-buzzers-at-once/505726)
    Storing Data in Flash: PROGMEM (https://www.arduino.cc/reference/tr/language/variables/utilities/progmem/)
    Problems Reading Back Strings from PROGMEM: using static buffer by DuaneB (https://forum.arduino.cc/t/problem-with-reading-back-from-progmem-help/113753/5)
*/

// include library files
#include "pitches.h"
#include <LiquidCrystal.h>
#include <digitalWriteFast.h>

// define LCD
LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

// define constant values
const int numSongs = 2;       // number of songs
const int numBuzzers = 2;     // number of buzzers (two-part harmony is used here)
const int numLeds = 8;        // number of LEDs (to span a full octave)

const int maxNumNotes = 44;   // maximum number of notes among all songs and buzzers

const int numNotes[numSongs] = { 44, 25 };    // number of notes - indexed by song
const int bpm[numSongs] = { 240, 200 };       // tempo of songs in beats per minute - indexed by song

// define I/O pins
const int buzzerPins[numBuzzers] = { 2, 9 };
const int ledPins[numLeds] = { 12, 11, 10, A0, A1, A2, A3, A4 };

// array to store the state of each buzzer (on or off)
bool buzzerStates[numBuzzers];

// stores note pitches from "pitches.h" file
// 1st dimension: indexed by song
// 2nd dimension: indexed by buzzer
// 3rd dimension: indexed by note
// each line is approximately one musical phrase
// shorter songs are padded at the end with 0's (no note played)
const uint16_t notes[numSongs][numBuzzers][maxNumNotes] PROGMEM = {

  // Twinkle Twinkle Little Star
  {
    // melody - buzzer 1
    {
      NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
      NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
      NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
      NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
      NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
      NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
      0, 0
    },

    // harmony - buzzer 2
    {
      NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_F3, NOTE_F3, NOTE_E3,
      NOTE_D3, NOTE_D3, NOTE_C3, NOTE_C3, NOTE_B2, NOTE_B2, NOTE_E3,
      NOTE_E3, NOTE_E3, NOTE_D3, NOTE_D3, NOTE_C3, NOTE_C3, NOTE_B2,
      NOTE_E3, NOTE_E3, NOTE_D3, NOTE_D3, NOTE_C3, NOTE_C3, NOTE_G2,
      NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4,
      NOTE_D3, NOTE_A3, NOTE_G3, NOTE_C3, NOTE_G3, NOTE_G2, NOTE_E3
    }
  },

  // Happy Birthday
  {
    // melody - buzzer 1
    {
      NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_C5, NOTE_B4,
      NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_D5, NOTE_C5,
      NOTE_G4, NOTE_G4, NOTE_G5, NOTE_E5, NOTE_C5, NOTE_B4, NOTE_A4,
      NOTE_F5, NOTE_F5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_C5,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },

    // harmony - buzzer 2
    {
            0, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_D4, NOTE_F4,
      NOTE_G4, NOTE_B3, NOTE_D4, NOTE_G4, NOTE_C4, NOTE_E4,
      NOTE_G4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_F4, NOTE_F4,
      NOTE_C4, NOTE_G4, NOTE_E4, NOTE_G4, NOTE_C4,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  }

};

// stores LED indexes to be lit up, with 0 being the leftmost and 7 being the rightmost LED
// follows the pattern of the melody (buzzer 1) to create an 'electronic organ'
// 1st dimension: indexed by song
// 2nd dimension: indexed by note
// each line is approximately one musical phrase
// shorter songs are padded at the end with 0's
const uint8_t leds[numSongs][maxNumNotes] PROGMEM = {

  // Twinkle Twinkle Little Star
  {
    0, 0, 4, 4, 5, 5, 4,
    3, 3, 2, 2, 1, 1, 0,
    4, 4, 3, 3, 2, 2, 1,
    4, 4, 3, 3, 2, 2, 1,
    0, 0, 4, 4, 5, 5, 4,
    3, 3, 2, 2, 1, 1, 0,
    0, 0
  },

  // Happy Birthday
  {
    0, 0, 1, 0, 3, 2,
    0, 0, 1, 0, 4, 3,
    0, 0, 7, 5, 3, 2, 1,
    6, 6, 5, 3, 4, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }
  
};

// stores note durations, with 1 representing one beat
// 1st dimension: indexed by song
// 2nd dimension: indexed by buzzer
// 3rd dimension: indexed by note
// each line is approximately one musical phrase
// shorter songs are padded at the end with 0's (no note played)
const uint8_t durations[numSongs][numBuzzers][maxNumNotes] PROGMEM = {

  // Twinkle Twinkle Little Star
  {
    // melody - buzzer 1
    {
      2, 2, 2, 2, 2, 2, 4,
      2, 2, 2, 2, 2, 2, 4,
      2, 2, 2, 2, 2, 2, 4,
      2, 2, 2, 2, 2, 2, 4,
      2, 2, 2, 2, 2, 2, 4,
      2, 2, 2, 2, 2, 2, 4,
      0, 0
    },

    // harmony - buzzer 2
    {
      2, 2, 2, 2, 2, 2, 4,
      2, 2, 2, 2, 2, 2, 4,
      2, 2, 2, 2, 2, 2, 4,
      2, 2, 2, 2, 2, 2, 4,
      2, 2, 2, 2, 1, 1, 1, 1, 4,
      2, 2, 2, 2, 2, 2, 4,
    }
  },

  // Happy Birthday
  {
    // melody - buzzer 1
    {
      1, 1, 2, 2, 2, 4,
      1, 1, 2, 2, 2, 4,
      1, 1, 2, 2, 2, 2, 4,
      1, 1, 2, 2, 2, 4,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },

    // harmony - buzzer 2
    {
      2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 4,
      2, 2, 2, 2, 4,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  }

};

// stores the number of characters to traverse in the lyrics for each note played in the melody
// 1st dimension: indexed by song
// 3rd dimension: indexed by note
// each line is approximately one musical phrase
// shorter songs are padded at the end with 0's (no note played)
const uint8_t lyricsDurations[numSongs][maxNumNotes + 1] PROGMEM = {

  // Twinkle Twinkle Little Star
  {
    0, 4, 5, 4, 5, 2, 5, 6,
    4, 2, 3, 4, 5, 4, 5,
    3, 1, 5, 4, 6, 3, 6,
    5, 2, 3, 5, 3, 4, 5,
    4, 5, 4, 5, 2, 5, 6,
    4, 2, 3, 4, 5, 4, 5,
    0, 0
  },

  // Happy Birthday
  {
    0, 2, 4, 5, 4, 3, 5,
    2, 4, 5, 4, 3, 5,
    2, 4, 5, 4, 5, 1, 5,
    2, 4, 5, 4, 3, 4,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }

};

// define lyrics for songs
const char song0[] PROGMEM = "Twinkle, twinkle, little star! How I wonder what you are. Up above the world so high, like a diamond in the sky. Twinkle, twinkle, little star! How I wonder what you are.";
const char song1[] PROGMEM = "Happy birthday to you, happy birthday to you! Happy birthday dear user. Happy birthday to you!";

// place lyrics in PROGMEM
const char *const lyrics[numSongs] PROGMEM = {
  song0,
  song1
};

// custom character for an upward arrow to indicate the current word being sung
byte arrow[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};

void setup() {

  // initialize LCD with # of columns and rows, and create custom character
  lcd.begin(16, 2);
  lcd.createChar(0, arrow);

  // initialize serial monitor
  Serial.begin(9600);

  // goes through loop for each buzzer
  for (int buzzerIndex = 0; buzzerIndex < numBuzzers; buzzerIndex++) {
    pinMode(buzzerPins[buzzerIndex], OUTPUT);   // sets pin mode to output
    buzzerStates[buzzerIndex] = false;          // initializes buzzer states to low
  }

  // goes through loop for each LED
  for (int ledIndex = 0; ledIndex < numLeds; ledIndex++) {
    pinMode(ledPins[ledIndex], OUTPUT);         // sets pin mode to output
  }

}

void loop() {
  showTitleScreen();                  // display title screen on LCD
  int songIndex = getSongChoice();    // get the user's choice of song
  playSong(songIndex - 1);            // play the selected song
}

// displays title screen on LCD
void showTitleScreen() {
  lcd.setCursor(1, 0);
  lcd.print("Welcome! Check");
  lcd.setCursor(1, 1);
  lcd.print("Serial Monitor");
}

// gets user input from serial monitor
// returns 1 or 2
int getSongChoice() {
  String userInput = "";

  // initial message to user
  Serial.println("\n________________________________________________________\n\nWelcome to Karaoke Boogie Nights!\n\nWould you like to play Song 1 or Song 2?\n");
  Serial.println("Song 1: Twinkle Twinkle Little Star\nSong 2: Happy Birthday\n");

  // loops until first user input is available
  while (true) {
    if (Serial.available()) {
      userInput = Serial.readString();
      break;
    }
  }

  // loops as long as the user input is not valid - prints an error message
  while (userInput.toInt() != 1 && userInput.toInt() != 2) {
    Serial.println("Not a valid input. Please enter '1' or '2'.\n");

    // waits for new user input
    while (true) {
      if (Serial.available()) {
        userInput = Serial.readString();
        break;
      }
    }
  }

  // returns the song index based on the user input
  return userInput.toInt();
}

// plays the song at the specified index
void playSong(int songIndex) {

  // DEFINE VARIABLES

  // these arrays are indexed by buzzer
  int noteIndex[numBuzzers], halfPeriod[numBuzzers];
  unsigned long noteDuration[numBuzzers], restDuration[numBuzzers], previousTime[numBuzzers], endTime[numBuzzers];

  unsigned long currentTime = micros();                   // gets the current time elapsed since start of program
  long beatDuration = calculateBeatDuration(songIndex);   // calculates the duration of one beat in microseconds

  // goes through each buzzer to initialize arrays
  for (int buzzerIndex = 0; buzzerIndex < numBuzzers; buzzerIndex++) {

    // the index of the note that each buzzer is currently on
    noteIndex[buzzerIndex] = 0;

    // calculates the duration of the first note in microseconds
    noteDuration[buzzerIndex] = calculateNoteDuration(beatDuration, songIndex, buzzerIndex, noteIndex[buzzerIndex]);

    // calculates the duration of the first rest in microseconds (10% of the duration of the note)
    restDuration[buzzerIndex] = noteDuration[buzzerIndex] * 0.1;

    // calculates the half period of the first note based on the pitch
    halfPeriod[buzzerIndex] = calculateHalfPeriod(songIndex, buzzerIndex, noteIndex[buzzerIndex]);

    // previous time that the buzzer switched states (HIGH/LOW)
    // initialized to the current time
    previousTime[buzzerIndex] = currentTime;

    // the timestamp when the current note will end
    endTime[buzzerIndex] = currentTime + noteDuration[buzzerIndex];

  }

  // turns on the first LED
  turnOnLed((int) pgm_read_byte_near(&leds[songIndex][0]));

  // index for which character in the lyrics it is currently on
  int lyricsIndex = 0;

  // reads the lyrics from PROGMEM into the String fullLyrics
  static char buffer[200];
  strcpy_P(buffer, (PGM_P) pgm_read_word_near(&lyrics[songIndex]));
  String fullLyrics = buffer;

  // displays the first lyrics on the LCD
  displayLyrics(fullLyrics, lyricsIndex);

  // LOOPS WHILE SONG HAS NOT FINISHED
  while (!songFinished(songIndex, noteIndex)) {

    // updates the current time
    currentTime = micros();

    // goes through each buzzer
    for (int buzzerIndex = 0; buzzerIndex < numBuzzers; buzzerIndex++) {

      // if the buzzer is still playing the same note
      if (currentTime <= endTime[buzzerIndex]) {

        // turns off the buzzer if it is not supposed to be playing a note
        // or it is currently in the pause between notes (last 10% of each note duration)
        if (halfPeriod[buzzerIndex] == -1 || buzzerNeedsRest(currentTime, endTime[buzzerIndex], restDuration[buzzerIndex])) {
          turnOffBuzzer(buzzerIndex);
        }

        // otherwise, it should be switching between HIGH or LOW to create the pitch
        // switches the buzzer state if a half period has elapsed
        else if (buzzerNeedsSwitch(halfPeriod[buzzerIndex], previousTime[buzzerIndex], currentTime)) {
          switchBuzzerState(buzzerIndex);

          // updates the previous time that the buzzer switched
          previousTime[buzzerIndex] = currentTime;
        }

      }

      // otherwise, the buzzer has to switch notes
      else {

        // increments the note index
        noteIndex[buzzerIndex]++;

        // same calculations for the new note being played
        noteDuration[buzzerIndex] = calculateNoteDuration(beatDuration, songIndex, buzzerIndex, noteIndex[buzzerIndex]);
        restDuration[buzzerIndex] = noteDuration[buzzerIndex] * 0.1;
        halfPeriod[buzzerIndex] = calculateHalfPeriod(songIndex, buzzerIndex, noteIndex[buzzerIndex]);

        // update previous time and end time for new note
        previousTime[buzzerIndex] = currentTime;
        endTime[buzzerIndex] = currentTime + noteDuration[buzzerIndex];

        // turns off current LED
        turnOffLed((int) pgm_read_byte_near(&leds[songIndex][noteIndex[0] - 1]));

        // turns on the next LED, as long as it has not yet reached the last note in the song
        if (noteIndex[0] < numNotes[songIndex]) {
          turnOnLed((int) pgm_read_byte_near(&leds[songIndex][noteIndex[0]]));
        }

        // if the current buzzer is the first one, update the lyrics
        if (buzzerIndex == 0) {

          // add to the lyrics index based on the lyricsDurations array
          lyricsIndex += pgm_read_byte_near(&lyricsDurations[songIndex][noteIndex[0]]);

          // displays new lyrics on the screen
          displayLyrics(fullLyrics, lyricsIndex);
          
        }

      }

    }

  }

  // SONG IS FINISHED

  // turns off all buzzers
  for (int buzzerIndex = 0; buzzerIndex < numBuzzers; buzzerIndex++) {
    turnOffBuzzer[buzzerIndex];
    buzzerStates[buzzerIndex] = false;
  }

  // turns off all LEDs
  for (int ledIndex = 0; ledIndex < numLeds; ledIndex++) {
    turnOffLed(ledIndex);
  }

  // clears LCD
  lcd.clear();
}

// BELOW ARE HELPER METHODS USED IN THE playSong() METHOD

// calculates the duration of one beat for a specific song, in microseconds
// duration of one beat = 1 / bpm * (60 s / 1 min) * (1000000 micros / s) = 60000000 / bpm
long calculateBeatDuration(int songIndex) {
  return 60000000 / bpm[songIndex];
}

// calculates the duration of a note, based on the beat and duration specified in the durations array, in microseconds
// duration = duration of one beat * number of beats
long calculateNoteDuration(long beatDuration, int songIndex, int buzzerIndex, int noteIndex) {
  return beatDuration * (int) pgm_read_byte_near(&durations[songIndex][buzzerIndex][noteIndex]);
}

// calculates half of a period for a specified frequency, in microseconds
int calculateHalfPeriod(int songIndex, int buzzerIndex, int noteIndex) {

  // reads the frequency of the specified note
  int note = pgm_read_dword_near(&notes[songIndex][buzzerIndex][noteIndex]);

  // if the frequency is 0, it is just there for padding (no note played)
  if (note == 0) {
    return -1;
  }

  // half period = 1 s / frequency / 2 = 1 000 000 micros / 2 / frequency = 500 000 / frequency
  return 500000 / note;
}

// displays lyrics on the LCD
// lyricsIndex: index of the first character to display from the String fullLyrics
void displayLyrics(String fullLyrics, int lyricsIndex) {
  lcd.clear();

  // print arrow
  lcd.setCursor(0, 1);
  lcd.write(byte(0));

  // print lyrics
  lcd.setCursor(0, 0);
  lcd.print(fullLyrics.substring(lyricsIndex, lyricsIndex + 16));
}

// returns whether the song is finished
bool songFinished(int songIndex, int noteIndex[]) {

  // goes through each buzzer for the current song
  // since the buzzers could have different number of notes for each song
  for (int buzzerIndex = 0; buzzerIndex < numBuzzers; buzzerIndex++) {

    // checks whether it has reached the end
    if (noteIndex[buzzerIndex] < numNotes[songIndex]) {
      return false;
    }

  }

  return true;
}

// returns whether the buzzer should be off for the pause between notes
// 10% of each note duration
bool buzzerNeedsRest(long currentTime, long endTime, long restDuration) {
  return (endTime - currentTime) <= restDuration;
}

// returns whether the buzzer needs to be switched from high to low, based on the frequency of the note being played
// halfPeriod: half of the period for this pitch, in microseconds
// previousTime: the previous time stamp when the buzzer was switched, in microseconds
// currentTime: the current time stamp, in microseconds
bool buzzerNeedsSwitch(int halfPeriod, long previousTime, long currentTime) {
  return currentTime - previousTime >= halfPeriod;
}

// returns the current state of the buzzer at the specified index
bool buzzerHigh(int buzzerIndex) {
  return buzzerStates[buzzerIndex];
}

// changes the specified buzzer to the opposite state
void switchBuzzerState(int buzzerIndex) {
  // if the buzzer is currently high, then switch it to low
  if (buzzerHigh(buzzerIndex)) {
    turnOffBuzzer(buzzerIndex);
  }

  // if the buzzer is currently low, then switch it to high
  else {
    turnOnBuzzer(buzzerIndex);
  }
}

// turns on the specified buzzer and updates the buzzerStates array
void turnOnBuzzer(int buzzerIndex) {
  digitalWriteFast(buzzerPins[buzzerIndex], HIGH);
  buzzerStates[buzzerIndex] = true;
}

// turns off the specified buzzer and updates the buzzerStates array
void turnOffBuzzer(int buzzerIndex) {
  digitalWriteFast(buzzerPins[buzzerIndex], LOW);
  buzzerStates[buzzerIndex] = false;
}

// turns on the specified LED
void turnOnLed(int ledIndex) {
  digitalWriteFast(ledPins[ledIndex], HIGH);
}

// turns off the specified LED
void turnOffLed(int ledIndex) {
  digitalWriteFast(ledPins[ledIndex], LOW);
}
