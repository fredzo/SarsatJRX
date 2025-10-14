#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <Settings.h>
#include <WiFi.h>

// Audio
#define LABEL_WIDTH                 200
#define BUZZER_LEVEL_LABEL          "Buzzer level:"
#define TOUCH_SOUND_LABEL           "Touch sound:"

// App connection
#define APP_TITLE_LABEL     "App. Connection"
#define APP_IP_LABEL        "Decoder address:"
#define APP_IP_LABEL_WIDTH  160
#define APP_QR_TITLE        "QR Code:"
#define APP_QR_TEXT         "Scan with SarsatJRX App."
// QR code
#define APP_QR_SIZE         130
#define DECODER_ADDRESS_TEMPALTE    "%s"


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

// App Connection
static lv_obj_t * appTitle;
static lv_obj_t * appIpTitle;
static lv_obj_t * appIpValue;
static lv_obj_t * appQrTitle;
static lv_obj_t * appQrText;
static lv_obj_t * appQrCode;


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
    lv_obj_set_size(buzzerLevelSpinbox,SPINBOX_WIDTH,SPINBOX_LINE_HEIGHT);
    lv_obj_set_pos(buzzerLevelSpinbox,SPINBOX_X,currentY);
    lv_obj_set_style_opa(buzzerLevelSpinbox, LV_OPA_TRANSP, LV_PART_CURSOR);

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
    currentY+=TOGGLE_LINE_HEIGHT;

    // Decoder IP
    currentY+=SPACER;
    appTitle = uiCreateLabel(tab,&style_section_title,APP_TITLE_LABEL,0,currentY,APP_IP_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    currentY+=SPACER;
    appIpTitle = uiCreateLabel(tab,&style_section_title,APP_IP_LABEL,0,currentY,APP_IP_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    appIpValue = uiCreateLabel(tab,&style_section_text,"",0,currentY,APP_IP_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+HALF_SPACER;
    // Qr code title
    appQrTitle = uiCreateLabel(tab,&style_section_title,APP_QR_TITLE,0,currentY,APP_IP_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    appQrText = uiCreateLabel(tab,&style_section_text,APP_QR_TEXT,0,currentY,tabWidth,LINE_HEIGHT);

    // App QR Code
    appQrCode = lv_qrcode_create(tab, APP_QR_SIZE, lv_color_black(), lv_color_white());
    // Add a border with bg_color
    lv_obj_set_style_border_color(appQrCode, lv_color_white(), 0);
    lv_obj_set_style_border_width(appQrCode, 5, 0);
    lv_obj_center(appQrCode);
    lv_obj_align(appQrCode,LV_ALIGN_BOTTOM_RIGHT,-6,-6);
}

void uiSettingsUpdateAudio()
{   // Display tab
    // Display reverse state
    Settings* settings = Settings::getSettings();
    buzzerLevelSpinboxValue = settings->getBuzzerLevel();
    updateBuzzerLevelSpinboxValue();
    settings->getTouchSound() ? lv_obj_add_state(touchSoundToggle, LV_STATE_CHECKED) : lv_obj_clear_state(touchSoundToggle, LV_STATE_CHECKED);
}

void uiSettingsUpdateParamWifi()
{ 
    // IP @
    String ipString = WiFi.localIP().toString();
    // IP
    lv_label_set_text(appIpValue,ipString.c_str());
    // QR code
    char buffer[32];
    lv_snprintf(buffer,sizeof(buffer),DECODER_ADDRESS_TEMPALTE, ipString.c_str());
    lv_qrcode_update(appQrCode, buffer, strlen(buffer));
}



