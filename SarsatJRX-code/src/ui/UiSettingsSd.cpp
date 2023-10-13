#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <Settings.h>

// SD card
#define SD_LABEL                "SD card mounted :"
#define SD_LABEL_WIDTH          160
#define TOTAL_SIZE_LABEL        "Total size :"
#define TOTAL_SIZE_LABEL_WIDTH  100
#define USED_SIZE_LABEL         "Used size :"
#define USED_SIZE_LABEL_WIDTH   100
#define LOG_FOLDER_LABEL        "Beacons folder :"
#define LOG_FOLDER_LABEL_WIDTH  160
#define BEACONS_LABEL           "Beacons :"
#define BEACONS_LABEL_WIDTH     100
#define BEACON_LIST_WIDTH       260
#define BEACON_BUTTON_WIDTH     70
#define BEACON_BUTTON_HEIGHT    50
#define BEACON_BUTTON_X1        (BEACON_LIST_WIDTH + 4)
#define BEACON_BUTTON_X2        (BEACON_BUTTON_X1 + BEACON_BUTTON_WIDTH + 4)

// Externs
extern bool readBeaconFromFile(const char * filename);

// SD
static lv_obj_t * sdTitle;
static lv_obj_t * sdToggle;
static lv_obj_t * totalBytesTitle;
static lv_obj_t * totalBytesLabel;
static lv_obj_t * usedBytesTitle;
static lv_obj_t * usedBytesLabel;
static lv_obj_t * logFolderTitle;
static lv_obj_t * logFolderLabel;
static lv_obj_t * beaconsTitle;
static lv_obj_t * beaconsList;
static lv_obj_t * beaconsListButtons;
static lv_obj_t * beaconsUpButton;
static lv_obj_t * beaconsDownButton;
static lv_obj_t * beaconsLoadButton;
static lv_obj_t * beaconsDeleteButton;
static lv_obj_t * currentBeacon = NULL;

static lv_obj_t * deleteConfirmBox;
static int lastBeaconIndex = 0;

static void toggle_sd_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(sdToggle, LV_STATE_CHECKED);
    //Serial.println("Toggle sd :" + String(state));
    Filesystems* filesystems = Filesystems::getFilesystems();
    if(state)
    {
        filesystems->initSdFs();
        uiSetSdCardStatus(filesystems->isSdFilesystemMounted());
        uiSettingsUpdateView();
    }
    else
    {
        filesystems->unmountSdFs();
        uiSetSdCardStatus(filesystems->isSdFilesystemMounted());
        uiSettingsUpdateView();
    }
}

static int beaconCount = 0;
void uiUpdateLogFolder()
{   // Log folder
    Filesystems *filesystems =  Hardware::getHardware()->getFilesystems();
    lv_label_set_text(logFolderLabel,(filesystems->isLogDirReady() ? ("OK (" + String(beaconCount) + " beacon(s))").c_str() : "KO"));
    lv_obj_set_style_text_color(logFolderLabel, (filesystems->isLogDirReady() ? uiOkColor : uiKoColor),0);
}


static void selectBeacon(lv_obj_t* beacon)
{
    if(beacon)
    {
        lv_obj_add_state(beacon, LV_STATE_CHECKED);
        if((currentBeacon != NULL) && (currentBeacon != beacon))
        {
            lv_obj_clear_state(currentBeacon, LV_STATE_CHECKED);
        }
        currentBeacon = beacon;
        lastBeaconIndex = lv_obj_get_index(currentBeacon);
    }
}

static void beacon_clicked_cb(lv_event_t * e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    selectBeacon(obj);
}

static void beacon_up_cb(lv_event_t * e)
{
    if(currentBeacon == NULL) return;
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) 
    {
        uint32_t index = lv_obj_get_index(currentBeacon);
        if(index <= 0) return;
        index--;
        selectBeacon(lv_obj_get_child(beaconsList,index));
        lv_obj_scroll_to_view(currentBeacon, LV_ANIM_ON);
    }    
}

static void beacon_down_cb(lv_event_t * e)
{
    if(currentBeacon == NULL) return;
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) 
    {
        uint32_t index = lv_obj_get_index(currentBeacon);
        index++;
        lv_obj_t *obj = lv_obj_get_child(beaconsList,index);
        if(obj)
        {
            selectBeacon(obj);
            lv_obj_scroll_to_view(currentBeacon, LV_ANIM_ON);
        }
    }    
}

