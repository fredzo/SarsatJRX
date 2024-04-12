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
#define RADIO_TOGGLE_X          0
#define RADIO_BUTTONS_WIDTH     40
#define RADIO_FREQ_NEXT_X       (TOGGLE_WIDTH+4)
#define RADIO_FREQ_X            (RADIO_FREQ_NEXT_X+RADIO_BUTTONS_WIDTH+4)
#define RADIO_FREQ_HEIGHT       (TOGGLE_LINE_HEIGHT+2*SPACER)
#define RADIO_METER_HEIGHT      10
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

// Radio
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
    Radio* radio = Radio::getRadio();
    if(state)
    {   // TODO Start audio with saved settings
        //Settings * settings = Settings::getSettings();
        //radio->radioInit(settings->getRadioAutoVolume(),settings->getRadioVolume(),settings->getRadioFilter1(),settings->getRadioFilter2(),settings->getRadioFilter3(),settings->getLastFrequency(),settings->getActiveFrequencies());
        //uiSettingsUpdateView();
    }
    else
    {   // TODO stop audio
        //radio->radioStop();
        //uiSettingsUpdateView();
    }
    // TODO Save state to settings
    Settings* settings = Settings::getSettings();
    //settings->setRadioState(state);
}

static void toggle_audio_filter1_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(audioFilter1Toggle, LV_STATE_CHECKED);
    Radio* radio = Radio::getRadio();
    radio->setFilter1(state);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioFilter1(state);
}

static void toggle_audio_filter2_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(audioFilter2Toggle, LV_STATE_CHECKED);
    Radio* radio = Radio::getRadio();
    radio->setFilter2(state);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioFilter2(state);
}

static void toggle_audio_filter3_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(audioFilter3Toggle, LV_STATE_CHECKED);
    Radio* radio = Radio::getRadio();
    radio->setFilter3(state);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioFilter3(state);
}

static void toggle_volume_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(audioVolumeToggle, LV_STATE_CHECKED);
    Radio* radio = Radio::getRadio();
    radio->setAutoVolume(state);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioAutoVolume(state);
}

static void audio_update_volume()
{
    int32_t volume = lv_spinbox_get_value(audioVolumeSpinbox);
    Radio* radio = Radio::getRadio();
    radio->setVolume(volume);
    // Update Settings
    Settings* settings = Settings::getSettings();
    settings->setRadioVolume(volume);
}

static void audio_volume_down_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_decrement(audioVolumeSpinbox);
        audio_update_volume();
    }
}

static void audio_volume_up_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(audioVolumeSpinbox);
        audio_update_volume();
    }
}

void createAudioTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight)
{   // Keep track on tab, tabWith and tabHeugth for keyboard creation
    audioTab = tab;
    audioTabWidth = tabWidth;
    audioTabHeight = tabHeight;
    Radio* radio = Radio::getRadio();
    // Radion on/off
    currentY+=SPACER;
    audioToggle = uiCreateToggle(tab,&style_section_text,toggle_audio_cb,RADIO_TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    // Meter
    audioMeter = lv_bar_create(tab);
    lv_obj_set_size(audioMeter, tabWidth-4, RADIO_METER_HEIGHT);
    lv_obj_set_pos(audioMeter,0,currentY);
    lv_obj_add_style(audioMeter, &style_meter, LV_PART_INDICATOR);
    lv_bar_set_range(audioMeter, 0, 255);
    lv_obj_set_style_anim_time(audioMeter,200,LV_PART_MAIN);
    currentY += (RADIO_METER_HEIGHT+HALF_SPACER);
    // Filter toggles
    int currentX = 0;
    audioFiltersTitle = uiCreateLabel(tab,&style_section_title,FILTERS_LABEL,0,currentY+HALF_SPACER,FILTERS_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTERS_LABEL_WIDTH+SPACER;
    audioFilter1Toggle = uiCreateToggle(tab,&style_section_text,toggle_audio_filter1_cb,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    audioFilter1Label = uiCreateLabel(tab,&style_section_text,FILTER1_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTER_LABEL_WIDTH+SPACER;
    audioFilter2Toggle = uiCreateToggle(tab,&style_section_text,toggle_audio_filter2_cb,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    audioFilter2Label = uiCreateLabel(tab,&style_section_text,FILTER2_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTER_LABEL_WIDTH+SPACER;
    audioFilter3Toggle = uiCreateToggle(tab,&style_section_text,toggle_audio_filter3_cb,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    audioFilter3Label = uiCreateLabel(tab,&style_section_text,FILTER3_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH-4,LINE_HEIGHT);
    currentY+=(TOGGLE_LINE_HEIGHT+HALF_SPACER);
    // Volume
    currentX = 0;
    audioVolumeTitle = uiCreateLabel(tab,&style_section_title,VOLUME_LABEL,0,currentY+HALF_SPACER,VOLUME_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=VOLUME_LABEL_WIDTH+SPACER;
    audioVolumeToggle = uiCreateToggle(tab,&style_section_text,toggle_volume_cb,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    audioVolumeAutoLabel = uiCreateLabel(tab,&style_section_text,VOLUME_AUTO_LABEL,currentX,currentY+HALF_SPACER,VOLUME_AUTO_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTER_LABEL_WIDTH+SPACER;
    // Volume down button
    audioVolumeDownButton = uiCreateImageButton(tab,LV_SYMBOL_MINUS,audio_volume_down_cb,LV_EVENT_ALL,RADIO_BUTTONS_WIDTH, TOGGLE_LINE_HEIGHT,currentX,currentY);
    currentX+=RADIO_BUTTONS_WIDTH+SPACER;
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
    audioVolumeUpButton = uiCreateImageButton(tab,LV_SYMBOL_PLUS,audio_volume_up_cb,LV_EVENT_ALL,RADIO_BUTTONS_WIDTH, TOGGLE_LINE_HEIGHT,currentX,currentY);
    currentY+=(TOGGLE_LINE_HEIGHT+HALF_SPACER);
}

void uiSettingsUpdateAudioStatus(bool radioStatus)
{
    if(radioStatus)
    {   // Audio toggle on
        lv_obj_add_state(audioToggle, LV_STATE_CHECKED);
        // TODO update audio
        // Radio * radio = Radio::getRadio();
        // Update filters
        //radio->getFilter1() ? lv_obj_add_state(audioFilter1Toggle, LV_STATE_CHECKED) : lv_obj_clear_state(audioFilter1Toggle, LV_STATE_CHECKED);
        //radio->getFilter2() ? lv_obj_add_state(audioFilter2Toggle, LV_STATE_CHECKED) : lv_obj_clear_state(audioFilter2Toggle, LV_STATE_CHECKED);
        //radio->getFilter3() ? lv_obj_add_state(audioFilter3Toggle, LV_STATE_CHECKED) : lv_obj_clear_state(audioFilter3Toggle, LV_STATE_CHECKED);
    }
    else
    {   // Audio toggle off
        lv_obj_clear_state(audioToggle, LV_STATE_CHECKED);
    }
}

void uiSettingsAudioPower(int power)
{
    lv_bar_set_value(audioMeter, power, LV_ANIM_ON);
}


