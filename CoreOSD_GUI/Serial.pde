
public boolean toggleRead = false,
        toggleMSP_Data = true,
        toggleReset = false,
        toggleCalibAcc = false,
        toggleCalibMag = false,
        toggleWrite = false,
        toggleSpekBind = false,
        toggleSetSetting = false;
Serial g_serial;      // The serial port
float LastPort = 0;
int time,time1,time2,time3,time4,time5;

boolean ClosePort = false;
boolean PortIsWriting = false;
boolean FontMode = false;
int FontCounter = 0;
int CloseMode = 0;

/******************************* Multiwii Serial Protocol **********************/

private static final String MSP_HEADER = "$M<";

private static final int
  MSP_IDENT                =100,
  MSP_STATUS               =101,
  MSP_RAW_IMU              =102,
  MSP_SERVO                =103,
  MSP_MOTOR                =104,
  MSP_RC                   =105,
  MSP_RAW_GPS              =106,
  MSP_COMP_GPS             =107,
  MSP_ATTITUDE             =108,
  MSP_ALTITUDE             =109,
  MSP_ANALOG               =110,
  MSP_RC_TUNING            =111,
  MSP_PID                  =112,
  MSP_BOX                  =113,
  MSP_MISC                 =114,
  MSP_MOTOR_PINS           =115,
  MSP_BOXNAMES             =116,
  MSP_PIDNAMES             =117,
  MSP_BOXIDS               =119,
  MSP_RSSI                 =120,
  MSP_SET_RAW_RC           =200,
  MSP_SET_RAW_GPS          =201,
  MSP_SET_PID              =202,
  MSP_SET_BOX              =203,
  MSP_SET_RC_TUNING        =204,
  MSP_ACC_CALIBRATION      =205,
  MSP_MAG_CALIBRATION      =206,
  MSP_SET_MISC             =207,
  MSP_RESET_CONF           =208,
  MSP_SET_WP               =209,
  MSP_SELECT_SETTING       =210,
  MSP_SET_HEAD             =211,
  
  MSP_SPEK_BIND            =240,

  MSP_EEPROM_WRITE         =250,
  
  MSP_DEBUGMSG             =253,
  MSP_DEBUG                =254;

private static final int
  MSP_OSD                  =220;

// Subcommands
private static final int
  OSD_NULL                 =0,
  OSD_READ_CMD             =1,
  OSD_WRITE_CMD            =2,
  OSD_GET_FONT             =3,
  OSD_SERIAL_SPEED         =4,
  OSD_RESET                =5;


// initialize the serial port selected in the listBox
void InitSerial(float portValue) {
  if (portValue < commListMax) {
    if(init_com == 0){ 
      try{
      String portPos = Serial.list()[int(portValue)];
      txtlblWhichcom.setValue("COM = " + shortifyPortName(portPos, 8));
      g_serial = new Serial(this, portPos, 115200);
      LastPort = portValue;
      init_com=1;
      toggleMSP_Data = true;
      ClosePort = false;
      buttonREAD.setColorBackground(green_);
      buttonRESET.setColorBackground(green_);
      commListbox.setColorBackground(green_);
      buttonRESTART.setColorBackground(green_);
      
      
      g_serial.buffer(256);
      System.out.println("Port Turned On " );
      FileUploadText.setText("");
      delay(1500);
      SendCommand(MSP_IDENT);
     
      SendCommand(MSP_STATUS);
      READ();
       } catch (Exception e) { // null pointer or serial port dead
         noLoop();
        JOptionPane.showConfirmDialog(null,"Error Opening Port It may be in Use", "Port Error", JOptionPane.PLAIN_MESSAGE,JOptionPane.WARNING_MESSAGE);
        loop();
        System.out.println("OpenPort error " + e);
     }
   }
 }
  else {
    if(init_com == 1){
     System.out.println("Begin Port Down " ); 
      txtlblWhichcom.setValue("Comm Closed");
      toggleMSP_Data = false;
      ClosePort = true;
      init_com=0;
    }
  }
  
}