static void beacon_load_cb(lv_event_t * e)
{
    if(currentBeacon == NULL) return;
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) 
    {
        String* fileName = (String*)lv_obj_get_user_data(currentBeacon);
        if(fileName)
        {
            //Serial.printf("Load file %s\n",(*fileName).c_str());
            readBeaconFromFile((*fileName).c_str());
        }
    }
}

static void deleteCurrentBeacon()
{
    if(currentBeacon == NULL) return;
    String* fileName = (String*)lv_obj_get_user_data(currentBeacon);
    if(Filesystems::getFilesystems()->deleteBeacon((*fileName).c_str()))
    {
        //Serial.printf("Deleted file %s\n",(*fileName).c_str());
        uint32_t index = lv_obj_get_index(currentBeacon);
        lv_obj_del(currentBeacon);
        currentBeacon = lv_obj_get_child(beaconsList,index);
        if(!currentBeacon)
        {
            currentBeacon = lv_obj_get_child(beaconsList,index-1);
        }
        selectBeacon(currentBeacon);
        // Update beacon count
        beaconCount--;
        uiUpdateLogFolder();
    }
}

static void beacon_delete_cb(lv_event_t * e)
{
    if(currentBeacon == NULL) return;
    //lv_event_code_t code = lv_event_get_code(e);
    //Serial.printf("Delete cb with event code %d\n",code);
    //if((code != LV_EVENT_SHORT_CLICKED)) return;
    static const char * btns[] = {"OK", "Cancel", NULL};
    deleteConfirmBox = lv_msgbox_create(NULL, "Delete ?", "Do you want to delete this beacon ?", btns, true);
    lv_obj_add_event_cb(deleteConfirmBox, [](lv_event_t * e) 
    {
        if(lv_msgbox_get_active_btn(deleteConfirmBox) == 0)
        {
            deleteCurrentBeacon(); 
        }
        lv_msgbox_close(deleteConfirmBox);
    }, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(deleteConfirmBox);
}

static void beacon_delete_longpress_cb(lv_event_t * e)
{
    deleteCurrentBeacon();
    lv_event_stop_processing(e);
}

void createSdTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight)
{   // SD card mounted
    currentY+=SPACER;
    sdTitle = uiCreateLabel(tab,&style_section_title,SD_LABEL,0,currentY,SD_LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    sdToggle = uiCreateToggle(tab,&style_section_text,toggle_sd_cb,TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+SPACER;
    // Total size
    totalBytesTitle = uiCreateLabel(tab,&style_section_title,TOTAL_SIZE_LABEL,0,currentY,TOTAL_SIZE_LABEL_WIDTH,LINE_HEIGHT);
    totalBytesLabel = uiCreateLabel(tab,&style_section_text,"",TOTAL_SIZE_LABEL_WIDTH,currentY,tabWidth-TOTAL_SIZE_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Used size
    usedBytesTitle = uiCreateLabel(tab,&style_section_title,USED_SIZE_LABEL,0,currentY,USED_SIZE_LABEL_WIDTH,LINE_HEIGHT);
    usedBytesLabel = uiCreateLabel(tab,&style_section_text,"",USED_SIZE_LABEL_WIDTH,currentY,tabWidth-USED_SIZE_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // SarsatJRX folder
    logFolderTitle = uiCreateLabel(tab,&style_section_title,LOG_FOLDER_LABEL,0,currentY,LOG_FOLDER_LABEL_WIDTH,LINE_HEIGHT);
    logFolderLabel = uiCreateLabel(tab,&style_section_text,"",LOG_FOLDER_LABEL_WIDTH,currentY,tabWidth-LOG_FOLDER_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Beacons
    beaconsTitle = uiCreateLabel(tab,&style_section_title,BEACONS_LABEL,0,currentY,BEACONS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    beaconsList = lv_list_create(tab); 
    lv_obj_set_pos(beaconsList,0,currentY);
    int bListHeight = tabHeight-currentY;
    lv_obj_set_size(beaconsList, BEACON_LIST_WIDTH, bListHeight);
    //lv_obj_set_style_pad_row(beaconsList, 2, 0);
    // Up
    beaconsUpButton = uiCreateImageButton(tab,LV_SYMBOL_UP,beacon_up_cb,LV_EVENT_ALL,BEACON_BUTTON_WIDTH, BEACON_BUTTON_HEIGHT,BEACON_BUTTON_X1,currentY);
    // Down
    int bottomButtonY = currentY+bListHeight-BEACON_BUTTON_HEIGHT;
    beaconsDownButton = uiCreateImageButton(tab,LV_SYMBOL_DOWN,beacon_down_cb,LV_EVENT_ALL,BEACON_BUTTON_WIDTH, BEACON_BUTTON_HEIGHT,BEACON_BUTTON_X1,bottomButtonY);
    // Load
    beaconsLoadButton = uiCreateImageButton(tab,LV_SYMBOL_PLAY,beacon_load_cb,LV_EVENT_CLICKED,BEACON_BUTTON_WIDTH, BEACON_BUTTON_HEIGHT,BEACON_BUTTON_X2,currentY);
    // Delete
    beaconsDeleteButton = uiCreateImageButton(tab,LV_SYMBOL_TRASH,beacon_delete_cb,LV_EVENT_SHORT_CLICKED,BEACON_BUTTON_WIDTH, BEACON_BUTTON_HEIGHT,BEACON_BUTTON_X2,bottomButtonY);
    lv_obj_add_event_cb(beaconsDeleteButton, beacon_delete_longpress_cb, LV_EVENT_LONG_PRESSED, NULL);
}



void uiSettingsUpdateSdView()
{
    Hardware* hardware = Hardware::getHardware();
    // SD tab
    Filesystems *filesystems = hardware->getFilesystems();
    // Clean list
    lv_obj_clean(beaconsList);
    beaconCount = 0;
    currentBeacon = NULL;
    if(filesystems->isSdFilesystemMounted())
    {   // SD toogle on
        lv_obj_add_state(sdToggle, LV_STATE_CHECKED);
        FS* sdfs = filesystems->getSdFilesystem();
        File logDir = filesystems->getLogDir();
        if(logDir && logDir.isDirectory())
        {
            File beacon = logDir.openNextFile();
            lv_obj_t * btn;
            char buffer[32];
            while(beacon)
            {
                if(!beacon.isDirectory())
                {
                    String name = beacon.name();
                    if(name.endsWith(LOG_FILE_EXTENSION))
                    {
                        formatBeaconFileName(buffer,name);
                        lv_obj_t *lab = lv_label_create(beaconsList);
                        // Reverse list order to have latest beacons at the top of the list
                        lv_obj_move_background(lab);
                        lv_obj_set_user_data(lab,new String(name));
                        lv_obj_add_event_cb(lab, beacon_clicked_cb, LV_EVENT_CLICKED, NULL);
                        lv_obj_add_style(lab,&style_section_text,0);
                        lv_obj_set_style_text_color(lab,uiOkColor,LV_STATE_CHECKED);
                        lv_obj_add_flag(lab,LV_OBJ_FLAG_CLICKABLE);
                        lv_label_set_text(lab, buffer);
                        beaconCount++;
                    }
                }
                beacon = logDir.openNextFile();
            }
            if(beaconCount > 0)
            {   // Try last selected item
                currentBeacon = lv_obj_get_child(beaconsList, lastBeaconIndex);
                if(!currentBeacon)
                {   // Else first item
                    currentBeacon = lv_obj_get_child(beaconsList, 0);
                }
                if(currentBeacon)
                {
                    lv_obj_add_state(currentBeacon, LV_STATE_CHECKED);
                    lv_obj_scroll_to_view(currentBeacon, LV_ANIM_OFF);
                }
            }
        }
    }
    else
    {   // Wifi toggle on
        lv_obj_clear_state(sdToggle, LV_STATE_CHECKED);
    }
    // total size
    lv_label_set_text(totalBytesLabel,formatMemoryValue((uint32_t)filesystems->getSdTotalBytes(),true).c_str());
    // total size
    lv_label_set_text(usedBytesLabel,formatMemoryValue((uint32_t)filesystems->getSdUsedBytes(),true).c_str());
    // Update log folder
    uiUpdateLogFolder();
}