#include <ui/Ui.h>
#include <ui/UiBeacon.h>
#include <Util.h>
#include <Display.h>
#include <extra/libs/qrcode/lv_qrcode.h>

// Header
#define HEADER_HEIGHT     30
#define HEADER_TEXT       "- SarsatJRX - "
#define HEADER_PAGES_TEMPLATE "%02d/%02d"
// Header LEDS
#define LED_RADIUS          7
#define LED_SIZE            2*LED_RADIUS
#define LED_SPACING         4
#define LED_SPACE           LED_SIZE+LED_SPACING
#define LED_CONTAINER_SIZE  4*LED_SIZE+6*LED_SPACING
// Power and time
#define HEADER_POWER_TEMPLATE "%sV"   // "4.98V"
#define HEADER_POWER_Y    (HEADER_HEIGHT-16)/2
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


// Externs
extern void readNextSampleFrame();
extern void previousFrame();
extern void nextFrame();

/**********************
 *  STATIC VARIABLES
 **********************/
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
lv_obj_t * previousButton;
lv_obj_t * nextButton;

bool uiBeaconVisible = false;

static void settings_handler(lv_event_t * e)
{
    // TODO : settings screen
    readNextSampleFrame();
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

    // Title
    lv_obj_t * title = lv_win_add_title(win, HEADER_TEXT);
    lv_obj_add_style(title,&style_title,0);
    lv_obj_get_style_translate_x(title,20);
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
    previousButton = lv_btn_create(footer);
    lv_obj_set_size(previousButton, 70, LV_PCT(100));
    lv_obj_add_event_cb(previousButton, previous_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_clear_flag(previousButton, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t * img = lv_img_create(previousButton);
    lv_img_set_src(img, LV_SYMBOL_LEFT);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    // Spinner
    spinner = lv_spinner_create(footer,2000,60);
    lv_obj_set_size(spinner, 56, LV_PCT(100));
    lv_obj_set_style_arc_width(spinner,6,LV_PART_INDICATOR);
    lv_obj_set_style_pad_top(spinner,4,0);
    lv_obj_set_style_pad_left(spinner,10,0);
    // Footer label
    footerLabel = lv_label_create(footer);
    lv_label_set_long_mode(footerLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(footerLabel, FOOTER_WAIT_LABEL);
    lv_obj_set_flex_grow(footerLabel, 1);
    lv_obj_add_style(footerLabel,&style_footer,0);
    lv_obj_set_style_text_align(footerLabel,LV_TEXT_ALIGN_CENTER,0);
    // Next button
    nextButton = lv_btn_create(footer);
    lv_obj_set_size(nextButton, 70, LV_PCT(100));
    lv_obj_add_event_cb(nextButton, next_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_clear_flag(nextButton, LV_OBJ_FLAG_CLICKABLE);
    img = lv_img_create(nextButton);
    lv_img_set_src(img, LV_SYMBOL_RIGHT);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
}

void createUi()
{   // Load default theme in dark mode
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);

    // Styles
    // Text mono
    lv_style_init(&style_text_mono);
    lv_style_set_text_font(&style_text_mono, font_mono);
    lv_style_set_text_align(&style_text_mono, LV_TEXT_ALIGN_CENTER);
    // Text title
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);
    lv_style_set_text_align(&style_title, LV_TEXT_ALIGN_RIGHT);
    // Section title / text
    lv_style_init(&style_section_title);
    lv_style_set_text_font(&style_section_title, font_normal);
    lv_style_set_text_color(&style_section_title, lv_palette_lighten(LV_PALETTE_AMBER,1));
    lv_style_init(&style_section_text);
    lv_style_set_text_font(&style_section_text, font_normal);
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
    lv_style_set_text_color(&style_footer_highlight, lv_palette_lighten(LV_PALETTE_RED,1));
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
    
    lv_obj_t * win = lv_win_create(lv_scr_act(),HEADER_HEIGHT);
    createHeader(win);


    // Window container
    lv_obj_t * cont = lv_win_get_content(win);  /*Content can be added here*/
    lv_obj_add_style(cont, &style_pad_none, 0);

    uiBeaconCreateTabView(cont);

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
    if(!uiBeaconVisible)
    {
        lv_obj_clear_flag(tabview, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(pagesLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(previousButton, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(nextButton, LV_OBJ_FLAG_CLICKABLE);
        uiBeaconVisible = true;
    }
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

void uiSetFooter(const char* footer, bool highlight)
{
    if(highlight)
    {
        lv_obj_add_style(footerLabel,&style_footer_highlight,0);
    }
    else
    {
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