void ClosePort(){
  init_com=0;
  g_serial.clear();
  g_serial.stop();
  init_com=0;
  commListbox.setColorBackground(red_);
  buttonREAD.setColorBackground(red_);
  buttonRESET.setColorBackground(red_);
  buttonWRITE.setColorBackground(red_);
  buttonRESTART.setColorBackground(red_);
  if (CloseMode > 0){
    InitSerial(LastPort);
    CloseMode = 0;
  }
    
  
}

void SetConfigItem(int index, int value) {
  if(index >= CONFIGITEMS)
    return;

  confItem[index].setValue(value);
  if (index == CONFIGITEMS-1)
    buttonWRITE.setColorBackground(green_);
    
  if (value >0){
    toggleConfItem[index].setValue(1);
  }
  else{
    toggleConfItem[index].setValue(0);
  }

  try{
    switch(value) {
    case(0):
      RadioButtonConfItem[index].activate(0);
      break;
    case(1):
      RadioButtonConfItem[index].activate(1);
      break;
    }
  }
  catch(Exception e) {
  }finally {
  }
BuildCallSign();  	
}

void PORTCLOSE(){
  toggleMSP_Data = false;
  CloseMode = 0;
  InitSerial(200.00);
}

void BounceSerial(){
  toggleMSP_Data = false;
  CloseMode = 1;
  InitSerial(200.00);
  
}  


void RESTART(){
  toggleMSP_Data = true;
  for (int txTimes = 0; txTimes<2; txTimes++) {
    headSerialReply(MSP_OSD, 1);
    serialize8(OSD_RESET);
    tailSerialReply();
  }
  toggleMSP_Data = false;
  CloseMode = 1;
  ClosePort();
  delay(1500);
  READ();
} 



public void READ(){
  
  for(int i = 0; i < CONFIGITEMS; i++){
    SetConfigItem((byte)i, 0);
  }
  PortRead = true; 
  MakePorts();
   for (int txTimes = 0; txTimes<2; txTimes++) {
     toggleMSP_Data = true;
     headSerialReply(MSP_OSD, 1);
     serialize8(OSD_READ_CMD);
     tailSerialReply();
   }
}

public void WRITE(){
  CheckCallSign();
  PortWrite = true;
  MakePorts();
  toggleMSP_Data = true;
  p = 0;
  inBuf[0] = OSD_WRITE_CMD;
  for (int txTimes = 0; txTimes<2; txTimes++) {
    headSerialReply(MSP_OSD, CONFIGITEMS+1);
    serialize8(OSD_WRITE_CMD);
    for(int i = 0; i < CONFIGITEMS; i++){
      serialize8(int(confItem[i].value()));
    }
    tailSerialReply();
  }
  
  toggleMSP_Data = false;
  g_serial.clear();
  PortWrite = false;
}

public void FONT_UPLOAD(){
  if (init_com==0){
    noLoop();
    JOptionPane.showConfirmDialog(null,"Please Select a Port", "Not Connected", JOptionPane.PLAIN_MESSAGE,JOptionPane.WARNING_MESSAGE);
    loop();
  }else
  {
  SimControlToggle.setValue(0);
  System.out.println("FONT_UPLOAD");
  //toggleMSP_Data = true;
  FontMode = true;
  PortWrite = true;
  MakePorts();
  FontCounter = 0;
  FileUploadText.setText("Please Wait...");
  p = 0;
  inBuf[0] = OSD_GET_FONT;
    headSerialReply(MSP_OSD, 5);
    serialize8(OSD_GET_FONT);
    serialize16(7456);  // safety code
    serialize8(0);    // first char
    serialize8(255);  // last char
    tailSerialReply();
 //}
  }

}

