#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <Settings.h>

// Externs
extern void createSystemTab(lv_obj_t * tab, int currentY, int tabWidth);
extern void createDisplayTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight);
extern void createAudioTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight);
extern void createWifiTab(lv_obj_t * tab, int currentY, int tabWidth);
extern void createNetworkTab(lv_obj_t * tab, int currentY, int tabWidth);
extern void createSdTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight);

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

    /*Add 6 tabs */
    lv_obj_t * tab1 = lv_tabview_add_tab(settingsTabview, "Sys.");
    lv_obj_t * tab2 = lv_tabview_add_tab(settingsTabview, "Wifi");
    lv_obj_t * tab3 = lv_tabview_add_tab(settingsTabview, "Net.");
    lv_obj_t * tab4 = lv_tabview_add_tab(settingsTabview, "SD");
    lv_obj_t * tab5 = lv_tabview_add_tab(settingsTabview, "Disp.");
    lv_obj_t * tab6 = lv_tabview_add_tab(settingsTabview, "Audio");
    lv_obj_add_style(tab1, &style_pad_small, 0);
    lv_obj_add_style(tab2, &style_pad_small, 0);
    lv_obj_add_style(tab3, &style_pad_small, 0);
    lv_obj_add_style(tab4, &style_pad_small, 0);
    lv_obj_add_style(tab5, &style_pad_small, 0);
    lv_obj_add_style(tab6, &style_pad_small, 0);
    int tabWidth = lv_obj_get_width(tab1) - 8; // 2*4 px padding
    int tabHeight = lv_obj_get_height(tab1) - 8 - FOOTER_HEIGHT; // 2*4 px padding

    //lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_AMBER, 3), 0);
    //lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);

    // Add content to the tabs
    int currentY = 0;
    createSystemTab(tab1,currentY,tabWidth);
    createWifiTab(tab2,currentY,tabWidth);
    createNetworkTab(tab3,currentY,tabWidth);
    createSdTab(tab4,currentY,tabWidth,tabHeight);
    createDisplayTab(tab5,currentY,tabWidth,tabHeight);
    createAudioTab(tab6,currentY,tabWidth,tabHeight);
    lv_obj_clear_flag(lv_tabview_get_content(settingsTabview), LV_OBJ_FLAG_SCROLLABLE);
}

void uiSettingsUpdateView()
{
    uiSettingsUpdateSystem();
    uiSettingsUpdateWifi();
    uiSettingsUpdateSd();
    uiSettingsUpdateDisplay();
    uiSettingsUpdateAudio();
}