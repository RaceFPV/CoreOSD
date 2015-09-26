float sAltitude = 0;
float sVario = 0;
float sVBat = 0;
float sMRSSI = 0;
int mode_armed = 0;
int mode_stable = 0;
int mode_baro = 0;
int mode_mag = 0;
int mode_gpshome = 0;
int mode_gpshold = 0;
int mode_llights = 0;
int mode_osd_switch = 0;

int SendSim = 0;

boolean[] keys = new boolean[526];
boolean armed = false;

Group SG,SGControlBox,SGModes,SGAtitude,SGRadio,SGSensors1,SGGPS; 

// Checkboxs
CheckBox checkboxSimItem[] = new CheckBox[SIMITEMS] ;
CheckBox UnlockControls, SGPS_FIX;
// Toggles
Toggle toggleModeItems[] = new Toggle[boxids.length] ;
Toggle SimControlToggle;
// Slider2d ---
Slider2D Pitch_Roll, Throttle_Yaw,MW_Pitch_Roll;
// Sliders ---
Slider s_Altitude,s_Vario,s_VBat,s_MRSSI;
// Label
Textlabel txtlblModeItems[] = new Textlabel[boxids.length] ;
Textlabel SimControlText;

// Knobs----
Knob HeadingKnob,SGPSHeadHome;

Numberbox SGPS_numSat, SGPS_altitude, SGPS_speed, SGPS_ground_course,SGPS_distanceToHome,SGPS_directionToHome,SGPS_update;
DecimalFormat OnePlaceDecimal = new DecimalFormat("0.0");

