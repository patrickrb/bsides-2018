/*
   ____ _                _ _            _           ____   ___                          
  / ___| |__   __ _ _ __| (_) ___ _ __ | | _____  _|___ \ / _ \ ___ _ __   _____      __
 | |   | '_ \ / _` | '__| | |/ _ \ '_ \| |/ _ \ \/ / __) | | | / __| '_ \ / _ \ \ /\ / /
 | |___| | | | (_| | |  | | |  __/ |_) | |  __/>  < / __/| |_| \__ \ | | | (_) \ V  V / 
  \____|_| |_|\__,_|_|  |_|_|\___| .__/|_|\___/_/\_\_____|\___/|___/_| |_|\___/ \_/\_/  
                                 |_|                                                    
 
 Charlieplexing 20 LEDs using 5 ATTiny85 pins with fading by
 varying the duty cycle of each LED in the 'tail'.
 
 ATTiny85 connections
 Leg  Function
 1    Reset, no connection
 2    D3 GREEN - Vertex 5 (was 0)
 3    D4 ORANGE - Vertex 2 (was 1)
 4    GND
 5    D0 WHITE - Vertex 3 (was 2)
 6    D1 BLUE - Vertex 4 (was 3)
 7    D2 YELLOW - Vertex 1 (was 4)
 8    +5V
 
 Tested on ATTiny85 running at 8MHz.
 */
// each block of 4 LEDs in the array is groupled by a common anode (+, long leg)
// for simplicity of wiring on breadboard, using a colour code
#define GREEN 4
#define ORANGE 1
#define WHITE 2
#define BLUE 3
#define YELLOW 0
// pin definitions {GREEN, ORANGE, WHITE, BLUE, YELLOW}
const int charliePin[5] = {
  3, 4, 0, 1, 2};
/*
 * Grid format:
 * 
 *  0  4  8 12 16
 *  1  5  9 13 17
 *  2  6 10 14 18
 *  3  7 11 15 19
 */
const int grid[20] = {
  1,0,0,0,1,
  1,1,0,0,0,
  1,1,1,0,0,
  1,1,1,1,0,
};
int disp[20];
//static unsigned char PROGMEM flappybird_frame_1[] = { 0x03, 0xF0, 0x0C, 0x48, 0x10, 0x84, 0x78, 0x8A, 0x84, 0x8A, 0x82, 0x42, 0x82, 0x3E, 0x44, 0x41,0x38, 0xBE, 0x20, 0x41, 0x18, 0x3E, 0x07, 0xC0 };
// Charlieplexed LED definitions (current flowing from-to pairs)
const int LED[20][2] = {
  {
    ORANGE, GREEN      }
  ,                            // 0 (GREEN GROUP)*
  {
    GREEN, ORANGE      }
  ,                            // 4 (ORANGE GROUP)
  {
    GREEN, WHITE      }
  ,                             // 8 (WHITE GROUP)
  {
    GREEN, BLUE      }
  ,                              // 12 (BLUE GROUP)
  {
    GREEN, YELLOW      }
  ,                            // 16 (YELLOW GROUP)
  {
    WHITE, GREEN      }
  ,                             // 1*
  {
    WHITE, ORANGE      }
  ,                            // 5
  {
    ORANGE, WHITE      }
  ,                            // 9
  {
    ORANGE, BLUE      }
  ,                             // 13
  {
    ORANGE, YELLOW      }
  ,                           // 17
    {
    BLUE, GREEN      }
  ,                              // 2
  {
    BLUE, ORANGE      }
  ,                             // 6
  {
    BLUE, WHITE      }
  ,                              // 10
  {
    WHITE, BLUE      }
  ,                              // 14*
  {
    WHITE, YELLOW      }
  ,                            // 18
  {
    YELLOW, GREEN      }
  ,                            // 3
  {
    YELLOW, ORANGE      }
  ,                           // 7*
  {
    YELLOW, WHITE      }
  ,                            // 11
  {
    YELLOW, BLUE      }
  ,                             // 15
  {
    BLUE, YELLOW      }                              // 19
};


// other
int current = 0;                              // LED in array with current focus
int previous = 0;                             // previous LED that was lit
bool ontime = false;
unsigned long toggleTime;
const int stepsize = 5;
int offset = 0;
//int dispidx = 0;
void setup() {
  randomSeed(analogRead(0));
  //Fill the display buffer.
  int dispidx=0;
  for(dispidx=0; dispidx<20; dispidx++){
    disp[dispidx] = grid[dispidx];
  }
}
void loop() {
  int randomNumber = random(2);
  if (randomNumber == 1) {
    fallingAnimation();
  }
  else {
    snowAnimation();
  }
}

