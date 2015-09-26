/*
   KV Team OSD GUI V2.3 May 2014
   http://code.google.com/p/rush-osd-development/
  
   Work released under the Creative Commons Attribution NonCommercial ShareAlike 3.0 Unported License.
   http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
*/


import processing.serial.Serial; // serial library
import controlP5.*; // controlP5 library
import java.io.File;
import java.lang.*; // for efficient String concatemation
import javax.swing.SwingUtilities; // required for swing and EDT
import javax.swing.JFileChooser;// Saving dialogue
import javax.swing.filechooser.FileFilter; // for our configuration file filter "*.mwi"
import javax.swing.JOptionPane; // for message dialogue
import java.io.PrintWriter;
import java.io.InputStream;
import java.io.OutputStream; 
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.util.*;
import java.io.FileNotFoundException;
import java.text.DecimalFormat;

String KV_OSD_GUI_Version = "2.3";
PImage img_Clear,GUIBackground,RadioPot;
  
int DisplayWindowX = 350;
int DisplayWindowY = 340; 
int WindowAdjX = -84; 
int WindowAdjY = -31;
int WindowShrinkX = 8;
int WindowShrinkY = 48;

int currentCol = 0;
int currentRow = 0;  

ControlP5 controlP5;
ControlP5 SmallcontrolP5;
ControlP5 ScontrolP5;
ControlP5 FontGroupcontrolP5;
ControlP5 GroupcontrolP5;
Textlabel txtlblWhichcom; 
ListBox commListbox;

boolean PortRead = false;
boolean PortWrite = false;
ControlGroup messageBox;
Textlabel MessageText;

// Int variables
String OSname = System.getProperty("os.name");
String LoadPercent = "";
//String CallSign = "";

int init_com = 0;
int commListMax = 0;
int whichKey = -1;  // Variable to hold keystoke values
int inByte = -1;    // Incoming serial data
int[] serialInArray = new int[3];    // Where we'll put what we receive


//int serialCount = 0;                 // A count of how many bytes we receive
int ConfigEEPROM = -1;
int ConfigVALUE = -1;

int windowsX    = 797;        int windowsY    = 459;
int xGraph      = 10;         int yGraph      = 325;
int xObj        = 520;        int yObj        = 293;
int xCompass    = 920;        int yCompass    = 341;
int xLevelObj   = 920;        int yLevelObj   = 80;
int xParam      = 120;        int yParam      = 5;
int xRC         = 690;        int yRC         = 10;
int xMot        = 690;        int yMot        = 155;
int xButton     = 845;        int yButton     = 231; 
int xBox        = 415;        int yBox        = 10;
int XSim        = DisplayWindowX+WindowAdjX;        int YSim        = 288-WindowShrinkY + 85;

// Box locations -------------------------------------------------------------------------
int Col1Width = 165;        int Col2Width = 181;      int Col3Width = 154;      int Col4Width = 110;

int XEEPROM     = 120;        int YEEPROM      = 5;  //hidden do not remove
int XBoard      = 120;        int YBoard       = 5;
int XRSSI       = 120;        int YRSSI        = 45;
int XVolts      = 288;        int YVolts       = 78;
int XAmps       = 640;        int YAmps        = 152;
int XVVolts     = 288;        int YVVolts      = 167;
int XTemp       = 305;        int YTemp        = 5;
int XGPS        = 288;        int YGPS         = 5; 
int XCS         = 5;          int YCS          = 242;
int XOther      = 456;        int YOther       = 60;
int XVolume     = 120;        int YVolume      = 182;
int XPortStat   = 5;          int YPortStat    = 290;
int XControlBox = 5;          int YControlBox  = 320;
int XUnitVideo  = 456;        int YUnitVideo   = 5;
int XADC        = 456;        int YADC         = 167;



String FontFileName = "data/KVTOSD_FMap_v1_Small.mcm";

int activeTab = 1;
int YLocation = 0;

static int MwHeading=0;

String[] ConfigHelp = {
  "EEPROM Loaded",
  
  "RSSI Min",
  "RSSI Max",
  "RSSI WARNING",
  "Input", //MW ADC
  "PWM",
  "PWM Divider",
  "Minim Volt WARNING",
  "Battery Cells",
  "Divider Ratio",
  "Input", //MW ADC  
  "Divider Ratio",
  "Input", //Mw ADC
  
  "", // for Board type do not remove
  
  "GPS",
  "Coords",
  "Heading",
  "Unit",   //unit
  "Video",  // signal
  "Stats",
  "OSD ADC",
  "Blink Frequency",
  "Input",
  "Sensitivity",
  "OffSet High",
  "OffSet Low",
  "V Speed WARNING (m/s)",
  "Dist Max (x100m)",
  "Alt Max (x2m)",
  "Alt Min",
  "Minim Volt WARNING",
  "Pitch Angle WARNING °",
  
  "Call Sign",
  
  "S_CS0",
  "S_CS1",
  "S_CS2",
  "S_CS3",
  "S_CS4",
  "S_CS5",
  "S_CS6",
  "S_CS7",
  "S_CS8",
  "S_CS9",
};

String[] ConfigNames = {
   "EEPROM Loaded",
  
  "RSSI Min",
  "RSSI Max",
  "RSSI WARNING",
  "Input", //MW ADC
  "PWM",
  "PWM Divider",
  "Minim Volt WARNING",
  "Battery Cells",
  "Divider Ratio",
  "Input", //MW ADC  
  "Divider Ratio",
  "Input", //Mw ADC
  
  "", // for Board type do not remove
  
  "GPS",
  "Coords",
  "Heading",
  "Unit",   //unit
  "Video",  // signal
  "Stats",
  " ",
  "Blink Frequency",
  "Input",
  "Sensitivity",
  "OffSet High",
  "OffSet Low",
  "V Speed WARNING (m/s)",
  "Dist Max (x100m)",
  "Alt Max (x10m)",
  "Alt Min",
  "Minim Volt WARNING",
  "Pitch Angle WARNING °",
  
  "Call Sign",
  
  "S_CS0",
  "S_CS1",
  "S_CS2",
  "S_CS3",
  "S_CS4",
  "S_CS5",
  "S_CS6",
  "S_CS7",
  "S_CS8",
  "S_CS9",
};


static int CHECKBOXITEMS=0;
int CONFIGITEMS=ConfigNames.length;
static int SIMITEMS=6;
  
