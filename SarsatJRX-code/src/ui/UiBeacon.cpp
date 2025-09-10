#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <extra/libs/qrcode/lv_qrcode.h>
#include <ui/Ui.h>
#include <ui/UiBeacon.h>

// Beacon info
#define BEACON_HEADER_HEIGHT    24
#define MAIN_BLOC_HEIGHT    BEACON_HEADER_HEIGHT+8*LINE_HEIGHT
#define INFO_LABEL          "Info:"
#define INFO_LABEL_WIDTH    50
#define SERIAL_LABEL        "Serial #:"
#define SERIAL_LABEL_WIDTH  75
#define MAIN_LABEL          "Main loc. device:"
#define MAIN_LABEL_WIDTH    144
#define AUX_LABEL           "Aux. loc. device:"
#define AUX_LABEL_WIDTH     144
#define LOCATION_LABEL      "Location:"
#define CONTROL_LABEL       "Control:"
#define CONTROL_TITLE_WIDTH 75
#define CONTROL_LABEL_WIDTH 80
#define BCH1_OK_LABEL       "BCH1-OK"
#define BCH1_KO_LABEL       "BCH1-KO"
#define BCH2_OK_LABEL       "BCH2-OK"
#define BCH2_KO_LABEL       "BCH2-KO"
#define HEX_ID_LABEL        "Hex ID:"
#define HEX_ID_WIDTH        75
#define DATA_LABEL          "Data:"
#define DATA_LABEL_WIDTH    70
#define DATE_LABEL          "Date:"
#define DATE_LABEL_WIDTH    55
#define BCH_LABEL_WIDTH     90
// QR code
#define QR_SIZE             130
// URL templates
#define MAPS_URL_TEMPLATE   "https://www.google.com/maps/search/?api=1&query=%s%%2C%s"
#define BEACON_URL_TEMPALTE "https://decoder2.herokuapp.com/decoded/%s"

lv_obj_t * beaconMainBloc;
lv_obj_t * beaconTabview;
lv_obj_t * mapQr;
lv_obj_t * beaconQr;

// Main bloc
lv_obj_t * beaconTimeLabel;
lv_obj_t * frameModeLabel;
lv_obj_t * infoTitle;
lv_obj_t * infoLabel1;
lv_obj_t * infoLabel2;
lv_obj_t * infoLabel3;
lv_obj_t * locationTitle;
lv_obj_t * locationLabel1;
lv_obj_t * locationLabel2;
lv_obj_t * locationLabel3;
lv_obj_t * controlTitle;
lv_obj_t * controlLabel1;
lv_obj_t * controlLabel2;
// Info
lv_obj_t * infoDateTitle;
lv_obj_t * infoDateLabel;
lv_obj_t * infoSerialTitle;
lv_obj_t * infoSerialLabel;
lv_obj_t * infoMldTitle;
lv_obj_t * infoMldLabel;
lv_obj_t * infoSldTitle;
lv_obj_t * infoSldLabel;
// Map
lv_obj_t * mapHexIdTitle;
lv_obj_t * mapHexIdLabel;
lv_obj_t * mapSerialTitle;
lv_obj_t * mapSerialLabel;
lv_obj_t * mapMldTitle;
lv_obj_t * mapMldLabel;
lv_obj_t * mapSldTitle;
lv_obj_t * mapSldLabel;
// Beacon
lv_obj_t * beaconHexIdTitle;
lv_obj_t * beaconHexIdLabel;
lv_obj_t * beaconDataTitle;
lv_obj_t * beaconDataLabel1;
lv_obj_t * beaconDataLabel2;
// Data
lv_obj_t * dataHexIdTitle;
lv_obj_t * dataHexIdLabel;
lv_obj_t * dataDataTitle;
lv_obj_t * dataDataLabel1;
lv_obj_t * dataDataLabel2;


