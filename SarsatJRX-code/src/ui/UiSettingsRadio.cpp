#include <ui/UiSettings.h>
#include <Util.h>
#include <lvgl.h>
#include <Display.h>
#include <ui/Ui.h>
#include <Hardware.h>
#include <Settings.h>

// Radio
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
#define SQUELCH_LABEL           "Squelch :"
#define SQUELCH_LABEL_WIDTH     75
#define SQUELCH_SPINBOX_WIDTH   50
// Freq list
#define FREQ_LIST_WIDTH         240
#define FREQ_BUTTON_WIDTH       50
#define FREQ_BUTTON_HEIGHT      40
#define FREQ_BUTTON_X1          (FREQ_LIST_WIDTH + 4)
#define FREQ_BUTTON_X2          (FREQ_BUTTON_X1 + FREQ_BUTTON_WIDTH + 4)

// Radio
static lv_obj_t * radioTab;
static int radioTabWidth;
static int radioTabHeight;
static lv_obj_t * radioToggle;
static lv_obj_t * radioFreqLabelButton;
static lv_obj_t * freqPrevButton;
static lv_obj_t * freqNextButton;
static lv_obj_t * radioFreqTextArea;
static lv_obj_t * radioFreqKeyboard;
static lv_obj_t * radioMeter;
static lv_obj_t * radioVersionTitle;
static lv_obj_t * radioVersionLabel;
static lv_obj_t * radioFiltersTitle;
static lv_obj_t * radioFilter1Toggle;
static lv_obj_t * radioFilter1Label;
static lv_obj_t * radioFilter2Toggle;
static lv_obj_t * radioFilter2Label;
static lv_obj_t * radioFilter3Toggle;
static lv_obj_t * radioFilter3Label;
static lv_obj_t * radioSquelchTitle;
static lv_obj_t * radioSquelchDownButton;
static lv_obj_t * radioSquelchUpButton;
static lv_obj_t * radioSquelchSpinbox;
// Freq list
static lv_obj_t * freqList;
static lv_obj_t * freqListUpButton;
static lv_obj_t * freqListDownButton;
static lv_obj_t * freqTickButton;
static lv_obj_t * freqEditButton;
static lv_obj_t * currentFreq = NULL;

static int lastFreqIndex = 0;

// True for main frequency edit, false for frequency list edit
static bool editMainFrequency = true;


static void toggle_radio_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(radioToggle, LV_STATE_CHECKED);
    //Serial.println("Toggle radio :" + String(state));
    Radio* radio = Radio::getRadio();
    if(state)
    {   // Start radio with saved settings
        Settings * settings = Settings::getSettings();
        radio->radioInit(settings->getRadioAutoVolume(),settings->getRadioVolume(),settings->getRadioFilter1(),settings->getRadioFilter2(),settings->getRadioFilter3(),settings->getLastFrequency(),settings->getActiveFrequencies());
        uiSettingsUpdateView();
    }
    else
    {
        radio->radioStop();
        uiSettingsUpdateView();
    }
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioState(state);
}

static void radio_prev_freq_cb(lv_event_t * e)
{
    Radio::getRadio()->previousFrequency();
}

static void radio_next_freq_cb(lv_event_t * e)
{   
    Radio::getRadio()->nextFrequency();
}


// Hack from https://github.com/lvgl/lvgl/issues/4295#issuecomment-1603873284 to prevent full redraw of keyboard on each button press
static void invalidate_button_area(const lv_obj_t * obj, uint16_t btn_idx)
{
    if(btn_idx == LV_BTNMATRIX_BTN_NONE) return;

    lv_area_t btn_area;
    lv_area_t obj_area;

    lv_btnmatrix_t * btnm = (lv_btnmatrix_t *)obj;;
    if(btn_idx >= btnm->btn_cnt) return;

    lv_area_copy(&btn_area, &btnm->button_areas[btn_idx]);
    lv_obj_get_coords(obj, &obj_area);

    /*The buttons might have outline and shadow so make the invalidation larger with the gaps between the buttons.
     *It assumes that the outline or shadow is smaller than the gaps*/
    lv_coord_t row_gap = lv_obj_get_style_pad_row(obj, LV_PART_MAIN);
    lv_coord_t col_gap = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

    /*Be sure to have a minimal extra space if row/col_gap is small*/
    lv_coord_t dpi = lv_disp_get_dpi(lv_obj_get_disp(obj));
    row_gap = LV_MAX(row_gap, dpi / 10);
    col_gap = LV_MAX(col_gap, dpi / 10);

    /*Convert relative coordinates to absolute*/
    btn_area.x1 += obj_area.x1 - row_gap;
    btn_area.y1 += obj_area.y1 - col_gap;
    btn_area.x2 += obj_area.x1 + row_gap;
    btn_area.y2 += obj_area.y1 + col_gap;

    lv_obj_invalidate_area(obj, &btn_area);
}