int[] ConfigRanges = {
1,   // used for check             0

255, // S_RSSIMIN                   1
255, // S_RSSIMAX                   2
70,  // S_RSSI_ALARM                3
1,   // S_MWRSSI                    4
1,   // S_PWMRSSI                   5
8,   // S_PWMRSSIDIVIDER            6
215, // S_VOLTAGEMIN                7
6,   // S_BATCELLS                  8
110, // S_DIVIDERRATIO              9
1,   // S_MAINVOLTAGE_VBAT          10
110, // S_VIDDIVIDERRATIO           11
1,   // S_VIDVOLTAGE_VBAT           12 
1,   // S_BOARDTYPE                 13
1,   // S_DISPLAYGPS                14
1,   // S_COORDINATES               15
1,   // S_HEADING360                16
1,   // S_UNITSYSTEM                17
1,   // S_VIDEOSIGNALTYPE           18
1,   // S_RESETSTATISTICS           19
1,   // S_ENABLEADC                 20
10,  // S_BLINKINGHZ                21
1,   // S_MWAMPERAGE                22
100,  // S_CURRSENSSENSITIVITY       23
2,   // S_CURRSENSOFFSET_H          24
255, // S_CURRSENSOFFSET_L          25
8,   // S_CLIMB_RATE_ALARM          26
250, // S_VOLUME_DIST_MAX           27
250, // S_VOLUME_ALT_MAX            28
100,  // S_VOLUME_ALT_MIN           29
110, // S_VIDVOLTAGEMIN             30
75,  // S_PITCH_WARNING             31

0,   // S_CALLSIGN                  32


 255,      //Call sign 10 chars 32 to 41
 255,
 255,
 255,
 255,
 255,
 255,
 255,
 255,
 255,

};
/*
int[] ItemLocationPAL = {
  // ROW= Row position on screen (255= no action)
// COL= Column position on screen (255= no action)
// DSPL= Display item on screen
2,   // L_GPS_NUMSATPOSITIONROW LINE02+6
18,  // L_GPS_NUMSATPOSITIONCOL 
1,   // L_GPS_NUMSATPOSITIONDSPL

6,   // L_GPS_DIRECTIONTOHOMEPOSROW LINE03+14
2,   // L_GPS_DIRECTIONTOHOMEPOSCOL
1,   // L_GPS_DIRECTIONTOHOMEPOSDSPL

10,   // L_GPS_DISTANCETOHOMEPOSROW LINE02+24
2,    // L_GPS_DISTANCETOHOMEPOSCOL
1,    // L_GPS_DISTANCETOHOMEPOSDSPL

10,   // L_SPEEDPOSITIONROW LINE03+24
23,   // L_SPEEDPOSITIONCOL
1,    // L_SPEEDPOSITIONDSPL

9,   // L_GPS_ANGLETOHOMEPOSROW LINE04+12
2,   // L_GPS_ANGLETOHOMEPOSCOL
0,   // L_GPS_ANGLETOHOMEPOSDSPL

2,   // L_SENSORPOSITIONROW LINE03+2
24,  // L_SENSORPOSITIONCOL
1,   // L_SENSORPOSITIONDSPL

2,   // L_MODEPOSITIONROW   LINE05+2
8,   // L_MODEPOSITIONCOL
1,   // L_MODEPOSITIONDSPL

3,   // L_MW_HEADINGPOSITIONROW LINE02+19
2,   // L_MW_HEADINGPOSITIONCOL
1,   // L_MW_HEADINGPOSITIONDSPL

2,   // L_MW_HEADINGGRAPHPOSROW LINE02+10
2,   // L_MW_HEADINGGRAPHPOSCOL
1,   // L_MW_HEADINGGRAPHPOSDSPL

6,   // L_MW_ALTITUDEPOSITIONROW LINE08+2
23,  // L_MW_ALTITUDEPOSITIONCOL
1,   // L_MW_ALTITUDEPOSITIONDSPL

8,   // L_CLIMBRATEPOSITIONROW LINE08+24
5,   // L_CLIMBRATEPOSITIONCOL
1,   // L_CLIMBRATEPOSITIONDSPL

6,   // L_HORIZONPOSITIONROW LINE06+8
8,   // L_HORIZONPOSITIONCOL
1,   // L_HORIZONPOSITIONDSPL

255, // L_HORIZONSIDEREFROW
255, // L_HORIZONSIDEREFCOL
1,   // L_HORIZONSIDEREFDSPL

255, // L_HORIZONCENTERREFROW
255, // L_HORIZONCENTERREFCOL
1,   // L_HORIZONCENTERREFDSPL

7,   // L_CURRENTTHROTTLEPOSITIONROW LINE14+22
20,  // L_CURRENTTHROTTLEPOSITIONCOL
1,   // L_CURRENTTHROTTLEPOSITIONDSPL

15,  // L_FLYTIMEPOSITIONROW LINE15+22
14,  // L_FLYTIMEPOSITIONCOL
1,   // L_FLYTIMEPOSITIONDSPL

15,  // L_ONTIMEPOSITIONROW LINE15+22
14,  // L_ONTIMEPOSITIONCOL
1,   // L_ONTIMEPOSITIONDSPL

3,   // L_MOTORARMEDPOSITIONROW LINE14+11
24,  // L_MOTORARMEDPOSITIONCOL
1,   // L_MOTORARMEDPOSITIONDSPL

14,  // L_MW_GPS_LATPOSITIONROW  LINE12+2
2,   // L_MW_GPS_LATPOSITIONCOL
1,   // L_MW_GPS_LATPOSITIONDSPL

15,  // L_MW_GPS_LONPOSITIONROW  LINE12+15
2,   // L_MW_GPS_LONPOSITIONCOL
1,   // L_MW_GPS_LONPOSITIONDSPL

2,   // L_RSSIPOSITIONROW LINE14+2
12,  // L_RSSIPOSITIONCOL
1,   // L_RSSIPOSITIONDSPL

15,  // L_VOLTAGEPOSITIONROW LINE15+3
23,  // L_VOLTAGEPOSITIONCOL
1,   // L_VOLTAGEPOSITIONDSPL

13,  // L_VIDVOLTAGEPOSITIONROW LINE13+3
23,  // L_VIDVOLTAGEPOSITIONCOL
0,   // L_VIDVOLTAGEPOSITIONDSPL

14,  // L_AMPERAGEPOSITIONROW LINE15+10
23,  // L_AMPERAGEPOSITIONCOL
1,   // L_AMPERAGEPOSITIONDSPL

14,  // L_PMETERSUMPOSITIONROW LINE15+16
14,  // L_PMETERSUMPOSITIONCOL
1,   // L_PMETERSUMPOSITIONDSPL

13,  // L_CALLSIGNPOSITIONROW LINE14+10
10,  // L_CALLSIGNPOSITIONCOL
1,   // L_CALLSIGNPOSITIONDSPL
};

int[] ItemLocationNTSC = {
2,   // L_GPS_NUMSATPOSITIONROW LINE02+6
18,  // L_GPS_NUMSATPOSITIONCOL 
1,   // L_GPS_NUMSATPOSITIONDSPL

6,   // L_GPS_DIRECTIONTOHOMEPOSROW LINE03+14
2,   // L_GPS_DIRECTIONTOHOMEPOSCOL
1,   // L_GPS_DIRECTIONTOHOMEPOSDSPL

10,  // L_GPS_DISTANCETOHOMEPOSROW LINE02+24
2,   // L_GPS_DISTANCETOHOMEPOSCOL
1,   // L_GPS_DISTANCETOHOMEPOSDSPL

10,  // L_SPEEDPOSITIONROW LINE03+24
23,  // L_SPEEDPOSITIONCOL 
1,   // L_SPEEDPOSITIONDSPL

9,   // L_GPS_ANGLETOHOMEPOSROW LINE04+12
2,   // L_GPS_ANGLETOHOMEPOSCOL
0,   // L_GPS_ANGLETOHOMEPOSDSPL

2,   // L_SENSORPOSITIONROW LINE03+2
24,  // L_SENSORPOSITIONCOL
1,   // L_SENSORPOSITIONDSPL

2,   // L_MODEPOSITIONROW   LINE05+2
8,   // L_MODEPOSITIONCOL
1,   // L_MODEPOSITIONDSPL

3,   // L_MW_HEADINGPOSITIONROW LINE02+19
2,   // L_MW_HEADINGPOSITIONCOL
1,   // L_MW_HEADINGPOSITIONDSPL

2,   // L_MW_HEADINGGRAPHPOSROW LINE02+10
2,   // L_MW_HEADINGGRAPHPOSCOL
1,   // L_MW_HEADINGGRAPHPOSDSPL

6,   // L_MW_ALTITUDEPOSITIONROW LINE08+2
23,  // L_MW_ALTITUDEPOSITIONCOL
1,   // L_MW_ALTITUDEPOSITIONDSPL

8,   // L_CLIMBRATEPOSITIONROW LINE08+24
5,   // L_CLIMBRATEPOSITIONCOL
1,   // L_CLIMBRATEPOSITIONDSPL

6,   // L_HORIZONPOSITIONROW LINE06+8
8,   // L_HORIZONPOSITIONCOL
1,   // L_HORIZONPOSITIONDSPL

255, // L_HORIZONSIDEREFROW
255, // L_HORIZONSIDEREFCOL
1,   // L_HORIZONSIDEREFDSPL

255, // L_HORIZONCENTERREFROW
255, // L_HORIZONCENTERREFCOL
1,   // L_HORIZONCENTERREFDSPL

7,   // L_CURRENTTHROTTLEPOSITIONROW LINE14+22
20,  // L_CURRENTTHROTTLEPOSITIONCOL
1,   // L_CURRENTTHROTTLEPOSITIONDSPL

13,  // L_FLYTIMEPOSITIONROW LINE15+22
14,  // L_FLYTIMEPOSITIONCOL
1,   // L_FLYTIMEPOSITIONDSPL

13,  // L_ONTIMEPOSITIONROW LINE15+22
14,  // L_ONTIMEPOSITIONCOL
1,   // L_ONTIMEPOSITIONDSPL

3,   // L_MOTORARMEDPOSITIONROW LINE14+11
24,  // L_MOTORARMEDPOSITIONCOL
1,   // L_MOTORARMEDPOSITIONDSPL

12,  // L_MW_GPS_LATPOSITIONROW  LINE12+2
2,   // L_MW_GPS_LATPOSITIONCOL
1,   // L_MW_GPS_LATPOSITIONDSPL

13,  // L_MW_GPS_LONPOSITIONROW  LINE12+15
2,   // L_MW_GPS_LONPOSITIONCOL
1,   // L_MW_GPS_LONPOSITIONDSPL

2,   // L_RSSIPOSITIONROW LINE14+2
12,  // L_RSSIPOSITIONCOL
1,   // L_RSSIPOSITIONDSPL

13,  // L_VOLTAGEPOSITIONROW LINE15+3
23,  // L_VOLTAGEPOSITIONCOL
1,   // L_VOLTAGEPOSITIONDSPL

11,  // L_VIDVOLTAGEPOSITIONROW LINE13+3
23,  // L_VIDVOLTAGEPOSITIONCOL
0,   // L_VIDVOLTAGEPOSITIONDSPL

12,  // L_AMPERAGEPOSITIONROW LINE15+10
23,  // L_AMPERAGEPOSITIONCOL
1,   // L_AMPERAGEPOSITIONDSPL

12,  // L_PMETERSUMPOSITIONROW LINE15+16
14,  // L_PMETERSUMPOSITIONCOL
1,   // L_PMETERSUMPOSITIONDSPL

11,  // L_CALLSIGNPOSITIONROW LINE14+10
10,  // L_CALLSIGNPOSITIONCOL
1,   // L_CALLSIGNPOSITIONDSPL
};
 */ 

