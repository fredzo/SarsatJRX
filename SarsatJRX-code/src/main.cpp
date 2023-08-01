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
#include <Country.h>
#include <qrcode.h>
#include <Beacon.h>

/********************************
  Definitions des constantes
*********************************/
// Interupt pin : use digital pin 18
const int ReceiverPin = 18;
unsigned long microseconds;
unsigned long duree_palier;
boolean der_bit = 1; // debut de trame à 1
boolean etat = 0; // debut de trame à 1
byte start_flag = 0;
byte count;
byte count_oct;
byte data_demod;
const byte Nb_octet = 17; //3 + 15 **18 octets 1er FF  = octet 0**
byte frame[Beacon::SIZE];
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

String toHexString(byte* frame, bool withSpace, int start, int end)
{
  char buffer[4];
  String result = "";
  for ( byte i = start; i < end; i++) 
  {
    sprintf(buffer, "%02X", frame[i]);
    if(withSpace) 
    {
      result += " ";
    }
    result += buffer;
  }
  return result;
}

static const int QR_VERSION = 6;
void generateQrCode(QRCode* qrcode, Beacon* beacon)
{ // Version 6 (41x41) allows 154 alphanumeric characters with medium error correcion
  // For some reason, to have the qr code library work properly, the version passed to getBufferSize needs to be one step ahed the actual version...
  uint8_t qrcodeData[qrcode_getBufferSize(QR_VERSION+2)];
  String url = "https://0w66030c9i.execute-api.ap-southeast-2.amazonaws.com/dev/v1/beacon/fgb/detection/" + toHexString(beacon->frame, false, 3, beacon->longFrame ? 18 : 14) + "/decode";
  Serial.println(url);
	qrcode_initText(qrcode, qrcodeData, QR_VERSION, ECC_LOW, url.c_str());
}

static const int MODULE_SIZE = 3;
void displayQrCode (QRCode* qrcode, int xPos, int yPos)
{
  display.setCursor(xPos,yPos);
  display.setColor(Display::Color::WHITE);
  // Add a padding around the QR code
  display.fillRectangle((qrcode->size+2)*MODULE_SIZE,(qrcode->size+2)*MODULE_SIZE);
  display.setColor(Display::Color::BLACK);
  xPos += MODULE_SIZE;
  yPos += MODULE_SIZE;
  for (int y = 0; y < qrcode->size; y++) 
  {
      for (int x = 0; x < qrcode->size; x++) 
      {
          if (qrcode_getModule(qrcode, x, y)) 
          {
              display.setCursor(xPos + (x * MODULE_SIZE),yPos + (y * MODULE_SIZE));
							display.fillRectangle(MODULE_SIZE, MODULE_SIZE);
          }
      }
  }
}

/*****************************
  Routine autotest 406
******************************/
void test406()
{
  Beacon beacon = Beacon(frame);
    
  display.setBackgroundColor(Display::Color::BLACK);
  display.setColor(Display::Color::WHITE);
  display.clearDisplay();        // rafraichissement Oled
  display.setCursor(0, 0);       // Balise TEST ou DETRESSE
  if (beacon.frameMode == Beacon::FrameMode::SELF_TEST) 
  {  // Self-test message frame synchronisation byte
    display.println("Self-test 406 - SarsatJRX");
    Serial.println("Self-test 406 - SarsatJRX");    
  }
  else if (beacon.frameMode == Beacon::FrameMode::NORMAL) 
  { // Normal message frame synchronisation byte
    display.println("Distress 406 - SarsatJRX");
    Serial.println("Distress 406 - SarsatJRX");
  }
  else
  { // Unknown fram format
    display.println("Unknown 406 - SarsatJRX");
    Serial.println("Unknown 406 - SarsatJRX");
  }

  // Description           
  display.setCursor(0, 15);
  display.println(beacon.desciption);
  Serial.println(beacon.desciption);   

  // Country
  display.setCursor(0, 30);
  String country = beacon.country.toString();
  display.println(country);
  Serial.println(country);

  if (beacon.longFrame) 
  { // Long frame

    // Location 
    display.setCursor(0, 45); 
    String locationSexa = beacon.location.toString(true);
    String locationDeci = beacon.location.toString(false);
    display.println(locationSexa);
    Serial.println(locationSexa);
    if(!beacon.location.isUnknown())
    {
      Serial.println(locationDeci);
    }

    // HEX ID 30 Hexa
    display.setCursor(0, 60);
    display.println(toHexString(beacon.frame,true,3,13));
    display.setCursor(0, 75);
    display.println(toHexString(beacon.frame,true,13,18));
  }
  else 
  { // Short frame
    display.setCursor(5, 45);
    display.println("22 HEX. No location");
    
    display.setCursor(0, 60);  
    // HEX ID 22 Hexa bit 26 to 112
    display.println(toHexString(beacon.frame,true,3,14));
  }

    QRCode qrCode;
    generateQrCode(&qrCode,&beacon);
    int xPos = display.getWidth()-((qrCode.size+3)*MODULE_SIZE);
    int yPos = display.getHeight()-((qrCode.size+3)*MODULE_SIZE);
    displayQrCode(&qrCode,xPos,yPos);
  
 // display.setCursor(80, 30); // Oled Voltmetre
 // display.print("V= ");
 // display.print(vin);
 // display.println();

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