public void SendChar(){
 time2=time;
    PortWrite = !PortWrite;  // toggle PortWrite to flash TX
    MakePorts();
    System.out.println("Sent Char "+FontCounter);
    buttonSendFile.getCaptionLabel().setText("  " +nf(FontCounter, 3)+"/256");
    headSerialReply(MSP_OSD, 56);
    serialize8(OSD_GET_FONT);
    for(int i = 0; i < 54; i++){
      serialize8(int(raw_font[FontCounter][i]));
    }
    serialize8(FontCounter);
    tailSerialReply();
    if (FontCounter <255){
      FontCounter++;
    }else{ 
      g_serial.clear();
      PortWrite = false;
      FontMode = false;      
      System.out.println("Finished Uploading Font");
      buttonSendFile.getCaptionLabel().setText("  Upload");
      FileUploadText.setText("");
      RESTART();
      READ();
    } 
  
}


public void RESET(){
 if (init_com==1){
    noLoop();
    int Reset_result = JOptionPane.showConfirmDialog(this,"Are you sure you wish to RESET?", "RESET OSD MEMORY",JOptionPane.WARNING_MESSAGE,JOptionPane.YES_NO_CANCEL_OPTION);
    loop();
    switch (Reset_result) {
      case JOptionPane.YES_OPTION:
        toggleConfItem[0].setValue(0);
        confItem[0].setValue(0);
        WRITE();
        RESTART();
        return;
      case JOptionPane.CANCEL_OPTION:
        return;
      default:
        return;
    }
 }else
 {
   noLoop();
   JOptionPane.showConfirmDialog(null,"Please Select a Port", "Not Connected", JOptionPane.PLAIN_MESSAGE,JOptionPane.WARNING_MESSAGE);
   loop();
 }
}

String boxids[] = {
    "ARM;",
    "ANGLE;",
    "HORIZON;",
    "BARO;",
    "MAG;",
    "GPS HOME;",
    "GPS HOLD;",
    "OSD SW;"
  };
String strBoxIds = join(boxids,"");  

void SendCommand(int cmd){
  switch(cmd) {
  
  case MSP_STATUS:
        PortIsWriting = true;
        Send_timer+=1;
        headSerialReply(MSP_STATUS, 11);
        serialize16(Send_timer);
        serialize16(0);
        serialize16(1|1<<1|1<<2|1<<3|0<<4);
        int modebits = 0;
        int BitCounter = 1;
        for (int i=0; i<boxids.length; i++) {
          if(toggleModeItems[i].getValue() > 0) modebits |= BitCounter;
          BitCounter += BitCounter;
        }
        serialize32(modebits);
        serialize8(0);   // current setting
        tailSerialReply();
        PortIsWriting = false;
      break;
      
      case MSP_RC:
        PortIsWriting = true;
        headSerialReply(MSP_RC, 14);
        serialize16(int(Pitch_Roll.arrayValue()[0]));
        serialize16(int(Pitch_Roll.arrayValue()[1]));
        serialize16(int(Throttle_Yaw.arrayValue()[0]));
        serialize16(int(Throttle_Yaw.arrayValue()[1]));
        for (int i=5; i<8; i++) {
          serialize16(1500);
        }
        tailSerialReply();
        PortIsWriting = false;
      break;
      
      case MSP_BOXIDS:
        headSerialReply(MSP_BOXIDS, strBoxIds.length());
        for(int i=0;i<4;i++) 
        {serialize8(i);
        serialize8(1);
        serialize8(2);
        serialize8(3);
        serialize8(5);
        serialize8(10);
        serialize8(11);
        serialize8(19);
        tailSerialReply();
        
        PortIsWriting = false;
     }
     
      case MSP_ATTITUDE:
        PortIsWriting = true;
        headSerialReply(MSP_ATTITUDE, 8);
        serialize16(int(MW_Pitch_Roll.arrayValue()[0])*20);
        serialize16(int(MW_Pitch_Roll.arrayValue()[1])*20);
        serialize16(MwHeading);
        serialize16(0);
        tailSerialReply();
        PortIsWriting = false;
      break;
     
     
     
      case MSP_ANALOG:
        PortIsWriting = true;
        headSerialReply(MSP_ANALOG, 5);
        serialize8(int(sVBat * 10));
        serialize16(0);
        serialize16(int(sMRSSI));
        tailSerialReply();
        PortIsWriting = false;
      break;
      
      
      case MSP_RAW_GPS:
        // We have: GPS_fix(0-2), GPS_numSat(0-15), GPS_coord[LAT & LON](signed, in 1/10 000 000 degres), GPS_altitude(signed, in meters) and GPS_speed(in cm/s)  
        headSerialReply(MSP_RAW_GPS,16);
        serialize8(int(SGPS_FIX.arrayValue()[0]));
        serialize8(int(SGPS_numSat.value()));
        serialize32(384627000);
        serialize32(-90803000);
        serialize16(int(SGPS_altitude.value()/100));
        serialize16(int(SGPS_speed.value()));
        serialize16(355);     
    break;
    
  
    case MSP_COMP_GPS:
      headSerialReply(MSP_COMP_GPS,5);
      serialize16(int(SGPS_distanceToHome.value()));
      int GPSheading = int(SGPSHeadHome.value());
      if(GPSheading < 0) GPSheading += 360;
      serialize16(GPSheading);
      serialize8(0);
    break;
    
    
    case MSP_ALTITUDE:
      headSerialReply(MSP_ALTITUDE, 6);
      serialize32(int(sAltitude) *100);
      serialize16(int(sVario) *10);     
    break;
      
    }
    tailSerialReply();   
  
}