PFont font8,font9,font10,font11,font12,font15;

// Colors------------------------------------------------------------------------------------------------------------------
color yellow_ = color(200, 200, 20), green_ = color(30, 100, 30), red_ = color(120, 30, 30), blue_ = color(25, 50, 80),
grey_ = color(30, 30, 30);
// Colors------------------------------------------------------------------------------------------------------------------

// Textlabels -------------------------------------------------------------------------------------------------------------
Textlabel txtlblconfItem[] = new Textlabel[CONFIGITEMS] ;
Textlabel txtlblSimItem[] = new Textlabel[SIMITEMS] ;
Textlabel FileUploadText, TXText, RXText, Links;
// Textlabels -------------------------------------------------------------------------------------------------------------

// Buttons-----------------------------------------------------------------------------------------------------------------
Button buttonIMPORT,buttonSAVE,buttonREAD,buttonRESET,buttonWRITE,buttonRESTART, buttonOSDHome, buttonOSDWiki, buttonMWiiForum, buttonSupport;
// Buttons------------------------------------------------------------------------------------------------------------------

// Toggles------------------------------------------------------------------------------------------------------------------
Toggle toggleConfItem[] = new Toggle[CONFIGITEMS] ;
// Toggles------------------------------------------------------------------------------------------------------------------    

// Checkboxes---------------------------------------------------------------------------------------------------------------
CheckBox checkboxConfItem[] = new CheckBox[CONFIGITEMS] ;
// Checkboxes---------------------------------------------------------------------------------------------------------------

// Toggles------------------------------------------------------------------------------------------------------------------    
RadioButton RadioButtonConfItem[] = new RadioButton[CONFIGITEMS] ;
RadioButton R_PortStat;
// Toggles------------------------------------------------------------------------------------------------------------------    

// Number boxes-------------------------------------------------------------------------------------------------------------
Numberbox confItem[] = new Numberbox[CONFIGITEMS] ;
// Number boxes-------------------------------------------------------------------------------------------------------------

Group MGUploadF,
  G_EEPROM,
  G_RSSI,
  G_Voltage,
  G_Amperage,
  G_VVoltage,
  G_Board,
  G_GPS,
  G_UV,
  G_Other,
  G_ADC,
  G_UnitVideo,
  G_Volume,
  G_CallSign,
  G_PortStatus  
  ;
 
  controlP5.Controller hideLabel(controlP5.Controller c) {
  c.setLabel("");
  c.setLabelVisible(false);
  return c;
}

//*****************************************************************************************************//

