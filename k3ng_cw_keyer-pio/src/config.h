#if defined(ARDUINO_SAM_DUE)
#include <SPI.h>
#include <Wire.h>
#define tone toneDUE
#define noTone noToneDUE
#elif defined(ARDUINO_MAPLE_MINI) || defined(ARDUINO_GENERIC_STM32F103C) || defined(__STM32F1__)
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include "keyer_stm32duino.h"
#elif defined(_BOARD_PIC32_PINGUINO_)
#include <EEPROM.h>
#elif defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
#include <FlashAsEEPROM.h>
#elif defined(ARDUINO_ARCH_MBED) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO)
#include <EEPROM.h>
#include <avr/pgmspace.h>
#else
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#endif // ARDUINO_SAM_DUE

#if defined(HARDWARE_OPENCWKEYER_MK2)
#include "keyer_features_and_options_opencwkeyer_mk2.h"
#elif defined(HARDWARE_NANOKEYER_REV_B)
#include "keyer_features_and_options_nanokeyer_rev_b.h"
#elif defined(HARDWARE_NANOKEYER_REV_D)
#include "keyer_features_and_options_nanokeyer_rev_d.h"
#elif defined(HARDWARE_OPEN_INTERFACE)
#include "keyer_features_and_options_open_interface.h"
#elif defined(HARDWARE_TINYKEYER)
#include "keyer_features_and_options_tinykeyer.h"
#elif defined(HARDWARE_FK_10)
#include "keyer_features_and_options_fk_10.h"
#elif defined(HARDWARE_FK_11)
#include "keyer_features_and_options_fk_11.h"
#elif defined(HARDWARE_MAPLE_MINI) // sp5iou 20180328
#include "keyer_features_and_options_maple_mini.h"
#elif defined(HARDWARE_GENERIC_STM32F103C) // sp5iou 20180329
#include "keyer_features_and_options_generic_STM32F103C.h"
#elif defined(HARDWARE_MORTTY)
#include "keyer_features_and_options_mortty.h"
#elif defined(HARDWARE_MORTTY_PICO_OVER_USB)
#include "keyer_features_and_options_mortty_pico_over_usb.h"
#elif defined(HARDWARE_MORTTY_REGULAR)
#include "keyer_features_and_options_mortty_regular.h"
#elif defined(HARDWARE_MORTTY_REGULAR_WITH_POTENTIOMETER)
#include "keyer_features_and_options_mortty_regular_with_potentiometer.h"
#elif defined(HARDWARE_MORTTY_SO2R)
#include "keyer_features_and_options_mortty_so2r.h"
#elif defined(HARDWARE_MORTTY_SO2R_WITH_POTENTIOMETER)
#include "keyer_features_and_options_mortty_so2r_with_potentiometer.h"
#elif defined(HARDWARE_K5BCQ)
#include "keyer_features_and_options_k5bcq.h"
#elif defined(HARDWARE_MEGAKEYER)
#include "keyer_features_and_options_megakeyer.h"
#elif defined(HARDWARE_TEST_EVERYTHING)
#include "keyer_features_and_options_test_everything.h"
#elif defined(HARDWARE_YAACWK)
#include "keyer_features_and_options_yaacwk.h"
#elif defined(HARDWARE_TEST)
#include "keyer_features_and_options_test.h"
#elif defined(HARDWARE_IZ3GME)
#include "keyer_features_and_options_iz3gme.h"
#elif defined(HARDWARE_YCCC_SO2R_MINI)
#include "keyer_features_and_options_yccc_so2r_mini.h"
#else
#include "keyer_features_and_options.h"
#endif

#include "keyer.h"

#ifdef FEATURE_EEPROM_E24C1024
#include <E24C1024.h>
#define EEPROM EEPROM1024
#endif

#include "keyer_dependencies.h"
#include "keyer_debug.h"

