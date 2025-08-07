#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <Settings.h>

// Display
#define TOGGLE_X_DISPLAY              180
#define LABEL_WIDTH                   200
#define SCREEN_REVERSE_LABEL          "Screen reverse:"
#define SCREEN_OFF_ON_CHARGE_LABEL    "Screen off on charge:"
#define SHOW_BAT_PERCENTAGE_LABEL     "Battery:      Show %:"
#define SHOW_BAT_WARN_LABEL           "Warn:"
#define SHOW_BAT_WARN_LABEL_WIDTH     50
#define FRAME_SIMU_LABEL_WIDTH        130
#define ALLOW_FRAM_SIMU_LABEL         "Frame simu:"


// Display
static lv_obj_t * displayTab;
static int displayTabWidth;
static int displayTabHeight;
static lv_obj_t * displayReverseLabel;
static lv_obj_t * displayReverseToggle;
static lv_obj_t * screenOffOnChargeLabel;
static lv_obj_t * screenOffOnChargeToggle;
static lv_obj_t * screenOffOnButton;
static lv_obj_t * showBatPercentageLabel;
static lv_obj_t * showBatPercentageToggle;
static lv_obj_t * showBatWarnMessageLabel;
static lv_obj_t * showBatWarnMessageToggle;
static lv_obj_t * allowFrameSimuLabel;
static lv_obj_t * allowFrameSimuToggle;

// Date and time picker
#define DATEPICKER_HEIGHT       100
#define DATEPICKER_WIDTH        400
#define TIME_SPINBOX_WIDTH      50
#define TIME_SPINBOX_HEIGTH     DATEPICKER_HEIGHT
#define CALENDAR_WIDTH          420
#define CALENDAR_HEIGHT         310

static Rtc::Date selectedDate;

// Global objects
static lv_obj_t * dateButton;
static lv_obj_t * dateButtonLabel;
static lv_obj_t * calendar;
static lv_obj_t * datePickerModal;
static lv_obj_t * hourSpinbox;
static lv_obj_t * minuteSpinbox;
static lv_obj_t * secondSpinbox;

typedef enum { TIME_DATA_HOUR, TIME_DATA_MINUTE, TIME_DATA_SECOND} TimeData;

static TimeData hour = TimeData::TIME_DATA_HOUR;
static TimeData minute = TimeData::TIME_DATA_MINUTE;
static TimeData second = TimeData::TIME_DATA_SECOND;

void uiSettingsUpdateDate()
{
    char buf[16];
    lv_snprintf(buf, sizeof(buf), "%04d-%02d-%02d", selectedDate.year, selectedDate.month, selectedDate.day);
    lv_label_set_text(dateButtonLabel, buf);
}

static lv_calendar_date_t lastPressedDate;
static bool dateSelected = false;

// Called when a date is pressed
static void calendarDatePressedCb(lv_event_t *e) {
    lv_calendar_date_t sel;
    if (lv_calendar_get_pressed_date(calendar, &sel)) {
        lastPressedDate = sel;
        dateSelected = true;
    }
}

// === Callback to close the calendar modal ===
static void closeCalendarCb(lv_event_t *e) 
{
    if (dateSelected) 
    {
        selectedDate.year = lastPressedDate.year;
        selectedDate.month = lastPressedDate.month;
        selectedDate.day = lastPressedDate.day;
        Rtc* rtc = Rtc::getRtc();
        Rtc::Date currentDate = rtc->getDate();
        currentDate.year = selectedDate.year;
        currentDate.month = selectedDate.month;
        currentDate.day = selectedDate.day;
        rtc->setDate(currentDate);
        uiSettingsUpdateDate();
    }
    lv_obj_del_async(datePickerModal);
}