void setup() {
  size(windowsX,windowsY);
  GUIBackground = loadImage ("Background.png");
  font8 = createFont("Arial bold",8,false);
  font9 = createFont("Arial bold",10,false);
  font10 = createFont("Arial bold",11,false);
  font11 = createFont("Arial bold",11,false);
  font12 = createFont("Arial bold",12,false);
  font15 = createFont("Arial bold",15,false);

  controlP5 = new ControlP5(this); // initialize the GUI controls
  controlP5.setControlFont(font10);
  SmallcontrolP5 = new ControlP5(this); // initialize the GUI controls
  SmallcontrolP5.setControlFont(font9); 
  ScontrolP5 = new ControlP5(this); // initialize the GUI controls
  ScontrolP5.setControlFont(font10);  
  GroupcontrolP5 = new ControlP5(this); // initialize the GUI controls
  GroupcontrolP5.setControlFont(font10);
  FontGroupcontrolP5 = new ControlP5(this); // initialize the GUI controls
  FontGroupcontrolP5.setControlFont(font10);
  
  SetupGroups();

  commListbox = controlP5.addListBox("portComList",5,100,110,260); // make a listbox and populate it with the available comm ports
  commListbox.setItemHeight(15);
  commListbox.setBarHeight(15);

  commListbox.captionLabel().set("PORT COM");
  commListbox.setColorBackground(red_);
  for(int i=0;i<Serial.list().length;i++) {
    String pn = shortifyPortName(Serial.list()[i], 13);
    if (pn.length() >0 ) commListbox.addItem(pn,i); // addItem(name,value)
    commListMax = i;
  }
  commListbox.addItem("Close Comm",++commListMax); // addItem(name,value)
  
  // text label for which comm port selected
  txtlblWhichcom   = controlP5.addTextlabel("txtlblWhichcom","No Port Selected",5,65); // textlabel(name,text,x,y)
  // Links Label
  Links            = controlP5.addTextlabel("LinkInf"," Handy Buttons >") .setPosition(XVolume, YVolume+74);    
  
  buttonSAVE       = controlP5.addButton("bSAVE",1,15,45,40,19); buttonSAVE.setLabel("SAVE"); buttonSAVE.setColorBackground(red_);
  buttonIMPORT     = controlP5.addButton("bIMPORT",1,65,45,40,19); buttonIMPORT.setLabel("LOAD"); buttonIMPORT.setColorBackground(red_);  
  buttonREAD       = controlP5.addButton("READ",1,XControlBox+34,YControlBox+25,37,15);buttonREAD.setColorBackground(red_);
  buttonRESET      = controlP5.addButton("RESET",1,XControlBox+32,YControlBox+50,40,15);buttonRESET.setColorBackground(red_);
  buttonWRITE      = controlP5.addButton("WRITE",1,XControlBox+32,YControlBox+75,42,15);buttonWRITE.setColorBackground(red_);
  buttonRESTART    = controlP5.addButton("RESTART",1,XControlBox+25,YControlBox+100,55,16);buttonRESTART.setColorBackground(red_); 
  buttonOSDHome    = controlP5.addButton("OSDHome", 1, XVolume+100, YVolume+74, 80, 15).setColorBackground(blue_).setLabel("      Home");//.setColorLabel(yellow_);
  buttonOSDWiki    = controlP5.addButton("OSDWiki", 1, XVolume+190, YVolume+74, 80, 15).setColorBackground(blue_).setLabel("        Wiki");//.setColorLabel(yellow_);
  buttonMWiiForum  = controlP5.addButton("MWiiForum",1, XVolume+280, YVolume+74, 80, 15).setColorBackground(blue_).setLabel(" MWii Forum");//.setColorLabel(yellow_);
  buttonSupport    = controlP5.addButton("Support",1, XVolume+370, YVolume+74, 80, 15).setColorBackground(blue_).setLabel("    Support");//.setColorLabel(yellow_);
  

// EEPROM---------------------------------------------------------------------------

CreateItem(GetSetting("S_CHECK_"), 5, 0, G_EEPROM);

// RSSI  ---------------------------------------------------------------------------
CreateItem(GetSetting("S_MWRSSI"),  5,0*17, G_RSSI);
BuildRadioButton(GetSetting("S_MWRSSI"),  5,0*17, G_RSSI, "ADC","MWii");
CreateItem(GetSetting("S_PWMRSSI"),  5,1*17, G_RSSI);
BuildRadioButton(GetSetting("S_PWMRSSI"),  5,1*17, G_RSSI, "Off","On");
CreateItem(GetSetting("S_PWMRSSIDIVIDER"),  5,2*17, G_RSSI);
CreateItem(GetSetting("S_RSSIMIN"), 5,3*17, G_RSSI);
CreateItem(GetSetting("S_RSSIMAX"), 5,4*17, G_RSSI);
RSSIWarning(GetSetting("S_RSSI_ALARM"), 5,5*17, G_RSSI);

// Voltage  ------------------------------------------------------------------------
CreateItem(GetSetting("S_MAINVOLTAGE_VBAT"), 5,0*17, G_Voltage);
BuildRadioButton(GetSetting("S_MAINVOLTAGE_VBAT"),  5,0*17, G_Voltage, "ADC","MWii");
BattWarningItem(GetSetting("S_VOLTAGEMIN"), 5,1*17, G_Voltage);
BattCellItem(GetSetting("S_BATCELLS"), 5,2*17, G_Voltage);
CreateItem(GetSetting("S_DIVIDERRATIO"), 5,3*17, G_Voltage);

// Video Voltage  -------------------------------------------------------------------
CreateItem(GetSetting("S_VIDVOLTAGE_VBAT"),  5,0*17, G_VVoltage);
BuildRadioButton(GetSetting("S_VIDVOLTAGE_VBAT"),  5,0, G_VVoltage, "ADC","MWii");
BattWarningItem(GetSetting("S_VIDVOLTAGEMIN"), 5,1*17, G_VVoltage);
CreateItem(GetSetting("S_VIDDIVIDERRATIO"),  5,2*17, G_VVoltage);

//  Board ---------------------------------------------------------------------------
CreateItem(GetSetting("S_BOARDTYPE"),  5,0, G_Board);
BuildRadioButton(GetSetting("S_BOARDTYPE"),  5,0, G_Board, "Rush","Minim");

//  GPS  ----------------------------------------------------------------------------
CreateItem(GetSetting("S_DISPLAYGPS"), 5,0, G_GPS);
BuildRadioButton(GetSetting("S_DISPLAYGPS"),  5,0, G_GPS, "Off","On");
CreateItem(GetSetting("S_COORDINATES"),  5,1*17, G_GPS);
BuildRadioButton(GetSetting("S_COORDINATES"),  5,1*17, G_GPS, "Off","On");
CreateItem(GetSetting("S_HEADING360"),  5,2*17, G_GPS);
BuildRadioButton(GetSetting("S_HEADING360"),  5,2*17, G_GPS, "180°","360°");

//  Unit & Video  ------------------------------------------------------------------
CreateItem(GetSetting("S_UNITSYSTEM"),  5,0, G_UnitVideo);
BuildRadioButton(GetSetting("S_UNITSYSTEM"),  5,0, G_UnitVideo, "Metric","Imperial");
CreateItem(GetSetting("S_VIDEOSIGNALTYPE"),  5,1*17, G_UnitVideo);
BuildRadioButton(GetSetting("S_VIDEOSIGNALTYPE"),  5,1*17, G_UnitVideo, "NTSC","PAL");

//  Other  -------------------------------------------------------------------------
CreateItem(GetSetting("S_RESETSTATISTICS"),  5,0*17, G_Other);
BuildRadioButton(GetSetting("S_RESETSTATISTICS"),  5,0*17, G_Other, "Maintain","Reset");
BlinkFreqItem(GetSetting("S_BLINKINGHZ"),  5,2*17, G_Other);
DescendWarningItem(GetSetting("S_CLIMB_RATE_ALARM"),  5,3*17, G_Other);
PitchWarningItem(GetSetting("S_PITCH_WARNING"),  5,4*17, G_Other);

// OSD ADC´s   --------------------------------------------------------------------------
CreateItem(GetSetting("S_ENABLEADC"),  53,1*17, G_ADC);
BuildRadioButton(GetSetting("S_ENABLEADC"),  53,1*17, G_ADC, "Off","On");

// Amperage  ------------------------------------------------------------------------
CreateItem(GetSetting("S_MWAMPERAGE"),  5,0, G_Amperage);
BuildRadioButton(GetSetting("S_MWAMPERAGE"),  5,0, G_Amperage, "ADC","MWii");
CreateItem(GetSetting("S_CURRSENSSENSITIVITY"),  5,1*17, G_Amperage);
CreateItem(GetSetting("S_CURRSENSOFFSET_H"),  5,2*17, G_Amperage);
CreateItem(GetSetting("S_CURRSENSOFFSET_L"),  5,3*17, G_Amperage);

// Volume Flight  -------------------------------------------------------------------
VolumeFlightDist(GetSetting("S_VOLUME_DIST_MAX"),  5,0*17, G_Volume);
VolumeFlightAltMax(GetSetting("S_VOLUME_ALT_MAX"),  5,1*17, G_Volume);
VolumeFlightAltMin(GetSetting("S_VOLUME_ALT_MIN"),  5,2*17, G_Volume);

// Call Sign ------------------------------------------------------------------------
CreateItem(GetSetting("S_CALLSIGN"),9,0,G_CallSign);
BuildRadioButton(GetSetting("S_CALLSIGN"),10,2*17,G_CallSign, "","");
controlP5.addTextfield("CallSign")
     .setPosition(15,0*16)
     .setSize(78,13)
     .setFont(font10)
     .setAutoClear(false)
     .setGroup(G_CallSign)
     ;
 controlP5.addTextlabel("","")
 .setGroup(G_CallSign);
 CreateCS(GetSetting("S_CS0"),  0,0, G_CallSign);
 CreateCS(GetSetting("S_CS1"),  0,0, G_CallSign);
 CreateCS(GetSetting("S_CS2"),  0,0, G_CallSign);
 CreateCS(GetSetting("S_CS3"),  0,0, G_CallSign);
 CreateCS(GetSetting("S_CS4"),  0,0, G_CallSign);
 CreateCS(GetSetting("S_CS5"),  0,0, G_CallSign);
 CreateCS(GetSetting("S_CS6"),  0,0, G_CallSign);
 CreateCS(GetSetting("S_CS7"),  0,0, G_CallSign);
 CreateCS(GetSetting("S_CS8"),  0,0, G_CallSign);
 CreateCS(GetSetting("S_CS9"),  0,0, G_CallSign);

//******************************************* Config Items ********************************************//

  for(int i=0;i<CONFIGITEMS;i++) {
    if (ConfigRanges[i] == 0) {
      toggleConfItem[i].hide();
      confItem[i].hide();
    }
    if (ConfigRanges[i] > 1) {
      try{
      toggleConfItem[i].hide();
      }catch(Exception e) {
      }finally {
   }  	
 }
      
    if (ConfigRanges[i] == 1){
      confItem[i].hide();  
    }
  }
  for (int txTimes = 0; txTimes<255; txTimes++) {
    inBuf[txTimes] = 0;
  }

  Font_Editor_setup();
   SimSetup();
  img_Clear = LoadFont(FontFileName);
  LoadConfig();
  
}