#if defined(HARDWARE_OPENCWKEYER_MK2)
#include "keyer_pin_settings_opencwkeyer_mk2.h"
#include "keyer_settings_opencwkeyer_mk2.h"
#elif defined(HARDWARE_NANOKEYER_REV_B)
#include "keyer_pin_settings_nanokeyer_rev_b.h"
#include "keyer_settings_nanokeyer_rev_b.h"
#elif defined(HARDWARE_NANOKEYER_REV_D)
#include "keyer_pin_settings_nanokeyer_rev_d.h"
#include "keyer_settings_nanokeyer_rev_d.h"
#elif defined(HARDWARE_OPEN_INTERFACE)
#include "keyer_pin_settings_open_interface.h"
#include "keyer_settings_open_interface.h"
#elif defined(HARDWARE_TINYKEYER)
#include "keyer_pin_settings_tinykeyer.h"
#include "keyer_settings_tinykeyer.h"
#elif defined(HARDWARE_FK_10)
#include "keyer_pin_settings_fk_10.h"
#include "keyer_settings_fk_10.h"
#elif defined(HARDWARE_FK_11)
#include "keyer_pin_settings_fk_11.h"
#include "keyer_settings_fk_11.h"
#elif defined(HARDWARE_MAPLE_MINI)
#include "keyer_pin_settings_maple_mini.h"
#include "keyer_settings_maple_mini.h"
#elif defined(HARDWARE_GENERIC_STM32F103C)
#include "keyer_pin_settings_generic_STM32F103C.h"
#include "keyer_settings_generic_STM32F103C.h"
#elif defined(HARDWARE_MORTTY)
#include "keyer_pin_settings_mortty.h"
#include "keyer_settings_mortty.h"
#elif defined(HARDWARE_MORTTY_PICO_OVER_USB)
#include "keyer_pin_settings_mortty_pico_over_usb.h"
#include "keyer_settings_mortty_pico_over_usb.h"
#elif defined(HARDWARE_MORTTY_REGULAR)
#include "keyer_pin_settings_mortty_regular.h"
#include "keyer_settings_mortty_regular.h"
#elif defined(HARDWARE_MORTTY_REGULAR_WITH_POTENTIOMETER)
#include "keyer_pin_settings_mortty_regular_with_potentiometer.h"
#include "keyer_settings_mortty_regular_with_potentiometer.h"
#elif defined(HARDWARE_MORTTY_SO2R)
#include "keyer_pin_settings_mortty_so2r.h"
#include "keyer_settings_mortty_so2r.h"
#elif defined(HARDWARE_MORTTY_SO2R_WITH_POTENTIOMETER)
#include "keyer_pin_settings_mortty_so2r_with_potentiometer.h"
#include "keyer_settings_mortty_so2r_with_potentiometer.h"
#elif defined(HARDWARE_K5BCQ)
#include "keyer_pin_settings_k5bcq.h"
#include "keyer_settings_k5bcq.h"
#elif defined(HARDWARE_MEGAKEYER)
#include "keyer_pin_settings_megakeyer.h"
#include "keyer_settings_megakeyer.h"
#elif defined(HARDWARE_TEST_EVERYTHING)
#include "keyer_pin_settings_test_everything.h"
#include "keyer_settings_test_everything.h"
#elif defined(HARDWARE_YAACWK)
#include "keyer_pin_settings_yaacwk.h"
#include "keyer_settings_yaacwk.h"
#elif defined(HARDWARE_TEST)
#include "keyer_pin_settings_test.h"
#include "keyer_settings_test.h"
#elif defined(HARDWARE_IZ3GME)
#include "keyer_pin_settings_iz3gme.h"
#include "keyer_settings_iz3gme.h"
#elif defined(HARDWARE_YCCC_SO2R_MINI)
#include "keyer_pin_settings_yccc_so2r_mini.h"
#include "keyer_settings_yccc_so2r_mini.h"
#else
#include "keyer_pin_settings.h"
#include "keyer_settings.h"
#endif

#if (paddle_left == 0) || (paddle_right == 0)
#error "You cannot define paddle_left or paddle_right as 0 to disable"
#endif

#if defined(FEATURE_DUAL_MODE_KEYER_AND_TINYFSK)
#include "TinyFSK.h"
uint8_t runTinyFSK = 0;
#endif

#if defined(FEATURE_BUTTONS)
#include "src/buttonarray/buttonarray.h"
#endif

#if defined(FEATURE_SIDETONE_NEWTONE) && !defined(OPTION_SIDETONE_DIGITAL_OUTPUT_NO_SQUARE_WAVE)
#include <NewTone.h>
#define tone NewTone
#define noTone noNewTone
#endif // FEATURE_SIDETONE_NEWTONE

#if defined(FEATURE_SLEEP) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
#include <avr/sleep.h> // It should be different library for ARM sp5iou
#endif

#if defined(FEATURE_PS2_KEYBOARD)
#include <K3NG_PS2Keyboard.h>
#endif

#if defined(FEATURE_LCD_4BIT) || defined(FEATURE_LCD1602_N07DH) || defined(FEATURE_LCD_8BIT) // works on 3.2V supply and logic, but do not work on every pins (SP5IOU)
#include <LiquidCrystal.h>
#include <Wire.h>
#endif

#if defined(FEATURE_LCD_ADAFRUIT_I2C) || defined(FEATURE_LCD_ADAFRUIT_BACKPACK) || defined(FEATURE_LCD_YDv1) || defined(FEATURE_LCD_SAINSMART_I2C) || defined(FEATURE_LCD_FABO_PCF8574) || defined(FEATURE_LCD_MATHERTEL_PCF8574)
#include <Wire.h>
#endif

#if defined(FEATURE_LCD_YDv1)
#include <LiquidCrystal_I2C.h>
#endif

#if defined(FEATURE_LCD_TWILIQUIDCRYSTAL)
#include <TwiLiquidCrystal.h>
#include <Wire.h>
#endif

#if defined(FEATURE_LCD_ADAFRUIT_I2C)
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#endif

#if defined(FEATURE_LCD_ADAFRUIT_BACKPACK)
#include <Adafruit_LiquidCrystal.h>
#endif

#if defined(FEATURE_LCD_SAINSMART_I2C)
#include <LiquidCrystal_I2C.h>
#endif

#if defined(FEATURE_LCD_FABO_PCF8574)
#include <FaBoLCD_PCF8574.h>
#endif

#if defined(FEATURE_LCD_MATHERTEL_PCF8574)
#include <LiquidCrystal_PCF8574.h>
#endif

#if defined(FEATURE_LCD_I2C_FDEBRABANDER)
#include <LiquidCrystal_I2C.h>
#endif

#if defined(FEATURE_LCD_HD44780)
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#define WIRECLOCK 400000L
#endif

#if defined(FEATURE_OLED_SSD1306)
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#endif

#if defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE)
// #include <BasicTerm.h>
#endif

#if defined(FEATURE_CW_DECODER) && defined(OPTION_CW_DECODER_GOERTZEL_AUDIO_DETECTOR)
#include <goertzel.h>
#endif

