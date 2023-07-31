/*******************************
   SARSAT decodeur 
   Oled Display 128x64
   F4GMU
   Jan_2018
   
   Detresse/AutoTest      OK
   Longueur Trame         OK
   Pays                   OK
   4 protocoles           OK   
   Coord Nat location     OK
   Offset coord           OK
   Coord Sdt location     OK
   Offset coord           OK
   Coord User loction     OK   
   HEX Id                 OK
   Buzzer trame suivante  OK
   
   *Loop Test406 sur trame courte
   *Alarme Batterie
   *AUX 121.5
********************************/

#include <Arduino.h>

#include <Display.h>
#include <Location.h>

/********************************
  Definitions des constantes
*********************************/
// Interupt pin : use digital pin 18
const int ReceiverPin = 18;
unsigned long microseconds;
unsigned long duree_palier, pays;
Location location;
long latofmin, latofsec, lonofmin, lonofsec, protocolCode;
boolean longtrame, protocolFlag, latoffset, lonoffset;
boolean der_bit = 1; // debut de trame à 1
boolean etat = 0; // debut de trame à 1
byte start_flag = 0;
byte count;
byte count_oct;
byte data_demod;
const byte Nb_octet = 17; //3 + 15 **18 octets 1er FF  = octet 0**
byte frame[18];
float vout = 0.0;   // pour lecture tension batterie
float vin = 0.0;    // pour lecture tension batterie
int value = 0;      // pour lecture tension batterie


Display display;

/***************************************
  Test octets recu et transfert data
  header 0xFF 0xFE 
****************************************/

void Test()
{ 
  // Frame format :
  // First Byte / Second Byte = Bit synchronization = 0xFF+0xFE
  // Third Byte = Frame synchrnization = 0xD0 for normal message / 0x2F for self-test message
  // First bit of fourth byte indicates the message format : 0 = short message (11 more bytes, i.e. 14 bytes in total) / 1 = long message (15 more bytes, i.e. 18 bytes in total)
  // Short Message :
  // |----------------------------------------------------------------------------------------|
  // | Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4  
  // Long Message :
  if (start_flag == 0) {
    if (data_demod == 255) {         // si 0xFF recu, start_flag = 1
      frame[0] = data_demod;
      start_flag = 1;
      count = 0;
      count_oct = 1;
      data_demod = 0;
    }
    else {
      return;
    }
  }

  if (start_flag == 1) {   //si 0xFF recu
    if (data_demod == 254) { // si 0xFE
      frame[1]= data_demod;
      start_flag = 2;
      count = 0;
      count_oct = 2;
      data_demod = 0;
    }
    else {
      return;
    }    
  }  
   
  if (start_flag == 2) {   //si 0xFE recu
    if (data_demod == 208 || data_demod == 47) { // si 0xD0 ou 2F
      frame[2]= data_demod;
      start_flag = 3;
      count = 0;
      count_oct = 3;
      data_demod = 0;
    }
    else {
      return;
    }    
  }
  
  else if (start_flag == 3) {           //si 0xFE recu 
    if (count == 7) {                   //si nombre de bits = 8
      frame[count_oct] = data_demod;    //data dans octet numero xx
      count_oct ++;
      data_demod = 0;
      count = 0;
    }
    else if (count < 7) {
      count ++;
    }
  }
}


/***********************************************
  Extraction des bits
***********************************************/
void analyze(void)
{
  duree_palier = micros() - microseconds;
  microseconds = micros();

  if (duree_palier > 1500 && duree_palier < 1700) {
    return;
  }
  if (duree_palier > 1150 && duree_palier < 1350 && der_bit == 0) {   // si T = 125ms ET der_bit == 0; der_bit = 1
    der_bit = 1;
    return;
  }
  if (duree_palier > 1150 && duree_palier < 1350 && der_bit == 1) {    // si T = 125ms ET der_bit == 1; bit = etat
    data_demod = data_demod << 1 | etat; // ajout du 1 en fin
    der_bit = 0;
  }
  if (duree_palier > 2450 && duree_palier < 2650) {    // si T = 250ms; bit = !etat
    etat = !etat;
    data_demod = data_demod << 1 | etat; // ajout du 1 en fin
    der_bit = 0;
  }
  Test();
}



