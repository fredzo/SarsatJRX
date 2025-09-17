#include <ui/UiSettings.h>

#ifdef BLUETOOTH

#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <WiFi.h>
#include <BluetoothManager.h>
#include <Settings.h>

// Bluetooth
#define BLUETOOTH_LABEL          "Wifi:"
#define BLUETOOTH_LABEL_WIDTH    80
#define BLUETOOTH_TOGGLE_X       120
// Status
#define STATUS_LABEL        "Status:"
#define STATUS_LABEL_WIDTH  80
// Clients
#define CLIENTS_LABEL       "Clients:"
#define CLIENTS_LABEL_WIDTH 80

// Wifi
static lv_obj_t * bluetoothTitle;
static lv_obj_t * bluetoothToggle;
static lv_obj_t * statusTitle;
static lv_obj_t * statusLabel;
static lv_obj_t * clientsTitle;
static lv_obj_t * clientsLabel;

static void toggle_bluetooth_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(bluetoothToggle, LV_STATE_CHECKED);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setBluetoothState(state);
    if(state)
    {
        bluetoothManagerStart();
    }
    else
    {
        bluetoothManagerStop();
    }
}

void createBluetoothTab(lv_obj_t * tab, int currentY, int tabWidth)
{
    // Bluetooth On/Off -> save to EEProm
    currentY+=SPACER;
    bluetoothTitle = uiCreateLabel(tab,&style_section_title,BLUETOOTH_LABEL,0,currentY,BLUETOOTH_LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    bluetoothToggle = uiCreateToggle(tab,&style_section_text,toggle_bluetooth_cb,BLUETOOTH_TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+SPACER;
    // Status (Connected etc)
    statusTitle = uiCreateLabel(tab,&style_section_title,STATUS_LABEL,0,currentY,STATUS_LABEL_WIDTH,LINE_HEIGHT);
    statusLabel = uiCreateLabel(tab,&style_section_text,"",STATUS_LABEL_WIDTH,currentY,tabWidth-STATUS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
    // Clients
    clientsTitle = uiCreateLabel(tab,&style_section_title,CLIENTS_LABEL,SEC_COL_X,currentY,CLIENTS_LABEL_WIDTH,LINE_HEIGHT);
    clientsLabel = uiCreateLabel(tab,&style_section_text,"",CLIENTS_LABEL_WIDTH,currentY,tabWidth-CLIENTS_LABEL_WIDTH,LINE_HEIGHT);
    currentY+=LINE_HEIGHT;
}

void uiSettingsUpdateBluetooth()
{   // Bluetooth state
    BluetoothStatus status = bluetoothManagerGetStatus();
    if(status == BluetoothStatus::OFF)
    {   // Wifi toggle off
        lv_obj_clear_state(bluetoothToggle, LV_STATE_CHECKED);
    }
    else
    {   // Wifi toggle on
        lv_obj_add_state(bluetoothToggle, LV_STATE_CHECKED);
    }

    // Status (Connected etc)
    lv_label_set_text(statusLabel,bluetoothManagerGetStatusString().c_str());
    lv_obj_set_style_text_color(statusLabel, (bluetoothManagerIsConnected() ? uiOkColor : uiKoColor),0);
    // Clients
    //lv_label_set_text(clientsLabel,bluetoothManagerGetClients().c_str());

}

#endif