#if defined(FEATURE_ETHERNET)
#if !defined(ARDUINO_MAPLE_MINI) && !defined(ARDUINO_GENERIC_STM32F103C) // sp5iou 20180329
#include <Ethernet.h>                                                    // if this is not included, compilation fails even though all ethernet code is #ifdef'ed out
#if defined(FEATURE_INTERNET_LINK)
#include <EthernetUdp.h>
#endif // FEATURE_INTERNET_LINK
#endif //! defined(ARDUINO_MAPLE_MINI) && !defined(ARDUINO_GENERIC_STM32F103C) //sp5iou 20180329
#endif // FEATURE_ETHERNET

#if defined(FEATURE_USB_KEYBOARD) || defined(FEATURE_USB_MOUSE) // note_usb_uncomment_lines
// #include <hidboot.h>  // Arduino 1.6.x (and maybe 1.5.x) has issues with these three lines, moreover we noted that Arduino 1.8.6 it's not afected by an issue during USB Shield SPI init see https://github.com/felis/USB_Host_Shield_2.0/issues/390
// #include <usbhub.h>   // Uncomment the three lines if you are using FEATURE_USB_KEYBOARD or FEATURE_USB_MOUSE
// #include <Usb.h>      // Note: the most updated USB Library can be downloaded at https://github.com/felis/USB_Host_Shield_2.0
#endif

#if defined(FEATURE_CW_COMPUTER_KEYBOARD)
#include <Keyboard.h> // Have a problem with Keyboard.h not found?  See https://github.com/k3ng/k3ng_cw_keyer/issues/35
                      // For some unknown reason this line uncommented in Arduino 1.8.1 <sometimes?> causes compilation error (sigh)
#endif                // defined(FEATURE_CW_COMPUTER_KEYBOARD)

#if defined(FEATURE_4x4_KEYPAD) || defined(FEATURE_3x4_KEYPAD)
#include <Keypad.h>
#endif

#if defined(FEATURE_SD_CARD_SUPPORT)
#include <SPI.h>
#include <SD.h>
#endif // FEATURE_SD_CARD_SUPPORT

#if defined(FEATURE_AUDIOPWMSINEWAVE)
#include "AudioPWMSineWave.h"
#define tone PWMTone
#define noTone noPWMTone
#endif

#if defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
extern uint32_t __get_MSP(void);
#define SP __get_MSP()
#endif

#define memory_area_start (sizeof(configuration) + 5)

byte sending_mode = UNDEFINED_SENDING;
byte command_mode_disable_tx = 0;
byte current_tx_key_line = tx_key_line_1;
#ifdef FEATURE_SO2R_BASE
byte current_tx_ptt_line = ptt_tx_1;
#endif
byte manual_ptt_invoke = 0;
byte manual_ptt_invoke_ptt_input_pin = 0;
byte qrss_dit_length = initial_qrss_dit_length;
byte keyer_machine_mode = KEYER_NORMAL; // KEYER_NORMAL, BEACON, KEYER_COMMAND_MODE
byte char_send_mode = 0;                // CW, HELL, AMERICAN_MORSE
byte key_tx = 0;                        // 0 = tx_key_line control suppressed
byte dit_buffer = 0;                    // used for buffering paddle hits in iambic operation
byte dah_buffer = 0;                    // used for buffering paddle hits in iambic operation
byte button0_buffer = 0;
byte being_sent = 0; // SENDING_NOTHING, SENDING_DIT, SENDING_DAH
byte key_state = 0;  // 0 = key up, 1 = key down
byte config_dirty = 0;
unsigned long ptt_time = 0;
byte ptt_line_activated = 0;
byte speed_mode = SPEED_NORMAL;
#if defined(FEATURE_COMMAND_LINE_INTERFACE) || defined(FEATURE_PS2_KEYBOARD) || defined(FEATURE_MEMORY_MACROS) || defined(FEATURE_MEMORIES) || defined(FEATURE_COMMAND_MODE)
unsigned int serial_number = 1;
#endif
byte pause_sending_buffer = 0;
byte length_letterspace = default_length_letterspace;
// byte keying_compensation = default_keying_compensation;
byte first_extension_time = default_first_extension_time;
byte ultimatic_mode = ULTIMATIC_NORMAL;
float ptt_hang_time_wordspace_units = default_ptt_hang_time_wordspace_units;
byte last_sending_mode = MANUAL_SENDING;
byte zero = 0;
byte iambic_flag = 0;
unsigned long last_config_write = 0;
uint16_t memory_area_end = 0;

#ifdef FEATURE_SLEEP
unsigned long last_activity_time = 0;
#endif

#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
unsigned long last_active_time = 0;
#endif

#ifdef FEATURE_DISPLAY
enum lcd_statuses
{
    LCD_CLEAR,
    LCD_REVERT,
    LCD_TIMED_MESSAGE,
    LCD_SCROLL_MSG
};
#define default_display_msg_delay 1000
#endif // FEATURE_DISPLAY

#ifdef FEATURE_LCD_ADAFRUIT_I2C
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
byte lcdcolor = GREEN; // default color for RGB LCD display
#endif                 // FEATURE_LCD_ADAFRUIT_I2C