controlP5.Controller hideCheckbox(controlP5.Controller c) {
  c.hide();
  c.setLabelVisible(false);
  return c;
}

controlP5.Controller CheckboxVisable(controlP5.Controller c) {
  c.isVisible(); 

  c.setLabelVisible(false);
  return c;
}

void BuildRadioButton(int ItemIndex, int XLoction, int YLocation,Group inGroup, String Cap1, String Cap2){   
     RadioButtonConfItem[ItemIndex] = controlP5.addRadioButton("RadioButton"+ItemIndex)
         .setPosition(XLoction,YLocation+3)
         .setSize(10,10)
         .setNoneSelectedAllowed(false) 
         .setItemsPerRow(2)
         .setSpacingColumn(int(textWidth(Cap1))+10)
         .addItem("First"+ItemIndex,0)
         .addItem("Second"+ItemIndex,1)
         .toUpperCase(false)
         //.hideLabels() 
         ;
     RadioButtonConfItem[ItemIndex].setGroup(inGroup);
     RadioButtonConfItem[ItemIndex].getItem(0).setCaptionLabel(Cap1);
     RadioButtonConfItem[ItemIndex].getItem(1).setCaptionLabel(Cap2 + "    " + ConfigNames[ItemIndex]);
    
     toggleConfItem[ItemIndex].hide();
     txtlblconfItem[ItemIndex].hide();    
}

void CreateCS(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setMin(0);
     confItem[ItemIndex].setMax(255);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
     confItem[ItemIndex].hide();
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].hide();
}

void CreateItem(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setColorBackground(blue_);
     confItem[ItemIndex].setMin(0);
     confItem[ItemIndex].setDirection(Controller.HORIZONTAL);
     confItem[ItemIndex].setMax(ConfigRanges[ItemIndex]);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
  //Toggle
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].setPosition(XLoction,YLocation+3);
     toggleConfItem[ItemIndex].setSize(35,10);
     toggleConfItem[ItemIndex].setMode(ControlP5.SWITCH);
     toggleConfItem[ItemIndex].setGroup(inGroup);
  //TextLabel
     txtlblconfItem[ItemIndex] = controlP5.addTextlabel("txtlblconfItem"+ItemIndex,ConfigNames[ItemIndex],XLoction+40,YLocation);
     txtlblconfItem[ItemIndex].setGroup(inGroup);
     controlP5.getTooltip().register("txtlblconfItem"+ItemIndex,ConfigHelp[ItemIndex]);
}

void BlinkFreqItem(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setColorBackground(blue_);
     confItem[ItemIndex].setMin(1);
     confItem[ItemIndex].setDirection(Controller.HORIZONTAL);
     confItem[ItemIndex].setMax(ConfigRanges[ItemIndex]);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
  //Toggle
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].setPosition(XLoction,YLocation+3);
     toggleConfItem[ItemIndex].setSize(35,10);
     toggleConfItem[ItemIndex].setMode(ControlP5.SWITCH);
     toggleConfItem[ItemIndex].setGroup(inGroup);
  //TextLabel
     txtlblconfItem[ItemIndex] = controlP5.addTextlabel("txtlblconfItem"+ItemIndex,ConfigNames[ItemIndex],XLoction+40,YLocation);
     txtlblconfItem[ItemIndex].setGroup(inGroup);
     controlP5.getTooltip().register("txtlblconfItem"+ItemIndex,ConfigHelp[ItemIndex]);
}

void PitchWarningItem(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setColorBackground(red_);
     confItem[ItemIndex].setMin(15);
     confItem[ItemIndex].setDirection(Controller.HORIZONTAL);
     confItem[ItemIndex].setMax(ConfigRanges[ItemIndex]);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
  //Toggle
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].setPosition(XLoction,YLocation+3);
     toggleConfItem[ItemIndex].setSize(35,10);
     toggleConfItem[ItemIndex].setMode(ControlP5.SWITCH);
     toggleConfItem[ItemIndex].setGroup(inGroup);
  //TextLabel
     txtlblconfItem[ItemIndex] = controlP5.addTextlabel("txtlblconfItem"+ItemIndex,ConfigNames[ItemIndex],XLoction+40,YLocation);
     txtlblconfItem[ItemIndex].setGroup(inGroup);
     controlP5.getTooltip().register("txtlblconfItem"+ItemIndex,ConfigHelp[ItemIndex]);
} 