void event_pre_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_PRESSED) {
        lv_disp_enable_invalidation(NULL, false); //Disable invalidation on press
    }
}

static void radio_keyboard_cb(lv_event_t * e);
void event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_keyboard_t * keyboard = (lv_keyboard_t *) obj;
    if(code == LV_EVENT_PRESSED) {
        //Invalidate only the button
        lv_disp_enable_invalidation(NULL, true);
        invalidate_button_area(obj, keyboard->btnm.btn_id_sel);
        lv_disp_enable_invalidation(NULL, false);
    }
    if(code == LV_EVENT_RELEASED) {
        lv_disp_enable_invalidation(NULL, true);
        invalidate_button_area(obj, keyboard->btnm.btn_id_sel); //Enable invalidation on release
        lv_obj_invalidate(lv_textarea_get_label(lv_keyboard_get_textarea(obj)));
    }
    if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_disp_enable_invalidation(NULL, true);
        radio_keyboard_cb(e);
    }
}
// End hack

void startEditFreq(bool mainFrequency)
{   // Move text area accodint to edited content
    editMainFrequency = mainFrequency;
    float placeHolderFreq;
    if(editMainFrequency)
    {
        lv_obj_set_pos(radioFreqTextArea,RADIO_FREQ_X,0);
        placeHolderFreq = Radio::getRadio()->getFrequency();
    }
    else
    {
        lv_obj_set_pos(radioFreqTextArea,0,0);
        if(currentFreq != NULL)
        {
            Settings* settings = Settings::getSettings();
            uint32_t index = lv_obj_get_index(currentFreq);
            placeHolderFreq = settings->getFrequency(index).value;
        }
    }
    // Show text area
    lv_obj_clear_flag(radioFreqTextArea, LV_OBJ_FLAG_HIDDEN);
    // Create a new Keyboard
    radioFreqKeyboard = lv_keyboard_create(radioTab);
    lv_obj_set_size(radioFreqKeyboard,  radioTabWidth, radioTabHeight-RADIO_FREQ_HEIGHT-4);
    lv_keyboard_set_textarea(radioFreqKeyboard, radioFreqTextArea);
    lv_keyboard_set_mode(radioFreqKeyboard, LV_KEYBOARD_MODE_NUMBER);
    // Hack (see above)
    lv_obj_add_event_cb(radioFreqKeyboard, event_pre_cb,  (lv_event_code_t)(LV_EVENT_ALL | LV_EVENT_PREPROCESS), NULL);
    lv_obj_add_event_cb(radioFreqKeyboard, event_cb, LV_EVENT_ALL, NULL);
    // End hack
    //lv_obj_add_event_cb(radioFreqKeyboard, radio_keyboard_cb, LV_EVENT_ALL, NULL);

    // Prepare text area content
    char buffer[16];
    sprintf(buffer,"%3.4f",placeHolderFreq);
    lv_textarea_set_text(radioFreqTextArea, "");
    lv_textarea_set_placeholder_text(radioFreqTextArea, buffer);
}

void stopEditFreq()
{   // Hide text area and delete keyboard
    lv_obj_add_flag(radioFreqTextArea, LV_OBJ_FLAG_HIDDEN);
    lv_obj_del(radioFreqKeyboard);
}

static void radio_freq_cb(lv_event_t * e)
{   // Keyboard input
    startEditFreq(true);
}