#if defined(OPTION_WINKEY_2_SUPPORT) && defined(FEATURE_WINKEY_EMULATION)
byte wk2_mode = 1;
#ifndef FEATURE_SO2R_BASE
byte wk2_both_tx_activated = 0;
#endif // FEATURE_SO2R_BASE
byte wk2_paddle_only_sidetone = 0;
#endif // defined(OPTION_WINKEY_2_SUPPORT) && defined(FEATURE_WINKEY_EMULATION)

#ifdef FEATURE_DISPLAY
byte lcd_status = LCD_CLEAR;
unsigned long lcd_timed_message_clear_time = 0;
byte lcd_previous_status = LCD_CLEAR;
byte lcd_scroll_buffer_dirty = 0;
String lcd_scroll_buffer[LCD_ROWS];
byte lcd_scroll_flag = 0;
byte lcd_paddle_echo = 1;
byte lcd_send_echo = 1;
#endif // FEATURE_DISPLAY

#ifdef DEBUG_VARIABLE_DUMP
long dit_start_time;
long dit_end_time;
long dah_start_time;
long dah_end_time;
#endif // DEBUG_VARIABLE_DUMP

#ifdef FEATURE_BUTTONS
int button_array_high_limit[analog_buttons_number_of_buttons];
int button_array_low_limit[analog_buttons_number_of_buttons];
long button_last_add_to_send_buffer_time = 0;
#endif // FEATURE_BUTTONS

byte pot_wpm_low_value;

#ifdef FEATURE_POTENTIOMETER
byte pot_wpm_high_value;
byte last_pot_wpm_read;
int pot_full_scale_reading = default_pot_full_scale_reading;
#endif // FEATURE_POTENTIOMETER

#if defined(FEATURE_SERIAL)
#if !defined(OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW)
byte loop_element_lengths_breakout_flag;
byte dump_current_character_flag;
#endif
byte incoming_serial_byte;
long primary_serial_port_baud_rate;
byte cw_send_echo_inhibit = 0;
#ifdef FEATURE_COMMAND_LINE_INTERFACE
byte serial_backslash_command;
byte cli_paddle_echo = cli_paddle_echo_on_at_boot;
byte cli_prosign_flag = 0;
byte cli_wait_for_cr_to_send_cw = 0;
#if defined(FEATURE_STRAIGHT_KEY_ECHO)
byte cli_straight_key_echo = cli_straight_key_echo_on_at_boot;
#endif
#endif // FEATURE_COMMAND_LINE_INTERFACE
#endif // FEATURE_SERIAL

byte send_buffer_array[send_buffer_size];
byte send_buffer_bytes = 0;
byte send_buffer_status = SERIAL_SEND_BUFFER_NORMAL;

#ifdef FEATURE_MEMORIES
byte play_memory_prempt = 0;
long last_memory_button_buffer_insert = 0;
byte repeat_memory = 255;
unsigned long last_memory_repeat_time = 0;
#endif // FEATURE_MEMORIES

#if defined(FEATURE_SERIAL)
byte primary_serial_port_mode = SERIAL_CLI;
#endif // FEATURE_SERIAL

#ifdef FEATURE_WINKEY_EMULATION
byte winkey_serial_echo = 1;
byte winkey_host_open = 0;
unsigned int winkey_last_unbuffered_speed_wpm = 0;
byte winkey_speed_state = WINKEY_UNBUFFERED_SPEED;
byte winkey_buffer_counter = 0;
byte winkey_buffer_pointer = 0;
byte winkey_dit_invoke = 0;
byte winkey_dah_invoke = 0;
long winkey_paddle_echo_buffer = 0;
byte winkey_paddle_echo_activated = 0;
unsigned long winkey_paddle_echo_buffer_decode_time = 0;
byte winkey_sending = 0;
byte winkey_interrupted = 0;
byte winkey_xoff = 0;
byte winkey_session_ptt_tail = 0;
byte winkey_pinconfig_ptt_bit = 1;
#ifdef OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE
byte winkey_breakin_status_byte_inhibit = 0;
#endif // OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE
#endif // FEATURE_WINKEY_EMULATION

#ifdef FEATURE_PS2_KEYBOARD
byte ps2_keyboard_mode = PS2_KEYBOARD_NORMAL;
byte ps2_keyboard_command_buffer[25];
byte ps2_keyboard_command_buffer_pointer = 0;
#endif // FEATURE_PS2_KEYBOARD