/*****************************
  Routine autotest 406
******************************/
void test406()
{
  longtrame = (frame[3] & 0x80) >> 7;      // taille            bit 25
  protocolFlag = (frame[3] & 0x40) >> 6;   // protocol flag     bit 26
  protocolCode = (frame[4] & 0x0F);        // protocol code     bit 37-40
  pays = ((frame[3] & 0x3F) << 4 | (frame[4] & 0xF0) >> 4); // pays bit 27-36
  pays = pays & 0x3FF; // pays
    
  display.clearDisplay();        // rafraichissement Oled
  display.setCursor(0, 0);       // Balise TEST ou DETRESSE
  if (frame[2] == 0xD0) {  // Self-test message frame synchronisation byte
    display.println("AutoTest 406 F4GMU");
    Serial.println("AutoTest 406 F4GMU");    
  }
  else if (frame[2] == 0x2F) { // Normal message frame synchronisation byte
    display.println("DETRESSE 406 F4GMU");
    Serial.println("DETRESSE 406 F4GMU");
  }
           
  if (longtrame == 1 && protocolFlag == 1) {    // User loc protocol trame longue
    // TODO : Check if protocol code = 1,2,3,6,7
    location.latitude.orientation = (frame[13] & 0x10) >> 4;   
    location.latitude.degrees = ((frame[13] & 0x0F) << 3 | (frame[14] & 0xE0) >> 5);
    location.latitude.minutes = (frame[14] & 0x1E) >> 1;
    location.latitude.minutes = (location.latitude.minutes * 4);
    location.longitude.orientation = (frame[14] & 0x01);           
    location.longitude.degrees = (frame[15]);
    location.longitude.minutes = (frame[16] & 0xF0) >> 4;
    location.longitude.minutes = (location.longitude.minutes * 4);

    
    display.setCursor(0, 10);
    display.println("User loc. Protocol");
    Serial.println("User location Protocol");   

    display.setCursor(0, 30); // Oled Latitude N/S 
    if (location.latitude.degrees == 127 || location.longitude.degrees == 255) {
      display.print("GPS non synchronise");
    }
    else {
    if (location.latitude.orientation == 0) {
      display.print("N");
      Serial.print("N");
    }
    else {
      display.print("S");
      Serial.print("S");
    }
    display.print(location.latitude.degrees);
    display.print("^");
    display.print(location.latitude.minutes);
    display.print("'");
    Serial.print(location.latitude.degrees, DEC);
    Serial.print("°");
    Serial.print(location.latitude.minutes, DEC);
    Serial.print("'");
    
    if (location.longitude.orientation == 0) {
      display.print("  E");
      Serial.print("  E");
    }
    else {
      display.print("  W");
      Serial.print("  W");
    }
    display.print(location.longitude.degrees);
    display.print("^");
    display.print(location.longitude.minutes);
    display.print("'");
    Serial.print(location.longitude.degrees, DEC);
    Serial.print("°");
    Serial.println(location.longitude.minutes, DEC);
    Serial.print("'");
    }
    display.println();

    display.setCursor(0, 40);      // HEX ID 30 Hexa
    for ( byte i = 3; i < 18; i++) {
      if (frame[i] < 16)
      display.print("0");
      display.printHex(frame[i]);
      display.print(" ");
    }
    display.println();
  }
  else if (longtrame == 1 && protocolFlag == 0 && (protocolCode == 8 /*ELT*/ || protocolCode == 10 /* EPIRB */ 
        || protocolCode == 11 /* PLB */ || protocolCode == 16 /* National Test Location Protocol */)) { //Nat loc protocol / trame longue
        // TODO : ELT(DT) Location protocol => protocolCode == 9
        // TODO ? : RTS Location protocol => protocolCode == 13

    latoffset = (frame[14] & 0x80) >> 7;   // Latitude NAT LOCATION
    location.latitude.orientation = (frame[7] & 0x20) >> 5;   
    location.latitude.degrees = ((frame[7] & 0x1F) << 2 | (frame[8] & 0xC0) >> 6);
    location.latitude.minutes = (frame[8] & 0x3E) >> 1;
    location.latitude.minutes = (location.latitude.minutes * 2);
    latofmin = (frame[14] & 0x60) >> 5;
    latofsec = (frame[14] & 0x1E) >> 1;
    latofsec = (latofsec * 4);
    if (latoffset == 1) {
      location.latitude.minutes = (location.latitude.minutes + latofmin);
      location.latitude.minutes = (location.latitude.minutes * 10000);
      location.latitude.minutes = (location.latitude.minutes / 60);
      latofsec = (latofsec * 10000);
      latofsec = (latofsec / 3600);
      location.latitude.minutes = (location.latitude.minutes + latofsec);
    }
    else if (latoffset == 0) {
      location.latitude.minutes = (location.latitude.minutes - latofmin);
      if (location.latitude.minutes < 0) {
        location.latitude.minutes = (60 - abs(location.latitude.minutes));
        location.latitude.degrees = (location.latitude.degrees - 1);
      }
      location.latitude.minutes = (location.latitude.minutes * 10000);
      location.latitude.minutes = (location.latitude.minutes / 60);
      latofsec = (latofsec * 10000);
      latofsec = (latofsec / 3600);
      location.latitude.minutes = (location.latitude.minutes - latofsec);
    }

    lonoffset = (frame[14] & 0x01);       //Longitude NAT LOCATION
    location.longitude.orientation = (frame[8] & 0x01);           
    location.longitude.degrees = (frame[9]);
    location.longitude.minutes = (frame[10] & 0xF8) >> 3;
    location.longitude.minutes = (location.longitude.minutes * 2);
    lonofmin = (frame[15] & 0xC0) >> 6;
    lonofsec = (frame[15] & 0x3C) >> 2;
    lonofsec = (lonofsec * 4);
    if (lonoffset == 1) {
      location.longitude.minutes = (location.longitude.minutes + lonofmin);
      location.longitude.minutes = (location.longitude.minutes * 10000);
      location.longitude.minutes = (location.longitude.minutes / 60);
      lonofsec = (lonofsec * 10000);
      lonofsec = (lonofsec / 3600);
      location.longitude.minutes = (location.longitude.minutes + lonofsec);
    }
    else if (lonoffset == 0) {
      location.longitude.minutes = (location.longitude.minutes - lonofmin);
      if (location.longitude.minutes < 0) {
        location.longitude.minutes = (60 - abs(location.longitude.minutes));
        location.longitude.degrees = (location.longitude.degrees - 1);
      }
      location.longitude.minutes = (location.longitude.minutes * 10000);
      location.longitude.minutes = (location.longitude.minutes / 60);
      lonofsec = (lonofsec * 10000);
      lonofsec = (lonofsec / 3600);
      location.longitude.minutes = (location.longitude.minutes - lonofsec);
    }
    
    
    
    display.println("National Protocol");
    Serial.println("National Location Protocol");
    
    display.setCursor(0, 30); // Oled Latitude N/S 
    if (location.latitude.degrees == 127 || location.longitude.degrees == 255) {
      display.print("GPS non synchronise");
    }
    else {
    if (location.latitude.orientation == 0) {
      display.print("N");
      Serial.print("N");
    }
    else {
      display.print("S");
      Serial.print("S");
    }
    display.print(location.latitude.degrees);
    display.print(".");
    display.print(location.latitude.minutes);
    Serial.print(location.latitude.degrees, DEC);
    Serial.print(".");
    Serial.print(location.latitude.minutes, DEC);
    
    if (location.longitude.orientation == 0) {
      display.print("  E");
      Serial.print("  E");
    }
    else {
      display.print("  W");
      Serial.print("  W");
    }
    display.print(location.longitude.degrees);
    display.print(".");
    display.print(location.longitude.minutes);
    Serial.print(location.longitude.degrees, DEC);
    Serial.print(".");
    Serial.println(location.longitude.minutes, DEC);
    }
    display.println();
    
    display.setCursor(0, 40);      // HEX ID 30 Hexa
    for ( byte i = 3; i < 18; i++) {
      if (frame[i] < 16)
      display.print("0");
      display.printHex(frame[i]);
      display.print(" ");
    }
    display.println();
  }
  else if (longtrame == 1 && protocolFlag == 0 && (protocolCode < 8 || protocolCode == 12 /* Ship Security Protocol */ || protocolCode == 14 /* Standard Test Location Prtocol */)) { //Std loc protocol trame longue
    // TODO : protocole code = 0 and 1 should be excluded
    Serial.print("longtrame = ");
    Serial.print(longtrame, DEC);
    Serial.println();
    latoffset = (frame[14] & 0x80) >> 7;  //Latitude STD LOCATION
    location.latitude.orientation = (frame[8] & 0x80) >> 7; 
    location.latitude.degrees = (frame[8] & 0x7F);
    location.latitude.minutes = (frame[9] & 0xC0) >> 6;
    location.latitude.minutes = (location.latitude.minutes * 15);
    latofmin = (frame[14] & 0x7C) >> 2;
    latofsec = ((frame[14] & 0x03) << 2 | (frame[15] & 0xC0) >> 6);
    latofsec = (latofsec * 4);
    if (latoffset == 1) {
      location.latitude.minutes = (location.latitude.minutes + latofmin);
      location.latitude.minutes = (location.latitude.minutes * 10000);
      location.latitude.minutes = (location.latitude.minutes / 60);
      latofsec = (latofsec * 10000);
      latofsec = (latofsec / 3600);
      location.latitude.minutes = (location.latitude.minutes + latofsec);
    }
    else if (latoffset == 0) {
      location.latitude.minutes = (location.latitude.minutes - latofmin);
      if (location.latitude.minutes < 0) {
        location.latitude.minutes = (60 - abs(location.latitude.minutes));
        location.latitude.degrees = (location.latitude.degrees - 1);
      }
      location.latitude.minutes = (location.latitude.minutes * 10000);
      location.latitude.minutes = (location.latitude.minutes / 60);
      latofsec = (latofsec * 10000);
      latofsec = (latofsec / 3600);
      location.latitude.minutes = (location.latitude.minutes - latofsec);
    }
    
    lonoffset = (frame[15] & 0x20) >> 5;  //Longitude STD LOCATION
    location.longitude.orientation = (frame[9] & 0x20) >> 5; 
    location.longitude.degrees = ((frame[9] & 0x1F) << 3 | (frame[10] & 0xE0) >> 5);
    location.longitude.minutes = (frame[10] & 0x18) >> 3;
    location.longitude.minutes = (location.longitude.minutes * 15);
    lonofmin = (frame[15] & 0x1F);
    lonofsec = (frame[16] & 0xF0) >> 4;
    lonofsec = (lonofsec * 4);
    if (lonoffset == 1) {
      location.longitude.minutes = (location.longitude.minutes + lonofmin);
      location.longitude.minutes = (location.longitude.minutes * 10000);
      location.longitude.minutes = (location.longitude.minutes / 60);
      lonofsec = (lonofsec * 10000);
      lonofsec = (lonofsec / 3600);
      location.longitude.minutes = (location.longitude.minutes + lonofsec);
    }
    else if (lonoffset == 0) {
      location.longitude.minutes = (location.longitude.minutes - lonofmin);
      if (location.longitude.minutes < 0) {
        location.longitude.minutes = (60 - abs(location.longitude.minutes));
        location.longitude.degrees = (location.longitude.degrees - 1);
      }
      location.longitude.minutes = (location.longitude.minutes * 10000);
      location.longitude.minutes = (location.longitude.minutes / 60);
      lonofsec = (lonofsec * 10000);
      lonofsec = (lonofsec / 3600);
      location.longitude.minutes = (location.longitude.minutes - lonofsec);
    }
    
    display.println("Standard Protocol");
    Serial.println("Standard Location Protocol");
    
    display.setCursor(0, 30); // Oled Latitude N/S 
    if (location.latitude.degrees == 127 || location.longitude.degrees == 255) {
      display.print("GPS non synchronise");
    }
    else {
    if (location.latitude.orientation == 0) {
      display.print("N");
      Serial.print("N");
    }
    else {
      display.print("S");
      Serial.print("S");
    }
    display.print(location.latitude.degrees);
    display.print(".");
      if (location.latitude.minutes < 0x0A) {
        display.print("000");
        display.print(location.latitude.minutes);
      }
      if (location.latitude.minutes < 0x64) {
        display.print("00");
        display.print(location.latitude.minutes);
      }
      if (location.latitude.minutes < 0x3E8) {
        display.print("0");
        display.print(location.latitude.minutes);
      }
      else {
        display.print(location.latitude.minutes);
      }
    Serial.print(location.latitude.degrees, DEC);
    Serial.print(".");
    Serial.print(location.latitude.minutes, DEC);
    if (location.longitude.orientation == 0) {
      display.print("  E");
      Serial.print("  E");
    }
    else {
      display.print("  W");
      Serial.print("  W");
    }
    display.print(location.longitude.degrees);
    display.print(".");
      if (location.longitude.minutes < 0x0A) {
        display.print("000");
        display.print(location.longitude.minutes);
      }
      if (location.longitude.minutes < 0x64) {
        display.print("00");
        display.print(location.longitude.minutes);
      }
      if (location.longitude.minutes < 0x3E8) {
        display.print("0");
        display.print(location.longitude.minutes);
      }
      else {
        display.print(location.longitude.minutes);
      }
    Serial.print(location.longitude.degrees, DEC);
    Serial.print(".");
    Serial.println(location.longitude.minutes, DEC);
    }
    display.println();

    display.setCursor(0, 40);      // HEX ID 30 Hexa
    for ( byte i = 3; i < 18; i++) {
      if (frame[i] < 16)
      display.print("0");
      display.printHex(frame[i]);
      display.print(" ");
    }
    display.println();
  }
  else {                                // User protocol trame courte
    display.setCursor(0, 10);
    display.println("User Protocol");
    Serial.println("User Protocol");
    display.setCursor(5, 30);
    display.println("22 HEX. No location");
    
    display.setCursor(0, 40);      // HEX ID 22 Hexa bit 26 to 112
    for ( byte i = 3; i < 14; i++) {
      if (frame[i] < 16)
      display.print("0");
      display.printHex(frame[i]);
      display.print(" ");
    }
    display.println();
  }
  display.setCursor(0, 20);     // Oled Pays
  if (pays == 226 || pays == 227 || pays == 228) {
    display.println("Pays= FRANCE");
    Serial.println("Pays= FRANCE");
  }
  else {
    display.print("Pays= ");
    Serial.print("Pays : ");
    display.println(String(pays));
    Serial.println(pays, DEC);
  }
  
  display.setCursor(80, 20); // Oled Voltmetre
  display.print("V= ");
  display.print(vin);

//  display.display();

  count_oct = 0;     // repart pour trame suivante
  count = 0;
  start_flag = 0;
  data_demod = 0;
  der_bit = 1;
}