void snowAnimation() {
  while( 1 == 1) {
    unsigned long loopCount = 0;                          // used to determine duty cycle of each LED
    unsigned long timeNow = millis();                     //
    unsigned long displayTime = 250; //10 + random(90);          // milliseconds to spend at each focus LED in descent
    while(millis()- timeNow < (displayTime+current*2)) {  // animation slows toward end
      loopCount++;
      // the "snowflake" gets full duty cycle.  When it gets to the end, hold it at the end until the tail collapses
      if (current > 19) charlieON(19); 
      else charlieON(current);
      // each member of tail has reduced duty cycle, and never get to the final position
      if(!(loopCount % 3)) if(current-1 >=0 && current-1 < 19) charlieON(current-1);
      if(!(loopCount % 6)) if(current-2 >=0 && current-2 < 19) charlieON(current-2);
      if(!(loopCount % 9)) if(current-3 >=0 && current-3 < 19) charlieON(current-3);
      if(!(loopCount % 12)) if(current-4 >=0 && current-4 < 19) charlieON(current-4);
    }
      if(ontime) {
      if(timeNow > (toggleTime + displayTime)) {
        // Has been on for delay time.  Switch off.
        //ontime=false;
        // Step thru (scroll) the array - move everything "down" 5 steps.
        offset+=stepsize;
        //offset = 5;
        if(offset >= 20){
          offset = 0;
        }
        for(int dispidx=19; dispidx>=0; dispidx--){
          if((dispidx-offset) >= 0){
            disp[dispidx] = grid[dispidx-offset];
          }
          else {
            disp[dispidx] = 0;
          }
        }
       
        //scrollDisp(disp);
        
        toggleTime=timeNow;
      }
    }
    else {
      if(timeNow > (toggleTime + displayTime)) {
        //ontime=true;
        toggleTime=timeNow;
      }
    }
  ontime=1;
    // Cycle thru each LED
    if(ontime){
      if(current > 0) {
        charlieOFF(current - 1);
      }
      if(disp[current]){
        charlieON(current);
      }
    }
    current++;
    if(current > 19) {
      charlieOFF(19);
      current = 0;
    }
    //delay(100);
  }
}

void fallingAnimation() {
  while( 1 == 1) {
    unsigned long loopCount = 0;                          // used to determine duty cycle of each LED
    unsigned long timeNow = millis();                     //
    unsigned long displayTime = 250; //10 + random(90);          // milliseconds to spend at each focus LED in descent
    /*while(millis()- timeNow < (displayTime+current*2)) {  // animation slows toward end
      loopCount++;
      // the "snowflake" gets full duty cycle.  When it gets to the end, hold it at the end until the tail collapses
      if (current > 19) charlieON(19); 
      else charlieON(current);
      // each member of tail has reduced duty cycle, and never get to the final position
      if(!(loopCount % 3)) if(current-1 >=0 && current-1 < 19) charlieON(current-1);
      if(!(loopCount % 6)) if(current-2 >=0 && current-2 < 19) charlieON(current-2);
      if(!(loopCount % 9)) if(current-3 >=0 && current-3 < 19) charlieON(current-3);
      if(!(loopCount % 12)) if(current-4 >=0 && current-4 < 19) charlieON(current-4);
    }*/
    if(ontime) {
      if(timeNow > (toggleTime + displayTime)) {
        // Has been on for delay time.  Switch off.
        //ontime=false;
        // Step thru (scroll) the array - move everything "down" 5 steps.
        offset+=stepsize;
        //offset = 5;
        if(offset >= 20){
          offset = 0;
        }
        for(int dispidx=19; dispidx>=0; dispidx--){
          if((dispidx-offset) >= 0){
            disp[dispidx] = grid[dispidx-offset];
          }
          else {
            disp[dispidx] = 0;
          }
        }
       
        //scrollDisp(disp);
        
        toggleTime=timeNow;
      }
    }
    else {
      if(timeNow > (toggleTime + displayTime)) {
        //ontime=true;
        toggleTime=timeNow;
      }
    }
  ontime=1;
    // Cycle thru each LED
    if(ontime){
      if(current > 0) {
        charlieOFF(current - 1);
      }
      if(disp[current]){
        charlieON(current);
      }
    }
    current++;
    if(current > 19) {
      charlieOFF(19);
      current = 0;
    }
    //delay(100);
  }
}
void scrollDisp(int curDisp){
  // Take curDisp, shift it all 5 points up.
  int scratchDisp[5];
}
// --------------------------------------------------------------------------------
// turns on LED #thisLED.  Turns off all LEDs if the value passed is out of range
//
void charlieON(int thisLED) {
  // turn off previous (reduces overhead, only switch 2 pins rather than 5)
  digitalWrite(charliePin[LED[previous][1]], LOW);   // ensure internal pull-ups aren't engaged on INPUT mode
  pinMode(charliePin[LED[previous][0]], INPUT);
  pinMode(charliePin[LED[previous][1]], INPUT);
  // turn on the one that's in focus
  if(thisLED >= 0 && thisLED <= 19) {
    pinMode(charliePin[LED[thisLED][0]], OUTPUT);
    pinMode(charliePin[LED[thisLED][1]], OUTPUT);
    digitalWrite(charliePin[LED[thisLED][0]], LOW);
    digitalWrite(charliePin[LED[thisLED][1]], HIGH);
  }
  previous = thisLED;
}
// --------------------------------------------------------------------------------
// turns off LED #thisLED.  
//
void charlieOFF(int thisLED) {
  digitalWrite(charliePin[LED[thisLED][1]], LOW);   // ensure internal pull-ups aren't engaged on INPUT mode
  pinMode(charliePin[LED[thisLED][0]], INPUT);
  pinMode(charliePin[LED[thisLED][1]], INPUT);
}

