#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define VENDOR_NAME "Adafruit Industries"
#define PRODUCT_NAME "Matrix Portal M4"
#define VOLUME_LABEL "MATRIXBOOT"
#define INDEX_URL "http://adafru.it/4745"
#define BOARD_ID "SAMD51J19A-MatrixPortal-v0"

#define USB_VID 0x239A
#define USB_PID 0x00C9

//#define LED_PIN PIN_PA23

#define BOARD_NEOPIXEL_PIN PIN_PA23
#define BOARD_NEOPIXEL_COUNT 1

#define BOOT_USART_MODULE                 SERCOM1
#define BOOT_USART_MASK                   APBAMASK
#define BOOT_USART_BUS_CLOCK_INDEX        MCLK_APBAMASK_SERCOM0
#define BOOT_USART_PAD_SETTINGS           UART_RX_PAD1_TX_PAD0
#define BOOT_USART_PAD3                   PINMUX_UNUSED
#define BOOT_USART_PAD2                   PINMUX_UNUSED
#define BOOT_USART_PAD1                   PINMUX_PA01D_SERCOM1_PAD1
#define BOOT_USART_PAD0                   PINMUX_PA00D_SERCOM1_PAD0
#define BOOT_GCLK_ID_CORE                 SERCOM1_GCLK_ID_CORE
#define BOOT_GCLK_ID_SLOW                 SERCOM1_GCLK_ID_SLOW

#endif