/****************************************
 * LED trame recu
 ****************************************/
void ledblink()
  {
  digitalWrite(13, HIGH);  // Clignotement LED Trame décodée
  delay(100);
  digitalWrite(13, LOW); 
  /*delay(45000);            // Alarme Buzzer Trame suivante
  digitalWrite(7, HIGH);  
  delay(4500);
  digitalWrite(7, LOW);*/
  }

/***************************************
 * Voltmetre sur A5
 ***************************************/
void voltmetre()
  {
    value = analogRead(3);
    vout = (value * 5.0) / 1024.0;
    vin = vout / (0.088); // vin = vout / (R2/(R1+R2)) R1=100k R2=10k
    if (vin<0.09) {
      vin=0.0;//Condition pour eviter des lectures indesirable !
    }
  }


void readHexString(String hexString) 
{
  for (unsigned int i = 0; i < hexString.length(); i += 2) {
    String byteString = hexString.substring(i, i+2);
    byte b = (byte)strtol(byteString.c_str(), NULL, 16);
    frame[i/2]=b;
  }
  count_oct = Nb_octet;
}

/*************************
**  Required functions  **
*************************/

int curFrame = 0;
static const String frames[] = {
  "FFFED0D6E6202820000C29FF51041775302D", // 1  - Selftest - Serial user -	ELT with Serial Identification
  "FFFE2FD6E6202820000C29FF51041775302D", // 2  - Serial user -	ELT with Serial Identification
  "FFFE2F8DB345B146202DDF3C71F59BAB7072", // 3  - Standard Location
  "FFFE2F8E0D0990014710021963C85C7009F5", // 4  - RLS Location
  "FFFE2F8E3B15F1DFC0FF07FD1F769F3C0672", // 5  - PLB Location: National Location 
  "FFFE2F8F4DCBC01ACD004BB10D4E79C4DD86", // 6  - RLS Location Protocol 
  "FFFE2F96E3AAAAAA7FDFF8211F3583E0FAA8", // 7  - Std Loc. ELT 24-bit Address Protocol 
  "FFFE2F96E61B0CAE7FDFFF0E58B583E0FAA8", // 8  - Standard Location Protocol - EPIRB (Serial) 
  "FFFE2F9F7B00F9E8EC737E5312378A1802B0", // 9  - PLB Location: National Location 
  "FFFE2FA0D205F260850F3DC9E0B70B6E4FD7", // 10 - Standard Location Protocol EPIRB-MMSI 
  "FFFE2FA3E7B10016150D364D8B3689C09437", // 11 - Standard Location Protocol - PLB (Serial) 
  "FFFE2FA5DC19E1A07FDFFE5C803483E0FCCA", // 12 - Std. Location ship security protocol (SSAS) 
  "FFFE2FCE46E76EF8C00C2BAA31CFE0FF0F61", // 13 - Serial user - ELT with Aircraft 24-bit Address 
  "FFFE2FD9D4EB28140AA6893F16A2C67282EC", // 14 - Maritime User Protocol MMSI - EPIRB 
  "FFFE2FDF76A9A9C800174DE27BE1F0277E45", // 15 - Serial user - Float Free EPIRB with Serial Identification Number 
  "FFFE2FDF77200000001168C610AFE0FF0146", // 16 - Serial user - Non Float Free EPIRB with Serial Identification 
  "FFFE2FE0DDAE599508268E4C05E054651307", // 17 - Radio Call Sign - EPIRB 
  "FFFE2FE29325ACB12E938B671E0FE0FF0F61", // 18 - ELT Aviation User 
  "FFFE2FEDF67B7182038C2F0E10CFE0FF0F61", // 19 - Serial user -	ELT with Aircraft Operator Designator & Serial Number 
  "FFFE2FA157B081437FDFF8B4833783E0F66C", // 20 - Standard Location Protocol - PLB (Serial) 
  "FFFE2F3EF613523F81FE0"                 // 21 - Short
  };
