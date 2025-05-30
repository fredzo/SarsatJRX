#include <ui/Ui.h>
#include <ui/UiBeacon.h>
#include <ui/UiSettings.h>
#include <Util.h>
#include <Display.h>
#include <extra/libs/qrcode/lv_qrcode.h>
#include <ui/SpiffsLvgl.h>
#include <Settings.h>

// Header
#define HEADER_TEXT         "SarsatJRX"
#define HEADER_PAGES_TEMPLATE "%02d/%02d"
// Header LEDS
#define LED_RADIUS          7
#define LED_SIZE            2*LED_RADIUS
#define LED_SPACING         4
#define LED_SPACE           LED_SIZE+LED_SPACING
#define LED_CONTAINER_SIZE  4*LED_SIZE+6*LED_SPACING
// Power and time
#define HEADER_POWER_TEMPLATE "%sV"   // "4.98V"
#define HEADER_POWER_Y      (HEADER_HEIGHT-16)/2
#define HEADER_TIME_Y       HEADER_POWER_Y
// Wifi
#define HEADER_WIFI_SIZE    20
// SD Card
#define HEADER_SDCARD_WIDTH     16
#define HEADER_SDCARD_HEIGHT    20
// Logo
#define HEADER_LOGO_SIZE    20
#define HEADER_LOGO_SRC     "J:/sarsat-jrx.bin"

// Footer
#define FOOTER_WAIT_LABEL           "Waiting for the wave..."
#define FOOTER_FRAME_LABEL          "Frame received !"
#define FOOTER_BUTTON_WIDTH         70
#define FOOTER_SPINNER_SIZE         42
#define FOOTER_METER_WIDTH          DISPLAY_WIDTH-2*FOOTER_BUTTON_WIDTH-FOOTER_SPINNER_SIZE-16
#define FOOTER_METER_HEIGHT         10
#define FOOTER_METTER_X             FOOTER_BUTTON_WIDTH+FOOTER_SPINNER_SIZE+8
#define FOOTER_METTER_Y             FOOTER_HEIGHT-FOOTER_METER_HEIGHT-8

// Additionnal Symbols
#define SYMBOL_WIFI_CONNECTED       "\xEF\x87\xAB"
#define SYMBOL_WIFI_AP              "\xEE\x8E\xB2"
#define SYMBOL_WIFI_AP_CONNECTED    "\xEE\x94\x91"

// Externs
extern void readNextSampleFrame();
extern void previousFrame();
extern void nextFrame();
extern void toggleScan();

/**********************
 *  VARIABLES
 **********************/

lv_style_t style_text_mono;
lv_style_t style_text_lcd_large;
lv_style_t style_title;
lv_style_t style_footer;
lv_style_t style_footer_highlight;
lv_style_t style_pad_small;
lv_style_t style_pad_tiny;
lv_style_t style_pad_none;
lv_style_t style_tag;
lv_style_t style_header;
lv_style_t style_section_title;
lv_style_t style_section_text;
lv_style_t style_section_toggle;
lv_style_t style_footer_text;
lv_style_t style_time;
lv_style_t style_meter;

lv_color_t uiBackgroundColor;
lv_color_t uiOkColor;
lv_color_t uiKoColor;
lv_color_t uiOnColor;
lv_color_t uiOffColor;


const lv_font_t * font_large = &lv_font_montserrat_24;
const lv_font_t * font_medium = &lv_font_montserrat_18;
const lv_font_t * font_normal = &lv_font_montserrat_16;
const lv_font_t * font_mono_medium = &casscadia_mono_16;
const lv_font_t * font_mono = &casscadia_mono;
const lv_font_t * font_symbols = &additional_symbols;

lv_obj_t * timeLabel;
lv_obj_t * wifiIndicator;
lv_obj_t * sdCardIndicator;
lv_obj_t * powerLabel;
lv_obj_t * headerledSig2;
lv_obj_t * ledSig1;
lv_obj_t * ledSig2;
lv_obj_t * ledInFrame;
lv_obj_t * ledFrameReceived;
lv_obj_t * pagesLabel;
lv_obj_t * footerLabel;
lv_obj_t * meter;
lv_obj_t * spinner;
lv_obj_t * previousButton;
lv_obj_t * nextButton;