void createMainBloc(lv_obj_t * bloc, int tabWidth)
{   // Frame mode
    int currentY = 0;

    // Time
    beaconTimeLabel = uiCreateLabel(bloc,&style_time,"",0,currentY,80,BEACON_HEADER_HEIGHT);

    frameModeLabel = uiCreateLabel(bloc,&style_header,"",-20,currentY,LV_PCT(100),BEACON_HEADER_HEIGHT);
    currentY+=BEACON_HEADER_HEIGHT;

    // Info           
    infoTitle =  uiCreateLabel(bloc,&style_section_title,INFO_LABEL,0,currentY,INFO_LABEL_WIDTH,LINE_HEIGHT);
    // Info line 1
    infoLabel1 = uiCreateLabel(bloc,&style_section_text,"",INFO_LABEL_WIDTH,currentY,tabWidth-INFO_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Protocol desciption
    infoLabel2 = uiCreateLabel(bloc,&style_section_text,"",0,currentY,LV_PCT(100),LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Protocol additional data
    infoLabel3 = uiCreateLabel(bloc,&style_section_text,"",0,currentY,LV_PCT(100),LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Location
    locationTitle =  uiCreateLabel(bloc,&style_section_title,LOCATION_LABEL,0,currentY,LV_PCT(100),LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    locationLabel1 = uiCreateLabel(bloc,&style_section_text,"",0,currentY,LV_PCT(100),LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    locationLabel2 = uiCreateLabel(bloc,&style_section_text,"",0,currentY,LV_PCT(100),LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    locationLabel3 = uiCreateLabel(bloc,&style_section_text,"",0,currentY,LV_PCT(100),LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Control codes
    controlTitle =  uiCreateLabel(bloc,&style_section_title,CONTROL_LABEL,0,currentY,CONTROL_TITLE_WIDTH,LINE_HEIGHT);
    controlLabel1 = uiCreateLabel(bloc,&style_section_text,"",CONTROL_TITLE_WIDTH,currentY,CONTROL_LABEL_WIDTH,LINE_HEIGHT);
    controlLabel2 = uiCreateLabel(bloc,&style_section_text,"",CONTROL_TITLE_WIDTH+CONTROL_LABEL_WIDTH,currentY,CONTROL_LABEL_WIDTH,LINE_HEIGHT);
    //currentY+=LINE_HEIGHT;
}

void createInfoTab(lv_obj_t * tab, int currentY, int tabWidth)
{  
    // Date
    infoDateTitle =  uiCreateLabel(tab,&style_section_title,DATE_LABEL,0,currentY,DATE_LABEL_WIDTH,LINE_HEIGHT);
    infoDateLabel = uiCreateLabel(tab,&style_section_text,"",DATE_LABEL_WIDTH,currentY,tabWidth-DATE_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Serial #
    infoSerialTitle =  uiCreateLabel(tab,&style_section_title,SERIAL_LABEL,0,currentY,SERIAL_LABEL_WIDTH,LINE_HEIGHT);
    infoSerialLabel = uiCreateLabel(tab,&style_section_text,"",SERIAL_LABEL_WIDTH,currentY,tabWidth-SERIAL_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Main location deveice
    infoMldTitle =  uiCreateLabel(tab,&style_section_title,MAIN_LABEL,0,currentY,MAIN_LABEL_WIDTH,LINE_HEIGHT);
    infoMldLabel = uiCreateLabel(tab,&style_section_text,"",MAIN_LABEL_WIDTH,currentY,tabWidth-MAIN_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Secondary location device
    infoSldTitle =  uiCreateLabel(tab,&style_section_title,AUX_LABEL,0,currentY,AUX_LABEL_WIDTH,LINE_HEIGHT);
    infoSldLabel = uiCreateLabel(tab,&style_section_text,"",AUX_LABEL_WIDTH,currentY,tabWidth-AUX_LABEL_WIDTH,LINE_HEIGHT);
    //currentY+=LINE_HEIGHT;
}

void createMapTab(lv_obj_t * tab, int currentY, int tabWidth)
{
    // Hex ID
    mapHexIdTitle =  uiCreateLabel(tab,&style_section_title,HEX_ID_LABEL,0,currentY,HEX_ID_WIDTH,LINE_HEIGHT);
    mapHexIdLabel = uiCreateLabel(tab,&style_section_text,"",HEX_ID_WIDTH,currentY,tabWidth-HEX_ID_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Serial #
    mapSerialTitle =  uiCreateLabel(tab,&style_section_title,SERIAL_LABEL,0,currentY,SERIAL_LABEL_WIDTH,LINE_HEIGHT);
    mapSerialLabel = uiCreateLabel(tab,&style_section_text,"",SERIAL_LABEL_WIDTH,currentY,tabWidth-SERIAL_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Main location deveice
    mapMldTitle =  uiCreateLabel(tab,&style_section_title,MAIN_LABEL,0,currentY,MAIN_LABEL_WIDTH,LINE_HEIGHT);
    mapMldLabel = uiCreateLabel(tab,&style_section_text,"",MAIN_LABEL_WIDTH,currentY,tabWidth-MAIN_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Secondary location device
    mapSldTitle =  uiCreateLabel(tab,&style_section_title,AUX_LABEL,0,currentY,AUX_LABEL_WIDTH,LINE_HEIGHT);
    mapSldLabel = uiCreateLabel(tab,&style_section_text,"",AUX_LABEL_WIDTH,currentY,tabWidth-AUX_LABEL_WIDTH,LINE_HEIGHT);
    //currentY+=LINE_HEIGHT;

    // Map QR Code
    mapQr = lv_qrcode_create(tab, QR_SIZE, lv_color_black(), lv_color_white());
    // Add a border with bg_color
    lv_obj_set_style_border_color(mapQr, lv_color_white(), 0);
    lv_obj_set_style_border_width(mapQr, 5, 0);
    lv_obj_align(mapQr,LV_ALIGN_BOTTOM_RIGHT,0,0);
}

void createBeaconTab(lv_obj_t * tab, int currentY, int tabWidth)
{
    // Hex ID
    beaconHexIdTitle =  uiCreateLabel(tab,&style_section_title,HEX_ID_LABEL,0,currentY,HEX_ID_WIDTH,LINE_HEIGHT);
    beaconHexIdLabel = uiCreateLabel(tab,&style_section_text,"",HEX_ID_WIDTH,currentY,tabWidth-HEX_ID_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Data
    beaconDataTitle =  uiCreateLabel(tab,&style_section_title,DATA_LABEL,0,currentY,LV_PCT(100),LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    beaconDataLabel1 = uiCreateLabel(tab,&style_section_text,"",0,currentY,LV_PCT(100),LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    beaconDataLabel2 = uiCreateLabel(tab,&style_section_text,"",0,currentY,LV_PCT(100),LINE_HEIGHT);

    // Map QR Code
    beaconQr = lv_qrcode_create(tab, QR_SIZE, lv_color_black(), lv_color_white());
    // Add a border with bg_color
    lv_obj_set_style_border_color(beaconQr, lv_color_white(), 0);
    lv_obj_set_style_border_width(beaconQr, 5, 0);
    lv_obj_center(beaconQr);
    lv_obj_align(beaconQr,LV_ALIGN_BOTTOM_RIGHT,0,0);
}

void createDataTab(lv_obj_t * tab, int currentY, int tabWidth)
{
    // Hex ID
    dataHexIdTitle =  uiCreateLabel(tab,&style_section_title,HEX_ID_LABEL,0,currentY,HEX_ID_WIDTH,LINE_HEIGHT);
    dataHexIdLabel = uiCreateLabel(tab,&style_section_text,"",HEX_ID_WIDTH,currentY,tabWidth-HEX_ID_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Data
    dataDataTitle =  uiCreateLabel(tab,&style_section_title,DATA_LABEL,0,currentY,LV_PCT(100),LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    dataDataLabel1 = uiCreateLabel(tab,&style_section_text,"",0,currentY,LV_PCT(100),LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    dataDataLabel2 = uiCreateLabel(tab,&style_section_text,"",0,currentY,LV_PCT(100),LINE_HEIGHT);
}

void uiBeaconCreateView(lv_obj_t * cont)
{   // Tab view
    beaconTabview = lv_tabview_create(cont, LV_DIR_LEFT, 50);
    // Hide tab view for now
    lv_obj_add_flag(beaconTabview, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(beaconTabview,uiBackgroundColor,0);

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(beaconTabview);
    //lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    //lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_RIGHT, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_font(tab_btns, &additional_symbols_medium, LV_PART_ITEMS | LV_STATE_DEFAULT);

    /*Add 4 tabs */
    lv_obj_t * tab1 = lv_tabview_add_tab(beaconTabview, SYMBOL_LOCATION);
    lv_obj_t * tab2 = lv_tabview_add_tab(beaconTabview, SYMBOL_SYSTEM);
    lv_obj_t * tab3 = lv_tabview_add_tab(beaconTabview, SYMBOL_EXTERNAL_LINK);
    lv_obj_t * tab4 = lv_tabview_add_tab(beaconTabview, SYMBOL_MAG_GLASS);
    lv_obj_add_style(tab1, &style_pad_small, 0);
    lv_obj_add_style(tab2, &style_pad_small, 0);
    lv_obj_add_style(tab3, &style_pad_small, 0);
    lv_obj_add_style(tab4, &style_pad_small, 0);
    int tabWidth = lv_obj_get_width(tab1) - 8; // 2*4 px padding

    //lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_AMBER, 3), 0);
    //lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);

    // Main bloc
    beaconMainBloc = lv_obj_create(cont);
    lv_obj_add_style(beaconMainBloc,&style_pad_none,0);
    lv_obj_set_align(beaconMainBloc,LV_ALIGN_TOP_RIGHT);
    lv_obj_set_size(beaconMainBloc,tabWidth+4,MAIN_BLOC_HEIGHT);
    lv_obj_set_style_bg_opa(beaconMainBloc,LV_OPA_0,0);
    lv_obj_set_style_border_width(beaconMainBloc,0,0);
    createMainBloc(beaconMainBloc,tabWidth);
    // Hide main bloc for now
    lv_obj_add_flag(beaconMainBloc, LV_OBJ_FLAG_HIDDEN);

    // Add content to the tabs
    int currentY = MAIN_BLOC_HEIGHT-4; // Remove padding
    createMapTab(tab1,currentY,tabWidth);
    createInfoTab(tab2,currentY,tabWidth);
    createBeaconTab(tab3,currentY,tabWidth);
    createDataTab(tab4,currentY,tabWidth);
    lv_obj_clear_flag(lv_tabview_get_content(beaconTabview), LV_OBJ_FLAG_SCROLLABLE);
}

void uiBeaconSetBeacon(Beacon* beacon)
{
    char buffer[128];
    // Date / time
    String date = beacon->date.getDateString();
    String time = beacon->date.getTimeString();
    lv_label_set_text(beaconTimeLabel,time.c_str());
    lv_snprintf(buffer,sizeof(buffer),"%s - %s",date.c_str(),time.c_str());
    lv_label_set_text(infoDateLabel,buffer);
    // Frame mode
    const char* frameMode;
    if (beacon->frameMode == Beacon::FrameMode::SELF_TEST) 
    {  // Self-test message frame synchronisation byte
        frameMode = "Self-test 406";
    }
    else if (beacon->frameMode == Beacon::FrameMode::NORMAL) 
    { // Normal message frame synchronisation byte
        frameMode = "Distress 406";
    }
    else
    { // Unknown fram format
        frameMode = "Unknown 406";
    }
    lv_label_set_text(frameModeLabel,frameMode);
    #ifdef SERIAL_OUT 
    Serial.println(frameMode);
    #endif
    // Protocol name
    if(beacon->protocol->isUnknown())
    {
        // Unknwon protocol (xxxx)
        lv_snprintf(buffer,sizeof(buffer),"%s (%lu)",beacon->getProtocolName().c_str(),beacon->protocolCode);
        lv_label_set_text(infoLabel1,buffer);
        #ifdef SERIAL_OUT 
        Serial.println(buffer);   
        #endif
    }
    else
    {
        lv_label_set_text(infoLabel1,beacon->getProtocolName().c_str());
        #ifdef SERIAL_OUT 
        Serial.println(beacon->getProtocolName());   
        #endif
    }
    // Protocol description
    lv_label_set_text(infoLabel2,beacon->getProtocolDesciption().c_str());
    #ifdef SERIAL_OUT 
    Serial.println(beacon->getProtocolDesciption());   
    #endif
    // Protocol additional data
    if(beacon->hasAdditionalData)
    {   // Additionnal protocol data
        lv_label_set_text(infoLabel3,beacon->additionalData.c_str());
        #ifdef SERIAL_OUT 
        Serial.println(beacon->additionalData);   
        #endif
    }
    else
    {
        lv_label_set_text(infoLabel3,"");
    }

    // Location
    // Country
    String country = beacon->country.toString();
    lv_label_set_text(locationLabel1,country.c_str());
    #ifdef SERIAL_OUT 
    Serial.println(country);
    #endif
    // Coordinates
    bool locationKnown = !beacon->location.isUnknown(); 
    if (beacon->longFrame) 
    {   // Long frame
        String locationSexa = beacon->location.toString(true);
        String locationDeci = beacon->location.toString(false);
        lv_label_set_text(locationLabel2,locationSexa.c_str());
        #ifdef SERIAL_OUT 
        Serial.println(locationSexa);
        #endif
        if(locationKnown)
        {
            lv_label_set_text(locationLabel3,locationDeci.c_str());
        #ifdef SERIAL_OUT 
            Serial.println(locationDeci);
        #endif
        }
        else
        {
            lv_label_set_text(locationLabel3,"");
        }
    }
    else 
    {   // Short frame
        lv_label_set_text(locationLabel2,"22 HEX. No location");
        lv_label_set_text(locationLabel3,"");
    }
    // Control codes
    // Append BCH values before frame data
    lv_label_set_text(controlLabel1,beacon->isBch1Valid() ? BCH1_OK_LABEL : BCH1_KO_LABEL);
    lv_obj_set_style_text_color(controlLabel1,beacon->isBch1Valid() ? uiOkColor : uiKoColor,0);
    if(beacon->longFrame && beacon->hasBch2) 
    {   // No second proteced field in short frames
        lv_label_set_text(controlLabel2,beacon->isBch2Valid() ? BCH2_OK_LABEL : BCH2_KO_LABEL);
        lv_obj_set_style_text_color(controlLabel2,beacon->isBch2Valid() ? uiOkColor : uiKoColor,0);
    }
    else
    {
        lv_label_set_text(controlLabel2,"");
    }
    #ifdef SERIAL_OUT 
    if(!beacon->isBch1Valid())
    {
        Serial.println("Wrong BCH1 value :");
        Serial.print("Found    :");
        Serial.println(beacon->bch1,2);
        Serial.print("Expected :");
        Serial.println(beacon->computedBch1,2);
    }
    if(beacon->longFrame && beacon->hasBch2 && !beacon->isBch2Valid())
    {
        Serial.println("Wrong BCH2 value :");
        Serial.print("Found    :");
        Serial.println(beacon->bch2,2);
        Serial.print("Expected :");
        Serial.println(beacon->computedBch2,2);
    }
    #endif

    // Hex ID
    // "Id = 0x1122334455667788"
    uint32_t msb = beacon->identifier >> 32;
    uint32_t lsb = beacon->identifier;
    lv_snprintf(buffer,sizeof(buffer),"%07lX%08lX",msb,lsb);
    lv_label_set_text(mapHexIdLabel,buffer);
    lv_label_set_text(beaconHexIdLabel,buffer);
    lv_label_set_text(dataHexIdLabel,buffer);
    #ifdef SERIAL_OUT 
    Serial.println(buffer);   
    #endif  

    // Serial #
    if(beacon->hasSerialNumber)
    { // Serial number
        String serial = beacon->serialNumber;
        lv_label_set_text(infoSerialTitle,SERIAL_LABEL);
        lv_label_set_text(infoSerialLabel,serial.c_str());
        lv_label_set_text(mapSerialTitle,SERIAL_LABEL);
        lv_label_set_text(mapSerialLabel,serial.c_str());
        #ifdef SERIAL_OUT 
        Serial.print(SERIAL_LABEL);   
        Serial.println(serial);   
        #endif
    }
    else
    {
        lv_label_set_text(infoSerialTitle,"");
        lv_label_set_text(infoSerialLabel,"");
        lv_label_set_text(mapSerialTitle,"");
        lv_label_set_text(mapSerialLabel,"");
    }

    // Location devices
    if(beacon->hasMainLocatingDevice())
    { // Main locating device
        String mld = beacon->getMainLocatingDeviceName();
        lv_label_set_text(infoMldLabel, mld.c_str());
        lv_label_set_text(mapMldLabel, mld.c_str());
        #ifdef SERIAL_OUT 
        Serial.print(MAIN_LABEL);   
        Serial.println(mld);   
        #endif
    }
    else
    {
        lv_label_set_text(infoMldLabel,"");
        lv_label_set_text(mapMldLabel,"");
    }

    if(beacon->hasAuxLocatingDevice())
    {   // Auxiliary locating device
        String sld = beacon->getAuxLocatingDeviceName();
        lv_label_set_text(infoSldLabel,sld.c_str());
        lv_label_set_text(mapSldLabel,sld.c_str());
        #ifdef SERIAL_OUT 
        Serial.print(AUX_LABEL);   
        Serial.println(sld);   
        #endif
    }
    else
    {
        lv_label_set_text(infoSldLabel,"");
        lv_label_set_text(mapSldLabel,"");
    }

    // Data
    String line1 = toHexString(beacon->frame,true,3,11);
    // HEX ID 30 Hexa or HEX ID 22 Hexa bit 26 to 112
    String line2 = beacon->longFrame ? toHexString(beacon->frame,true,11,18) : toHexString(beacon->frame,true,11,14);
    lv_label_set_text(beaconDataLabel1,line1.c_str());
    lv_label_set_text(beaconDataLabel2,line2.c_str());
    lv_label_set_text(dataDataLabel1,line1.c_str());
    lv_label_set_text(dataDataLabel2,line2.c_str());

    // Set map QR data
    beacon->location.formatFloatLocation(buffer,sizeof(buffer),MAPS_URL_TEMPLATE);
    #ifdef SERIAL_OUT 
    Serial.println(buffer); 
    #endif
    const char * data = BEACON_URL_TEMPALTE;
    lv_qrcode_update(mapQr, buffer, strlen(buffer));
    // Set beacon QR data
    lv_snprintf(buffer,sizeof(buffer),BEACON_URL_TEMPALTE, toHexString(beacon->frame, false, 3, beacon->longFrame ? 18 : 14).c_str());
    #ifdef SERIAL_OUT 
    Serial.println(buffer); 
    #endif
    lv_qrcode_update(beaconQr, buffer, strlen(buffer));
}
