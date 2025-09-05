#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <Settings.h>

// Audio
#define LABEL_WIDTH                 200
#define COUNTDOWN_DURATION_LABEL    "Countdown duration:"
#define COUNTDOWN_DURATION_SEC      "sec."
#define COUNTDOWN_DURATION_SEC_W    35
#define FRAME_SOUND_LABEL           "Frame received sound:"
#define COUNTDOWN_SOUND_LABEL       "Countdown sound:"
#define COUNTDOWN_LED_LABEL         "Led:"
#define COUNTDOWN_LED_LABEL_WIDTH   40
#define COUNTDOWN_RELOAD_LABEL      "Countdown auto reload:"
#define FILTER_ORBITO_LABEL         "Filter orbito.:"

// Audio
static lv_obj_t * settingsTab;
static int settingsTabWidth;
static int settingsTabHeight;
static lv_obj_t * countdownDurationLabel;
static lv_obj_t * countdownDurationSpinbox;
static int countdownDurationSpinboxValue;
static lv_obj_t * countdownDurationSpinboxUpButton;
static lv_obj_t * countdownDurationSpinboxDownButton;
static lv_obj_t * countdownSecLabel;

static lv_obj_t * frameSoundLabel;
static lv_obj_t * frameSoundToggle;
static lv_obj_t * countdownSoundLabel;
static lv_obj_t * countdownSoundToggle;
static lv_obj_t * countdownLedLabel;
static lv_obj_t * countdownLedToggle;
static lv_obj_t * countdownReloadLabel;
static lv_obj_t * countdownReloadToggle;
static lv_obj_t * filterOrbitoLabel;
static lv_obj_t * filterOrbitoToggle;

static void updateCountdownDurationSpinboxValue()
{
    lv_obj_t * label = lv_textarea_get_label(countdownDurationSpinbox);
    lv_label_set_text_fmt(label,"%d",countdownDurationSpinboxValue);

}

static void updateCountdownDuration()
{
    Settings* settings = Settings::getSettings();
    settings->setCountdownDuration(countdownDurationSpinboxValue);
}

// Up button event callback
static void spinboxBtnUpEventCb(lv_event_t * e)
{
    //lv_spinbox_increment(countdownDurationSpinbox);
    countdownDurationSpinboxValue+=1;
    if(countdownDurationSpinboxValue > 255) countdownDurationSpinboxValue = 255;
    updateCountdownDurationSpinboxValue();
    updateCountdownDuration();
}

// Down button event callback
static void spinboxBtnDownEventCb(lv_event_t * e)
{
    //lv_spinbox_decrement(countdownDurationSpinbox);
    countdownDurationSpinboxValue-=1;
    if(countdownDurationSpinboxValue < 0) countdownDurationSpinboxValue = 0;
    updateCountdownDurationSpinboxValue();
    updateCountdownDuration();
}

static void toggleFilterOrbitoCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(filterOrbitoToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setFilterOrbito(state);
}

static void toggleFrameSoundCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(frameSoundToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setFrameSound(state);
}

static void toggleCountDownSoundCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(countdownSoundToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setCountDownSound(state);
}

static void toggleCountDownLedCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(countdownLedToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setCountDownLeds(state);
}

static void toggleCountDownReloadCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(countdownReloadToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setReloadCountDown(state);
}

void createSettingsTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight)
{   // Keep track on tab, tabWith and tabHeugth for keyboard creation
    settingsTab = tab;
    settingsTabWidth = tabWidth;
    settingsTabHeight = tabHeight;
    currentY+=SPACER;

    // Frame sound
    frameSoundLabel  = uiCreateLabel (tab,&style_section_title,FRAME_SOUND_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    frameSoundToggle = uiCreateToggle(tab,&style_section_text,toggleFrameSoundCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;

    // Countdown duration spinbox
    countdownDurationLabel   = uiCreateLabel (tab,&style_section_title,COUNTDOWN_DURATION_LABEL,0,currentY,LABEL_WIDTH,SPINBOX_LINE_HEIGHT);
    countdownDurationSpinbox = lv_textarea_create(tab);
    lv_obj_t * label = lv_textarea_get_label(countdownDurationSpinbox);
    lv_obj_set_style_text_font(label,font_mono_medium,0);
    lv_textarea_set_one_line(countdownDurationSpinbox, true);
    lv_textarea_set_cursor_click_pos(countdownDurationSpinbox, false);
    lv_obj_set_style_pad_all(countdownDurationSpinbox, 6, 0);
    lv_obj_set_style_text_align(countdownDurationSpinbox, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_clear_flag(countdownDurationSpinbox, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(countdownDurationSpinbox,SPINBOX_WIDTH,SPINBOX_LINE_HEIGHT);
    lv_obj_set_pos(countdownDurationSpinbox,SPINBOX_X,currentY);
    lv_obj_set_style_opa(countdownDurationSpinbox, LV_OPA_TRANSP, LV_PART_CURSOR);

    // Add up/down buttons
    countdownDurationSpinboxUpButton = lv_btn_create(tab);
    lv_obj_set_size(countdownDurationSpinboxUpButton, SPINBOX_BUTTON_SIZE, SPINBOX_BUTTON_SIZE);
    lv_obj_align_to(countdownDurationSpinboxUpButton, countdownDurationSpinbox, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_set_style_bg_img_src(countdownDurationSpinboxUpButton, LV_SYMBOL_PLUS, 0);
    
    countdownDurationSpinboxDownButton = lv_btn_create(tab);
    lv_obj_set_size(countdownDurationSpinboxDownButton, SPINBOX_BUTTON_SIZE, SPINBOX_BUTTON_SIZE);
    lv_obj_align_to(countdownDurationSpinboxDownButton, countdownDurationSpinbox, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_obj_set_style_bg_img_src(countdownDurationSpinboxDownButton, LV_SYMBOL_MINUS, 0);

    // Link buttons to callbacks
    lv_obj_add_event_cb(countdownDurationSpinboxUpButton, spinboxBtnUpEventCb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(countdownDurationSpinboxDownButton, spinboxBtnDownEventCb, LV_EVENT_CLICKED, NULL);

    // Add sec label
    countdownSecLabel  = uiCreateLabel (tab,&style_section_title,COUNTDOWN_DURATION_SEC,tabWidth-COUNTDOWN_DURATION_SEC_W-2,currentY,COUNTDOWN_DURATION_SEC_W,TOGGLE_LINE_HEIGHT);

    currentY+=SPINBOX_LINE_HEIGHT+2*SPACER;

    // Countdown sound
    countdownSoundLabel  = uiCreateLabel (tab,&style_section_title,COUNTDOWN_SOUND_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    countdownSoundToggle = uiCreateToggle(tab,&style_section_text,toggleCountDownSoundCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    // Countdown led
    countdownLedLabel  = uiCreateLabel (tab,&style_section_title,COUNTDOWN_LED_LABEL,TOGGLE_X+TOGGLE_WIDTH+20,currentY,COUNTDOWN_LED_LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    countdownLedToggle = uiCreateToggle(tab,&style_section_text,toggleCountDownLedCb,TOGGLE_X+TOGGLE_WIDTH+COUNTDOWN_LED_LABEL_WIDTH+40,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Countdown reload
    countdownReloadLabel  = uiCreateLabel (tab,&style_section_title,COUNTDOWN_RELOAD_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    countdownReloadToggle = uiCreateToggle(tab,&style_section_text,toggleCountDownReloadCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Filter orbito
    filterOrbitoLabel  = uiCreateLabel (tab,&style_section_title,FILTER_ORBITO_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    filterOrbitoToggle = uiCreateToggle(tab,&style_section_text,toggleFilterOrbitoCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
}


void uiSettingsUpdateSettings()
{   // Display tab
    // Display reverse state
    Settings* settings = Settings::getSettings();
    countdownDurationSpinboxValue = settings->getCountdownDuration();
    updateCountdownDurationSpinboxValue();
    settings->getFilterOrbito() ? lv_obj_add_state(filterOrbitoToggle, LV_STATE_CHECKED) : lv_obj_clear_state(filterOrbitoToggle, LV_STATE_CHECKED);
    settings->getFrameSound() ? lv_obj_add_state(frameSoundToggle, LV_STATE_CHECKED) : lv_obj_clear_state(frameSoundToggle, LV_STATE_CHECKED);
    settings->getCountDownSound() ? lv_obj_add_state(countdownSoundToggle, LV_STATE_CHECKED) : lv_obj_clear_state(countdownSoundToggle, LV_STATE_CHECKED);
    settings->getCountDownLeds() ? lv_obj_add_state(countdownLedToggle, LV_STATE_CHECKED) : lv_obj_clear_state(countdownLedToggle, LV_STATE_CHECKED);
    settings->getReloadCountDown() ? lv_obj_add_state(countdownReloadToggle, LV_STATE_CHECKED) : lv_obj_clear_state(countdownReloadToggle, LV_STATE_CHECKED);
}