#ifdef FEATURE_HELL
PROGMEM const char hell_font1[] = {B00111111, B11100000, B00011001, B11000000, B01100011, B00000001, B10011100, B00111111, B11100000, // A
                                   B00110000, B00110000, B11111111, B11000011, B00110011, B00001100, B11001100, B00011100, B11100000, // B
                                   B00111111, B11110000, B11000000, B11000011, B00000011, B00001100, B00001100, B00110000, B00110000, // C
                                   B00110000, B00110000, B11111111, B11000011, B00000011, B00001100, B00001100, B00011111, B11100000, // D
                                   B00111111, B11110000, B11001100, B11000011, B00110011, B00001100, B00001100, B00110000, B00110000,
                                   B00111111, B11110000, B00001100, B11000000, B00110011, B00000000, B00001100, B00000000, B00110000,
                                   B00111111, B11110000, B11000000, B11000011, B00000011, B00001100, B11001100, B00111111, B00110000,
                                   B00111111, B11110000, B00001100, B00000000, B00110000, B00000000, B11000000, B00111111, B11110000,
                                   B00000000, B00000000, B00000000, B00000011, B11111111, B00000000, B00000000, B00000000, B00000000,
                                   B00111100, B00000000, B11000000, B00000011, B00000000, B00001100, B00000000, B00111111, B11110000,
                                   B00111111, B11110000, B00001100, B00000000, B01110000, B00000011, B00110000, B00111000, B11100000,
                                   B00111111, B11110000, B11000000, B00000011, B00000000, B00001100, B00000000, B00110000, B00000000,
                                   B00111111, B11110000, B00000001, B10000000, B00001100, B00000000, B00011000, B00111111, B11110000,
                                   B00111111, B11110000, B00000011, B10000000, B00111000, B00000011, B10000000, B00111111, B11110000,
                                   B00111111, B11110000, B11000000, B11000011, B00000011, B00001100, B00001100, B00111111, B11110000,
                                   B00110000, B00110000, B11111111, B11000011, B00110011, B00000000, B11001100, B00000011, B11110000,
                                   B00111111, B11110000, B11000000, B11000011, B11000011, B00001111, B11111100, B11110000, B00000000,
                                   B00111111, B11110000, B00001100, B11000000, B00110011, B00000011, B11001100, B00111001, B11100000,
                                   B00110001, B11100000, B11001100, B11000011, B00110011, B00001100, B11001100, B00011110, B00110000,
                                   B00000000, B00110000, B00000000, B11000011, B11111111, B00000000, B00001100, B00000000, B00110000,
                                   B00111111, B11110000, B11000000, B00000011, B00000000, B00001100, B00000000, B00111111, B11110000,
                                   B00111111, B11110000, B01110000, B00000000, B01110000, B00000000, B01110000, B00000000, B01110000,
                                   B00011111, B11110000, B11000000, B00000001, B11110000, B00001100, B00000000, B00011111, B11110000,
                                   B00111000, B01110000, B00110011, B00000000, B01111000, B00000011, B00110000, B00111000, B01110000,
                                   B00000000, B01110000, B00000111, B00000011, B11110000, B00000000, B01110000, B00000000, B01110000,
                                   B00111000, B00110000, B11111000, B11000011, B00110011, B00001100, B01111100, B00110000, B01110000}; // Z

PROGMEM const char hell_font2[] = {B00011111, B11100000, B11000000, B11000011, B00000011, B00001100, B00001100, B00011111, B11100000, // 0
                                   B00000000, B00000000, B00000011, B00000000, B00000110, B00001111, B11111100, B00000000, B00000000,
                                   B00111000, B01100000, B11110000, B11000011, B00110011, B00001100, B01111000, B00110000, B00000000,
                                   B11000000, B00000011, B00000000, B11000110, B00110011, B00001100, B11111100, B00011110, B00000000,
                                   B00000111, B11111000, B00011000, B00000000, B01100000, B00001111, B11111100, B00000110, B00000000,
                                   B00110000, B00000000, B11000000, B00000011, B00011111, B10000110, B01100110, B00001111, B00011000,
                                   B00011111, B11110000, B11001100, B01100011, B00011000, B11001100, B01100000, B00011111, B00000000,
                                   B01110000, B00110000, B01110000, B11000000, B01110011, B00000000, B01111100, B00000000, B01110000,
                                   B00111100, B11110001, B10011110, B01100110, B00110001, B10011001, B11100110, B00111100, B11110000,
                                   B00000011, B11100011, B00011000, B11000110, B01100011, B00001100, B00001100, B00011111, B11100000}; // 9

PROGMEM const char hell_font3[] = {B00000011, B00000000, B00001100, B00000001, B11111110, B00000000, B11000000, B00000011, B00000000,
                                   B00000011, B00000000, B00001100, B00000000, B00110000, B00000000, B11000000, B00000011, B00000000,
                                   B00000000, B00110000, B00000000, B11001110, B01110011, B00000000, B01111100, B00000000, B00000000,
                                   B01110000, B00000000, B01110000, B00000000, B01110000, B00000000, B01110000, B00000000, B01110000,
                                   B00111000, B00000000, B11100000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                                   B00001100, B00000001, B11110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                                   B00000000, B00111000, B00000011, B10000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                                   B00001100, B11000000, B00110011, B00000000, B11001100, B00000011, B00110000, B00001100, B11000000,
                                   B01110000, B00111000, B01110011, B10000000, B01111000, B00000000, B00000000, B00000000, B00000000,
                                   B00000000, B00000000, B00000000, B00000000, B01111000, B00000111, B00111000, B01110000, B00111000,
                                   B00000000, B00000000, B01110011, B10000001, B11001110, B00000000, B00000000, B00000000, B00000000,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0};

#endif // FEATURE_HELL

#ifdef FEATURE_DEAD_OP_WATCHDOG
byte dead_op_watchdog_active = 1;
byte dit_counter = 0;
byte dah_counter = 0;
#endif // FEATURE_DEAD_OP_WATCHDOG

#ifdef FEATURE_BUTTONS
#ifdef OPTION_REVERSE_BUTTON_ORDER
ButtonArray button_array(analog_buttons_pin, analog_buttons_number_of_buttons, true);
#else
ButtonArray button_array(analog_buttons_pin, analog_buttons_number_of_buttons, false);
#endif
#endif // FEATURE_BUTTONS

#ifdef FEATURE_ROTARY_ENCODER        // Rotary Encoder State Tables
#ifdef OPTION_ENCODER_HALF_STEP_MODE // Use the half-step state table (emits a code at 00 and 11)
const unsigned char ttable[6][4] = {
    {0x3, 0x2, 0x1, 0x0}, {0x23, 0x0, 0x1, 0x0}, {0x13, 0x2, 0x0, 0x0}, {0x3, 0x5, 0x4, 0x0}, {0x3, 0x3, 0x4, 0x10}, {0x3, 0x5, 0x3, 0x20}};
