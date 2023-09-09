#include <Ui.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <extra/libs/qrcode/lv_qrcode.h>

// Defines
// Enable serial out
#define SERIAL_OUT

// Header
#define HEADER_HEIGHT     30
#define HEADER_TEXT       "- SarsatJRX -"
#define HEADER_PAGES_TEMPLATE "%02d/%02d"
#define HEADER_PAGES_X    4
#define HEADER_PAGES_Y    (HEADER_HEIGHT-12)/2
#define HEADER_TIME_X     56
#define HEADER_BUTTON_LEFT    DISPLAY_WIDTH/3
#define HEADER_BUTTON_RIGHT   (2*DISPLAY_WIDTH)/3
#define HEADER_BUTTON_BOTTOM  2*HEADER_HEIGHT
// Header LEDS
#define LED_SIG_1_X       DISPLAY_WIDTH-(4*(2*LED_RADIUS+6))+LED_RADIUS
#define LED_SIG_1_Y       HEADER_HEIGHT/2
//Display::Led ledSig1 =    Display::Led(LED_SIG_1_X,LED_SIG_1_Y,Display::LedColor::Green);
#define LED_SIG_2_X       DISPLAY_WIDTH-(3*(2*LED_RADIUS+6))+LED_RADIUS
#define LED_SIG_2_Y       LED_SIG_1_Y
//Display::Led ledSig2 =    Display::Led(LED_SIG_2_X,LED_SIG_2_Y,Display::LedColor::Green);
#define LED_SIG_IN_FRAME_X      DISPLAY_WIDTH-(2*(2*LED_RADIUS+6))+LED_RADIUS
#define LED_SIG_IN_FRAME_Y      LED_SIG_1_Y
//Display::Led ledInFrame =       Display::Led(LED_SIG_IN_FRAME_X,LED_SIG_IN_FRAME_Y,Display::LedColor::Red);
#define LED_SIG_FRAME_R_X       DISPLAY_WIDTH-(1*(2*LED_RADIUS+6))+LED_RADIUS
#define LED_SIG_FRAME_R_Y       LED_SIG_1_Y
//Display::Led ledFrameReceived = Display::Led(LED_SIG_FRAME_R_X,LED_SIG_FRAME_R_Y,Display::LedColor::Blue);
// Power and time
#define HEADER_POWER_TEMPLATE "%sV"   // "4.98V"
#define HEADER_POWER_Y    (HEADER_HEIGHT-10)/2
#define HEADER_TIME_Y     HEADER_POWER_Y

// Footer
#define FOOTER_HEIGHT       60
#define FOOTER_LABEL_X      DISPLAY_WIDTH/2
#define FOOTER_LABEL_Y      DISPLAY_HEIGHT-SMALL_BUTTON_HEIGHT+16
#define FOOTER_WAIT_LABEL   "Waiting for the wave..."
#define FOOTER_FRAME_LABEL  "Frame received !"
#define FOOTER_RADIUS_MAX   SMALL_BUTTON_HEIGHT/2-5
#define FOOTER_RADIUS_MIN   FOOTER_RADIUS_MAX-4
#define FOOTER_SPINNER_X    SMALL_BUTTON_WIDTH+FOOTER_RADIUS_MAX+5
#define FOOTER_SPINNER_Y    DISPLAY_HEIGHT-(SMALL_BUTTON_HEIGHT/2)

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
// MAPS and BEACON buttons
#define QR_SIZE             130
#define MAPS_BUTTON_X     DISPLAY_WIDTH-BUTTON_WIDTH
#define MAPS_BUTTON_Y     HEADER_BOTTOM-1
#define MAPS_BUTTON_CAPTION "MAPS"
//Display::Button mapsButton = Display::Button(MAPS_BUTTON_X,MAPS_BUTTON_Y,MAPS_BUTTON_CAPTION,Display::ButtonStyle::NORMAL);
#define BEACON_BUTTON_X   MAPS_BUTTON_X
#define BEACON_BUTTON_Y   HEADER_BOTTOM+BUTTON_HEIGHT+2
#define BEACON_BUTTON_CAPTION "BEACON"
//Display::Button beaconButton = Display::Button(BEACON_BUTTON_X,BEACON_BUTTON_Y,BEACON_BUTTON_CAPTION,Display::ButtonStyle::NORMAL);
// Navigation BUTTONS
#define PREVIONS_BUTTON_X   0
#define PREVIONS_BUTTON_Y   DISPLAY_HEIGHT-SMALL_BUTTON_HEIGHT-1
#define PREVIONS_BUTTON_CAPTION "<"
//Display::Button previousButton = Display::Button(PREVIONS_BUTTON_X,PREVIONS_BUTTON_Y,PREVIONS_BUTTON_CAPTION,Display::ButtonStyle::SMALL);
#define NEXT_BUTTON_X   DISPLAY_WIDTH-SMALL_BUTTON_WIDTH-1
#define NEXT_BUTTON_Y   DISPLAY_HEIGHT-SMALL_BUTTON_HEIGHT-1
#define NEXT_BUTTON_CAPTION ">"
//Display::Button nextButton = Display::Button(NEXT_BUTTON_X,NEXT_BUTTON_Y,NEXT_BUTTON_CAPTION,Display::ButtonStyle::SMALL);
// URL templates
#define MAPS_URL_TEMPLATE   "https://www.google.com/maps/search/?api=1&query=%s%%2C%s"
#define BEACON_URL_TEMPALTE "https://cryptic-earth-89063heroku-20.herokuapp.com/decoded/%s"