static void timeSpinboxChanged(TimeData dataType,int increment)
{   // Update RTC
    Rtc* rtc = Rtc::getRtc();
    Rtc::Date currentDate = rtc->getDate();
    switch(dataType)
    {
        case TimeData::TIME_DATA_HOUR:
            currentDate.hour = atoi(lv_label_get_text(hourSpinbox));
            currentDate.hour += increment;
            if(currentDate.hour > 23) currentDate.hour = 0;
            if(currentDate.hour < 0) currentDate.hour = 23;
            lv_label_set_text_fmt(hourSpinbox,"%02d",selectedDate.hour);
            break;
        case TimeData::TIME_DATA_MINUTE:
            currentDate.minute = atoi(lv_label_get_text(minuteSpinbox));
            currentDate.minute += increment;
            if(currentDate.minute > 59) currentDate.minute = 0;
            if(currentDate.minute < 0) currentDate.minute = 59;
            lv_label_set_text_fmt(minuteSpinbox,"%02d",selectedDate.minute);
            break;
        case TimeData::TIME_DATA_SECOND:
        default:
            currentDate.second = atoi(lv_label_get_text(secondSpinbox));
            currentDate.second += increment;
            if(currentDate.second > 59) currentDate.second = 0;
            if(currentDate.second < 0) currentDate.second = 59;
            lv_label_set_text_fmt(secondSpinbox,"%02d",selectedDate.second);
            break;
    }
    rtc->setDate(currentDate);
}

