#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>

// System info
#define VERSION_LABEL       "Version:"
#define VERSION_LABEL_WIDTH 80
#define SKETCH_LABEL        "Sketch:"
#define SKETCH_LABEL_WIDTH  80
#define VBAT_LABEL          "Vbat:"
#define VBAT_LABEL_WIDTH    80
#define VBAT_TEXT_WIDTH     50
#define VBAT_RAW_TEXT_WIDTH 90

#define UPTIME_LABEL          "Uptime:"
#define UPTIME_LABEL_WIDTH    80

#define CHIP_LABEL          "Chip:"
#define CHIP_LABEL_WIDTH    50
#define CHIPM_LABEL         "- Model:"
#define CHIPM_LABEL_WIDTH   70
#define CHIPC_LABEL         "- Cores:"
#define CHIPC_LABEL_WIDTH   70
#define CHIPF_LABEL         "- Freq.:"
#define CHIPF_LABEL_WIDTH   70

#define RAM_LABEL          "Ram:"
#define RAM_LABEL_WIDTH    50
#define RAMS_LABEL         "- Total:"
#define RAMS_LABEL_WIDTH   70
#define RAMF_LABEL         "- Free:"
#define RAMF_LABEL_WIDTH   70

#define PSRAM_LABEL          "PS-Ram:"
#define PSRAM_LABEL_WIDTH    75
#define PSRAMS_LABEL         "- Total:"
#define PSRAMS_LABEL_WIDTH   75
#define PSRAMF_LABEL         "- Free:"
#define PSRAMF_LABEL_WIDTH   75

#define FLASH_LABEL          "Flash:"
#define FLASH_LABEL_WIDTH    75
#define FLASHS_LABEL         "- Size:"
#define FLASHS_LABEL_WIDTH   75
#define FLASHF_LABEL         "- Speed:"
#define FLASHF_LABEL_WIDTH   75