// coded by Eberhard Rensch
// Truncates a long port name for better (readable) display in the GUI
String shortifyPortName(String portName, int maxlen)  {
  String shortName = portName;
  if(shortName.startsWith("/dev/")) shortName = shortName.substring(5);  
  if(shortName.startsWith("tty.")) shortName = shortName.substring(4); // get rid of leading tty. part of device name
  if(portName.length()>maxlen) shortName = shortName.substring(0,(maxlen-1)/2) + "~" +shortName.substring(shortName.length()-(maxlen-(maxlen-1)/2));
  if(shortName.startsWith("cu.")) shortName = "";// only collect the corresponding tty. devices
  return shortName;
}

public static final int
  IDLE = 0,
  HEADER_START = 1,
  HEADER_M = 2,
  HEADER_ARROW = 3,
  HEADER_SIZE = 4,
  HEADER_CMD = 5,
  HEADER_ERR = 6;

private static final String MSP_SIM_HEADER = "$M>";
int c_state = IDLE;
boolean err_rcvd = false;
byte checksum=0;
byte cmd = 0;
int offset=0, dataSize=0;
byte[] inBuf = new byte[256];
int Send_timer = 1;
int p=0;
int read32() {return (inBuf[p++]&0xff) + ((inBuf[p++]&0xff)<<8) + ((inBuf[p++]&0xff)<<16) + ((inBuf[p++]&0xff)<<24); }
int read16() {return (inBuf[p++]&0xff) + ((inBuf[p++])<<8); }
int read8()  {return inBuf[p++]&0xff;}

int outChecksum;

void serialize8(int val) {
 if (init_com==1)  {
       try{
       g_serial.write(val);
       outChecksum ^= val;
       } catch(java.lang.Throwable t) {
         System.out.println( t.getClass().getName() ); //this'll tell you what class has been thrown
         t.printStackTrace(); //get a stack trace
    }
  }
}

void serialize16(int a) {
  if (str(a)!=null ){
  serialize8((a   ) & 0xFF);
  serialize8((a>>8) & 0xFF);
  }
}

void serialize32(int a) {
  if (str(a)!=null ){
    serialize8((a    ) & 0xFF);
    serialize8((a>> 8) & 0xFF);
    serialize8((a>>16) & 0xFF);
    serialize8((a>>24) & 0xFF);
  }
 
}

void headSerialResponse(int requestMSP, Boolean err, int s) {
  serialize8('$');
  serialize8('M');
  serialize8(err ? '!' : '>');
  outChecksum = 0; // start calculating a new checksum
  serialize8(s);
  serialize8(requestMSP);
}

void headSerialReply(int requestMSP, int s) {
  if ((str(requestMSP) !=null) && (str(s)!=null)){
    headSerialResponse(requestMSP, false, s);
  }
}

void tailSerialReply() {
  if (outChecksum > 0) serialize8(outChecksum);
}