//**************************************************************//

 
void SimSetup(){


  SG = ScontrolP5.addGroup("SG")
    .setPosition(120,YSim + -71)
    //.setTab ("Simulate") // Test OK
    .setWidth(674)
    .setBarHeight(14)
    .activateEvent(true)
    .setValue(0)
    .close()
    //.hide()
    .setBackgroundColor(color(0,255))
    .setColorLabel(color(yellow_))
    .setBackgroundHeight(204)
   .setLabel("                                                                 Multiwii flight control data simulator ")
   .setMoveable(false)
    ;
                
 
  SGModes = ScontrolP5.addGroup("SGModes")
                .setPosition(570,20)
                .setWidth(100)
                .setBarHeight(15)
                .activateEvent(true)
                .disableCollapse()
                .setBackgroundColor(color(30,255))
                .setBackgroundHeight((boxids.length*17) + 9)
                .setLabel("  Flight Modes")
                .setColorLabel(color(0, 300, 320))
                .setGroup(SG)
                .disableCollapse()
                //.close() 
               ; 
               
  SGAtitude = ScontrolP5.addGroup("SGAtitude")
                .setPosition(186,124)
                .setWidth(200)
                .setBarHeight(15)
                .activateEvent(true)
                .disableCollapse()
                .setBackgroundColor(color(30,255))
                .setBackgroundHeight(75)
                .setLabel("                       Attitude")
                .setColorLabel(color(0, 300, 320))
                .setGroup(SG)
                //.close() 
               ;
               
 SGRadio = ScontrolP5.addGroup("SGRadio")
                .setPosition(391,20)
                .setWidth(174)
                .setBarHeight(15)
                .activateEvent(true)
                .disableCollapse()
                .setBackgroundColor(color(30,255))
                .setBackgroundHeight(72)
                .setLabel("  RC Radio Control")
                .setColorLabel(color(0, 300, 320))
                .setGroup(SG)
                //.close() 
               ; 

SGSensors1 = ScontrolP5.addGroup("SGSensors1")
                .setPosition(5,20)
                .setWidth(175)
                .setBarHeight(15)
                .activateEvent(true)
                .disableCollapse()
                .setBackgroundColor(color(30,255))
                .setBackgroundHeight(110)
                .setLabel("      MWii Sensors & ADC´s ")
                .setColorLabel(color(0, 300, 320))
                .setGroup(SG)
                //.close() 
               ;                                  
SGGPS = ScontrolP5.addGroup("SGGPS")
                .setPosition(186,20)
                .setWidth(200)
                .setBarHeight(15)
                .activateEvent(true)
                .disableCollapse()
                .setBackgroundColor(color(30,255))
                .setBackgroundHeight(85)
                .setLabel("               Multiwii GPS Data")
                .setColorLabel(color(0, 300, 320))
                .setGroup(SG)
                //.close() 
               ;

SGControlBox = ScontrolP5.addGroup("SGControlBox")
                .setPosition(5,150)
                .setWidth(175)
                .setBarHeight(15)
                .activateEvent(true)
                .disableCollapse()
                .setBackgroundColor(color(30,255))
                .setBackgroundHeight(49)
                .setLabel("       Simulator Control")
                .setColorLabel(color(0, 300, 320))
                .setGroup(SG)
                //.close() 
               ;   

SimControlToggle = (controlP5.Toggle) hideLabel(controlP5.addToggle("SendSim"));
SimControlToggle.setPosition(60,5);
SimControlToggle.setSize(40,10);
SimControlToggle.setMode(ControlP5.SWITCH);
SimControlToggle.setColorActive(color(0, 160, 100));
SimControlToggle.setColorBackground(color(80,0,0));
SimControlToggle.setGroup(SGControlBox);
SimControlText = controlP5.addTextlabel("SimControlText","On  Off",56,15);
SimControlText.setGroup(SGControlBox);
               
               
SGPS_FIX =  ScontrolP5.addCheckBox("GPS_FIX",5,5);
    SGPS_FIX.setColorBackground(color(80));
    SGPS_FIX.setColorActive(color(0, 160, 100));
    SGPS_FIX.addItem("GPS Fix",1);
    SGPS_FIX.setGroup(SGGPS);
    
SGPS_numSat = ScontrolP5.addNumberbox("SGPS_numSat",0,5,20,40,14);
    SGPS_numSat.setLabel("Sats");
   // SGPS_numSat.setColorBackground(red_);
    SGPS_numSat.setMin(0);
    SGPS_numSat.setDirection(Controller.HORIZONTAL);
    SGPS_numSat.setMax(15);
    SGPS_numSat.setDecimalPrecision(0);
    SGPS_numSat.setGroup(SGGPS); 
 ScontrolP5.getController("SGPS_numSat").getCaptionLabel()
   .align(ControlP5.LEFT, ControlP5.RIGHT_OUTSIDE).setPaddingX(45);

SGPS_altitude = ScontrolP5.addNumberbox("SGPS_altitude",0,5,40,40,14);
    SGPS_altitude.setLabel("Alt-cm");
    SGPS_altitude.setMin(0);
    SGPS_altitude.setDirection(Controller.HORIZONTAL);
    SGPS_altitude.setMax(10000);
    SGPS_altitude.setDecimalPrecision(0);
    SGPS_altitude.setGroup(SGGPS); 
 ScontrolP5.getController("SGPS_altitude").getCaptionLabel()
   .align(ControlP5.LEFT, ControlP5.RIGHT_OUTSIDE).setPaddingX(45); 
   SGPS_altitude.hide();   
 
 SGPS_speed = ScontrolP5.addNumberbox("SGPS_speed",0,5,40,40,14);
    SGPS_speed.setLabel("Speed-cm/s");
    //SGPS_speed.setColorBackground(blue_);
    SGPS_speed.setMin(0);
    SGPS_speed.setDirection(Controller.HORIZONTAL);
    SGPS_speed.setMax(10000);
    SGPS_speed.setDecimalPrecision(0);
    SGPS_speed.setGroup(SGGPS); 
 ScontrolP5.getController("SGPS_speed").getCaptionLabel()
   .align(ControlP5.LEFT, ControlP5.RIGHT_OUTSIDE).setPaddingX(45);   
 
 SGPS_distanceToHome = ScontrolP5.addNumberbox("SGPS_distanceToHome",0,5,60,40,14);
    SGPS_distanceToHome.setLabel("Dist Home-M");
    //SGPS_numSat.setColorBackground(red_);
    SGPS_distanceToHome.setMin(0);
    SGPS_distanceToHome.setDirection(Controller.HORIZONTAL);
    SGPS_distanceToHome.setMax(1000);
    SGPS_distanceToHome.setDecimalPrecision(0);
    SGPS_distanceToHome.setGroup(SGGPS); 
 ScontrolP5.getController("SGPS_distanceToHome").getCaptionLabel()
   .align(ControlP5.LEFT, ControlP5.RIGHT_OUTSIDE).setPaddingX(45);   
                 
  SGPSHeadHome = ScontrolP5.addKnob("SGPSHeadHome")
   .setRange(-180,+180)
   .setValue(0)
   .setPosition(140,5)
   .setRadius(25)
   .setLabel("Home")
   .setColorBackground(color(0, 160, 100))
   .setColorActive(color(255,255,0))
   .setDragDirection(Knob.HORIZONTAL)
   .setGroup(SGGPS);
  ScontrolP5.getTooltip().register("SGPSHeadHome","Home direction Simulation")
   ;

                 
               
               
 MW_Pitch_Roll = ScontrolP5.addSlider2D("MWPitch/Roll")
    .setPosition(25,5)
    .setSize(50,50)
    .setArrayValue(new float[] {50, 50})
    .setMaxX(20) 
    .setMaxY(-20) 
    .setMinX(-20) 
    .setMinY(20)
    .setValueLabel("") 
    .setLabel("Roll/Pitch")
    .setGroup(SGAtitude)
    ;
 ScontrolP5.getController("MWPitch/Roll").getCaptionLabel()
   .align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0);   
 ScontrolP5.getController("MWPitch/Roll").getValueLabel().hide();
 ScontrolP5.getTooltip().register("MWPitch/Roll","Attitude / Horizon Simulator")
 ;
 
 HeadingKnob = ScontrolP5.addKnob("MwHeading")
   .setRange(-180,+180)
   .setValue(0)
   .setPosition(140,5)
   .setRadius(25)
   .setLabel("Heading")
   .setColorBackground(color(0, 160, 100))
   .setColorActive(color(255,255,0))
   .setDragDirection(Knob.HORIZONTAL)
   .setGroup(SGAtitude);
  ScontrolP5.getTooltip().register("MwHeading","Heading Simulation")
   ;
   
 Throttle_Yaw = ScontrolP5.addSlider2D("Throttle/Yaw")
         .setPosition(15,9)
         .setSize(50,50)
         .setArrayValue(new float[] {50, 100})
         .setMaxX(2000) 
         .setMaxY(1000) 
         .setMinX(1000) 
         .setMinY(2000)
         .setValueLabel("") 
        .setLabel("")
         .setGroup(SGRadio)
        ;
 ScontrolP5.getController("Throttle/Yaw").getValueLabel().hide();
 ScontrolP5.getTooltip().register("Throttle/Yaw","Ctrl Key to hold position");