static const int framesSize = 21;

void setup()
{
  pinMode(ReceiverPin, INPUT);          // entree sur interruption 0
  pinMode(14, OUTPUT);                  // sortie LED pin 14
  pinMode(15, INPUT);                   // entree batterie pin 15
  pinMode(16, OUTPUT);                  // sortie Buzzer pin 16
  
  Serial.begin(9600);
  attachInterrupt(0, analyze, CHANGE);  // interruption sur Rise et Fall

  display.setup();
  //display.drawButtons();  
  readHexString(frames[curFrame]);
}


void loop()
{
  if(display.touchAvailable())
  {
    curFrame++;
    if(curFrame >= framesSize)
    {
      curFrame = 0;
    }
    readHexString(frames[curFrame]);
  }
  if (count_oct == Nb_octet)
  {
    // Pour debug 
    /*
    for ( byte i = 0; i < Nb_octet; i++) // RAW data
    {
      if (frame[i] < 16)
      Serial.print('0');
      Serial.print (frame[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");*/
    

    if (((frame[1] == 0xFE) && (frame[2] == 0xD0)) || ((frame[1] == 0xFE) && (frame[2] == 0x2F)))// 0XFE/0x2F for normal mode, 0xFE/0xD0  for autotest
    {
      test406();
      ledblink();
      voltmetre();

      count_oct = 0;     // repart pour trame suivante
      count = 0;
      start_flag = 0;
      data_demod = 0;
      der_bit = 1;
      etat = 1;
      
    } 
  }
}