void BattCellItem(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setColorBackground(blue_);
     confItem[ItemIndex].setMin(2);
     confItem[ItemIndex].setDirection(Controller.HORIZONTAL);
     confItem[ItemIndex].setMax(ConfigRanges[ItemIndex]);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
  //Toggle
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].setPosition(XLoction,YLocation+3);
     toggleConfItem[ItemIndex].setSize(35,10);
     toggleConfItem[ItemIndex].setMode(ControlP5.SWITCH);
     toggleConfItem[ItemIndex].setGroup(inGroup);
  //TextLabel
     txtlblconfItem[ItemIndex] = controlP5.addTextlabel("txtlblconfItem"+ItemIndex,ConfigNames[ItemIndex],XLoction+40,YLocation);
     txtlblconfItem[ItemIndex].setGroup(inGroup);
     controlP5.getTooltip().register("txtlblconfItem"+ItemIndex,ConfigHelp[ItemIndex]);
}

void BattWarningItem(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setColorBackground(red_);
     confItem[ItemIndex].setMin(7);
     confItem[ItemIndex].setDirection(Controller.HORIZONTAL);
     confItem[ItemIndex].setMax(ConfigRanges[ItemIndex]);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
  //Toggle
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].setPosition(XLoction,YLocation+3);
     toggleConfItem[ItemIndex].setSize(35,10);
     toggleConfItem[ItemIndex].setMode(ControlP5.SWITCH);
     toggleConfItem[ItemIndex].setGroup(inGroup);
  //TextLabel
     txtlblconfItem[ItemIndex] = controlP5.addTextlabel("txtlblconfItem"+ItemIndex,ConfigNames[ItemIndex],XLoction+40,YLocation);
     txtlblconfItem[ItemIndex].setGroup(inGroup);
     controlP5.getTooltip().register("txtlblconfItem"+ItemIndex,ConfigHelp[ItemIndex]);
}

void RSSIWarning(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setColorBackground(red_);
     confItem[ItemIndex].setMin(10);
     confItem[ItemIndex].setDirection(Controller.HORIZONTAL);
     confItem[ItemIndex].setMax(ConfigRanges[ItemIndex]);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
  //Toggle
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].setPosition(XLoction,YLocation+3);
     toggleConfItem[ItemIndex].setSize(35,10);
     toggleConfItem[ItemIndex].setMode(ControlP5.SWITCH);
     toggleConfItem[ItemIndex].setGroup(inGroup);
  //TextLabel
     txtlblconfItem[ItemIndex] = controlP5.addTextlabel("txtlblconfItem"+ItemIndex,ConfigNames[ItemIndex],XLoction+40,YLocation);
     txtlblconfItem[ItemIndex].setGroup(inGroup);
     controlP5.getTooltip().register("txtlblconfItem"+ItemIndex,ConfigHelp[ItemIndex]);
}

void DescendWarningItem(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setColorBackground(red_);
     confItem[ItemIndex].setMin(0);
     confItem[ItemIndex].setDirection(Controller.HORIZONTAL);
     confItem[ItemIndex].setMax(ConfigRanges[ItemIndex]);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
  //Toggle
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].setPosition(XLoction,YLocation+3);
     toggleConfItem[ItemIndex].setSize(35,10);
     toggleConfItem[ItemIndex].setMode(ControlP5.SWITCH);
     toggleConfItem[ItemIndex].setGroup(inGroup);
  //TextLabel
     txtlblconfItem[ItemIndex] = controlP5.addTextlabel("txtlblconfItem"+ItemIndex,ConfigNames[ItemIndex],XLoction+40,YLocation);
     txtlblconfItem[ItemIndex].setGroup(inGroup);
     controlP5.getTooltip().register("txtlblconfItem"+ItemIndex,ConfigHelp[ItemIndex]);
}

void VolumeFlightDist(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setColorBackground(red_);
     confItem[ItemIndex].setMin(0);
     confItem[ItemIndex].setDirection(Controller.HORIZONTAL);
     confItem[ItemIndex].setMax(ConfigRanges[ItemIndex]);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
  //Toggle
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].setPosition(XLoction,YLocation+3);
     toggleConfItem[ItemIndex].setSize(35,10);
     toggleConfItem[ItemIndex].setMode(ControlP5.SWITCH);
     toggleConfItem[ItemIndex].setGroup(inGroup);
  //TextLabel
     txtlblconfItem[ItemIndex] = controlP5.addTextlabel("txtlblconfItem"+ItemIndex,ConfigNames[ItemIndex],XLoction+40,YLocation);
     txtlblconfItem[ItemIndex].setGroup(inGroup);
     controlP5.getTooltip().register("txtlblconfItem"+ItemIndex,ConfigHelp[ItemIndex]);
}

void VolumeFlightAltMax(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setColorBackground(red_);
     confItem[ItemIndex].setMin(0);
     confItem[ItemIndex].setDirection(Controller.HORIZONTAL);
     confItem[ItemIndex].setMax(ConfigRanges[ItemIndex]);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
  //Toggle
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].setPosition(XLoction,YLocation+3);
     toggleConfItem[ItemIndex].setSize(35,10);
     toggleConfItem[ItemIndex].setMode(ControlP5.SWITCH);
     toggleConfItem[ItemIndex].setGroup(inGroup);
  //TextLabel
     txtlblconfItem[ItemIndex] = controlP5.addTextlabel("txtlblconfItem"+ItemIndex,ConfigNames[ItemIndex],XLoction+40,YLocation);
     txtlblconfItem[ItemIndex].setGroup(inGroup);
     controlP5.getTooltip().register("txtlblconfItem"+ItemIndex,ConfigHelp[ItemIndex]);
}

void VolumeFlightAltMin(int ItemIndex, int XLoction, int YLocation, Group inGroup){
  //numberbox
     confItem[ItemIndex] = (controlP5.Numberbox) hideLabel(controlP5.addNumberbox("configItem"+ItemIndex,0,XLoction,YLocation,35,14));
     confItem[ItemIndex].setColorBackground(red_);
     confItem[ItemIndex].setMin(0);
     confItem[ItemIndex].setDirection(Controller.HORIZONTAL);
     confItem[ItemIndex].setMax(ConfigRanges[ItemIndex]);
     confItem[ItemIndex].setDecimalPrecision(0);
     confItem[ItemIndex].setGroup(inGroup);
  //Toggle
     toggleConfItem[ItemIndex] = (controlP5.Toggle) hideLabel(controlP5.addToggle("toggleValue"+ItemIndex));
     toggleConfItem[ItemIndex].setPosition(XLoction,YLocation+3);
     toggleConfItem[ItemIndex].setSize(35,10);
     toggleConfItem[ItemIndex].setMode(ControlP5.SWITCH);
     toggleConfItem[ItemIndex].setGroup(inGroup);
  //TextLabel
     txtlblconfItem[ItemIndex] = controlP5.addTextlabel("txtlblconfItem"+ItemIndex,ConfigNames[ItemIndex],XLoction+40,YLocation);
     txtlblconfItem[ItemIndex].setGroup(inGroup);
     controlP5.getTooltip().register("txtlblconfItem"+ItemIndex,ConfigHelp[ItemIndex]);
}