UiScreen currentScreen = UiScreen::START;

UiScreen previousScreen = UiScreen::START;

static void settings_handler(lv_event_t * e)
{
    if(currentScreen != UiScreen::SETTINGS)
    {
        previousScreen = currentScreen;
        uiShowScreen(UiScreen::SETTINGS);
    }
    else
    {
        uiShowScreen(previousScreen);
    }
}

static void title_long_press_handler(lv_event_t * e)
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

void createHeader(lv_obj_t * win)
{
    // Header
    lv_obj_t * header = lv_win_get_header(win);
    lv_obj_add_style(header, &style_pad_tiny, 0);
    // Time
    timeLabel = lv_label_create(header);
    lv_label_set_text(timeLabel,"");
    lv_label_set_long_mode(timeLabel, LV_LABEL_LONG_DOT);
    lv_obj_add_style(timeLabel,&style_text_mono,0);
    lv_obj_set_size(timeLabel, 80, LV_PCT(100));
    lv_obj_set_style_pad_top(timeLabel,HEADER_TIME_Y,0);

    // Wifi status
    wifiIndicator = lv_label_create(header);
    lv_label_set_text(wifiIndicator,"");
    lv_obj_set_size(wifiIndicator, HEADER_WIFI_SIZE, HEADER_WIFI_SIZE);
    lv_obj_set_style_text_font(wifiIndicator,font_symbols,0);

    // SD card indicator
    sdCardIndicator = lv_label_create(header);
    lv_label_set_text(sdCardIndicator,"");
    lv_obj_set_style_pad_left(sdCardIndicator,4,0);
    lv_obj_set_size(sdCardIndicator, HEADER_SDCARD_WIDTH, HEADER_SDCARD_HEIGHT);
    //lv_obj_set_style_text_font(sdCardIndicator,font_symbols,0);

    // Logo
    lv_obj_t * logo = lv_img_create(header);
    lv_img_set_src(logo,HEADER_LOGO_SRC);
    lv_obj_set_size(logo,HEADER_LOGO_SIZE,HEADER_LOGO_SIZE);
    lv_obj_set_style_translate_x(logo,18,0);
    lv_obj_add_flag(logo, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(logo, title_long_press_handler, LV_EVENT_LONG_PRESSED, NULL);
    // Title
    lv_obj_t * title = lv_win_add_title(win, HEADER_TEXT);
    lv_obj_add_style(title,&style_title,0);
    lv_obj_set_style_translate_x(title,-15,0);
    lv_obj_add_flag(title, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(title, title_long_press_handler, LV_EVENT_LONG_PRESSED, NULL);
    // Power
    powerLabel = lv_label_create(header);
    lv_label_set_text(powerLabel,"");
    lv_label_set_long_mode(powerLabel, LV_LABEL_LONG_DOT);
    lv_obj_add_style(powerLabel,&style_text_mono,0);
    lv_obj_set_size(powerLabel, 60, LV_PCT(100));
    lv_obj_set_style_pad_top(powerLabel,HEADER_TIME_Y,0);
    
    // Leds
    // Led container
    lv_obj_t * ledContainer = lv_obj_create(header);
    lv_obj_set_size(ledContainer, LED_CONTAINER_SIZE, LV_PCT(100));
    lv_obj_add_style(ledContainer, &style_pad_none, 0);
    lv_obj_set_style_bg_color(ledContainer,lv_obj_get_style_bg_color(header,0),0);
    // Led sig1
    ledSig1 = lv_led_create(ledContainer);
    lv_led_set_color(ledSig1,lv_palette_main(LV_PALETTE_GREEN));
    lv_obj_set_size(ledSig1, LED_SIZE, LED_SIZE);
    lv_obj_align(ledSig1, LV_ALIGN_LEFT_MID, LED_SPACING, 0);
    // Led sig2
    ledSig2 = lv_led_create(ledContainer);
    lv_led_set_color(ledSig2,lv_palette_main(LV_PALETTE_GREEN));
    lv_obj_set_size(ledSig2, LED_SIZE, LED_SIZE);
    lv_obj_align(ledSig2, LV_ALIGN_CENTER, -LED_SPACE, 0);
    // Led In Frame
    ledInFrame = lv_led_create(ledContainer);
    lv_led_set_color(ledInFrame,lv_palette_main(LV_PALETTE_ORANGE));
    lv_obj_set_size(ledInFrame, LED_SIZE, LED_SIZE);
    lv_obj_align(ledInFrame, LV_ALIGN_CENTER, LED_RADIUS+2, 0);
    // Led Frame received
    ledFrameReceived = lv_led_create(ledContainer);
    lv_led_set_color(ledFrameReceived,lv_palette_main(LV_PALETTE_BLUE));
    lv_obj_set_size(ledFrameReceived, LED_SIZE, LED_SIZE);
    lv_obj_align(ledFrameReceived, LV_ALIGN_RIGHT_MID, -LED_SPACING, 0);

    // Settigns button
    lv_obj_t * btn = lv_win_add_btn(win, LV_SYMBOL_SETTINGS, 40);
    lv_obj_add_event_cb(btn, settings_handler, LV_EVENT_CLICKED, NULL);
}

void createFooter(lv_obj_t * win)
{
    // Footer
    lv_obj_t * footer = lv_obj_create(win);
    lv_obj_set_size(footer, LV_PCT(100), FOOTER_HEIGHT);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(footer,LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(footer, &style_pad_tiny, 0);
    // Previous button
    previousButton = uiCreateImageButton(footer,LV_SYMBOL_LEFT,previous_handler,LV_EVENT_CLICKED,FOOTER_BUTTON_WIDTH, LV_PCT(100));
    lv_obj_clear_flag(previousButton, LV_OBJ_FLAG_CLICKABLE);
    // Spinner
    spinner = lv_spinner_create(footer,2000,60);
    lv_obj_set_size(spinner, FOOTER_SPINNER_SIZE, LV_PCT(100));
    lv_obj_set_style_arc_width(spinner,6,LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(spinner,6,LV_PART_MAIN);
    lv_obj_set_style_pad_top(spinner,4,0);
    lv_obj_set_style_pad_left(spinner,4,0);
    lv_obj_set_style_pad_right(spinner,4,0);
    // Footer label
    footerLabel = lv_label_create(footer);
    lv_label_set_long_mode(footerLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(footerLabel, FOOTER_WAIT_LABEL);
    lv_obj_set_height(footerLabel, FOOTER_HEIGHT-FOOTER_METER_HEIGHT-4);
    lv_obj_set_style_pad_left(footerLabel,4,0);
    lv_obj_set_x(footerLabel,4);
    lv_obj_set_flex_grow(footerLabel, 1);
    lv_obj_add_style(footerLabel,&style_footer,0);
    //lv_obj_set_style_translate_y(footerLabel,-8,0);
    // Meter
    meter = lv_bar_create(footer);
    lv_obj_add_flag(meter,LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_set_size(meter, FOOTER_METER_WIDTH, FOOTER_METER_HEIGHT);
    lv_obj_set_pos(meter,FOOTER_METTER_X,FOOTER_METTER_Y);
    lv_obj_add_style(meter, &style_meter, LV_PART_INDICATOR);
    lv_bar_set_range(meter, POWER_MIN_VALUE, POWER_MAX_VALUE);
    lv_obj_set_style_anim_time(meter,200,LV_PART_MAIN);
    // Next button
    nextButton = uiCreateImageButton(footer,LV_SYMBOL_RIGHT,next_handler,LV_EVENT_CLICKED,FOOTER_BUTTON_WIDTH, LV_PCT(100));
    lv_obj_clear_flag(nextButton, LV_OBJ_FLAG_CLICKABLE);
}

void createUi()
{   // Init SPIFFS driver
    spiffs_drv_init();                 

    // Load default theme in dark mode
    lv_theme_t * theme = lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);

    // Styles
    // Text mono
    lv_style_init(&style_text_mono);
    lv_style_set_text_font(&style_text_mono, font_mono);
    lv_style_set_text_align(&style_text_mono, LV_TEXT_ALIGN_CENTER);
    // Text lcd large
    lv_style_init(&style_text_lcd_large);
    lv_style_set_text_font(&style_text_lcd_large, &lcd_44);
    lv_style_set_text_align(&style_text_lcd_large, LV_TEXT_ALIGN_CENTER);
    // Text title
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);
    lv_style_set_text_align(&style_title, LV_TEXT_ALIGN_RIGHT);
    // Header
    lv_style_init(&style_header);
    lv_style_set_text_font(&style_header, font_large);
    lv_style_set_text_align(&style_header, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_color(&style_header, lv_palette_lighten(LV_PALETTE_CYAN,1));
    // Section title / text / toggle
    lv_style_init(&style_section_title);
    lv_style_set_text_font(&style_section_title, font_normal);
    lv_style_set_text_color(&style_section_title, lv_palette_lighten(LV_PALETTE_CYAN,1));
    lv_style_init(&style_section_text);
    lv_style_set_text_font(&style_section_text, font_mono_medium);
    lv_style_set_text_color(&style_section_text, lv_palette_lighten(LV_PALETTE_YELLOW,4));
    lv_style_set_pad_top(&style_section_text, 2);
    lv_style_init(&style_section_toggle);
    lv_style_set_pad_top(&style_section_toggle, 2);
    // Footer
    lv_style_init(&style_footer);
    lv_style_set_text_font(&style_footer, font_medium);
    lv_style_set_text_align(&style_footer, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_left(&style_footer, -30);
    lv_style_set_text_color(&style_footer, lv_color_white());
    lv_style_init(&style_footer_highlight);
    lv_style_set_text_font(&style_footer_highlight, font_large);
    lv_style_set_text_align(&style_footer_highlight, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_left(&style_footer_highlight, 0);
    lv_style_set_text_color(&style_footer_highlight, lv_palette_lighten(LV_PALETTE_RED,2));
    // Meter
    lv_style_init(&style_meter);
    lv_style_set_bg_opa(&style_meter, LV_OPA_COVER);
    lv_style_set_bg_color(&style_meter, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_grad_color(&style_meter, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_dir(&style_meter, LV_GRAD_DIR_HOR);
    // Small padding
    lv_style_init(&style_pad_small);
    lv_style_set_pad_all(&style_pad_small, 4);
    lv_style_set_pad_row(&style_pad_small, 4);
    lv_style_set_pad_column(&style_pad_small, 4);
    // Tiny padding
    lv_style_init(&style_pad_tiny);
    lv_style_set_pad_all(&style_pad_tiny, 1);
    lv_style_set_pad_row(&style_pad_tiny, 1);
    lv_style_set_pad_column(&style_pad_tiny, 1);
    // No padding
    lv_style_init(&style_pad_none);
    lv_style_set_pad_all(&style_pad_none, 0);
    lv_style_set_pad_row(&style_pad_none, 0);
    lv_style_set_pad_column(&style_pad_none, 0);
    // Tag content
    lv_style_init(&style_tag);
    lv_style_set_text_font(&style_tag, font_mono);
    lv_style_set_radius(&style_tag, 5);
    //lv_style_set_bg_opa(&style_tag, LV_OPA_COVER);
    //lv_style_set_bg_color(&style_tag, lv_palette_darken(LV_PALETTE_GREY,2));
    lv_style_set_border_width(&style_tag, 2);
    lv_style_set_border_color(&style_tag, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_align(&style_tag,LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_all(&style_tag,4);
    //lv_style_set_text_color(&style_tag, lv_palette_main(LV_PALETTE_BLUE));
    // Time text
    lv_style_init(&style_time);
    lv_style_set_text_font(&style_time, font_mono);
    lv_style_set_text_color(&style_time, lv_palette_lighten(LV_PALETTE_CYAN,1));
    lv_style_set_pad_top(&style_time,4);


    // BG color
    uiBackgroundColor = lv_color_make(7, 7, 7);
    // OK  / KO color
    uiOkColor = lv_palette_main(LV_PALETTE_GREEN);
    uiKoColor = lv_palette_lighten(LV_PALETTE_RED,2);
    // Scan ON / OFF color
    uiOffColor = lv_palette_main(LV_PALETTE_GREEN);
    uiOnColor = lv_palette_lighten(LV_PALETTE_RED,1);
    
    lv_obj_t * win = lv_win_create(lv_scr_act(),HEADER_HEIGHT);
    createHeader(win);


    // Window container
    lv_obj_t * cont = lv_win_get_content(win);  /*Content can be added here*/
    lv_obj_add_style(cont, &style_pad_none, 0);

    uiBeaconCreateView(cont);

    uiSettingsCreateView(cont);

    // Pages label
    pagesLabel = lv_label_create(cont);
    lv_obj_add_flag(pagesLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_align(pagesLabel,LV_ALIGN_TOP_RIGHT,0,0);
    lv_obj_add_style(pagesLabel, &style_tag, 0);
    lv_obj_set_width(pagesLabel,70);

    createFooter(win);
    
}

void uiSetBeacon(Beacon* beacon, int curPage, int pageCount)
{
    uiBeaconSetBeacon(beacon);
    // Set pages
    lv_label_set_text_fmt(pagesLabel,HEADER_PAGES_TEMPLATE,curPage,pageCount);
    uiShowScreen(UiScreen::BEACON);
}

void uiShowScreen(UiScreen screen)
{
    if(screen != currentScreen)
    {
        switch(screen)
        {
            case UiScreen::BEACON:
                lv_obj_add_flag(settingsTabview, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(beaconMainBloc, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(beaconTabview, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(pagesLabel, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(previousButton, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_flag(nextButton, LV_OBJ_FLAG_CLICKABLE);
                break;
            case UiScreen::SETTINGS:
                uiSettingsUpdateView();
                lv_obj_add_flag(pagesLabel, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(previousButton, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_clear_flag(nextButton, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_clear_flag(settingsTabview, LV_OBJ_FLAG_HIDDEN);
                break;
            case UiScreen::START:
            default:
                lv_obj_add_flag(settingsTabview, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(beaconMainBloc, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(beaconTabview, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(pagesLabel, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(previousButton, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_clear_flag(nextButton, LV_OBJ_FLAG_CLICKABLE);
                break;

        }
        if(currentScreen == UiScreen::SETTINGS)
        {   // Save settings on settings screen exit
            Settings* settings = Settings::getSettings();
            settings->save();
        }
        currentScreen = screen;
    }
}

void uiSetTime(const char* time)
{
    lv_label_set_text(timeLabel, time);
}

#ifdef WIFI   
void uiSetWifiStatus(WifiStatus status)
{
  switch(status)
  {
    case WifiStatus::CONNECTED : 
        lv_label_set_text(wifiIndicator,SYMBOL_WIFI_CONNECTED);
        break;
    case WifiStatus::PORTAL :
        lv_label_set_text(wifiIndicator,SYMBOL_WIFI_AP);
        break;
    case WifiStatus::PORTAL_CONNECTED:
        lv_label_set_text(wifiIndicator,SYMBOL_WIFI_AP_CONNECTED);
        break;
    case WifiStatus::DISCONNECTED:
    case WifiStatus::OFF:
    default:
        lv_label_set_text(wifiIndicator,"");
        break;
  }
  // Update settings page
  uiSettingsUpdateWifi();
}
#endif

void uiSetSdCardStatus(bool mounted)
{
    if(mounted)
    {
        lv_label_set_text(sdCardIndicator,LV_SYMBOL_SD_CARD);
    }
    else
    {
        lv_label_set_text(sdCardIndicator,"");
    }
}

void uiSetPower(const char* time)
{
    lv_label_set_text(powerLabel, time);
}

void uiShowFrameReceived(bool show)
{
    if(show)
    {
        lv_obj_add_flag(spinner, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(footerLabel, FOOTER_FRAME_LABEL);
        lv_obj_add_style(footerLabel,&style_footer_highlight,0);
    }
    else
    {
        lv_obj_clear_flag(spinner, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(footerLabel, FOOTER_WAIT_LABEL);
        lv_obj_add_style(footerLabel,&style_footer,0);
    }
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

void uiSetPower(int power)
{
    lv_bar_set_value(meter, power, LV_ANIM_ON);
    uiSettingsUpdatePower(power);
}

lv_obj_t * uiCreateLabel(lv_obj_t * parent, lv_style_t * style, const char* text, int x, int y, int width, int height)
{
    lv_obj_t *  result = lv_label_create(parent);
    lv_label_set_text(result, text);
    lv_obj_add_style(result,style,0);
    lv_obj_set_size(result,width,height);
    lv_obj_set_pos(result,x,y);
    return result;
}

lv_obj_t * uiCreateToggle(lv_obj_t * parent, lv_style_t * style, lv_event_cb_t event_cb, int x, int y, int width, int height)
{
    lv_obj_t *  result = lv_switch_create(parent);
    lv_obj_add_state(result, LV_STATE_CHECKED);
    lv_obj_add_event_cb(result, event_cb, LV_EVENT_VALUE_CHANGED,nullptr);
    lv_obj_add_style(result,style,0);
    lv_obj_set_size(result,width,height);
    lv_obj_set_pos(result,x,y);
    return result;
}

lv_obj_t * uiCreateImageButton(lv_obj_t * parent, const void* src, lv_event_cb_t event_cb, lv_event_code_t filter, int width, int height, int x, int y)
{   // Image button
    lv_obj_t * imageButton = lv_btn_create(parent);
    lv_obj_set_size(imageButton, width, height);
    lv_obj_add_event_cb(imageButton, event_cb, filter, NULL);
    lv_obj_t * img = lv_img_create(imageButton);
    lv_img_set_src(img, src);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    if((x >= 0) && (y>=0))
    {
        lv_obj_set_pos(imageButton,x,y);
    }
    return imageButton;
}

lv_obj_t * uiCreateLabelButton(lv_obj_t * parent, const char* text, lv_event_cb_t event_cb, lv_event_code_t filter,lv_color_t buttonColor, int width, int height, int x, int y)
{   // Text button
    lv_obj_t * textButton = lv_btn_create(parent);
    if((x >= 0) && (y>=0))
    {
        lv_obj_add_flag(textButton,LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_obj_set_pos(textButton,x,y);
    }
    lv_obj_set_size(textButton, width, height);
    lv_obj_set_style_bg_color(textButton,buttonColor,0);
    lv_obj_add_event_cb(textButton, event_cb, filter, NULL);
    lv_obj_t * label = lv_label_create(textButton);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    return label;
}

lv_obj_t * uiCreateTextArea(lv_obj_t * parent, lv_event_cb_t event_cb, int width, int height, int x, int y)
{   // Text area
    lv_obj_t * ta = lv_textarea_create(parent);
    //lv_textarea_set_accepted_chars(ta, "0123456789.");
    //lv_obj_set_scrollbar_mode(ta,LV_SCROLLBAR_MODE_OFF);
    lv_textarea_set_max_length(ta, 8);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_text(ta, "");
    if((x >= 0) && (y>=0))
    {
        lv_obj_set_pos(ta,x,y);
    }
    lv_obj_set_size(ta, width, height);
    lv_obj_add_event_cb(ta, event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    return ta;
}