public void DelayTimer(int ms){
  int time = millis();
  while(millis()-time < ms);
}

public void evaluateCommand(byte cmd, int size) {
  if ((init_com==0)  || (toggleMSP_Data == false)) return;
  MakePorts(); 
  int icmd = int(cmd&0xFF);
  if (icmd !=MSP_OSD)return;

    time2=time;
    switch(icmd) {
      case MSP_OSD:
        int cmd_internal = read8();
        
        if(cmd_internal == OSD_NULL) {
        }

        if(cmd_internal == OSD_READ_CMD) {
          if(size == 1) {
          }
          else {
            // Returned result from OSD.
            for(int i = 0; i < CONFIGITEMS; i++){
              SetConfigItem(i, read8());
            }
            if (FontMode == false){
              toggleMSP_Data = false;
              g_serial.clear();
              
              
            }
          }
        }
        
        if(cmd_internal == OSD_GET_FONT) {
          if( size == 1) {
          }
          if(size == 3) {
       }
     }
    break;
  }
}

void MWData_Com() {
  if ((toggleMSP_Data == false) ||(init_com==0)) return;
  int i,aa;
  float val,inter,a,b,h;
  int c = 0;
    while (g_serial.available()>0 && (toggleMSP_Data == true)) {
    try{
      c = (g_serial.read());
      
     if (str(c) == null)return;
      
      PortRead = true;
      if (c_state == IDLE) {
        c_state = (c=='$') ? HEADER_START : IDLE;
      }
      else if (c_state == HEADER_START) {
        c_state = (c=='M') ? HEADER_M : IDLE;
      }
      else if (c_state == HEADER_M) {
        if (c == '<') {
          c_state = HEADER_ARROW;
        } else if (c == '!') {
          c_state = HEADER_ERR;
        } else {
          c_state = IDLE;
        }
      }
      else if (c_state == HEADER_ARROW || c_state == HEADER_ERR) {
        /* is this an error message? */
        err_rcvd = (c_state == HEADER_ERR);        /* now we are expecting the payload size */
        dataSize = (c&0xFF);
        /* reset index variables */
        p = 0;
        offset = 0;
        checksum = 0;
        checksum ^= (c&0xFF);
        /* the command is to follow */
        c_state = HEADER_SIZE;
      }
      else if (c_state == HEADER_SIZE) {
        cmd = (byte)(c&0xFF);
        checksum ^= (c&0xFF);
        c_state = HEADER_CMD;
      }
      else if (c_state == HEADER_CMD && offset < dataSize) {
          checksum ^= (c&0xFF);
          inBuf[offset++] = (byte)(c&0xFF);
      } 
      else if (c_state == HEADER_CMD && offset >= dataSize) {
        /* compare calculated and transferred checksum */
        if ((checksum&0xFF) == (c&0xFF)) {
          if (err_rcvd) {
          } else {
            /* we got a valid response packet, evaluate it */
            try{
              if ((init_com==1)  && (toggleMSP_Data == true)) {
                  evaluateCommand(cmd, (int)dataSize);
              }
              else{
                System.out.println("port is off ");
              }
              
              
              } catch (Exception e) { // null pointer or serial port dead
              System.out.println("write error " + e);
              }
           }
        }
        else {
          System.out.println("invalid checksum for command "+((int)(cmd&0xFF))+": "+(checksum&0xFF)+" expected, got "+(int)(c&0xFF));
          System.out.print("<"+(cmd&0xFF)+" "+(dataSize&0xFF)+"> {");
          for (i=0; i<dataSize; i++) {
            if (i!=0) { System.err.print(' '); }
            System.out.print((inBuf[i] & 0xFF));
          }
          System.out.println("} ["+c+"]");
          System.out.println(new String(inBuf, 0, dataSize));
      }
        c_state = IDLE;
        
      }
      
      } catch(java.lang.Throwable t) {
         System.out.println( t.getClass().getName() ); //this'll tell you what class has been thrown
         t.printStackTrace(); //get a stack trace
      }
   }
}





      
