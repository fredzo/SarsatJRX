#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <extra/libs/qrcode/lv_qrcode.h>
#include <ui/Ui.h>
#include <ui/UiBeacon.h>

// Beacon info
#define LINE_HEIGHT         16
#define FRAME_MODE_LABEL    F("Frame mode :")
#define FRAME_MODE_WIDTH    140
#define INFO_LABEL          F("Info :")
#define INFO_LABEL_WIDTH    80
#define SERIAL_LABEL        F("Serial # :")
#define SERIAL_LABEL_WIDTH  120
#define MAIN_LABEL          F("Main loc. device :")
#define MAIN_LABEL_WIDTH    200
#define AUX_LABEL           F("Aux. loc. device :")
#define AUX_LABEL_WIDTH     200
#define LOCATION_LABEL      F("Location :")
#define HEX_ID_LABEL        F("Hex ID:")
#define HEX_ID_WIDTH        90
#define DATA_LABEL          F("Data :")
#define DATA_LABEL_WIDTH    70
#define BCH1_OK_LABEL       F("BCH1-OK")
#define BCH1_KO_LABEL       F("BCH1-KO")
#define BCH2_OK_LABEL       F("BCH2-OK")
#define BCH2_KO_LABEL       F("BCH2-KO")
#define BCH_LABEL_WIDTH     90
// QR code
#define QR_SIZE             130
// URL templates
#define MAPS_URL_TEMPLATE   "https://www.google.com/maps/search/?api=1&query=%s%%2C%s"
#define BEACON_URL_TEMPALTE "https://cryptic-earth-89063heroku-20.herokuapp.com/decoded/%s"

lv_obj_t * tabview;
lv_obj_t * mapQr;
lv_obj_t * beaconQr;

void createInfoTab(lv_obj_t * tab)
{
    lv_obj_t * label = lv_label_create(tab);
    lv_label_set_text(label, "First tab");
}

void createMapTab(lv_obj_t * tab)
{
    lv_obj_t * label = lv_label_create(tab);
    int tabWidth = lv_obj_get_width(tab);
    lv_label_set_text(label, "Location");
    // Map QR Code
    mapQr = lv_qrcode_create(tab, QR_SIZE, lv_color_black(), lv_color_white());
    // Add a border with bg_color
    lv_obj_set_style_border_color(mapQr, lv_color_white(), 0);
    lv_obj_set_style_border_width(mapQr, 5, 0);
    lv_obj_align(mapQr,LV_ALIGN_BOTTOM_RIGHT,0,0);
}

void createBeaconTab(lv_obj_t * tab)
{
    lv_obj_t * label = lv_label_create(tab);
    lv_label_set_text(label, "Beacon");
    // Map QR Code
    beaconQr = lv_qrcode_create(tab, QR_SIZE, lv_color_black(), lv_color_white());
    // Add a border with bg_color
    lv_obj_set_style_border_color(beaconQr, lv_color_white(), 0);
    lv_obj_set_style_border_width(beaconQr, 5, 0);
    lv_obj_center(beaconQr);
    lv_obj_align(beaconQr,LV_ALIGN_BOTTOM_RIGHT,0,0);
}

void createDataTab(lv_obj_t * tab)
{
    lv_obj_t * label = lv_label_create(tab);
    lv_label_set_text(label, "Data");
}

void uiBeaconCreateTabView(lv_obj_t * cont)
{
    // Tab view
    tabview = lv_tabview_create(cont, LV_DIR_LEFT, 50);
    lv_obj_add_flag(tabview, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
    //lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    //lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_RIGHT, LV_PART_ITEMS | LV_STATE_CHECKED);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Info");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Map");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Bcn.");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "Data");
    lv_obj_add_style(tab1, &style_pad_small, 0);
    lv_obj_add_style(tab2, &style_pad_small, 0);
    lv_obj_add_style(tab3, &style_pad_small, 0);
    lv_obj_add_style(tab4, &style_pad_small, 0);

    //lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_AMBER, 3), 0);
    //lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);

    /*Add content to the tabs*/
    createInfoTab(tab1);
    createMapTab(tab2);
    createBeaconTab(tab3);
    createDataTab(tab4);
    // Hide tab view for now
    lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);
}

void uiBeaconSetBeacon(Beacon* beacon)
{
    char buffer[128];
    // Set map QR data
    beacon->location.formatFloatLocation(buffer,MAPS_URL_TEMPLATE);
    #ifdef SERIAL_OUT 
    Serial.println(buffer); 
    #endif
    const char * data = BEACON_URL_TEMPALTE;
    lv_qrcode_update(mapQr, buffer, strlen(buffer));
    // Set beacon QR data
    sprintf(buffer,BEACON_URL_TEMPALTE, toHexString(beacon->frame, false, 3, beacon->longFrame ? 18 : 14).c_str());
    #ifdef SERIAL_OUT 
    Serial.println(buffer); 
    #endif
    lv_qrcode_update(beaconQr, buffer, strlen(buffer));
}