#else // Use the full-step state table (emits a code at 00 only)
const unsigned char ttable[7][4] = {
    {0x0, 0x2, 0x4, 0x0},
    {0x3, 0x0, 0x1, 0x10},
    {0x3, 0x2, 0x0, 0x0},
    {0x3, 0x2, 0x1, 0x0},
    {0x6, 0x0, 0x4, 0x0},
    {0x6, 0x5, 0x0, 0x20},
    {0x6, 0x5, 0x4, 0x0},
};

#endif // OPTION_ENCODER_HALF_STEP_MODE
unsigned char state = 0;
#define DIR_CCW 0x10 // CW Encoder Code (do not change)
#define DIR_CW 0x20  // CCW Encoder Code (do not change)
#endif               // FEATURE_ENCODER_SUPPORT

#ifdef FEATURE_USB_KEYBOARD
unsigned long usb_keyboard_special_mode_start_time = 0;
String keyboard_string;
#endif // FEATURE_USB_KEYBOARD

#if defined(FEATURE_USB_MOUSE) || defined(FEATURE_USB_KEYBOARD)
byte usb_dit = 0;
byte usb_dah = 0;
#endif

#if defined(FEATURE_PS2_KEYBOARD)
#ifdef OPTION_USE_ORIGINAL_VERSION_2_1_PS2KEYBOARD_LIB
PS2Keyboard keyboard;
#else  // OPTION_USE_ORIGINAL_VERSION_2_1_PS2KEYBOARD_LIB
K3NG_PS2Keyboard keyboard;
#endif // OPTION_USE_ORIGINAL_VERSION_2_1_PS2KEYBOARD_LIB
#endif

#if defined(FEATURE_LCD_4BIT) || defined(FEATURE_LCD1602_N07DH)
LiquidCrystal lcd(lcd_rs, lcd_enable, lcd_d4, lcd_d5, lcd_d6, lcd_d7);
#endif

#if defined(FEATURE_LCD_8BIT)
LiquidCrystal lcd(lcd_rs, lcd_enable, lcd_d0, lcd_d1, lcd_d2, lcd_d3, lcd_d4, lcd_d5, lcd_d6, lcd_d7);
#endif

#if defined(FEATURE_LCD_ADAFRUIT_I2C)
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
#endif

#if defined(FEATURE_LCD_ADAFRUIT_BACKPACK)
Adafruit_LiquidCrystal lcd(0);
#endif

#if defined(FEATURE_LCD_SAINSMART_I2C)
// #define I2C_ADDR      0x27
// #define BACKLIGHT_PIN 3
// #define En_pin        2
// #define Rw_pin        1
// #define Rs_pin        0
// #define D4_pin        4
// #define D5_pin        5
// #define D6_pin        6
// #define D7_pin        7
// LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BACKLIGHT_PIN, POSITIVE);
LiquidCrystal_I2C lcd(lcd_i2c_address_sainsmart_lcd, LCD_COLUMNS, LCD_ROWS);
#endif // FEATURE_SAINSMART_I2C_LCD

#if defined(FEATURE_LCD_YDv1)
// LiquidCrystal_I2C lcd(0x38);
LiquidCrystal_I2C lcd(lcd_i2c_address_ydv1_lcd, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // for FEATURE_LCD_YDv1; set the LCD I2C address needed for LCM1602 IC V1
#endif

#if defined(FEATURE_LCD_TWILIQUIDCRYSTAL)
TwiLiquidCrystal lcd(lcd_i2c_address_twiliquidcrystal_lcd);
#endif

#if defined(FEATURE_LCD_FABO_PCF8574)
FaBoLCD_PCF8574 lcd;
#endif

#if defined(FEATURE_LCD_MATHERTEL_PCF8574)
LiquidCrystal_PCF8574 lcd(lcd_i2c_address_mathertel_PCF8574);
#endif

#if defined(FEATURE_LCD_I2C_FDEBRABANDER)
LiquidCrystal_I2C lcd(lcd_i2c_address_fdebrander_lcd, LCD_COLUMNS, LCD_ROWS, /*charsize*/ LCD_5x8DOTS);
#endif

#if defined(FEATURE_LCD_HD44780)
hd44780_I2Cexp lcd;
#endif

#if defined(FEATURE_OLED_SSD1306)
SSD1306AsciiWire lcd;
#endif

#if defined(FEATURE_USB_KEYBOARD) || defined(FEATURE_USB_MOUSE)
USB Usb;
uint32_t next_time;
#endif

#if defined(FEATURE_USB_KEYBOARD)
class KbdRptParser : public KeyboardReportParser
{
protected:
    virtual void OnKeyDown(uint8_t mod, uint8_t key);
    virtual void OnKeyUp(uint8_t mod, uint8_t key);
};
HIDBoot<HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);
KbdRptParser KeyboardPrs;
#endif