/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * tv;
static lv_obj_t * calendar;
static lv_style_t style_text_mono;
static lv_style_t style_title;
static lv_style_t style_footer;
static lv_style_t style_icon;
static lv_style_t style_bullet;
static lv_style_t style_pad_small;
static lv_style_t style_pad_tiny;
static lv_style_t style_pad_none;
static lv_style_t style_tag;

static lv_obj_t * meter1;
static lv_obj_t * meter2;
static lv_obj_t * meter3;

static lv_obj_t * chart1;
static lv_obj_t * chart2;
static lv_obj_t * chart3;

static lv_chart_series_t * ser1;
static lv_chart_series_t * ser2;
static lv_chart_series_t * ser3;
static lv_chart_series_t * ser4;

static const lv_font_t * font_large;
static const lv_font_t * font_medium;
static const lv_font_t * font_normal;
static const lv_font_t * font_mono;

lv_obj_t * timeLabel;
lv_obj_t * powerLabel;
lv_obj_t * headerledSig2;
lv_obj_t * ledSig1;
lv_obj_t * ledSig2;
lv_obj_t * ledInFrame;
lv_obj_t * ledFrameReceived;
lv_obj_t * pagesLabel;
lv_obj_t * footerLabel;
lv_obj_t * spinner;
lv_obj_t * tabview;
lv_obj_t * mapQr;
lv_obj_t * beaconQr;

extern void readNextSampleFrame();
extern void previousFrame();
extern void nextFrame();

static void settings_handler(lv_event_t * e)
{
    readNextSampleFrame();
}

static void previous_handler(lv_event_t * e)
{
    previousFrame();
}

static void next_handler(lv_event_t * e)
{
    nextFrame();
}

