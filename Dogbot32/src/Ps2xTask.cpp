#include "Ps2xTask.h"
#include "PS2X_lib.h"
#include "Utils.h"

#define PS2_DAT        MOSI  //14    
#define PS2_CMD        MISO  //15
#define PS2_SEL        SS  //16
#define PS2_CLK        SCK  //17

PS2X ps2x; // create PS2 Controller Class

int error = 0;
byte type = 0;
byte vibrate = 0;

void attempt_to_connect() {
  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  TRACE("%s\n", "Attempt to connect");
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
  TRACE("%s\n", "after config gamepad");

  if(error == 0){
    TRACE("%s", "Found Controller, configured successful ");
  }  
  else {
    TRACE("%s\n", "No controller found");
  }
  
  type = ps2x.readType(); 
  switch(type) {
    case 0:
      TRACE("%s", "Unknown Controller type found ");
      type = 0;
      break;
    case 1:
      TRACE("%s", "DualShock Controller found ");
      break;
    case 2:
      TRACE("%s", "GuitarHero Controller found ");
      break;
	case 3:
      TRACE("%s", "Wireless Sony DualShock Controller found ");
      break;
   }
}

Ps2xTask::Ps2xTask() {
    spot = nullptr;
    attempt_to_connect();
}

void Ps2xTask::configure(int interval, SpotFacade *sf) {
    setEnabled(false);
    updateInterval = interval;
    spot = sf;
    attempt_to_connect();
}

void Ps2xTask::performUpdate() {
    if (spot == nullptr) {
        return;
    }
  
    if(type == 2) { 
        ps2x.read_gamepad(); 
        handleGuitarHeroController();
    }
    else { 
        ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
        handleDualShockController();
    }
}

void Ps2xTask::reconnectIfNecessary() {
    if (type < 1) {
        attempt_to_connect();
    }
}

void Ps2xTask::handleDualShockController() {
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      TRACE("%s\n", "Start is being held");
    if(ps2x.Button(PSB_SELECT))
      TRACE("%s\n", "Select is being held");      

    if(ps2x.Button(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
      TRACE("%s", "Up held this hard: ");
      TRACE("%s\n", ps2x.Analog(PSAB_PAD_UP), DEC);
    }
    if(ps2x.Button(PSB_PAD_RIGHT)){
      TRACE("%s", "Right held this hard: ");
      TRACE("%s\n", ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    }
    if(ps2x.Button(PSB_PAD_LEFT)){
      TRACE("%s", "LEFT held this hard: ");
      TRACE("%s\n", ps2x.Analog(PSAB_PAD_LEFT), DEC);
    }
    if(ps2x.Button(PSB_PAD_DOWN)){
      TRACE("%s", "DOWN held this hard: ");
      TRACE("%s\n", ps2x.Analog(PSAB_PAD_DOWN), DEC);
    }   

    vibrate = ps2x.Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
    if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
      if(ps2x.Button(PSB_L3))
        TRACE("%s\n", "L3 pressed");
      if(ps2x.Button(PSB_R3))
        TRACE("%s\n", "R3 pressed");
      if(ps2x.Button(PSB_L2))
        TRACE("%s\n", "L2 pressed");
      if(ps2x.Button(PSB_R2))
        TRACE("%s\n", "R2 pressed");
      if(ps2x.Button(PSB_TRIANGLE))
        TRACE("%s\n", "Triangle pressed");        
    }

    if(ps2x.ButtonPressed(PSB_CIRCLE))               //will be TRUE if button was JUST pressed
      TRACE("%s\n", "Circle just pressed");
    if(ps2x.NewButtonState(PSB_CROSS))               //will be TRUE if button was JUST pressed OR released
      TRACE("%s\n", "X just changed");
    if(ps2x.ButtonReleased(PSB_SQUARE))              //will be TRUE if button was JUST released
      TRACE("%s\n", "Square just released");     

    if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //print stick values if either is TRUE
      TRACE("%s", "Stick Values:");
      TRACE("%s", ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
      TRACE("%s", ",");
      TRACE("%s", ps2x.Analog(PSS_LX), DEC); 
      TRACE("%s", ",");
      TRACE("%s", ps2x.Analog(PSS_RY), DEC); 
      TRACE("%s", ",");
      TRACE("%s\n", ps2x.Analog(PSS_RX), DEC); 
    }     
}

void Ps2xTask::handleGuitarHeroController() {
    if(ps2x.ButtonPressed(GREEN_FRET))
      TRACE("%s\n", "Green Fret Pressed");
    if(ps2x.ButtonPressed(RED_FRET))
      TRACE("%s\n", "Red Fret Pressed");
    if(ps2x.ButtonPressed(YELLOW_FRET))
      TRACE("%s\n", "Yellow Fret Pressed");
    if(ps2x.ButtonPressed(BLUE_FRET))
      TRACE("%s\n", "Blue Fret Pressed");
    if(ps2x.ButtonPressed(ORANGE_FRET))
      TRACE("%s\n", "Orange Fret Pressed"); 

    if(ps2x.ButtonPressed(STAR_POWER))
      TRACE("%s\n", "Star Power Command");
    
    if(ps2x.Button(UP_STRUM))          //will be TRUE as long as button is pressed
      TRACE("%s\n", "Up Strum");
    if(ps2x.Button(DOWN_STRUM))
      TRACE("%s\n", "DOWN Strum");
 
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      TRACE("%s\n", "Start is being held");
    if(ps2x.Button(PSB_SELECT))
      TRACE("%s\n", "Select is being held");
    
    if(ps2x.Button(ORANGE_FRET)) {     // print stick value IF TRUE
      TRACE("%s", "Wammy Bar Position:");
      TRACE("%s\n", ps2x.Analog(WHAMMY_BAR), DEC); 
    }
}