#if defined(FEATURE_USB_MOUSE)
class MouseRptParser : public MouseReportParser
{
protected:
    virtual void OnMouseMove(MOUSEINFO *mi);
    virtual void OnLeftButtonUp(MOUSEINFO *mi);
    virtual void OnLeftButtonDown(MOUSEINFO *mi);
    virtual void OnRightButtonUp(MOUSEINFO *mi);
    virtual void OnRightButtonDown(MOUSEINFO *mi);
    virtual void OnMiddleButtonUp(MOUSEINFO *mi);
    virtual void OnMiddleButtonDown(MOUSEINFO *mi);
};
HIDBoot<HID_PROTOCOL_MOUSE> HidMouse(&Usb);
MouseRptParser MousePrs;
#endif // FEATURE_USB_MOUSE

#if defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE)
// BasicTerm term(&Serial);
#endif

PRIMARY_SERIAL_CLS *primary_serial_port;

#if defined(FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT)
SECONDARY_SERIAL_CLS *secondary_serial_port;
#endif

PRIMARY_SERIAL_CLS *debug_serial_port;

#ifdef FEATURE_PTT_INTERLOCK
byte ptt_interlock_active = 0;
#endif // FEATURE_PTT_INTERLOCK

#ifdef FEATURE_QLF
byte qlf_active = qlf_on_by_default;
#endif // FEATURE_QLF

#if defined(FEATURE_PADDLE_ECHO)
byte paddle_echo = 0;
long paddle_echo_buffer = 0;
unsigned long paddle_echo_buffer_decode_time = 0;
#endif // FEATURE_PADDLE_ECHO

#ifndef FEATURE_PADDLE_ECHO
long paddle_echo_buffer = 0;
#endif // FEATURE_PADDLE_ECHO

#if defined(FEATURE_CW_DECODER) && defined(OPTION_CW_DECODER_GOERTZEL_AUDIO_DETECTOR)
Goertzdetector cwtonedetector;
#endif

#if defined(FEATURE_COMPETITION_COMPRESSION_DETECTION)
unsigned long compression_detection_key_down_time = 0;
unsigned long compression_detection_key_up_time = 0;
int time_array[COMPETITION_COMPRESSION_DETECTION_ARRAY_SIZE];
byte time_array_index = 0;
#endif // FEATURE_COMPETITION_COMPRESSION_DETECTION

#if defined(FEATURE_CW_COMPUTER_KEYBOARD)
byte cw_keyboard_capslock_on = 0;
#endif // defined(FEATURE_CW_COMPUTER_KEYBOARD)

#if defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE) && defined(FEATURE_WINKEY_EMULATION)
byte send_winkey_breakin_byte_flag = 0;
#endif // defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE) && defined(FEATURE_WINKEY_EMULATION)

#if defined(FEATURE_ETHERNET)
uint8_t default_ip[] = FEATURE_ETHERNET_IP;              // default IP address ("192.168.1.178")
uint8_t default_gateway[] = FEATURE_ETHERNET_GATEWAY;    // default gateway
uint8_t default_subnet[] = FEATURE_ETHERNET_SUBNET_MASK; // default subnet mask
uint8_t dns_server[] = FEATURE_ETHERNET_DNS;
uint8_t mac[] = FEATURE_ETHERNET_MAC; // default physical mac address
uint8_t restart_networking = 0;

#if defined(FEATURE_WEB_SERVER)
#define MAX_WEB_REQUEST 512
String web_server_incoming_string;
uint8_t valid_request = 0;
EthernetServer server(FEATURE_ETHERNET_WEB_LISTENER_PORT); // default server port
#define MAX_PARSE_RESULTS 32
struct parse_get_result_t
{
    String parameter;
    String value_string;
    long value_long;
};
struct parse_get_result_t parse_get_results[MAX_PARSE_RESULTS];
int parse_get_results_index = 0;
unsigned long web_control_tx_key_time = 0;
#endif // FEATURE_WEB_SERVER

#if defined(FEATURE_UDP)
unsigned int udp_listener_port = FEATURE_INTERNET_LINK_DEFAULT_RCV_UDP_PORT;
EthernetUDP Udp;
#if defined(FEATURE_INTERNET_LINK)
uint8_t udp_send_buffer[FEATURE_UDP_SEND_BUFFER_SIZE];
uint8_t udp_send_buffer_bytes = 0;
uint8_t udp_receive_packet_buffer[FEATURE_UDP_RECEIVE_BUFFER_SIZE];
uint8_t udp_receive_packet_buffer_bytes = 0;
#endif // FEATURE_INTERNET_LINK
#endif
#endif // FEATURE_ETHERNET

unsigned long automatic_sending_interruption_time = 0;

#ifdef FEATURE_4x4_KEYPAD
// Define the Keymap for 4x4 matrix keypad
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
#endif

#ifdef FEATURE_3x4_KEYPAD
// Define the Keymap for 3x4 matrix keypad
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
#endif

// Setup for 4x4 matrix keypad
#ifdef FEATURE_4x4_KEYPAD
byte rowPins[KEYPAD_ROWS] = {Row3, Row2, Row1, Row0}; // Arduino Mega Pins: 30,31,32,33---Keypad Pins 5,6,7,8
byte colPins[KEYPAD_COLS] = {Col3, Col2, Col1, Col0}; // Arduino Mega Pins: 34,35,36,37---Keypad Pins 1,2,3,4
#endif

#ifdef FEATURE_3x4_KEYPAD
byte rowPins[KEYPAD_ROWS] = {Row3, Row2, Row1, Row0}; // Arduino Mega Pins: 30,31,32,33--Keypad Pins 4,6,7,2
byte colPins[KEYPAD_COLS] = {Col2, Col1, Col0};       // Arduino Mega Pins: 34,35,36-----Keypad Pins 5,1,3
#endif