static bool ignoreNext = false;
static String previousContent;
static void radio_freq_ta_cb(lv_event_t * e)
{
    if(radioFreqTextArea)
    {
        String content = String(lv_textarea_get_text(radioFreqTextArea));
        //Serial.printf("Input ctrl, previous = %s, content = %s, length = %d\n",previousContent.c_str(),content.c_str(),content.length());
        if (content.length() == 3)
        {
            if(content.indexOf('.')<0)
            {   // Check that we are not backspacing
                if(previousContent.length()<4)
                {
                    lv_textarea_set_cursor_pos(radioFreqTextArea, 3);
                    lv_textarea_add_char(radioFreqTextArea, '.');
                    ignoreNext = true;
                }
            }
        }
        else if (content.length() == 4)
        {
            if(content.charAt(3) == '.')
            {   // Check that we are backspacing
                if(previousContent.length()>4)
                {
                    lv_textarea_set_cursor_pos(radioFreqTextArea, 4);
                    lv_textarea_del_char(radioFreqTextArea);
                    ignoreNext = true;
                }
            }
            else if(content.indexOf('.')<0)
            {   // Add a '.' at position 3
                int oldPos = lv_textarea_get_cursor_pos(radioFreqTextArea);
                lv_textarea_set_cursor_pos(radioFreqTextArea, 3);
                lv_textarea_add_char(radioFreqTextArea,'.');
                lv_textarea_set_cursor_pos(radioFreqTextArea, oldPos+1);
                ignoreNext = true;
            }
        }
        else if (content.length() > 4)
        {
            if(content.charAt(content.length()-1) == '.')
            {   // Prevent '.' at other positions
                lv_textarea_set_cursor_pos(radioFreqTextArea, (content.length()));
                lv_textarea_del_char(radioFreqTextArea);
                ignoreNext = true;
            }
        }
        if(ignoreNext)
        {
            ignoreNext = false;
        }
        else
        {
            previousContent = content;
        }
    }
}

static void radio_keyboard_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_READY) || (code == LV_EVENT_CANCEL)) 
    {
        stopEditFreq();
        if(code == LV_EVENT_READY)
        {   // Change frequency
            const char * txt = lv_textarea_get_text(radioFreqTextArea);
            float newFreq = atof(txt);
            //Serial.printf("Found frequency %3.4f\n",newFreq);
            if(editMainFrequency)
            {   // Change mainf frequency
                Radio::getRadio()->setFrequency(newFreq);
                // Update settings
                Settings* settings = Settings::getSettings();
                settings->setLastFrequency(newFreq);
            }
            else
            {   // Change currently edited frequency
                if(currentFreq != NULL)
                {
                    Settings* settings = Settings::getSettings();
                    uint32_t index = lv_obj_get_index(currentFreq);
                    Settings::Frequency frequency = settings->getFrequency(index);
                    settings->setFrequency(index,newFreq);
                    char buffer[24];
                    formatFrequencyItem(buffer,index,newFreq,frequency.on);
                    lv_label_set_text(currentFreq, buffer);
                    if(frequency.on)
                    {
                        // Uupdate radio active frequencies
                        Radio::getRadio()->setScanFrequencies(settings->getActiveFrequencies());
                    }
                }
            }
        }
    }
}

static void toggle_filter1_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(radioFilter1Toggle, LV_STATE_CHECKED);
    Radio* radio = Radio::getRadio();
    radio->setFilter1(state);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioFilter1(state);
}

static void toggle_filter2_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(radioFilter2Toggle, LV_STATE_CHECKED);
    Radio* radio = Radio::getRadio();
    radio->setFilter2(state);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioFilter2(state);
}

static void toggle_filter3_cb(lv_event_t * e)
{
    bool state = lv_obj_has_state(radioFilter3Toggle, LV_STATE_CHECKED);
    Radio* radio = Radio::getRadio();
    radio->setFilter3(state);
    // Save state to settings
    Settings* settings = Settings::getSettings();
    settings->setRadioFilter3(state);
}

static void radio_update_squelch()
{
    int32_t squelch = lv_spinbox_get_value(radioSquelchSpinbox);
    Radio* radio = Radio::getRadio();
    radio->setQuelch(squelch);
    // Update Settings
    Settings* settings = Settings::getSettings();
    settings->setRadioSquelch(squelch);
}

static void radio_squelch_down_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_decrement(radioSquelchSpinbox);
        radio_update_squelch();
    }
}

static void radio_squelch_up_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(radioSquelchSpinbox);
        radio_update_squelch();
    }
}

