#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <Settings.h>

// Externs
extern void createSettingsTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight);
extern void createAudioTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight);
extern void createDisplayTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight);
extern void createWifiTab(lv_obj_t * tab, int currentY, int tabWidth);
extern void createBluetoothTab(lv_obj_t * tab, int currentY, int tabWidth);
extern void createSdTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight);
extern void createSystemTab(lv_obj_t * tab, int currentY, int tabWidth);

lv_obj_t * settingsTabview;

void uiSettingsCreateView(lv_obj_t * cont)
{   // Tab view
    settingsTabview = lv_tabview_create(cont, LV_DIR_LEFT, 50);
    // Hide tab view for now
    lv_obj_add_flag(settingsTabview, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(settingsTabview,uiBackgroundColor,0);

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(settingsTabview);
    //lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    //lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_RIGHT, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_font(tab_btns, &additional_symbols, LV_PART_ITEMS | LV_STATE_DEFAULT);

    /*Add 6 tabs */
    lv_obj_t * tab1 = lv_tabview_add_tab(settingsTabview, SYMBOL_RADIO);
    // Warning PARAM_TAB_INDEX should match this position
    lv_obj_t * tab2 = lv_tabview_add_tab(settingsTabview, SYMBOL_WRENCH);
    lv_obj_t * tab3 = lv_tabview_add_tab(settingsTabview, SYMBOL_DISPLAY);
    lv_obj_t * tab4 = lv_tabview_add_tab(settingsTabview, SYMBOL_WIFI_CONNECTED);
#ifdef BLUETOOTH
    lv_obj_t * tab5 = lv_tabview_add_tab(settingsTabview, SYMBOL_BLUETOOTH);
#endif
    lv_obj_t * tab6 = lv_tabview_add_tab(settingsTabview, SYMBOL_SD);
    lv_obj_t * tab7 = lv_tabview_add_tab(settingsTabview, SYMBOL_SYSTEM);
    lv_obj_add_style(tab1, &style_pad_small, 0);
    lv_obj_add_style(tab2, &style_pad_small, 0);
    lv_obj_add_style(tab3, &style_pad_small, 0);
    lv_obj_add_style(tab4, &style_pad_small, 0);
#ifdef BLUETOOTH
    lv_obj_add_style(tab5, &style_pad_small, 0);
#endif
    lv_obj_add_style(tab6, &style_pad_small, 0);
    lv_obj_add_style(tab7, &style_pad_small, 0);
    int tabWidth = lv_obj_get_width(tab1) - 8; // 2*4 px padding
    int tabHeight = lv_obj_get_height(tab1) - 8 - FOOTER_HEIGHT; // 2*4 px padding

    //lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_AMBER, 3), 0);
    //lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);

    // Add content to the tabs
    int currentY = 0;
    createSettingsTab(tab1,currentY,tabWidth,tabHeight);
    createAudioTab(tab2,currentY,tabWidth,tabHeight);
    createDisplayTab(tab3,currentY,tabWidth,tabHeight);
    createWifiTab(tab4,currentY,tabWidth);
#ifdef BLUETOOTH
    createBluetoothTab(tab5,currentY,tabWidth);
#endif
    createSdTab(tab6,currentY,tabWidth,tabHeight);
    createSystemTab(tab7,currentY,tabWidth);
    lv_obj_clear_flag(lv_tabview_get_content(settingsTabview), LV_OBJ_FLAG_SCROLLABLE);
}

uint16_t uiSettingsGetActiveTab()
{
    return lv_tabview_get_tab_act(settingsTabview);
}

void uiSettingsUpdateView()
{
    uiSettingsUpdateSettings();
    uiSettingsUpdateAudio();
    uiSettingsUpdateDisplay();
    uiSettingsUpdateWifi();
    uiSettingsUpdateParamWifi();
#ifdef BLUETOOTH
    uiSettingsUpdateBluetooth();
#endif
    uiSettingsUpdateSd();
    uiSettingsUpdateSystem();
    // To update battery percentage display when setting is changed
    uiUpdatePower();
}