#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <Settings.h>

// Audio
#define VERSION_LABEL           "Version :"
#define VERSION_LABEL_WIDTH     80
#define AUDIO_TOGGLE_X          0
#define AUDIO_BUTTONS_WIDTH     40
#define AUDIO_METER_HEIGHT      10
#define FILTERS_LABEL           "Filters :"
#define FILTERS_LABEL_WIDTH     75
#define FILTER1_LABEL           "Emph."
#define FILTER2_LABEL           "HiP."
#define FILTER3_LABEL           "LoP."
#define FILTER_LABEL_WIDTH      50
#define VOLUME_LABEL            "Volume :"
#define VOLUME_LABEL_WIDTH      75
#define VOLUME_AUTO_LABEL       "Auto"
#define VOLUME_AUTO_LABEL_WIDTH 50
#define VOLUME_SPINBOX_WIDTH    50

// Audio
static lv_obj_t * audioTab;
static int audioTabWidth;
static int audioTabHeight;
static lv_obj_t * audioToggle;
static lv_obj_t * audioMeter;
static lv_obj_t * audioFiltersTitle;
static lv_obj_t * audioFilter1Toggle;
static lv_obj_t * audioFilter1Label;
static lv_obj_t * audioFilter2Toggle;
static lv_obj_t * audioFilter2Label;
static lv_obj_t * audioFilter3Toggle;
static lv_obj_t * audioFilter3Label;
static lv_obj_t * audioVolumeTitle;
static lv_obj_t * audioVolumeToggle;
static lv_obj_t * audioVolumeAutoLabel;
static lv_obj_t * audioVolumeDownButton;
static lv_obj_t * audioVolumeUpButton;
static lv_obj_t * audioVolumeSpinbox;


static void toggle_audio_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(audioToggle, LV_STATE_CHECKED);
    //Serial.println("Toggle audio :" + String(state));
    Audio* audio = Audio::getAudio();
    if(state)
    {   // TODO Start audio with saved settings
        //Settings * settings = Settings::getSettings();
        //audio->audioInit(settings->getAudioAutoVolume(),settings->getAudioVolume(),settings->getAudioFilter1(),settings->getAudioFilter2(),settings->getAudioFilter3(),settings->getLastFrequency(),settings->getActiveFrequencies());
        //uiSettingsUpdateView();
    }
    else
    {   // TODO stop audio
        //audio->audioStop();
        //uiSettingsUpdateView();
    }
    // TODO Save state to settings
    Settings* settings = Settings::getSettings();
    //settings->setAudioState(state);
}

void createAudioTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight)
{   // Keep track on tab, tabWith and tabHeugth for keyboard creation
    audioTab = tab;
    audioTabWidth = tabWidth;
    audioTabHeight = tabHeight;
    Audio* audio = Audio::getAudio();
    // Audion on/off
    currentY+=SPACER;
    audioToggle = uiCreateToggle(tab,&style_section_text,toggle_audio_cb,AUDIO_TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    // Meter
    audioMeter = lv_bar_create(tab);
    lv_obj_set_size(audioMeter, tabWidth-4, AUDIO_METER_HEIGHT);
    lv_obj_set_pos(audioMeter,0,currentY);
    lv_obj_add_style(audioMeter, &style_meter, LV_PART_INDICATOR);
    lv_bar_set_range(audioMeter, POWER_MIN_VALUE, POWER_MAX_VALUE);
    lv_obj_set_style_anim_time(audioMeter,200,LV_PART_MAIN);
    currentY += (AUDIO_METER_HEIGHT+HALF_SPACER);
    // Filter toggles
    int currentX = 0;
    audioFiltersTitle = uiCreateLabel(tab,&style_section_title,FILTERS_LABEL,0,currentY+HALF_SPACER,FILTERS_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTERS_LABEL_WIDTH+SPACER;
    audioFilter1Toggle = uiCreateToggle(tab,&style_section_text,NULL,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    audioFilter1Label = uiCreateLabel(tab,&style_section_text,FILTER1_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTER_LABEL_WIDTH+SPACER;
    audioFilter2Toggle = uiCreateToggle(tab,&style_section_text,NULL,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    audioFilter2Label = uiCreateLabel(tab,&style_section_text,FILTER2_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTER_LABEL_WIDTH+SPACER;
    audioFilter3Toggle = uiCreateToggle(tab,&style_section_text,NULL,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    audioFilter3Label = uiCreateLabel(tab,&style_section_text,FILTER3_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH-4,LINE_HEIGHT);
    currentY+=(TOGGLE_LINE_HEIGHT+HALF_SPACER);
    // Volume
    currentX = 0;
    audioVolumeTitle = uiCreateLabel(tab,&style_section_title,VOLUME_LABEL,0,currentY+HALF_SPACER,VOLUME_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=VOLUME_LABEL_WIDTH+SPACER;
    audioVolumeToggle = uiCreateToggle(tab,&style_section_text,NULL,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    audioVolumeAutoLabel = uiCreateLabel(tab,&style_section_text,VOLUME_AUTO_LABEL,currentX,currentY+HALF_SPACER,VOLUME_AUTO_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTER_LABEL_WIDTH+SPACER;
    // Volume down button
    audioVolumeDownButton = uiCreateImageButton(tab,LV_SYMBOL_MINUS,NULL,LV_EVENT_ALL,AUDIO_BUTTONS_WIDTH, TOGGLE_LINE_HEIGHT,currentX,currentY);
    currentX+=AUDIO_BUTTONS_WIDTH+SPACER;
    // Spinbox
    audioVolumeSpinbox = lv_spinbox_create(tab);
    lv_spinbox_set_range(audioVolumeSpinbox, 1, 8);
    lv_spinbox_set_digit_format(audioVolumeSpinbox, 1, 0);
    lv_obj_set_style_pad_all(audioVolumeSpinbox,2,0);
    lv_obj_set_pos(audioVolumeSpinbox,currentX,currentY);
    lv_obj_set_size(audioVolumeSpinbox, VOLUME_SPINBOX_WIDTH, TOGGLE_LINE_HEIGHT);
    lv_obj_set_style_text_align(audioVolumeSpinbox,LV_TEXT_ALIGN_CENTER,0);
    // Hide cursor
    lv_obj_set_style_opa(audioVolumeSpinbox,0,LV_PART_CURSOR);
    currentX+=VOLUME_SPINBOX_WIDTH+SPACER;
    // Volume up button
    audioVolumeUpButton = uiCreateImageButton(tab,LV_SYMBOL_PLUS,NULL,LV_EVENT_ALL,AUDIO_BUTTONS_WIDTH, TOGGLE_LINE_HEIGHT,currentX,currentY);
    currentY+=(TOGGLE_LINE_HEIGHT+HALF_SPACER);
}

void uiSettingsAudioPower(int power)
{
    lv_bar_set_value(audioMeter, power, LV_ANIM_ON);
}


