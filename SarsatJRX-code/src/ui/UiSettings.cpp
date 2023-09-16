#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <extra/libs/qrcode/lv_qrcode.h>
#include <ui/Ui.h>
#include <ui/UiBeacon.h>
#include <Hardware.h>

// Beacon info
#define LINE_HEIGHT         18
#define VERSION_LABEL       "Version :"
#define VERSION_LABEL_WIDTH 80
#define VBAT_LABEL          "Vbat :"
#define VBAT_LABEL_WIDTH    50
#define DATE_LABEL          "Date :"
#define DATE_LABEL_WIDTH    50
#define CHIP_LABEL          "Chip :"
#define CHIP_LABEL_WIDTH    50
#define CHIPM_LABEL         "- Model ="
#define CHIPM_LABEL_WIDTH   80
#define CHIPR_LABEL         "- Rev. ="
#define CHIPR_LABEL_WIDTH   80
#define CHIPC_LABEL         "- Cores ="
#define CHIPC_LABEL_WIDTH   80
#define CHIPF_LABEL         "- Freq. ="
#define CHIPF_LABEL_WIDTH   80

lv_obj_t * settingsTabview;

// System
static lv_obj_t * versionTitle;
static lv_obj_t * versionLabel;
static lv_obj_t * vBatTitle;
static lv_obj_t * vBatLabel;
static lv_obj_t * dateTitle;
static lv_obj_t * dateLabel;
static lv_obj_t * chipTitle;
static lv_obj_t * chipModelTitle;
static lv_obj_t * chipModelLabel;
static lv_obj_t * chipRevisionTitle;
static lv_obj_t * chipRevisionLabel;
static lv_obj_t * chipCoresTitle;
static lv_obj_t * chipCoresLabel;
static lv_obj_t * chipFreqTitle;
static lv_obj_t * chipFreqLabel;
static lv_obj_t * ramTitle;
static lv_obj_t * ramSizeTitle;
static lv_obj_t * ramSizeLabel;
static lv_obj_t * ramFreeTitle;
static lv_obj_t * ramFreeLabel;
static lv_obj_t * psRamTitle;
static lv_obj_t * psRamSizeTitle;
static lv_obj_t * psRamSizeLabel;
static lv_obj_t * psRamFreeTitle;
static lv_obj_t * psRamFreeLabel;
static lv_obj_t * flashTitle;
static lv_obj_t * flashSizeTitle;
static lv_obj_t * flashSizeLabel;
static lv_obj_t * flashFreqTitle;
static lv_obj_t * flashFreqLabel;


void createSystemTab(lv_obj_t * tab, int currentY, int tabWidth)
{  
    // Version
    versionTitle = uiCreateLabel(tab,&style_section_title,VERSION_LABEL,0,currentY,VERSION_LABEL_WIDTH,LINE_HEIGHT);
    versionLabel = uiCreateLabel(tab,&style_section_text,SARSAT_JRX_VERSION,VERSION_LABEL_WIDTH,currentY,tabWidth-VERSION_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Vbat
    vBatTitle = uiCreateLabel(tab,&style_section_title,VBAT_LABEL,0,currentY,VBAT_LABEL_WIDTH,LINE_HEIGHT);
    vBatLabel = uiCreateLabel(tab,&style_section_text,"",VBAT_LABEL_WIDTH,currentY,tabWidth-VBAT_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Date
    dateTitle = uiCreateLabel(tab,&style_section_title,DATE_LABEL,0,currentY,DATE_LABEL_WIDTH,LINE_HEIGHT);
    dateLabel = uiCreateLabel(tab,&style_section_text,"",DATE_LABEL_WIDTH,currentY,tabWidth-DATE_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Chip
    chipTitle = uiCreateLabel(tab,&style_section_title,CHIP_LABEL,0,currentY,CHIP_LABEL_WIDTH,LINE_HEIGHT);
    chipModelTitle = uiCreateLabel(tab,&style_section_title,CHIPM_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPM_LABEL_WIDTH,LINE_HEIGHT);
    chipModelLabel = uiCreateLabel(tab,&style_section_text,ESP.getChipModel(),CHIP_LABEL_WIDTH+CHIPM_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPM_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    chipRevisionTitle = uiCreateLabel(tab,&style_section_title,CHIPR_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPR_LABEL_WIDTH,LINE_HEIGHT);
    chipRevisionLabel = uiCreateLabel(tab,&style_section_text,(String(ESP.getChipRevision())).c_str(),CHIP_LABEL_WIDTH+CHIPR_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPR_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    chipCoresTitle = uiCreateLabel(tab,&style_section_title,CHIPC_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPC_LABEL_WIDTH,LINE_HEIGHT);
    chipCoresLabel = uiCreateLabel(tab,&style_section_text,(String(ESP.getChipCores())).c_str(),CHIP_LABEL_WIDTH+CHIPC_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPC_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    chipFreqTitle = uiCreateLabel(tab,&style_section_title,CHIPF_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPF_LABEL_WIDTH,LINE_HEIGHT);
    chipFreqLabel = uiCreateLabel(tab,&style_section_text,(String(ESP.getCpuFreqMHz()) + " Mhz").c_str(),CHIP_LABEL_WIDTH+CHIPF_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPF_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    //currentY+=LINE_HEIGHT;
}

void createWifiTab(lv_obj_t * tab, int currentY, int tabWidth)
{
    // TODO

}

void createSdTab(lv_obj_t * tab, int currentY, int tabWidth)
{
    // TODO
}

void createRadioTab(lv_obj_t * tab, int currentY, int tabWidth)
{
    // TODO
}

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

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(settingsTabview, "Sys.");
    lv_obj_t * tab2 = lv_tabview_add_tab(settingsTabview, "Wifi");
    lv_obj_t * tab3 = lv_tabview_add_tab(settingsTabview, "SD");
    lv_obj_t * tab4 = lv_tabview_add_tab(settingsTabview, "Radio");
    lv_obj_add_style(tab1, &style_pad_small, 0);
    lv_obj_add_style(tab2, &style_pad_small, 0);
    lv_obj_add_style(tab3, &style_pad_small, 0);
    lv_obj_add_style(tab4, &style_pad_small, 0);
    int tabWidth = lv_obj_get_width(tab1) - 8; // 2*4 px padding

    //lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_AMBER, 3), 0);
    //lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);

    // Add content to the tabs
    int currentY = 0;
    createSystemTab(tab1,currentY,tabWidth);
    createWifiTab(tab2,currentY,tabWidth);
    createSdTab(tab3,currentY,tabWidth);
    createRadioTab(tab4,currentY,tabWidth);
    lv_obj_clear_flag(lv_tabview_get_content(settingsTabview), LV_OBJ_FLAG_SCROLLABLE);
}

void uiSettingsUpdateView()
{
    Hardware* hardware = Hardware::getHardware();
    // Vbat
    lv_label_set_text(vBatLabel,hardware->getVccStringValue().c_str());
    // Date
    lv_label_set_text(dateLabel,(hardware->getRtc()->getDateString() + " - " + hardware->getRtc()->getTimeString()).c_str());
}