#if defined(FEATURE_4x4_KEYPAD) || defined(FEATURE_3x4_KEYPAD)
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);
#endif

unsigned long millis_rollover = 0;

#if defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE)
byte check_serial_override = 0;
#if defined(OPTION_WORDSWORTH_CZECH)
#include "keyer_training_text_czech.h"
#elif defined(OPTION_WORDSWORTH_DEUTSCH)
#include "keyer_training_text_deutsch.h"
#elif defined(OPTION_WORDSWORTH_NORSK)
#include "keyer_training_text_norsk.h"
#elif defined(OPTION_WORDSWORTH_POLISH)
#include "keyer_training_text_polish.h"
#else
#include "keyer_training_text_english.h"
#endif

#include "keyer_callsign_prefixes.h"
#endif

#ifdef FEATURE_CLOCK
unsigned long clock_years = 0;
unsigned long clock_months = 0;
unsigned long clock_days = 0;
unsigned long clock_hours = 0;
unsigned long clock_minutes = 0;
unsigned long clock_seconds = 0;
long local_clock_years = 0;
long local_clock_months = 0;
long local_clock_days = 0;
long local_clock_hours = 0;
long local_clock_minutes = 0;
long local_clock_seconds = 0;
int clock_year_set = 2017;
byte clock_month_set = 1;
byte clock_day_set = 1;
byte clock_sec_set = 0;
unsigned long clock_hour_set = 0;
unsigned long clock_min_set = 0;
unsigned long millis_at_last_calibration = 0;
#endif // FEATURE_CLOCK

#if defined(FEATURE_SD_CARD_SUPPORT)
uint8_t sd_card_state = SD_CARD_UNINITIALIZED;
File sdfile;
File sdlogfile;
uint8_t sd_card_log_state = SD_CARD_LOG_NOT_OPEN;
#endif // FEATURE_SD_CARD_SUPPORT

#ifdef FEATURE_SEQUENCER
unsigned long sequencer_ptt_inactive_time = 0;
byte sequencer_1_active = 0;
byte sequencer_2_active = 0;
byte sequencer_3_active = 0;
byte sequencer_4_active = 0;
byte sequencer_5_active = 0;
#endif

#ifdef FEATURE_SO2R_BASE
uint8_t so2r_rx = 1;
uint8_t so2r_tx = 1;
uint8_t so2r_pending_tx = 0;
uint8_t so2r_ptt = 0;
uint8_t so2r_latch = 0;

#ifdef FEATURE_SO2R_SWITCHES
uint8_t so2r_open = 0;
uint8_t so2r_switch_rx = 0;
uint8_t so2r_debounce = 0;
unsigned long so2r_debounce_time;
#endif // FEATURE_SO2R_SWITCHES

#ifdef FEATURE_SO2R_ANTENNA
uint8_t so2r_antenna_1 = 0;
uint8_t so2r_antenna_2 = 0;
#endif // FEATURE_SO2R_ANTENNA
#endif // FEATURE_SO2R_BASE

byte async_eeprom_write = 0;

// Variables and stuff
struct config_t
{ // 120 bytes total

    uint8_t paddle_mode;
    uint8_t keyer_mode;
    uint8_t sidetone_mode;
    uint8_t pot_activated;
    uint8_t length_wordspace;
    uint8_t autospace_active;
    uint8_t current_ptt_line;
    uint8_t current_tx;
    uint8_t weighting;
    uint8_t dit_buffer_off;
    uint8_t dah_buffer_off;
    uint8_t cmos_super_keyer_iambic_b_timing_percent;
    uint8_t cmos_super_keyer_iambic_b_timing_on;
    uint8_t link_receive_enabled;
    uint8_t paddle_interruption_quiet_time_element_lengths;
    uint8_t wordsworth_wordspace;
    uint8_t wordsworth_repetition;
    uint8_t cli_mode;
    uint8_t ptt_buffer_hold_active;
    uint8_t ptt_disabled;
    uint8_t beacon_mode_on_boot_up;
    uint8_t keying_compensation;
    // 22 bytes

    unsigned int wpm;
    unsigned int hz_sidetone;
    unsigned int dah_to_dit_ratio;
    unsigned int wpm_farnsworth;
    unsigned int memory_repeat_time;
    unsigned int wpm_command_mode;
    unsigned int link_receive_udp_port;
    unsigned int wpm_ps2_usb_keyboard;
    unsigned int wpm_cli;
    unsigned int wpm_winkey;
    unsigned int cw_echo_timing_factor;
    unsigned int autospace_timing_factor;
    // 24 bytes

    uint8_t ip[4];
    uint8_t gateway[4];
    uint8_t subnet[4];
    uint8_t dns_server[4];
    // 16 bytes

    uint8_t link_send_ip[4][FEATURE_INTERNET_LINK_MAX_LINKS]; // FEATURE_INTERNET_LINK_MAX_LINKS = 2
    uint8_t link_send_enabled[FEATURE_INTERNET_LINK_MAX_LINKS];
    unsigned int link_send_udp_port[FEATURE_INTERNET_LINK_MAX_LINKS];
    // 14 bytes

    unsigned int ptt_lead_time[6];
    unsigned int ptt_tail_time[6];
    unsigned int ptt_active_to_sequencer_active_time[5];
    unsigned int ptt_inactive_to_sequencer_inactive_time[5];
    // 44 bytes

    int sidetone_volume;
    // 2 bytes

} configuration;