void MakePorts(){
  
  if (PortWrite){  
       TXText.setColorValue(color(255,10,0));
  }
  else
  {
    TXText.setColorValue(color(100,10,0));
  }
  if (PortRead){  
    RXText.setColorValue(color(0,240,0));
  }
   else
  {
    RXText.setColorValue(color(0,100,0));
  }
}

void draw() {
  time=millis();
  if ((init_com==1)  && (toggleMSP_Data == true)) {
    //time2 = time;
    PortRead = true;
    MakePorts();
    MWData_Com();
    if (!FontMode) PortRead = false;
    
  }
  if ((SendSim ==1) && (ClosePort == false)){
    if ((init_com==1)  && (time-time5 >5000) && (toggleMSP_Data == false) && (!FontMode)){
      if(ClosePort) return;
      time5 = time;
       
      if (init_com==1){
        SendCommand(MSP_BOXIDS);
      }
    }
    if ((init_com==1)  && (time-time4 >200) && (!FontMode)){
      if(ClosePort) return;
      time4 = time; 

      if (init_com==1)SendCommand(MSP_ANALOG);
      if (init_com==1)SendCommand(MSP_STATUS);
      if (init_com==1)SendCommand(MSP_RC);
      if (init_com==1)SendCommand(MSP_ALTITUDE);
      if (init_com==1)SendCommand(MSP_RAW_GPS);
      if (init_com==1)SendCommand(MSP_COMP_GPS); 
    }
      if ((init_com==1)  && (time-time1 >40) && (!FontMode)){
      if(ClosePort) return;
      time1 = time; 
      PortWrite = !PortWrite;
      
      if (init_com==1)SendCommand(MSP_ATTITUDE);
    }
  }
  else
  {
      if (!FontMode) PortWrite = false; 
  }
      if ((FontMode) && (time-time2 >100)){
    SendChar();
   }   
    
  MakePorts();   

  background(80);
  
  // ----------------------------------------------------------------------------------------------------------------------
  // Draw background control boxes
  // ----------------------------------------------------------------------------------------------------------------------

  // Coltrol Box
  fill(80); strokeWeight(2);stroke(100); rectMode(CORNERS); rect(XControlBox,YControlBox, XControlBox+108, YControlBox+134); 
  textFont(font12); fill(255, 255, 255); text("OSD Controls",XControlBox + 15,YControlBox + 15); 
 // if (activeTab == 1) {}
  fill(60, 40, 40);
  strokeWeight(3);stroke(0);
  rectMode(CORNERS);
  rect(5,5,113,40);
  textFont(font12);
  // version
  fill(255, 255, 255);
  text("  KV Team OSD",10,19);
  text(" GUI Version",10,35);
  text(KV_OSD_GUI_Version, 88, 35);
  fill(0, 0, 0);
  strokeWeight(3);stroke(0);
  rectMode(CORNERS);
  image(GUIBackground,DisplayWindowX+WindowAdjX, DisplayWindowY+WindowAdjY);
  MatchConfigs();
  MakePorts();

  if ((ClosePort ==true)&& (PortWrite == false)){ 
    ClosePort();
  }
}

int GetSetting(String test){
  int TheSetting = 0;
  for (int i=0; i<Settings.values().length; i++) 
  if (Settings.valueOf(test) == Settings.values()[i]){ 
      TheSetting = Settings.values()[i].ordinal();
  }
  return TheSetting;
}

public void BuildCallSign(){
  String CallSText = "";
  for (int i=0; i<10; i++){ 
    if (int(confItem[GetSetting("S_CS0")+i].getValue())>0){
    CallSText+=char(int(confItem[GetSetting("S_CS0")+i].getValue()));
    }
  }
  controlP5.get(Textfield.class,"CallSign").setText(CallSText);
}  

public void CheckCallSign() {
  // automatically receives results from controller input
  String CallSText = controlP5.get(Textfield.class,"CallSign").getText().toUpperCase();
  controlP5.get(Textfield.class,"CallSign").setText(CallSText);
    if (CallSText.length()  >10){
      controlP5.get(Textfield.class,"CallSign").setText(CallSText.substring(0, 10));
      CallSText = controlP5.get(Textfield.class,"CallSign").getText();
    } 
    for (int i=0; i<10; i++){ 
    confItem[GetSetting("S_CS0")+i].setValue(0);
    }
    for (int i=0; i<CallSText.length(); i++){ 
      confItem[(GetSetting("S_CS0"))+i].setValue(int(CallSText.charAt(i)));
    }
}

void MatchConfigs()

{
 for(int i=0;i<CONFIGITEMS;i++) {
   try{ 
       if (RadioButtonConfItem[i].isVisible()){
          confItem[i].setValue(int(RadioButtonConfItem[i].getValue()));
       }
        }catch(Exception e) {}finally {}
    
  
     if  (toggleConfItem[i].isVisible()){
       if (int(toggleConfItem[i].getValue())== 1){
       confItem[i].setValue(1);
     }
     else{ 
       confItem[i].setValue(0);
      }
    }
  }
}

//***************************************************************//