// Freq List
static void selectFreq(lv_obj_t* freq)
{
    if(freq)
    {
        lv_obj_add_state(freq, LV_STATE_CHECKED);
        if((currentFreq != NULL) && (currentFreq != freq))
        {
            lv_obj_clear_state(currentFreq, LV_STATE_CHECKED);
        }
        currentFreq = freq;
        lastFreqIndex = lv_obj_get_index(currentFreq);
    }
}

static void freq_clicked_cb(lv_event_t * e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    selectFreq(obj);
}

static void freq_up_cb(lv_event_t * e)
{
    if(currentFreq == NULL) return;
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) 
    {
        uint32_t index = lv_obj_get_index(currentFreq);
        if(index <= 0) return;
        index--;
        selectFreq(lv_obj_get_child(freqList,index));
        lv_obj_scroll_to_view(currentFreq, LV_ANIM_ON);
    }    
}

static void freq_down_cb(lv_event_t * e)
{
    if(currentFreq == NULL) return;
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) 
    {
        uint32_t index = lv_obj_get_index(currentFreq);
        index++;
        lv_obj_t *obj = lv_obj_get_child(freqList,index);
        if(obj)
        {
            selectFreq(obj);
            lv_obj_scroll_to_view(currentFreq, LV_ANIM_ON);
        }
    }    
}

static void freq_tick_cb(lv_event_t * e)
{
    if(currentFreq == NULL) return;
    Settings* settings = Settings::getSettings();
    uint32_t index = lv_obj_get_index(currentFreq);
    Settings::Frequency frequency = settings->getFrequency(index);
    frequency.on = !frequency.on;
    settings->setFrequencyOn(index,frequency.on);
    char buffer[24];
    formatFrequencyItem(buffer,index,frequency.value,frequency.on);
    lv_label_set_text(currentFreq, buffer);
    // Uupdate radio active frequencies
    Radio::getRadio()->setScanFrequencies(settings->getActiveFrequencies());
}

static void freq_edit_cb(lv_event_t * e)
{
    if(currentFreq == NULL) return;
    startEditFreq(false);
}


