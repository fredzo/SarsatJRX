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

// Filesystem
#define SARSATJRX_LOG_DIR       "/SarsatJRX"
#define LOG_FILE_EXTENSION      ".bcn"
#define LOG_FILENAME_TEMPLATE   "%s/%02d%02d%02d-%02d%02d%02d.bcn"

// Hardware 
// SD Card
#define SD_CS               13
#define SD_MISO             2
#define SD_MOSI             15
#define SD_SCLK             14

#endif