// controls comport list click
public void controlEvent(ControlEvent theEvent) {
  
  try{
  if (theEvent.isGroup())
    if (theEvent.name()=="portComList")
      InitSerial(theEvent.group().value()); // initialize the serial port selected
  }catch(Exception e){
    System.out.println("error with Port");
  }
  
  
if (theEvent.name()=="CallSign"){
  CheckCallSign();
}
      
  try{
    if ((theEvent.getController().getName().substring(0, 7).equals("CharPix")) && (theEvent.getController().isMousePressed())) {
        int ColorCheck = int(theEvent.getController().value());
        curPixel = theEvent.controller().id();
    }
    if ((theEvent.getController().getName().substring(0, 7).equals("CharMap")) && (theEvent.getController().isMousePressed())) {
      curChar = theEvent.controller().id();    
    }
   } catch(ClassCastException e){}
     catch(StringIndexOutOfBoundsException se){}
      
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////// BEGIN FILE OPS//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//save the content of the model to a file
public void bSAVE() {
  updateModel();
  SwingUtilities.invokeLater(new Runnable(){
    public void run() {
     final JFileChooser fc = new JFileChooser(dataPath("")) {

        private static final long serialVersionUID = 7919427933588163126L;

        public void approveSelection() {
            File f = getSelectedFile();
            if (f.exists() && getDialogType() == SAVE_DIALOG) {
                int result = JOptionPane.showConfirmDialog(this,
                        "The file exists, overwrite?", "Existing file",
                        JOptionPane.YES_NO_CANCEL_OPTION);
                switch (result) {
                case JOptionPane.YES_OPTION:
                    super.approveSelection();
                    return;
                case JOptionPane.CANCEL_OPTION:
                    cancelSelection();
                    return;
                default:
                    return;
                }
            }
        super.approveSelection();
        }
    };

      fc.setDialogType(JFileChooser.SAVE_DIALOG);
      fc.setFileFilter(new MwiFileFilter());
      int returnVal = fc.showSaveDialog(null);
      if (returnVal == JFileChooser.APPROVE_OPTION) {
        File file = fc.getSelectedFile();
        String filePath = file.getPath();
        if(!filePath.toLowerCase().endsWith(".osd")){
          file = new File(filePath + ".osd");
        }

        
        FileOutputStream out =null;
        String error = null;
        try{
          out = new FileOutputStream(file) ;
          MWI.conf.storeToXML(out, "KVTOSD Configuration File  " + new  Date().toString());
          JOptionPane.showMessageDialog(null,new StringBuffer().append("configuration saved : ").append(file.toURI()) );
           }catch(FileNotFoundException e){
         
          error = e.getCause().toString();
           }catch( IOException ioe){
                /*failed to write the file*/
                ioe.printStackTrace();
                error = ioe.getCause().toString();
           }finally{
             
          if (out!=null){
            try{
              out.close();
            }catch( IOException ioe){/*failed to close the file*/error = ioe.getCause().toString();}
          }
          if (error !=null){
                  JOptionPane.showMessageDialog(null, new StringBuffer().append("error : ").append(error) );
           }
         }
       }
     }
   }
 );
}

public void updateModel(){
  for(int j=0;j<ConfigNames.length;j++) {
         MWI.setProperty(ConfigNames[j],String.valueOf(confItem[j].value()));
  }
}

public void updateView(){
  for(int j=0; j<ConfigNames.length; j++) {
    
    if(j >= CONFIGITEMS)
    return;
  int value = int(MWI.getProperty(ConfigNames[j]));
  confItem[j].setValue(value);
  if (j == CONFIGITEMS-1){
  }  
  if (value >0){
    toggleConfItem[j].setValue(1);
    }
    else {
    toggleConfItem[j].setValue(0);
  }

  try{
    switch(value) {
    case(0):
      RadioButtonConfItem[j].activate(0);
      break;
    case(1):
      RadioButtonConfItem[j].activate(1);
      break;
    }
  }
  catch(Exception e) {}finally {}
  }
  
  BuildCallSign(); 
}

public class MwiFileFilter extends FileFilter {
  public boolean accept(File f) {
    if(f != null) {
      if(f.isDirectory()) {
        return true;
      }
      String extension = getExtension(f);
      if("osd".equals(extension)) {
        return true;
      }
    }
    return false;
 }
  
  public String getExtension(File f) {
    if(f != null) {
      String filename = f.getName();
      int i = filename.lastIndexOf('.');
      if(i>0 && i<filename.length()-1) {
        return filename.substring(i+1).toLowerCase();
      }
    }
    return null;
  } 

  public String getDescription() {
    return "*.osd KVT_OSD configuration file";
  }   
}

//*****************************************************************************************************//

// import the content of a file into the model
public void bIMPORT(){
  SwingUtilities.invokeLater(new Runnable(){
    public void run(){
      final JFileChooser fc = new JFileChooser(dataPath(""));
      fc.setDialogType(JFileChooser.SAVE_DIALOG);
      fc.setFileFilter(new MwiFileFilter());
      int returnVal = fc.showOpenDialog(null);
      if (returnVal == JFileChooser.APPROVE_OPTION) {
        File file = fc.getSelectedFile();
        FileInputStream in = null;
        boolean completed = false;
        String error = null;
        try{
          in = new FileInputStream(file) ;
          MWI.conf.loadFromXML(in); 
          JOptionPane.showMessageDialog(null,new StringBuffer().append("configuration loaded : ").append(file.toURI()) );
          completed  = true;
          
        }catch(FileNotFoundException e){
                error = e.getCause().toString();

        }catch( IOException ioe){/*failed to read the file*/
                ioe.printStackTrace();
                error = ioe.getCause().toString();
        }finally{
          if (!completed){
                 // MWI.conf.clear();
                 // or we can set the properties with view values, sort of 'nothing happens'
                 updateModel();
          }
          updateView();
          if (in!=null){
            try{
              in.close();
            }catch( IOException ioe){/*failed to close the file*/}
          }
          
          if (error !=null){
                  JOptionPane.showMessageDialog(null, new StringBuffer().append("error : ").append(error) );
          }
        }
      }
    }
  }
 );
}

//*****************************************************************************************************//
//  our model 
static class MWI {
  private static Properties conf = new Properties();
  public static void setProperty(String key ,String value ){
    conf.setProperty( key,value );
  }
  public static String getProperty(String key ){
    return conf.getProperty( key,"0");
  }
  public static void clear( ){
    conf= null; // help gc
    conf = new Properties();
  }
}

public void updateConfig(){
  String error = null;
  FileOutputStream out =null;
  ConfigClass.setProperty("StartFontFile",FontFileName);
  File file = new File(dataPath("GUI.Config"));
  try{
    out = new FileOutputStream(file) ;
    ConfigClass.conf.storeToXML(out, "KV_Team_OSD GUI Configuration File  " + new  Date().toString());
   }catch(FileNotFoundException e){
      error = e.getCause().toString();
   }catch( IOException ioe){
        /*failed to write the file*/
        ioe.printStackTrace();
        error = ioe.getCause().toString();
      }finally{
        if (out!=null){
          try{
            out.close();
            }catch( IOException ioe){/*failed to close the file*/error = ioe.getCause().toString();}
            }
            if (error !=null){
            JOptionPane.showMessageDialog(null, new StringBuffer().append("error : ").append(error) );
    }
  }
}

public void LoadConfig(){
  String error = null;
  FileInputStream in =null;  
  try{
    in = new FileInputStream(dataPath("GUI.Config"));
  }catch(FileNotFoundException e){
    System.out.println("Configuration Failed- Creating Default");
    updateConfig();
    }catch( IOException ioe){
      /*failed to write the file*/
      ioe.printStackTrace();
      error = ioe.getCause().toString();
    }//finally{
      if (in!=null){
        try{
          ConfigClass.conf.loadFromXML(in); 
          FontFileName = ConfigClass.getProperty("StartFontFile");
          img_Clear = LoadFont(FontFileName);
          System.out.println("Configuration Successful");
          in.close();
          }catch( IOException ioe){/*failed to close the file*/error = ioe.getCause().toString();}
          }
          if (error !=null){
          JOptionPane.showMessageDialog(null, new StringBuffer().append("error : ").append(error) );
  }
}

//***************************************************************//
//  our configuration 
static class ConfigClass {
  private static Properties conf = new Properties();
  public static void setProperty(String key ,String value ){
    conf.setProperty( key,value );
  }
  public static String getProperty(String key ){
    return conf.getProperty( key,"0");
  }
  public static void clear( ){
    conf= null; // help gc
    conf = new Properties();
  }
}

void mouseReleased() {
  mouseDown = false;
  mouseUp = true;
  if (curPixel>-1)changePixel(curPixel);
  if (curChar>-1)GetChar(curChar);
  ControlLock();
  
} 

public void mousePressed() {
                mouseDown = false;
                mouseUp = true;
        }

        public boolean mouseDown() {
                return mouseDown;
        }

        public boolean mouseUp() {
                return mouseUp;
        
        }
        
  // Web Links
   public void OSDHome()   { link("http://code.google.com/p/rush-osd-development/");}
   public void OSDWiki()   { link("http://code.google.com/p/rush-osd-development/w/");}
   public void MWiiForum() { link("http://www.multiwii.com/forum/viewtopic.php?f=8&t=2918");}
   public void Support ()  { link("https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=Q5VJ43Q2U2J6C");}
   

