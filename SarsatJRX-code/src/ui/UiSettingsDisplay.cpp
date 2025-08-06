#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <Settings.h>

// Display
#define LABEL_WIDTH                   200
#define SCREEN_REVERSE_LABEL          "Screen reverse:"
#define SCREEN_OFF_ON_CHARGE_LABEL    "Screen off on charge:"
#define SHOW_BAT_PERCENTAGE_LABEL     "Show bat. %:"
#define ALLOW_FRAM_SIMU_LABEL         "Allow frame simu.:"


// Display
static lv_obj_t * displayTab;
static int displayTabWidth;
static int displayTabHeight;
static lv_obj_t * displayReverseLabel;
static lv_obj_t * displayReverseToggle;
static lv_obj_t * screenOffOnChargeLabel;
static lv_obj_t * screenOffOnChargeToggle;
static lv_obj_t * showBatPercentageLabel;
static lv_obj_t * showBatPercentageToggle;
static lv_obj_t * allowFrameSimuLabel;
static lv_obj_t * allowFrameSimuToggle;


static void toggleDisplayReverseCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(displayReverseToggle, LV_STATE_CHECKED);
    Hardware* hardware = Hardware::getHardware();
    Display* display = hardware->getDisplay();
    // Reverse display if needed
    display->setReverse(state);
    // Update Settings in flash
    Settings::getSettings()->setDisplayReverse(state);
    uiSettingsUpdateView();
}

static void toggleScreenOffOnChargeCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(screenOffOnChargeToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setScreenOffOnCharge(state);
}

static void toggleShowBatPercentageCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(showBatPercentageToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setShowBatteryPercentage(state);
    uiUpdatePower();
}

static void toggleAllowFrameSimuCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(allowFrameSimuToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setAllowFrameSimu(state);
}


void createDisplayTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight)
{   // Keep track on tab, tabWith and tabHeugth for keyboard creation
    displayTab = tab;
    displayTabWidth = tabWidth;
    displayTabHeight = tabHeight;
    currentY+=SPACER;
    // Screen reverse
    displayReverseLabel  = uiCreateLabel (tab,&style_section_title,SCREEN_REVERSE_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    displayReverseToggle = uiCreateToggle(tab,&style_section_text,toggleDisplayReverseCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Screen off on charge
    screenOffOnChargeLabel  = uiCreateLabel (tab,&style_section_title,SCREEN_OFF_ON_CHARGE_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    screenOffOnChargeToggle = uiCreateToggle(tab,&style_section_text,toggleScreenOffOnChargeCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Show bat percentage
    showBatPercentageLabel  = uiCreateLabel (tab,&style_section_title,SHOW_BAT_PERCENTAGE_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    showBatPercentageToggle = uiCreateToggle(tab,&style_section_text,toggleShowBatPercentageCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Allow frame simu
    allowFrameSimuLabel  = uiCreateLabel (tab,&style_section_title,ALLOW_FRAM_SIMU_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    allowFrameSimuToggle = uiCreateToggle(tab,&style_section_text,toggleAllowFrameSimuCb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // TODO : date/time picker
}


void uiSettingsUpdateDisplay()
{   // Display tab
    // Display reverse state
    Settings* settings = Settings::getSettings();
    settings->getDisplayReverse() ? lv_obj_add_state(displayReverseToggle, LV_STATE_CHECKED) : lv_obj_clear_state(displayReverseToggle, LV_STATE_CHECKED);
    settings->getScreenOffOnCharge() ? lv_obj_add_state(screenOffOnChargeToggle, LV_STATE_CHECKED) : lv_obj_clear_state(screenOffOnChargeToggle, LV_STATE_CHECKED);
    settings->getShowBatteryPercentage() ? lv_obj_add_state(showBatPercentageToggle, LV_STATE_CHECKED) : lv_obj_clear_state(showBatPercentageToggle, LV_STATE_CHECKED);
    settings->getAllowFrameSimu() ? lv_obj_add_state(allowFrameSimuToggle, LV_STATE_CHECKED) : lv_obj_clear_state(allowFrameSimuToggle, LV_STATE_CHECKED);
}

