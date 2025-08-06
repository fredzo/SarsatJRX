#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <Settings.h>

// Audio
#define LABEL_WIDTH                 200
#define BUZZER_LEVEL_LABEL          "Buzzer level:"
#define TOUCH_SOUND_LABEL           "Touch sound:"
#define FRAME_SOUND_LABEL           "Frame received sound:"
#define COUNTDOWN_SOUND_LABEL       "Countdown sound:"
#define COUNTDOWN_LED_LABEL         "Led:"
#define COUNTDOWN_LED_LABEL_WIDTH   40
#define COUNTDOWN_RELOAD_LABEL      "Countdown auto reload:"

// Audio
static lv_obj_t * audioTab;
static int audioTabWidth;
static int audioTabHeight;
static lv_obj_t * buzzerLeveLabel;
static lv_obj_t * buzzerLevelSpinbox;
static int buzzerLevelSpinboxValue;
static lv_obj_t * buzzerLevelSpinboxUpButton;
static lv_obj_t * buzzerLevelSpinboxDownButton;

static lv_obj_t * touchSoundLabel;
static lv_obj_t * touchSoundToggle;
static lv_obj_t * frameSoundLabel;
static lv_obj_t * frameSoundToggle;
static lv_obj_t * countDownSoundLabel;
static lv_obj_t * countDownSoundToggle;
static lv_obj_t * countDownLedLabel;
static lv_obj_t * countDownLedToggle;
static lv_obj_t * countDownReloadLabel;
static lv_obj_t * countDownReloadToggle;

static void updateBuzzerLevelSpinboxValue()
{
    lv_obj_t * label = lv_textarea_get_label(buzzerLevelSpinbox);
    lv_label_set_text_fmt(label,"%d",buzzerLevelSpinboxValue);

}

static void updateBuzzerSound()
{
    SoundManager* soundManager = SoundManager::getSoundManager();
    Settings* settings = Settings::getSettings();
    soundManager->setVolume(buzzerLevelSpinboxValue);
    settings->setBuzzerLevel(buzzerLevelSpinboxValue);
    if(!settings->getTouchSound())
    {   // Force touch sound to give feedback on sound settings
        SoundManager::getSoundManager()->playTouchSound();
    }
}

// Up button event callback
static void spinboxBtnUpEventCb(lv_event_t * e)
{
    //lv_spinbox_increment(buzzerLevelSpinbox);
    buzzerLevelSpinboxValue+=10;
    if(buzzerLevelSpinboxValue > 255) buzzerLevelSpinboxValue = 255;
    updateBuzzerLevelSpinboxValue();
    updateBuzzerSound();
}

// Down button event callback
static void spinboxBtnDownEventCb(lv_event_t * e)
{
    //lv_spinbox_decrement(buzzerLevelSpinbox);
    buzzerLevelSpinboxValue-=10;
    if(buzzerLevelSpinboxValue < 0) buzzerLevelSpinboxValue = 0;
    updateBuzzerLevelSpinboxValue();
    updateBuzzerSound();
}

static void toggleTouchSoundCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(touchSoundToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setTouchSound(state);
}

static void toggleFrameSoundCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(frameSoundToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setFrameSound(state);
}

static void toggleCountDownSoundCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(countDownSoundToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setCountDownSound(state);
}

static void toggleCountDownLedCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(countDownLedToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setCountDownLeds(state);
}

static void toggleCountDownReloadCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(countDownReloadToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setReloadCountDown(state);
}

void createAudioTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight)
{   // Keep track on tab, tabWith and tabHeugth for keyboard creation
    audioTab = tab;
    audioTabWidth = tabWidth;
    audioTabHeight = tabHeight;
    currentY+=SPACER;

    // Buzzer level spinbox
    buzzerLeveLabel      = uiCreateLabel (tab,&style_section_title,BUZZER_LEVEL_LABEL,0,currentY,LABEL_WIDTH,SPINBOX_LINE_HEIGHT);
    buzzerLevelSpinbox = lv_textarea_create(tab);
    lv_obj_t * label = lv_textarea_get_label(buzzerLevelSpinbox);
    lv_obj_set_style_text_font(label,font_mono_medium,0);
    lv_textarea_set_one_line(buzzerLevelSpinbox, true);
    lv_textarea_set_cursor_click_pos(buzzerLevelSpinbox, false);
    lv_obj_set_style_pad_all(buzzerLevelSpinbox, 6, 0);
    lv_obj_set_style_text_align(buzzerLevelSpinbox, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_clear_flag(buzzerLevelSpinbox, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_width(buzzerLevelSpinbox, SPINBOX_WIDTH);
    lv_obj_set_size(buzzerLevelSpinbox,SPINBOX_WIDTH,SPINBOX_LINE_HEIGHT);
    lv_obj_set_pos(buzzerLevelSpinbox,SPINBOX_X,currentY);

    // Add up/down buttons
    buzzerLevelSpinboxUpButton = lv_btn_create(tab);
    lv_obj_set_size(buzzerLevelSpinboxUpButton, SPINBOX_BUTTON_SIZE, SPINBOX_BUTTON_SIZE);
    lv_obj_align_to(buzzerLevelSpinboxUpButton, buzzerLevelSpinbox, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_set_style_bg_img_src(buzzerLevelSpinboxUpButton, LV_SYMBOL_PLUS, 0);
    
    buzzerLevelSpinboxDownButton = lv_btn_create(tab);
    lv_obj_set_size(buzzerLevelSpinboxDownButton, SPINBOX_BUTTON_SIZE, SPINBOX_BUTTON_SIZE);
    lv_obj_align_to(buzzerLevelSpinboxDownButton, buzzerLevelSpinbox, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_obj_set_style_bg_img_src(buzzerLevelSpinboxDownButton, LV_SYMBOL_MINUS, 0);

    // Link buttons to callbacks
    lv_obj_add_event_cb(buzzerLevelSpinboxUpButton, spinboxBtnUpEventCb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(buzzerLevelSpinboxDownButton, spinboxBtnDownEventCb, LV_EVENT_CLICKED, NULL);

    currentY+=SPINBOX_LINE_HEIGHT+2*SPACER;

    // Touch sound
    touchSoundLabel  = uiCreateLabel (tab,&style_section_title,TOUCH_SOUND_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    touchSoundToggle = uiCreateToggle(tab,&style_section_text,toggleTouchSoundCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Frame sound
    frameSoundLabel  = uiCreateLabel (tab,&style_section_title,FRAME_SOUND_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    frameSoundToggle = uiCreateToggle(tab,&style_section_text,toggleFrameSoundCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Countdown sound
    countDownSoundLabel  = uiCreateLabel (tab,&style_section_title,COUNTDOWN_SOUND_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    countDownSoundToggle = uiCreateToggle(tab,&style_section_text,toggleCountDownSoundCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    // Countdown led
    countDownLedLabel  = uiCreateLabel (tab,&style_section_title,COUNTDOWN_LED_LABEL,TOGGLE_X+TOGGLE_WIDTH+20,currentY,COUNTDOWN_LED_LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    countDownLedToggle = uiCreateToggle(tab,&style_section_text,toggleCountDownLedCb,TOGGLE_X+TOGGLE_WIDTH+COUNTDOWN_LED_LABEL_WIDTH+40,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Countdown reload
    countDownReloadLabel  = uiCreateLabel (tab,&style_section_title,COUNTDOWN_RELOAD_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    countDownReloadToggle = uiCreateToggle(tab,&style_section_text,toggleCountDownReloadCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
}


void uiSettingsUpdateAudio()
{   // Display tab
    // Display reverse state
    Settings* settings = Settings::getSettings();
    buzzerLevelSpinboxValue = settings->getBuzzerLevel();
    updateBuzzerLevelSpinboxValue();
    settings->getTouchSound() ? lv_obj_add_state(touchSoundToggle, LV_STATE_CHECKED) : lv_obj_clear_state(touchSoundToggle, LV_STATE_CHECKED);
    settings->getFrameSound() ? lv_obj_add_state(frameSoundToggle, LV_STATE_CHECKED) : lv_obj_clear_state(frameSoundToggle, LV_STATE_CHECKED);
    settings->getCountDownSound() ? lv_obj_add_state(countDownSoundToggle, LV_STATE_CHECKED) : lv_obj_clear_state(countDownSoundToggle, LV_STATE_CHECKED);
    settings->getCountDownLeds() ? lv_obj_add_state(countDownLedToggle, LV_STATE_CHECKED) : lv_obj_clear_state(countDownLedToggle, LV_STATE_CHECKED);
    settings->getReloadCountDown() ? lv_obj_add_state(countDownReloadToggle, LV_STATE_CHECKED) : lv_obj_clear_state(countDownReloadToggle, LV_STATE_CHECKED);
}