UnlockControls =  ScontrolP5.addCheckBox("UnlockControls",83,25);
    UnlockControls.setColorBackground(color(80));
    UnlockControls.setColorActive(color(255,255,0));
    UnlockControls.addItem("UnlockControls1",1);
    UnlockControls.hideLabels();
    UnlockControls.setGroup(SGRadio);

  Pitch_Roll = ScontrolP5.addSlider2D("Pitch/Roll")
         .setPosition(110,9)
         .setSize(50,50)
         .setArrayValue(new float[] {50, 50})
         .setMaxX(2000) 
         .setMaxY(1000) 
         .setMinX(1000) 
         .setMinY(2000)
         .setLabel("")
         .setGroup(SGRadio)
         ;
  ScontrolP5.getController("Pitch/Roll").getValueLabel().hide();
               
s_Altitude = ScontrolP5.addSlider("sAltitude")
  .setPosition(5,10)
  .setSize(8,75)
  .setRange(0,1000)
  .setValue(0)
  .setLabel("Alt")
  .setDecimalPrecision(0)
  .setGroup(SGSensors1);
  ScontrolP5.getController("sAltitude").getValueLabel()
    .setFont(font9);
  ScontrolP5.getController("sAltitude").getCaptionLabel()
    .setFont(font9)
    .align(ControlP5.LEFT, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0);
  ScontrolP5.getTooltip().register("sAltitude","Barometric Altitude Simulation");