void createRadioTab(lv_obj_t * tab, int currentY, int tabWidth, int tabHeight)
{   // Keep track on tab, tabWith and tabHeugth for keyboard creation
    radioTab = tab;
    radioTabWidth = tabWidth;
    radioTabHeight = tabHeight;
    Radio* radio = Radio::getRadio();
    // Radion on/off
    currentY+=SPACER;
    radioToggle = uiCreateToggle(tab,&style_section_text,toggle_radio_cb,RADIO_TOGGLE_X,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    // Previous button
    freqPrevButton = uiCreateImageButton(tab,LV_SYMBOL_LEFT,radio_prev_freq_cb,LV_EVENT_CLICKED,RADIO_BUTTONS_WIDTH, RADIO_FREQ_HEIGHT,RADIO_FREQ_NEXT_X,0);
    // Freq
    radioFreqLabelButton = uiCreateLabelButton(tab,"000.0000 MHz",radio_freq_cb,LV_EVENT_CLICKED,lv_color_make(10, 10, 10),tabWidth-RADIO_FREQ_X-RADIO_BUTTONS_WIDTH-8, RADIO_FREQ_HEIGHT,RADIO_FREQ_X,0);
    lv_obj_add_style(radioFreqLabelButton,&style_text_lcd_large,0);
    // Freq text area
    radioFreqTextArea = uiCreateTextArea(tab,radio_freq_ta_cb,tabWidth-RADIO_FREQ_X-RADIO_BUTTONS_WIDTH-8, RADIO_FREQ_HEIGHT,RADIO_FREQ_X,0);
    lv_obj_add_style(radioFreqTextArea,&style_header,0);
    lv_obj_set_style_pad_all(radioFreqTextArea,4,0);
    // Hide it for now
    lv_obj_add_flag(radioFreqTextArea, LV_OBJ_FLAG_HIDDEN);
    // Next button
    freqNextButton = uiCreateImageButton(tab,LV_SYMBOL_RIGHT,radio_next_freq_cb,LV_EVENT_CLICKED,RADIO_BUTTONS_WIDTH, RADIO_FREQ_HEIGHT,tabWidth-RADIO_BUTTONS_WIDTH-4,0);
    currentY+=(TOGGLE_LINE_HEIGHT+SPACER+HALF_SPACER);
    // Meter
    radioMeter = lv_bar_create(tab);
    lv_obj_set_size(radioMeter, tabWidth-4, RADIO_METER_HEIGHT);
    lv_obj_set_pos(radioMeter,0,currentY);
    lv_obj_add_style(radioMeter, &style_meter, LV_PART_INDICATOR);
    lv_bar_set_range(radioMeter, POWER_MIN_VALUE, POWER_MAX_VALUE);
    lv_obj_set_style_anim_time(radioMeter,200,LV_PART_MAIN);
    currentY += (RADIO_METER_HEIGHT+HALF_SPACER);
    // Version
    radioVersionTitle = uiCreateLabel(tab,&style_section_title,VERSION_LABEL,0,currentY,VERSION_LABEL_WIDTH,LINE_HEIGHT);
    radioVersionLabel = uiCreateLabel(tab,&style_section_text,radio->getVersion().c_str(),VERSION_LABEL_WIDTH,currentY,tabWidth-VERSION_LABEL_WIDTH,LINE_HEIGHT);
    currentY += (LINE_HEIGHT+HALF_SPACER);
    // Filter toggles
    int currentX = 0;
    radioFiltersTitle = uiCreateLabel(tab,&style_section_title,FILTERS_LABEL,0,currentY+HALF_SPACER,FILTERS_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTERS_LABEL_WIDTH+SPACER;
    radioFilter1Toggle = uiCreateToggle(tab,&style_section_text,toggle_filter1_cb,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    radioFilter1Label = uiCreateLabel(tab,&style_section_text,FILTER1_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTER_LABEL_WIDTH+SPACER;
    radioFilter2Toggle = uiCreateToggle(tab,&style_section_text,toggle_filter2_cb,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    radioFilter2Label = uiCreateLabel(tab,&style_section_text,FILTER2_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=FILTER_LABEL_WIDTH+SPACER;
    radioFilter3Toggle = uiCreateToggle(tab,&style_section_text,toggle_filter3_cb,currentX,currentY,TOGGLE_WIDTH,TOGGLE_LINE_HEIGHT);
    currentX+=TOGGLE_WIDTH+SPACER;
    radioFilter3Label = uiCreateLabel(tab,&style_section_text,FILTER3_LABEL,currentX,currentY+HALF_SPACER,FILTER_LABEL_WIDTH-4,LINE_HEIGHT);
    currentY+=(TOGGLE_LINE_HEIGHT+HALF_SPACER);
    // Squelch
    currentX = 0;
    radioSquelchTitle = uiCreateLabel(tab,&style_section_title,SQUELCH_LABEL,0,currentY+HALF_SPACER,SQUELCH_LABEL_WIDTH,LINE_HEIGHT);
    currentX+=SQUELCH_LABEL_WIDTH+SPACER;
    // Squelch down button
    radioSquelchDownButton = uiCreateImageButton(tab,LV_SYMBOL_MINUS,radio_squelch_down_cb,LV_EVENT_ALL,RADIO_BUTTONS_WIDTH, TOGGLE_LINE_HEIGHT,currentX,currentY);
    currentX+=RADIO_BUTTONS_WIDTH+SPACER;
    // Spinbox
    radioSquelchSpinbox = lv_spinbox_create(tab);
    lv_spinbox_set_range(radioSquelchSpinbox, 0, 8);
    lv_spinbox_set_digit_format(radioSquelchSpinbox, 1, 0);
    lv_obj_set_style_pad_all(radioSquelchSpinbox,2,0);
    lv_obj_set_pos(radioSquelchSpinbox,currentX,currentY);
    lv_obj_set_size(radioSquelchSpinbox, SQUELCH_SPINBOX_WIDTH, TOGGLE_LINE_HEIGHT);
    lv_obj_set_style_text_align(radioSquelchSpinbox,LV_TEXT_ALIGN_CENTER,0);
    // Hide cursor
    lv_obj_set_style_opa(radioSquelchSpinbox,0,LV_PART_CURSOR);
    currentX+=SQUELCH_SPINBOX_WIDTH+SPACER;
    // Squelch up button
    radioSquelchUpButton = uiCreateImageButton(tab,LV_SYMBOL_PLUS,radio_squelch_up_cb,LV_EVENT_ALL,RADIO_BUTTONS_WIDTH, TOGGLE_LINE_HEIGHT,currentX,currentY);
    currentY+=(TOGGLE_LINE_HEIGHT+HALF_SPACER);
    // Freq list
    freqList = lv_list_create(tab); 
    lv_obj_set_pos(freqList,0,currentY);
    int bListHeight = tabHeight-currentY;
    lv_obj_set_size(freqList, FREQ_LIST_WIDTH, bListHeight);
    //lv_obj_set_style_pad_row(beaconsList, 2, 0);
    // Up
    freqListUpButton = uiCreateImageButton(tab,LV_SYMBOL_UP,freq_up_cb,LV_EVENT_ALL,FREQ_BUTTON_WIDTH, FREQ_BUTTON_HEIGHT,FREQ_BUTTON_X1,currentY);
    // Down
    int bottomButtonY = currentY+bListHeight-FREQ_BUTTON_HEIGHT;
    freqListDownButton = uiCreateImageButton(tab,LV_SYMBOL_DOWN,freq_down_cb,LV_EVENT_ALL,FREQ_BUTTON_WIDTH, FREQ_BUTTON_HEIGHT,FREQ_BUTTON_X1,bottomButtonY);
    // Tick
    freqTickButton = uiCreateImageButton(tab,LV_SYMBOL_OK,freq_tick_cb,LV_EVENT_CLICKED,FREQ_BUTTON_WIDTH, FREQ_BUTTON_HEIGHT,FREQ_BUTTON_X2,currentY);
    // Edit
    freqEditButton = uiCreateImageButton(tab,LV_SYMBOL_EDIT,freq_edit_cb,LV_EVENT_CLICKED,FREQ_BUTTON_WIDTH, FREQ_BUTTON_HEIGHT,FREQ_BUTTON_X2,bottomButtonY);
    // Fill list
    int freqCount = 0;
    currentFreq = NULL;
    lv_obj_t * btn;
    char buffer[24];
    Settings* settings = Settings::getSettings();
    for(int i = 0; i < settings->getFrequencyCount() ; i++)
    {
        Settings::Frequency freq = settings->getFrequency(i);
        formatFrequencyItem(buffer,i,freq.value,freq.on);
        lv_obj_t *lab = lv_label_create(freqList);
        lv_obj_add_event_cb(lab, freq_clicked_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_add_style(lab,&style_section_text,0);
        lv_obj_set_style_text_color(lab,uiOkColor,LV_STATE_CHECKED);
        lv_obj_add_flag(lab,LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_text_align(lab,LV_TEXT_ALIGN_LEFT,0);
        lv_label_set_text(lab, buffer);
        if(currentFreq == NULL)
        {   // Select first freq
            selectFreq(lab);
        }
        freqCount++;
    }
}

void uiSettingsUpdateRadioStatus(bool radioStatus)
{
    if(radioStatus)
    {   // Radio toggle on
        lv_obj_add_state(radioToggle, LV_STATE_CHECKED);
        Radio * radio = Radio::getRadio();
        // Upadte squelch
        lv_spinbox_set_value(radioSquelchSpinbox,radio->getSquelch());
        // Update version
        lv_label_set_text(radioVersionLabel,radio->getVersion().c_str());
        // Update filters
        radio->getFilter1() ? lv_obj_add_state(radioFilter1Toggle, LV_STATE_CHECKED) : lv_obj_clear_state(radioFilter1Toggle, LV_STATE_CHECKED);
        radio->getFilter2() ? lv_obj_add_state(radioFilter2Toggle, LV_STATE_CHECKED) : lv_obj_clear_state(radioFilter2Toggle, LV_STATE_CHECKED);
        radio->getFilter3() ? lv_obj_add_state(radioFilter3Toggle, LV_STATE_CHECKED) : lv_obj_clear_state(radioFilter3Toggle, LV_STATE_CHECKED);
    }
    else
    {   // Radio toggle off
        lv_obj_clear_state(radioToggle, LV_STATE_CHECKED);
    }
}

void uiSettingsUpdatePower(int power)
{
    lv_bar_set_value(radioMeter, power, LV_ANIM_ON);
}

void uiSettingsUpdateFreq(char* freqBuffer, bool scanOn)
{
    lv_label_set_text(radioFreqLabelButton,freqBuffer);
    lv_obj_set_style_text_color(radioFreqLabelButton,scanOn ? uiOnColor : uiOffColor,0);
}



