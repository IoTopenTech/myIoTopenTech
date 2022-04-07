#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define VENDOR_NAME "Winterbloom"
#define PRODUCT_NAME "Gemini"
#define VOLUME_LABEL "GEMINIBOOT"
#define INDEX_URL "https://wntr.dev/gemini"
#define BOARD_ID "SAMD21G18A-Winterbloom-Gemini-v0"

#define USB_VID 0x239A
// Allocated at https://github.com/adafruit/uf2-samdx1/issues/136
#define USB_PID 0x00C3

#define BOARD_RGBLED_CLOCK_PIN PIN_PB23
#define BOARD_RGBLED_DATA_PIN PIN_PB22

#endif