void createUi()
{
    font_medium = &lv_font_montserrat_18;
    font_normal = &lv_font_montserrat_12;
    font_large = &lv_font_montserrat_24;
    font_mono = &lv_font_unscii_8;

    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);

    lv_style_init(&style_text_mono);
    lv_style_set_text_font(&style_text_mono, font_mono);
    lv_style_set_text_align(&style_text_mono, LV_TEXT_ALIGN_CENTER);


    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);
    lv_style_set_text_align(&style_title, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&style_footer);
    lv_style_set_text_font(&style_footer, font_medium);
    lv_style_set_text_align(&style_footer, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&style_pad_small);
    lv_style_set_pad_all(&style_pad_small, 4);
    lv_style_set_pad_row(&style_pad_small, 4);
    lv_style_set_pad_column(&style_pad_small, 4);

    lv_style_init(&style_pad_tiny);
    lv_style_set_pad_all(&style_pad_tiny, 1);
    lv_style_set_pad_row(&style_pad_tiny, 1);
    lv_style_set_pad_column(&style_pad_tiny, 1);

    lv_style_init(&style_pad_none);
    lv_style_set_pad_all(&style_pad_none, 0);
    lv_style_set_pad_row(&style_pad_none, 0);
    lv_style_set_pad_column(&style_pad_none, 0);

    lv_style_init(&style_tag);
    lv_style_set_text_font(&style_tag, font_mono);
    lv_style_set_radius(&style_tag, 5);
    lv_style_set_bg_opa(&style_tag, LV_OPA_COVER);
    lv_style_set_bg_color(&style_tag, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_border_width(&style_tag, 2);
    lv_style_set_border_color(&style_tag, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_pad_all(&style_tag, 10);
    //lv_style_set_text_color(&style_tag, lv_palette_main(LV_PALETTE_BLUE));
    
    lv_obj_t * btn;
    // Header
    lv_obj_t * win = lv_win_create(lv_scr_act(),HEADER_HEIGHT);
    lv_obj_t * header = lv_win_get_header(win);
    lv_obj_add_style(header, &style_pad_tiny, 0);
    // Time
    timeLabel = lv_label_create(header);
    lv_label_set_long_mode(timeLabel, LV_LABEL_LONG_DOT);
    lv_obj_add_style(timeLabel,&style_text_mono,0);
    lv_obj_set_size(timeLabel, 80, LV_PCT(100));
    lv_obj_set_style_pad_top(timeLabel,HEADER_TIME_Y,0);

    // Title
    lv_obj_t * title = lv_win_add_title(win, HEADER_TEXT);
    lv_obj_add_style(title,&style_title,0);
    // Power
    powerLabel = lv_label_create(header);
    lv_label_set_long_mode(powerLabel, LV_LABEL_LONG_DOT);
    lv_obj_add_style(powerLabel,&style_text_mono,0);
    lv_obj_set_size(powerLabel, 60, LV_PCT(100));
    lv_obj_set_style_pad_top(powerLabel,HEADER_TIME_Y,0);
    
    // Leds
    ledSig1 = lv_led_create(header);
    lv_led_set_color(ledSig1,lv_palette_main(LV_PALETTE_GREEN));
    lv_obj_set_size(ledSig1, LED_RADIUS*2, LED_RADIUS*2);
    lv_obj_add_style(ledSig1,&style_pad_small,0);

    ledSig2 = lv_led_create(header);
    lv_led_set_color(ledSig2,lv_palette_main(LV_PALETTE_GREEN));
    lv_obj_set_size(ledSig2, LED_RADIUS*2, LED_RADIUS*2);
    lv_obj_add_style(ledSig2,&style_pad_small,0);

    ledInFrame = lv_led_create(header);
    lv_led_set_color(ledInFrame,lv_palette_main(LV_PALETTE_ORANGE));
    lv_obj_set_size(ledInFrame, LED_RADIUS*2, LED_RADIUS*2);
    lv_obj_add_style(ledInFrame,&style_pad_small,0);

    ledFrameReceived = lv_led_create(header);
    lv_led_set_color(ledFrameReceived,lv_palette_main(LV_PALETTE_BLUE));
    lv_obj_set_size(ledFrameReceived, LED_RADIUS*2, LED_RADIUS*2);
    lv_obj_add_style(ledFrameReceived,&style_pad_small,0);

    // Settigns button
    btn = lv_win_add_btn(win, LV_SYMBOL_SETTINGS, 40);
    lv_obj_add_event_cb(btn, settings_handler, LV_EVENT_CLICKED, NULL);

    // Window container
    lv_obj_t * cont = lv_win_get_content(win);  /*Content can be added here*/
    lv_obj_add_style(cont, &style_pad_none, 0);

    tabview = lv_tabview_create(cont, LV_DIR_LEFT, 50);

    // lv_obj_set_style_bg_color(tabview, lv_palette_lighten(LV_PALETTE_RED, 2), 0);

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
    lv_obj_t * label = lv_label_create(tab1);
    lv_label_set_text(label, "First tab");

    label = lv_label_create(tab2);
    int tabWidth = lv_obj_get_width(tab2);
    lv_label_set_text(label, "Location");
    // Map QR Code
    mapQr = lv_qrcode_create(tab2, QR_SIZE, lv_color_black(), lv_color_white());
    // Add a border with bg_color
    lv_obj_set_style_border_color(mapQr, lv_color_white(), 0);
    lv_obj_set_style_border_width(mapQr, 5, 0);
    lv_obj_align(mapQr,LV_ALIGN_BOTTOM_RIGHT,0,0);

    label = lv_label_create(tab3);
    lv_label_set_text(label, "Beacon");
    // Map QR Code
    beaconQr = lv_qrcode_create(tab3, QR_SIZE, lv_color_black(), lv_color_white());
    // Add a border with bg_color
    lv_obj_set_style_border_color(beaconQr, lv_color_white(), 0);
    lv_obj_set_style_border_width(beaconQr, 5, 0);
    lv_obj_center(beaconQr);
    lv_obj_align(beaconQr,LV_ALIGN_BOTTOM_RIGHT,0,0);
    
    label = lv_label_create(tab4);
    lv_label_set_text(label, "Data");

    lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);

    // Pages label
    pagesLabel = lv_label_create(cont);
    lv_obj_align(pagesLabel,LV_ALIGN_TOP_RIGHT,0,0);
    lv_obj_add_style(pagesLabel, &style_tag, 0);
    lv_obj_set_width(pagesLabel,100);
    
    lv_obj_t * footer = lv_obj_create(win);
    lv_obj_set_size(footer, LV_PCT(100), FOOTER_HEIGHT);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(footer,LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(footer, &style_pad_tiny, 0);

    btn = lv_btn_create(footer);
    lv_obj_set_size(btn, 70, LV_PCT(100));
    lv_obj_add_event_cb(btn, previous_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t * img = lv_img_create(btn);
    lv_img_set_src(img, LV_SYMBOL_LEFT);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    spinner = lv_spinner_create(footer,2000,60);
    lv_obj_set_size(spinner, 56, LV_PCT(100));
    lv_obj_set_style_arc_width(spinner,6,LV_PART_INDICATOR);
    lv_obj_set_style_pad_top(spinner,4,0);
    lv_obj_set_style_pad_left(spinner,10,0);

    footerLabel = lv_label_create(footer);
    lv_label_set_long_mode(footerLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(footerLabel, FOOTER_WAIT_LABEL);
    lv_obj_set_flex_grow(footerLabel, 1);
    lv_obj_add_style(footerLabel,&style_footer,0);
    lv_obj_set_style_text_align(footerLabel,LV_TEXT_ALIGN_CENTER,0);

    btn = lv_btn_create(footer);
    lv_obj_set_size(btn, 70, LV_PCT(100));
    lv_obj_add_event_cb(btn, next_handler, LV_EVENT_CLICKED, NULL);

    img = lv_img_create(btn);
    lv_img_set_src(img, LV_SYMBOL_RIGHT);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

}

void uiSetBeacon(Beacon* beacon, int curPage, int pageCount)
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
    // Set pages
    lv_label_set_text_fmt(pagesLabel,HEADER_PAGES_TEMPLATE,curPage,pageCount);
}


void uiSetTime(const char* time)
{
    lv_label_set_text(timeLabel, time);
}
    

void uiSetPower(const char* time)
{
    lv_label_set_text(powerLabel, time);
}

void uiSetSpinnerVisible(bool visible)
{
    visible ? lv_obj_clear_flag(spinner, LV_OBJ_FLAG_HIDDEN) : lv_obj_add_flag(spinner, LV_OBJ_FLAG_HIDDEN);
}

void uiSetFooter(const char* footer)
{
    lv_label_set_text(footerLabel, footer);
}


void uiSetLedSig1State(bool on)
{
    on ? lv_led_on(ledSig1) : lv_led_off(ledSig1);
}

void uiSetLedSig2State(bool on)
{
    on ? lv_led_on(ledSig2) : lv_led_off(ledSig2);
}

void uiSetLedInFrameState(bool on)
{
    on ? lv_led_on(ledInFrame) : lv_led_off(ledInFrame);
}

void uiSetLedFrameReceivedState(bool on)
{
    on ? lv_led_on(ledFrameReceived) : lv_led_off(ledFrameReceived);
}