s_Vario = ScontrolP5.addSlider("sVario")
  .setPosition(47,10)
  .setSize(8,75)
  .setRange(-40,40)
  .setNumberOfTickMarks(41)
  .showTickMarks(false)
  .setValue(0)
  .setLabel("Vario")
  .setDecimalPrecision(1)
  .setGroup(SGSensors1);
  ScontrolP5.getController("sVario").getValueLabel()
    .setFont(font9);
  ScontrolP5.getController("sVario").getCaptionLabel()
    .setFont(font9)
    .align(ControlP5.LEFT, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0);
  ScontrolP5.getTooltip().register("sVario","Vertical Speed simulation");

s_VBat = ScontrolP5.addSlider("sVBat")
  .setPosition(90,10)
  .setSize(8,75)
  .setRange(7,25.2)
  .setValue(12.6)
  .setLabel("VBat")
  .setDecimalPrecision(1)
  .setGroup(SGSensors1);
  ScontrolP5.getController("sVBat").getValueLabel()
    .setFont(font9);
  ScontrolP5.getController("sVBat")
    .getCaptionLabel()
    .setFont(font9)
    .align(ControlP5.LEFT, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0); 
  ScontrolP5.getTooltip().register("sVBat","MWii VBat Simulation");    

s_MRSSI = ScontrolP5.addSlider("sMRSSI")
  .setPosition(130,10)
  .setSize(8,75)
  .setRange(0,1023)
  .setValue(1023)
  .setLabel("RSSI")
  .setDecimalPrecision(0)
  .setGroup(SGSensors1);
  ScontrolP5.getController("sMRSSI").getValueLabel()
     .setFont(font9);
  ScontrolP5.getController("sMRSSI")
    .getCaptionLabel()
    .setFont(font9)
    .align(ControlP5.LEFT, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0);
  ScontrolP5.getTooltip().register("sMRSSI","RSSI via MWii Simulation");
    
  for(int i=0;i<boxids.length ;i++) {
    toggleModeItems[i] = (controlP5.Toggle) hideLabel(ScontrolP5.addToggle("toggleModeItems"+i,false));
    toggleModeItems[i].setPosition(5,3+i*17);
    toggleModeItems[i].setSize(10,10);
    toggleModeItems[i].setGroup(SGModes);
    txtlblModeItems[i] = controlP5.addTextlabel("ModeItems"+i,boxids[i].substring(0, boxids[i].length()-1) ,20,i*17);
    txtlblModeItems[i].setGroup(SGModes);
  } 
} 

boolean checkKey(int k)
{
  if (keys.length >= k) {
    return keys[k];  
  }
  return false;
}

void keyPressed()
{ 
  keys[keyCode] = true;
  //println(keyCode);
  if((checkKey(CONTROL) == true) && (checkKey(85) == true)){
  //SketchUploader();
  }
}

void keyReleased()
{ 
  keys[keyCode] = false; 
  ControlLock();
  
}

String RightPadd(int inInt,int Places){
  String OutString = nf(inInt,Places).replaceFirst("^0+(?!$)",  " ");
  for(int X=0; X<=3; X++) {
    if (OutString.length() < Places){ OutString = " " + OutString;}
  }
  return OutString;  
  
}


void ControlLock(){
  Pitch_Roll.setArrayValue(new float[] {500, -500});
  if(checkKey(CONTROL) == false) {
    if(UnlockControls.arrayValue()[0] < 1){
      float A = (2000-Throttle_Yaw.getArrayValue()[1])*-1;
      Throttle_Yaw.setArrayValue(new float[] {500, A});
      s_Vario.setValue(0);
      sVario = 0;
    }
  }    
}


