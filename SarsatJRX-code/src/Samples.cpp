#include "Samples.h"
#include "Util.h"

static const String frames[] = {
  "FFFED0D6E6202820000C29FF51041775302D", // 1  - Selftest - Serial user Location Protocol
//  "FFFE2FD6E6202820000C29FF51041775302D", // 2  - Serial user -	Serial user Location Protocol
  "FFFE2F8e3e0425a8318074fe44b735cd7b46", // 2  - File "ExerciceADRASEC02_30_11_2014.wav" -> Standard Test - N 49d16m32s /  E 3d16m32s
  "FFFE2F3EF613523F81FE0",                // 3  - User protocol Radio call sign (wrong BCH1)
//"FFFE2F56E6804002202009655250",         // 3  - User protocol Radio call sign (valid BCH1)
  "FFFE2F8E0D0990014710021963C85C7009F5", // 4  - RLS Location
  "FFFE2F8E3B15F1DFC0FF07FD1F769F3C0672", // 5  - PLB Location: National Location 
  "FFFE2F8F4DCBC01ACD004BB10D4E79C4DD86", // 6  - RLS Location Protocol 
  "FFFE2F96E3AAAAAA7FDFF8211F3583E0FAA8", // 7  - Std Loc. ELT 24-bit Address Protocol 
  "FFFE2F96E61B0CAE7FDFFF0E58B583E0FAA8", // 8  - Standard Location Protocol - EPIRB (Serial) 
  "FFFE2F9F7B00F9E8EC737E5312378A1802B0", // 9  - PLB Location: National Location 
  "FFFE2FA0D205F260850F3DC9E0B70B6E4FD7", // 10 - Standard Location Protocol EPIRB-MMSI 
  "FFFE2FA3E7B10016150D364D8B3689C09437", // 11 - Standard Location Protocol - PLB (Serial) 
  "FFFE2FA5DC19E1A07FDFFE5C803483E0FCCA", // 12 - Std. Location ship security protocol (SSAS) 
  "FFFE2FCE46E76EF8C00C2BAA31CFE0FF0F61", // 13 - Serial user Location Protocol (ELT with Aircraft 24-bit Address)
  "FFFE2FD9D4EB28140AA6893F16A2C67282EC", // 14 - Maritime User Protocol MMSI - EPIRB 
  "FFFE2FDF76A9A9C800174DE27BE1F0277E45", // 15 - Serial user - Float Free EPIRB with Serial Identification Number 
  "FFFE2FDF77200000001168C610AFE0FF0146", // 16 - Serial user - Non Float Free EPIRB with Serial Identification 
  "FFFE2FE0DDAE599508268E4C05E054651307", // 17 - Radio Call Sign - EPIRB 
  "FFFE2FE29325ACB12E938B671E0FE0FF0F61", // 18 - ELT Aviation User 
  "FFFE2FEDF67B7182038C2F0E10CFE0FF0F61", // 19 - Serial user -	ELT with Aircraft Operator Designator & Serial Number 
  "FFFE2FA157B081437FDFF8B4833783E0F66C", // 20 - Standard Location Protocol - PLB (Serial) 
  "FFFED096ED09900149D4D467EE0851A3B2E8", // 21 - RLS Location Protocol
  "FFFE2F8DB345B146202DDF3C71F59BAB7072"  // 22 - ELT 24 bits
  };
static const int sampleFrameNumber = 22;

int curFrame = 0;

/**
 * @brief Read the sample frame with the provided index into the buffer
 * 
 * @param frameBuffer the buffer to read the sample into
 * @param frameIndex the sample frame index
 */
void readNextSample(byte* frameBuffer)
{ // Read the frame content
  readHexString(frameBuffer,frames[curFrame]);
  curFrame++;
  if(curFrame >= sampleFrameNumber)
  {
    curFrame = 0;
  }
}