// === Create a spinbox with stacked up/down arrow buttons ===
static lv_obj_t *createSpinboxWithArrows(lv_obj_t *parent, int x, int y, TimeData* timeData) 
{
    lv_obj_t *col = lv_obj_create(parent);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(col, TIME_SPINBOX_WIDTH, TIME_SPINBOX_HEIGTH);
    lv_obj_set_style_bg_opa(col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(col,0,0);
    lv_obj_set_style_pad_all(col, 0, 0);
    lv_obj_set_style_radius(col, 0, 0);
    lv_obj_clear_flag(col, LV_OBJ_FLAG_SCROLLABLE);

    if((x >= 0) && (y>=0))
    {
        lv_obj_set_pos(col,x,y);
    }

    // ↑ Up button
    lv_obj_t *btnUp = lv_btn_create(col);
    lv_obj_set_style_bg_img_src(btnUp, LV_SYMBOL_UP, 0);
    lv_obj_set_size(btnUp,TIME_SPINBOX_WIDTH,25);

    // Spinbox
    lv_obj_t *sb = lv_textarea_create(col);
    lv_obj_t * label = lv_textarea_get_label(sb);
    lv_obj_set_style_text_font(label,font_mono_medium,0);
    lv_textarea_set_one_line(sb, true);
    lv_textarea_set_cursor_click_pos(sb, false);
    lv_obj_set_style_pad_all(sb, 0, 0);
    lv_obj_set_style_text_align(sb, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_clear_flag(sb, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(sb,TIME_SPINBOX_WIDTH,20);
    lv_obj_set_style_opa(sb, LV_OPA_TRANSP, LV_PART_CURSOR);

    // ↓ Down button
    lv_obj_t *btnDown = lv_btn_create(col);
    lv_obj_set_style_bg_img_src(btnDown, LV_SYMBOL_DOWN, 0);
    lv_obj_set_size(btnDown,TIME_SPINBOX_WIDTH,25);

    // Increment callback
    lv_obj_add_event_cb(btnUp, [](lv_event_t *e) {
        timeSpinboxChanged(*((TimeData*)e->user_data),1);
    }, LV_EVENT_CLICKED, timeData);

    // Decrement callback
    lv_obj_add_event_cb(btnDown, [](lv_event_t *e) {
        timeSpinboxChanged(*((TimeData*)e->user_data),-1);
    }, LV_EVENT_CLICKED, timeData);

    return label;
}

// === Open the modal calendar window ===
static void openCalendarModal(lv_event_t *e) {
    lv_obj_t *scr = lv_scr_act();

    // Dark background
    datePickerModal = lv_obj_create(scr);
    lv_obj_set_size(datePickerModal, lv_pct(100), lv_pct(100));
    lv_obj_center(datePickerModal);
    lv_obj_set_style_bg_opa(datePickerModal, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(datePickerModal, lv_color_black(), 0);
    lv_obj_clear_flag(datePickerModal, LV_OBJ_FLAG_SCROLLABLE);

    // Modal container
    lv_obj_t *modal = lv_obj_create(datePickerModal);
    lv_obj_set_size(modal, CALENDAR_WIDTH, CALENDAR_HEIGHT);
    lv_obj_center(modal);
    lv_obj_set_style_pad_all(modal, 10, 0);
    lv_obj_set_flex_flow(modal, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(modal, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Calendar
    calendar = lv_calendar_create(modal);
    lv_obj_set_size(calendar, CALENDAR_WIDTH-40, CALENDAR_HEIGHT-75);
    lv_calendar_set_today_date(calendar, selectedDate.year, selectedDate.month, selectedDate.day);
    lv_calendar_set_showed_date(calendar, selectedDate.year, selectedDate.month);

    // Optional: add dropdown header
    lv_calendar_header_dropdown_create(calendar);
    lv_calendar_header_dropdown_set_year_list(calendar,"2025\n2026\n2027\n2028\n2029\n2030\n2031\n2032\n2033\n2034\n2035\n2036\n2037\n2038\n2039\n2040");

    // OK button
    lv_obj_t *btnOk = lv_btn_create(modal);
    lv_obj_t *lblOk = lv_label_create(btnOk);
    lv_label_set_text(lblOk, "OK");
    lv_obj_center(lblOk);
    lv_obj_add_event_cb(btnOk, closeCalendarCb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(calendar, calendarDatePressedCb, LV_EVENT_PRESSED, NULL);
}

// === Create the full date + time UI ===
void createDateTimeUI(lv_obj_t * tab, int x, int y, int width, int heigth) {
    // Main horizontal container
    lv_obj_t *mainCont = lv_obj_create(tab);
    lv_obj_set_pos(mainCont,x,y);
    lv_obj_set_size(mainCont, width, heigth);
    lv_obj_set_style_bg_opa(mainCont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(mainCont,0,0);
    lv_obj_set_style_pad_all(mainCont, 0, 0);
    lv_obj_set_style_radius(mainCont, 0, 0);
    lv_obj_clear_flag(mainCont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(mainCont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(mainCont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Button showing selected date
    dateButton = lv_btn_create(mainCont);
    dateButtonLabel = lv_label_create(dateButton);
    lv_obj_center(dateButtonLabel);
    lv_obj_add_event_cb(dateButton, openCalendarModal, LV_EVENT_CLICKED, NULL);

    lv_obj_set_style_text_align(uiCreateLabel (mainCont,&style_section_title,"-",-1,-1,20,20),LV_TEXT_ALIGN_CENTER,0);

    // Hour spinbox
    hourSpinbox = createSpinboxWithArrows(mainCont, -1, -1, &hour);

    lv_obj_set_style_text_align(uiCreateLabel (mainCont,&style_section_title,":",-1,-1,20,20),LV_TEXT_ALIGN_CENTER,0);

    // Minute spinbox
    minuteSpinbox = createSpinboxWithArrows(mainCont, -1, -1, &minute);

    lv_obj_set_style_text_align(uiCreateLabel (mainCont,&style_section_title,":",-1,-1,20,20),LV_TEXT_ALIGN_CENTER,0);

    // Second spinbox
    secondSpinbox = createSpinboxWithArrows(mainCont, -1, -1, &second);
}

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

static void toggleShowBatWarnMessageCb(lv_event_t * e)
{
    bool state = lv_obj_has_state(showBatWarnMessageToggle, LV_STATE_CHECKED);
    Settings::getSettings()->setShowBatteryWarnMessage(state);
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
    displayReverseToggle = uiCreateToggle(tab,&style_section_text,toggleDisplayReverseCb,TOGGLE_X_DISPLAY,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    // Allow frame simu
    allowFrameSimuLabel  = uiCreateLabel (tab,&style_section_title,ALLOW_FRAM_SIMU_LABEL,TOGGLE_X_DISPLAY+TOGGLE_WIDTH+20,currentY,FRAME_SIMU_LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    allowFrameSimuToggle = uiCreateToggle(tab,&style_section_text,toggleAllowFrameSimuCb,TOGGLE_X_DISPLAY+TOGGLE_WIDTH+FRAME_SIMU_LABEL_WIDTH,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Screen off on charge
    screenOffOnChargeLabel  = uiCreateLabel (tab,&style_section_title,SCREEN_OFF_ON_CHARGE_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    screenOffOnChargeToggle = uiCreateToggle(tab,&style_section_text,toggleScreenOffOnChargeCb,TOGGLE_X_DISPLAY,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    screenOffOnButton = uiCreateLabelButton(tab,"Screen Off",[](lv_event_t * e) {
        if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
            Serial.println("Screen Off pressed");
            Hardware::getHardware()->getDisplay()->backlightOff();
        }
    },LV_EVENT_CLICKED,100,BUTTON_HEIGHT,TOGGLE_X_DISPLAY+FRAME_SIMU_LABEL_WIDTH,currentY-SPACER/2);
    currentY+=TOGGLE_LINE_HEIGHT+2*SPACER;
    // Show bat percentage
    showBatPercentageLabel  = uiCreateLabel (tab,&style_section_title,SHOW_BAT_PERCENTAGE_LABEL,0,currentY,LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    showBatPercentageToggle = uiCreateToggle(tab,&style_section_text,toggleShowBatPercentageCb,TOGGLE_X_DISPLAY,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    // Warn message
    showBatWarnMessageLabel  = uiCreateLabel (tab,&style_section_title,SHOW_BAT_WARN_LABEL,TOGGLE_X_DISPLAY+TOGGLE_WIDTH+20,currentY,FRAME_SIMU_LABEL_WIDTH,TOGGLE_LINE_HEIGHT);
    showBatWarnMessageToggle = uiCreateToggle(tab,&style_section_text,toggleShowBatWarnMessageCb,TOGGLE_X_DISPLAY+TOGGLE_WIDTH+FRAME_SIMU_LABEL_WIDTH,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentY+=TOGGLE_LINE_HEIGHT+SPACER;
    // Date/time picker
    createDateTimeUI(tab, 12, currentY, DATEPICKER_WIDTH, DATEPICKER_HEIGHT);
}

void uiSettingsUpdateDateAndTime()
{
    Rtc::Date oldDate = selectedDate;
    selectedDate = Hardware::getHardware()->getRtc()->getDate();
    if((selectedDate.day != oldDate.day) || (selectedDate.month != oldDate.month) || (selectedDate.year != oldDate.year)) uiSettingsUpdateDate();
    if(selectedDate.hour != oldDate.hour)  lv_label_set_text_fmt(hourSpinbox,"%02d",selectedDate.hour);
    if(selectedDate.minute != oldDate.minute) lv_label_set_text_fmt(minuteSpinbox,"%02d",selectedDate.minute);
    if(selectedDate.second != oldDate.second) lv_label_set_text_fmt(secondSpinbox,"%02d",selectedDate.second);
}

void uiSettingsUpdateDisplay()
{   // Display tab
    // Display reverse state
    Settings* settings = Settings::getSettings();
    settings->getDisplayReverse() ? lv_obj_add_state(displayReverseToggle, LV_STATE_CHECKED) : lv_obj_clear_state(displayReverseToggle, LV_STATE_CHECKED);
    settings->getScreenOffOnCharge() ? lv_obj_add_state(screenOffOnChargeToggle, LV_STATE_CHECKED) : lv_obj_clear_state(screenOffOnChargeToggle, LV_STATE_CHECKED);
    settings->getShowBatteryPercentage() ? lv_obj_add_state(showBatPercentageToggle, LV_STATE_CHECKED) : lv_obj_clear_state(showBatPercentageToggle, LV_STATE_CHECKED);
    settings->getShowBatteryWarnMessage() ? lv_obj_add_state(showBatWarnMessageToggle, LV_STATE_CHECKED) : lv_obj_clear_state(showBatWarnMessageToggle, LV_STATE_CHECKED);
    settings->getAllowFrameSimu() ? lv_obj_add_state(allowFrameSimuToggle, LV_STATE_CHECKED) : lv_obj_clear_state(allowFrameSimuToggle, LV_STATE_CHECKED);
    // Date and time
    uiSettingsUpdateDateAndTime();
}

