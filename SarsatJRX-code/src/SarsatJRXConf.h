#ifndef SARSAT_JRX_CONF_H
#define SARSAT_JRX_CONF_H

#define SARSAT_JRX_VERSION "0.1.0"

// Enable decode debuging
//#define DEBUG_DECODE

// Enable RAM debuging
// #define DEBUG_RAM

// Enable serial out
#define SERIAL_OUT

// Enable WIFI
#define WIFI

// NTP Server and Timezone
#define NTP_SERVER   "pool.ntp.org"
#define TIME_ZONE    "CET-1CEST,M3.5.0,M10.5.0/3" // Europe/Paris (See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv )

// Frame countdown default duration
#define FRAME_COUNTDOWN_DEFAULT_DURATION    50


// Filesystem
#define SARSATJRX_LOG_DIR_NAME          "/SarsatJRX"
#define SARSATJRX_LOG_DIR_PATH_TEMPLATE "%s/%02d%02d%02d"
#define LOG_FILE_EXTENSION              ".bcn"
#define LOG_FILENAME_TEMPLATE           "%s/%02d%02d%02d-%02d%02d%02d.bcn"
#define SARSATJRX_CONFIG_FILENAME       "/sarsat-jrx-conf.ini"

// UI
#define AUDIO_POWER_DISPLAY_PERIOD    5 // 5ms
#define AUDIO_POWER_MIN_VALUE         0
#define AUDIO_POWER_MAX_VALUE         384

// Hardware 
// SD Card
#define SD_CS               13
#define SD_MISO             2
#define SD_MOSI             15
#define SD_SCLK             14

// Button 
#define BUTTON_INPUT_PIN    0

// Battery voltage input
#define BATTERY_ADC_PIN     35
// Charge input
#define CHARGE_INPUT_PIN    32 // Low when charged or unplugged, high when charging

// Audio power input
#define AUDIO_ADC_PIN       34

// RTC
#define RTC_INT_PIN         36

// Touch
#define TOUCH_INT_PIN       34 // Not used as such

// TFT
#define LILYPI_TFT_MISO     23
#define LILYPI_TFT_MOSI     19
#define LILYPI_TFT_SCLK     18
#define LILYPI_TFT_CS       5
#define LILYPI_TFT_DC       27
#define LILYPI_TFT_RST      -1
#define LILYPI_TFT_BL       12

// I2C (RTC + Touch)
#define I2C_SDA_PIN         21    
#define I2C_SCL_PIN         22

// UART 1 (unused)
#define UART1_RX_PIN        39
#define UART1_TX_PIN        33


// Receiver Pin
#define RECEIVER_PIN        25 // Int
// Frame led
#define NOTIFICATION_PIN    26 // => LED Frame
// Error Led
#define ERROR_PIN           04 // => LED Err // Input only => swap with PIN 04

// Buzzer
#define BUZZER_PIN          33 // Buzzer

// Discri jack detection pin
#define DISCRI_JACK_PIN     39 // Discri jack inserted



#endif