// System
static lv_obj_t * versionTitle;
static lv_obj_t * versionLabel;
static lv_obj_t * sketchTitle;
static lv_obj_t * sketchLabel;
static lv_obj_t * vBatTitle;
static lv_obj_t * vBatLabel;
static lv_obj_t * vBatRawLabel;
static lv_obj_t * powerStateLabel;
static lv_obj_t * uptimeTitle;
static lv_obj_t * uptimeLabel;
static lv_obj_t * chipTitle;
static lv_obj_t * chipModelTitle;
static lv_obj_t * chipModelLabel;
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
    vBatLabel = uiCreateLabel(tab,&style_section_text,"",VBAT_LABEL_WIDTH,currentY,VBAT_TEXT_WIDTH,LINE_HEIGHT);
    vBatRawLabel = uiCreateLabel(tab,&style_section_text,"",VBAT_LABEL_WIDTH+VBAT_TEXT_WIDTH,currentY,VBAT_RAW_TEXT_WIDTH,LINE_HEIGHT);
    // Power State
    powerStateLabel = uiCreateLabel(tab,&style_section_text,"",VBAT_LABEL_WIDTH+VBAT_TEXT_WIDTH+VBAT_RAW_TEXT_WIDTH,currentY,tabWidth-VBAT_LABEL_WIDTH-VBAT_TEXT_WIDTH-VBAT_RAW_TEXT_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Uptime
    uptimeTitle = uiCreateLabel(tab,&style_section_title,UPTIME_LABEL,0,currentY,UPTIME_LABEL_WIDTH,LINE_HEIGHT);
    uptimeLabel = uiCreateLabel(tab,&style_section_text,"",UPTIME_LABEL_WIDTH,currentY,tabWidth-UPTIME_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Sketch
    sketchTitle = uiCreateLabel(tab,&style_section_title,SKETCH_LABEL,0,currentY,SKETCH_LABEL_WIDTH,LINE_HEIGHT);
    sketchLabel = uiCreateLabel(tab,&style_section_text,formatSketchInformation(ESP.getSketchSize(),ESP.getSketchMD5()).c_str(),SKETCH_LABEL_WIDTH,currentY,tabWidth-SKETCH_LABEL_WIDTH,LINE_HEIGHT);
    // lv_label_set_long_mode(sketchLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    currentY+=LINE_HEIGHT;

    // Chip
    chipTitle = uiCreateLabel(tab,&style_section_title,CHIP_LABEL,0,currentY,CHIP_LABEL_WIDTH,LINE_HEIGHT);
    chipModelTitle = uiCreateLabel(tab,&style_section_title,CHIPM_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPM_LABEL_WIDTH,LINE_HEIGHT);
    chipModelLabel = uiCreateLabel(tab,&style_section_text,ESP.getChipModel(),CHIP_LABEL_WIDTH+CHIPM_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPM_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    chipCoresTitle = uiCreateLabel(tab,&style_section_title,CHIPC_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPC_LABEL_WIDTH,LINE_HEIGHT);
    chipCoresLabel = uiCreateLabel(tab,&style_section_text,(String(ESP.getChipCores())).c_str(),CHIP_LABEL_WIDTH+CHIPC_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPC_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    chipFreqTitle = uiCreateLabel(tab,&style_section_title,CHIPF_LABEL,CHIP_LABEL_WIDTH,currentY,CHIPF_LABEL_WIDTH,LINE_HEIGHT);
    chipFreqLabel = uiCreateLabel(tab,&style_section_text,formatFrequencyValue(ESP.getCpuFreqMHz()).c_str(),CHIP_LABEL_WIDTH+CHIPF_LABEL_WIDTH,currentY,tabWidth-CHIP_LABEL_WIDTH-CHIPF_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Ram
    ramTitle = uiCreateLabel(tab,&style_section_title,RAM_LABEL,0,currentY,RAM_LABEL_WIDTH,LINE_HEIGHT);
    ramSizeTitle = uiCreateLabel(tab,&style_section_title,RAMS_LABEL,RAM_LABEL_WIDTH,currentY,RAMS_LABEL_WIDTH,LINE_HEIGHT);
    ramSizeLabel = uiCreateLabel(tab,&style_section_text,"",RAM_LABEL_WIDTH+RAMS_LABEL_WIDTH,currentY,tabWidth-RAM_LABEL_WIDTH-RAMS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    ramFreeTitle = uiCreateLabel(tab,&style_section_title,RAMF_LABEL,RAM_LABEL_WIDTH,currentY,RAMF_LABEL_WIDTH,LINE_HEIGHT);
    ramFreeLabel = uiCreateLabel(tab,&style_section_text,"",RAM_LABEL_WIDTH+RAMF_LABEL_WIDTH,currentY,tabWidth-RAM_LABEL_WIDTH-RAMF_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // PS-Ram
    psRamTitle = uiCreateLabel(tab,&style_section_title,PSRAM_LABEL,0,currentY,PSRAM_LABEL_WIDTH,LINE_HEIGHT);
    psRamSizeTitle = uiCreateLabel(tab,&style_section_title,PSRAMS_LABEL,PSRAM_LABEL_WIDTH,currentY,PSRAMS_LABEL_WIDTH,LINE_HEIGHT);
    psRamSizeLabel = uiCreateLabel(tab,&style_section_text,"",PSRAM_LABEL_WIDTH+PSRAMS_LABEL_WIDTH,currentY,tabWidth-PSRAM_LABEL_WIDTH-PSRAMS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    psRamFreeTitle = uiCreateLabel(tab,&style_section_title,PSRAMF_LABEL,PSRAM_LABEL_WIDTH,currentY,PSRAMF_LABEL_WIDTH,LINE_HEIGHT);
    psRamFreeLabel = uiCreateLabel(tab,&style_section_text,"",PSRAM_LABEL_WIDTH+PSRAMF_LABEL_WIDTH,currentY,tabWidth-PSRAM_LABEL_WIDTH-PSRAMF_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;

    // Flash
    flashTitle = uiCreateLabel(tab,&style_section_title,FLASH_LABEL,0,currentY,FLASH_LABEL_WIDTH,LINE_HEIGHT);
    flashSizeTitle = uiCreateLabel(tab,&style_section_title,FLASHS_LABEL,FLASH_LABEL_WIDTH,currentY,FLASHS_LABEL_WIDTH,LINE_HEIGHT);
    flashSizeLabel = uiCreateLabel(tab,&style_section_text,"",FLASH_LABEL_WIDTH+FLASHS_LABEL_WIDTH,currentY,tabWidth-FLASH_LABEL_WIDTH-FLASHS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    flashFreqTitle = uiCreateLabel(tab,&style_section_title,FLASHF_LABEL,FLASH_LABEL_WIDTH,currentY,FLASHF_LABEL_WIDTH,LINE_HEIGHT);
    flashFreqLabel = uiCreateLabel(tab,&style_section_text,"",FLASH_LABEL_WIDTH+FLASHF_LABEL_WIDTH,currentY,tabWidth-FLASH_LABEL_WIDTH-FLASHF_LABEL_WIDTH,LINE_HEIGHT);
    //currentY+=LINE_HEIGHT;
}

void uiSettingsUpdateSystem()
{
    // System tab
    uiSettingsUpdateSystemPower();
    uiSettingsUpdateSystemUptime();
    // Ram
    lv_label_set_text(ramSizeLabel,formatMemoryValue(ESP.getHeapSize(),true).c_str());
    lv_label_set_text(ramFreeLabel,formatMemoryValue(ESP.getFreeHeap(),true).c_str());
    // PS Ram
    lv_label_set_text(psRamSizeLabel,formatMemoryValue(ESP.getPsramSize(),false).c_str());
    lv_label_set_text(psRamFreeLabel,formatMemoryValue(ESP.getFreePsram(),false).c_str());
    // Flash
    lv_label_set_text(flashSizeLabel,formatMemoryValue(ESP.getFlashChipSize(),false).c_str());
    lv_label_set_text(flashFreqLabel,formatHzFrequencyValue(ESP.getFlashChipSpeed()).c_str());
}

void uiSettingsUpdateSystemRawPower()
{
    Hardware* hardware = Hardware::getHardware();
    Power* power = hardware->getPower();
    // Vbat
    String vBatRawString =  " (" + power->getRawVccStringValue() + ")";
    lv_label_set_text(vBatRawLabel,vBatRawString.c_str());
}


void uiSettingsUpdateSystemPower()
{
    Hardware* hardware = Hardware::getHardware();
    Power* power = hardware->getPower();
    // Vbat
    String vBatString =  + " (" + power->getRawVccStringValue() + ")";
    lv_label_set_text(vBatLabel,power->getVccStringValue().c_str());
    uiSettingsUpdateSystemRawPower();
    // Power state
    String powerString = power->getPowerStateString();
    if((power->getPowerState() != Power::PowerState::NO_BATTERY)) powerString = (powerString + " ("+ power->getBatteryPercentage() +"%)");
    lv_label_set_text(powerStateLabel,powerString.c_str());
}

void uiSettingsUpdateSystemUptime()
{
    Hardware* hardware = Hardware::getHardware();
    // Uptime
    lv_label_set_text(uptimeLabel,hardware->getRtc()->getUptimeString().c_str());
    uiSettingsUpdateSystemRawPower();
}
