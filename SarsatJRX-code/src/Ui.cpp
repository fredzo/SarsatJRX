#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <lv_qrcode.h>

// Defines
// Enable serial out
#define SERIAL_OUT

// Header
#define HEADER_HEIGHT     60
#define HEADER_TEXT       "- SarsatJRX -"
#define HEADER_BOTTOM     HEADER_HEIGHT+4
#define HEADER_PAGES_TEMPLATE "%02d/%02d"
#define HEADER_PAGES_X    4
#define HEADER_PAGES_Y    (HEADER_HEIGHT-12)/2
#define HEADER_TIME_X     56
#define HEADER_TIME_Y     HEADER_PAGES_Y
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
// Power
#define HEADER_POWER_TEMPLATE "%sV"   // "4.98V"
#define HEADER_POWER_X    LED_SIG_1_X-70
#define HEADER_POWER_Y    (HEADER_HEIGHT-12)/2

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
static lv_style_t styles_pad_tiny;

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

static void event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    LV_UNUSED(obj);
    Serial.printf("Button %d clicked ! \n",((int)lv_obj_get_index(obj)));
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
    // lv_style_set_size()

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);
    lv_style_set_text_align(&style_title, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&style_footer);
    lv_style_set_text_font(&style_footer, font_medium);
    lv_style_set_text_align(&style_footer, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&styles_pad_tiny);
    lv_style_set_pad_all(&styles_pad_tiny, 1);
    lv_style_set_pad_row(&styles_pad_tiny, 1);
    lv_style_set_pad_column(&styles_pad_tiny, 1);


    lv_obj_t * win = lv_win_create(lv_scr_act(),HEADER_HEIGHT);
    lv_obj_t * btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_LEFT, 70);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t * title = lv_win_add_title(win, HEADER_TEXT);
    lv_obj_add_style(title,&style_title,0);
    


    lv_obj_t * header = title->parent;
    //lv_obj_add_style(header, &styles_pad_tiny, 0);

    lv_obj_t * led = lv_led_create(header);

    btn = lv_win_add_btn(win, LV_SYMBOL_SETTINGS, 40);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    btn = lv_win_add_btn(win, LV_SYMBOL_RIGHT, 70);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t * cont = lv_win_get_content(win);  /*Content can be added here*/
    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, "This is\n"
                      "a pretty\n"
                      "long text\n"
                      "to see how\n"
                      "the window\n"
                      "becomes\n"
                      "scrollable.\n"
                      "Some more\n"
                      "text to be\n"
                      "sure it\n"
                      "overflows. :)");    

    
    // Test QR Code
    lv_obj_t * qr = lv_qrcode_create(lv_scr_act(), 150, lv_color_black(), lv_color_white());
    /*Set data*/
    const char * data = "https://lvgl.io";
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_center(qr);
    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, lv_color_white(), 0);
    lv_obj_set_style_border_width(qr, 5, 0);



    lv_obj_t * footer = lv_obj_create(win);
    lv_obj_set_size(footer, LV_PCT(100), FOOTER_HEIGHT);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(footer,LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(footer, &styles_pad_tiny, 0);

    btn = lv_btn_create(footer);
    lv_obj_set_size(btn, 70, LV_PCT(100));

    lv_obj_t * img = lv_img_create(btn);
    lv_img_set_src(img, LV_SYMBOL_LEFT);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * spinner = lv_spinner_create(footer,1000,60);
    lv_obj_set_size(spinner, 40, LV_PCT(100));
    lv_obj_align(spinner, LV_ALIGN_OUT_TOP_MID, 10, 10);

    lv_obj_t * footerLabel = lv_label_create(footer);
    lv_label_set_long_mode(footerLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(footerLabel, FOOTER_WAIT_LABEL);
    lv_obj_set_flex_grow(footerLabel, 1);
    lv_obj_add_style(footerLabel,&style_footer,0);
    lv_obj_set_style_text_align(footerLabel,LV_TEXT_ALIGN_CENTER,0);

    btn = lv_btn_create(footer);
    lv_obj_set_size(btn, 70, LV_PCT(100));

    img = lv_img_create(btn);
    lv_img_set_src(img, LV_SYMBOL_RIGHT);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

}

