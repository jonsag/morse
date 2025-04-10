Recent Update History

2.2.2015040402 More work on ARDUINO_SAM_DUE (documented)

2.2.2015040501 Fixed bug with O command not working when any display feature was compiled in

2.2.2015040801 FEATURE_EEPROM_E24C1024; working on FEATURE_CW_COMPUTER_KEYBOARD (documented)

2.2.2015040901 updated serial help text with recently added commands, consolidated the three paddle echo features into one subroutine

2.2.2015040902 Minor typos fixed

2.2.2015042002 Eliminated keyer.h declaration (upgrade Stino if you're still using keyer.h)

2.2.2015042301
'#define PRIMARY_SERIAL_PORT &Serial' is now '#define PRIMARY_SERIAL_PORT &Serial' (documented on website 2015-04-25)
OPTION_SERIAL_PORT_DEFAULT_WINKEY_EMULATION is now OPTION_PRIMARY_SERIAL_PORT_DEFAULT_WINKEY_EMULATION (documented on website 2015-04-25)
'#define default_serial_baud_rate 115200' is now '#define PRIMARY_SERIAL_PORT_BAUD 115200' (documented on website 2015-04-25)
#define SECONDARY_SERIAL_PORT_BAUD 115200 (documented on website 2015-04-25)
FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT  (documented on website 2015-04-25)
FEATURE_LCD1602_N07DH (Thanks Xigco for code!)  (documented on website 2015-04-25)

2.2.2015042302
OPTION_CW_KEYBOARD_ITALIAN (Thanks Giorgio IZ2XBZ)  (documented on website 2015-04-25)
FEATURE_CW_COMPUTER_KEYBOARD repeating backspace, fixed caps lock sounds

2.2.2015042303
Test of GitHub - no changes

2.2.2015042501
FEATURE_CW_COMPUTER_KEYBOARD update from Giorgio IZ2XBZ
Website documentation up to date!  Yeahhhhhh!  :-)

2.2.2015042901
HARDWARE_NANOKEYER_REV_D

2.2.2015043001
Fixed compilation bug with FEATURE_COMMAND_LINE_INTERFACE when FEATURE_WINKEY_EMULATION not enabled

2.2.2015051201
OPTION_CW_DECODER_GOERTZEL_AUDIO_DETECTOR (website updated 2015-05-12)

2.2.2015051301
Improvements to FEATURE_CW_DECODER for better decoding and Goetzel settings for Arduino Due

2.2.2015061101
lcd_columns and lcd_rows in keyer_settings*.h files renamed to LCD_COLUMNS and LCD_ROWS
OPTION_INVERT_PADDLE_PIN_LOGIC - paddle closed = HIGH, paddle open = LOW

2.2.2015082801
Added E24C1024.h and E24C1024.cpp to git
Fixed compilation issue with Due involving E24C1024 library

2.2.2015082802
FEATURE_STRAIGHT_KEY  {documented on web page 2015-09-05}

2.2.2015090501
Memories can now be programmed in commmand mode (FEATURE_BUTTONS) by pressing the memory button
FEATURE_CW_DECODER now has digital input pin (cw_decoder_pin) and if OPTION_CW_DECODER_GOERTZEL_AUDIO_DETECTOR is enable, cw_decoder_audio_input_pin will work in parallel

2.2.2015090801
Fixed issue with FEATURE_CW_DECODER + OPTION_CW_DECODER_GOERTZEL_AUDIO_DETECTOR and wrong GOERTZ_SAMPLING_FREQ and GOERTZ_SAMPLES used in goertzel.h causing keyer lockups after startup

2.2.2015091301
FEATURE_DYNAMIC_DAH_TO_DIT_RATIO (code contributed by Giorgio, IZ2XBZ)
#ifdef FEATURE_DYNAMIC_DAH_TO_DIT_RATIO  (keyer_settings.h)
  #define DYNAMIC_DAH_TO_DIT_RATIO_LOWER_LIMIT_WPM 30
  #define DYNAMIC_DAH_TO_DIT_RATIO_LOWER_LIMIT_RATIO 300 // 300 = 3:1 ratio
  #define DYNAMIC_DAH_TO_DIT_RATIO_UPPER_LIMIT_WPM 70
  #define DYNAMIC_DAH_TO_DIT_RATIO_UPPER_LIMIT_RATIO 240 // 240 = 2.4:1 ratio
#endif //FEATURE_DYNAMIC_DAH_TO_DIT_RATIO

2.2.2015091302
FEATURE_COMPETITION_COMPRESSION_DETECTION - Experimental
Fixed compiler error when only FEATURE_BUTTONS was enabled

2.2.2015091801
OPTION_DIT_DAH_BUFFERS_OFF_BY_DEFAULT_FOR_FEATURE_DIT_DAH_BUFFER_CONTROL
OPTION_ADVANCED_SPEED_DISPLAY (code contributed by Giorgio, IZ2XBZ)

2.2.2015091802
Improved handling of spaces in LCD display

2.2.2015092101
Fixed bugs in OPTION_CW_KEYBOARD_ITALIAN and OPTION_UNKNOWN_CHARACTER_ERROR_TONE (courtesy of Giorgio, IZ2XBZ)

2.2.2015092301
FEATURE_COMPETITION_COMPRESSION_DETECTION improvements

2.2.2015092401
#define compression_detection_pin 0
default potentiometer_change_threshold changed to 0.9

2.2.2015101201
Additional DEBUG_PS2_KEYBOARD code

2.2.2015101301
OPTION_STRAIGHT_KEY_ECHO

2.2.2015101302
OPTION_STRAIGHT_KEY_ECHO is now FEATURE_STRAIGHT_KEY_ECHO
CLI command: \`     Toggle straight key echo
#define cli_paddle_echo_on_at_boot 1
#define cli_straight_key_echo_on_at_boot 1
FEATURE_STRAIGHT_KEY now works with FEATURE_CW_COMPUTER_KEYBOARD
Straight Key can now program memories

2.2.2015101401
Fixed compile bug with FEATURE_DISPLAY and cli_straight_key_echo

2.2.2015101402
K3NG_PS2Keyboard Library: Fixed issues with CTRL and ALT key combinations with German and French keyboards

2.2.2015101801
OPTION_WINKEY_IGNORE_LOWERCASE

2.2.2015111501
Fixed storage of KN prosign in memory (Thank Stefan, DL1SMF)

2.2.2015120401
Fixed compiler warning: large integer implicitly truncated to unsigned type - jump_back_to_y = 99999;

2.2.2015121901
OPTION_PROSIGN_SUPPORT - additional prosign support for memory storage

2.2.2015122001
OPTION_PROSIGN_SUPPORT - updated; forgot to add functionality to paddle echo

2.2.2015122801
void send_the_dits_and_dahs(char * cw_to_send) compile warning fix

2.2.2016010301
Fixed compiler error when OPTION_SAVE_MEMORY_NANOKEYER and FEATURE_COMMAND_LINE_INTERFACE are enabled (Thanks, Gerd, DD4DA)
void play_memory (byte memory_number) near line 10049 - static String serial_number_string - removed static declration to fix compiler warning (Thanks, Gerd, DD4DA)

2.2.2016010302
Winkey emulation pin config bug fix (Thanks, Gerd, DD4DA)

2.2.2016011801
New and improved FEATURE_SLEEP code contributed by Graeme, ZL2APV

2.2.2016012001
Fixed compile error involving serial_number, FEATURE_PS2_KEYBOARD, and HARDWARE_NANOKEYER_REV_D (Thanks, Kari, OH6FSG)

2.2.2016012002
HARDWARE_TEST
Enhanced FEATURE_SLEEP to have pin that indicates sleep state: define keyer_awake 0 ; KEYER_AWAKE_PIN_AWAKE_STATE, KEYER_AWAKE_PIN_ASLEEP_STATE

2.2.2016012003
Fixed compiler warning for void play_memory() and returns; (Thanks, Gerd, DD4DA)

2.2.2016012004
Modified includes so library files can be put in \libraries\ folder rather than ino directory so Arduino 1.6.7 works (thanks Giorgio, IZ2XBZ))

2.2.2016012101
Beta testing FEATURE_INTERRUPT_PADDLE_READS

2.2.2016012301
Fixed compilation error: 10306: error: return-statement with no value, in function returning byte (thanks Giorgio, IZ2XBZ))

2.2.2016012302
Merge of bug fix from JG2RZF: Winkey - CTESTWIN sends 0x00 as "HSCW Speed Change" to keyer (thanks JG2RZF)

2.2.2016012501
loop_element_lengths - minor change to paddle reading that may have an effect at high speeds

2.2.2016012502
tx_key_dit_and_dah_pins_active_state and tx_key_dit_and_dah_pins_inactive_state settings
OPTION_RUSSIAN_LANGUAGE_SEND_CLI contributed by Павел Бирюков, UA1AQC

2.2.2016012601
Winkey emulation support for 0x1D HSCW overloaded command to switch transmitters (thanks JG2RZF)
Moved stuff from keyer_settings*.h to keyer.h (no need to tweak these or have different entries for different hardware)

2.2.2016012801
Fixed issue with goertzel.h being required for compilation even when it wasn't needed

2.2.2016012901
Removed experimental feature

2.2.2016012902
FEATURE_LCD_ADAFRUIT_BACKPACK - support for Adafruit I2C LCD Backup using MCP23008 (courtesy Josiah Ritchie, KE0BLL)

2.2.2016020801
PROSIGN_HH (courtesy of Vincenzo, IZ0RUS)

2.2.2016020802
OPTION_DO_NOT_SEND_UNKNOWN_CHAR_QUESTION

2.2.2016030501
FEATURE_LCD_SAINSMART_I2C

2.2.2016030701
Fixed FEATURE_LCD_SAINSMART_I2C initialization

2.2.2016030801
Fixed FEATURE_LCD_SAINSMART_I2C again

2.2.2016031801
Ethernet, web server and Internet linking functionality in beta / development (DEFINEs are in HARDWARE_TEST files only right now)
#define FEATURE_WEB_SERVER
#define FEATURE_INTERNET_LINK

#define OPTION_INTERNET_LINK_NO_UDP_SVC_DURING_KEY_DOWN

#define FEATURE_ETHERNET_IP {192,168,1,179}                      // default IP address
#define FEATURE_ETHERNET_MAC {0xDE,0xAD,0xBE,0xEF,0xFE,0xEE}

#define FEATURE_ETHERNET_GATEWAY {192,168,1,1}                   // default gateway
#define FEATURE_ETHERNET_SUBNET_MASK {255,255,255,0}                  // default subnet mask
#define FEATURE_ETHERNET_WEB_LISTENER_PORT 80
#define FEATURE_UDP_SEND_BUFFER_SIZE 128
#define FEATURE_UDP_RECEIVE_BUFFER_SIZE 128

#define FEATURE_INTERNET_LINK_MAX_LINKS 2
#define FEATURE_INTERNET_LINK_DEFAULT_RCV_UDP_PORT 8888
#define FEATURE_INTERNET_LINK_BUFFER_TIME_MS 500

2.2.2016040501
Fixed bug with OPTION_DO_NOT_SEND_UNKNOWN_CHAR_QUESTION and ? character not being sent with keyboard and Winkey operation
Still working on web server functionality

2.2.2016042601
More web server functionality work
#define FEATURE_INTERNET_LINK_KEY_DOWN_TIMEOUT_SECS 8
\P command now can program memories above #10

2.2.2016053001
Additional DEBUG_WINKEY messages for Winkeyer troubleshooting
#define WINKEY_DEFAULT_BAUD 1200 (added setting for UCXLog 9600 baud Winkey setting)
Fixed minor Winkey emulation bug with recognizing byte 0x7C as a half dit space when OPTION_WINKEY_IGNORE_LOWERCASE is enabled

2.2.2016062101
New CLI commands:
  \>     Send serial number, then increment
  \<     Send current serial number
  \(     Send current serial number in cut numbers
  \)     Send serial number with cut numbers, then increment

2.2.2016070701
Corrected Nanokeyer Rev B and Rev D configurations

2.2.2016070702
Setting for speed potentiometer check interval: #define potentiometer_check_interval_ms 150

2.2.2016071001
OPTION_WINKEY_UCXLOG_9600_BAUD for UCXLog 9600 baud support (I can't get UCXlog to work at 1200 baud)

2.2.2016071801
Now have FEATURE_AUTOSPACE and FEATURE_DEAD_OP_WATCHDOG disabled by default for HARDWARE_NANOKEYER_REV_D

2.2.2016071802
FEATURE_CAPACITIVE_PADDLE_PINS: capactive_paddle_pin_inhibit_pin

2.2.2016072301
Added dependency check for FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
More Winkey emulation debugging; working on strange issues with UcxLog interoperability.  UcxLog working with normal 1200 baud mode today.  Hmmm.

2.2.2016080101
Troubleshooting some UCXLog Winkey weirdness some users are experiencing.  Created OPTION_WINKEY_UCXLOG_SUPRESS_C4_STATUS_BYTE

2.2.2016080301
Disabled echoing of 7C (half space character) byte in Winkey emulation

2.2.2016080601
More messing around with UCXlog...
OPTION_WINKEY_DO_NOT_ECHO_7C_BYTE                    // Might need for UCXlog? (7C = half space character)
OPTION_WINKEY_DO_NOT_SEND_7C_BYTE_HALF_SPACE

2.2.2016081201
OPTION_WINKEY_DO_NOT_ECHO_7C_BYTE is changed to OPTION_WINKEY_ECHO_7C_BYTE and only in the test feature and options file for testing/debugging purposes
OPTION_WINKEY_DO_NOT_SEND_7C_BYTE_HALF_SPACE - not placing this into production.  this was to troubleshoot issues with UCXLog

2.2.2016081601
Updated paddle echo to work with bug mode

2.2.2016090701
More efficient code suggestion from Paul, K1XM, implemented in loop_element_lengths()

2.2.2016090801
Removed legacy option: OPTION_USE_ORIGINAL_VERSION_2_1_PS2KEYBOARD_LIB

2.2.2016090802
Corrected error in FEATURE_ROTARY_ENCODER ttable (thanks, frye.dale)

2.2.2016091401
More frequent PTT line tail time checking

2.2.2016091602
Reversing munged GitHub merge

2.2.2016091901
Manual merge of toyo pull request #22
It is no longer necessary to specify HARDWARE_ARDUINO_DUE in keyer_hardware.h.  It is automatically detected now.

2.2.2016092701
Command Mode: command L - adjust weighting

2.2.2016092702
Winkey Emulation - changed paddle interrupt behavior to send 0xC2 and then 0xC0 rather than just 0xC0

2.2.2016092801
Winkey Emulation - changed paddle interrupt behavior to send 0xC6,0xC0 rather than 0x64,0xC0

2.2.2016092802
Fixed issue with configuration in eeprom colliding with memory 0 (1) (Thanks, Ivan, IX1FJG)

2.2.2016092803
Winkey Emulation - changed paddle interrupt behavior to also clear send buffer

2.2.2016092901
Improved opposite paddle dit/dah insertion in Ultimatic mode

2.2.2016100601
Improved paddle break in for memory playing and Winkey interruption
Fixed various compile bugs that have crept into the code

2.2.2016102401
Updated \J (dah to dit ratio) and \L (weighting) CLI commands so that without arguments they set the parameters to defaults

2.2.2016102801
Single Paddle mode, C command

2.2.2016103101
Quiet Paddle Interruption feature - set with \[ command in CLI.  Value is 0 to 20 element lengths; 0 = off

2.2.2016110801
Integrated OK1RR Tiny Keyer hardware files - HARDWARE_TINYKEYER in keyer_hardware.h file

2.2.2016110802
New command mode command H - set weighting and dah to dit ratio to defaults
New command mode command ? - Status

2.2.2016111701
FEATURE_CW_COMPUTER_KEYBOARD enhancements from Giorgio IZ2XBZ

2.2.2016111702
Eliminated FEATURE_DIT_DAH_BUFFER_CONTROL code; it's compiled in with core code now.  Also depricated OPTION_DIT_DAH_BUFFERS_OFF_BY_DEFAULT_FOR_FEATURE_DIT_DAH_BUFFER_CONTROL

2.2.2016112301
New command mode command K: toggle dit and dah buffer on and off

2.2.2016112302
Updated keyer_hardware.h to accomodate Leonardo, Yun, Esplora, and other boards to compile with Serial related functionality.

2.2.2016112401
Updated dit and dah buffer control to change automatically with Iambic A & B and Ultimatic

2.2.2016112501
Code comment update

2.2.2016112502
Merged in GitHub pull request 24 https://github.com/k3ng/k3ng_cw_keyer/pull/24 from Giorgio IZ2XBZ

2.2.2016112701
Improved performance when sending large macros from logging and contest programs using Winkey emulation.  Thanks, Martin OK1RR for discovery and testing

2.2.2016112702
Updated command mode K command to work only when in Ultimatic mode

2.2.2016112901
Fixed bug with command mode status command reporting wrong keyer mode.  Also fixed CLI status query reporting wrong keyer mode while in command mode

2.2.2016120101
Compilation of serial related functionality for TEENSYDUINO

2.2.2016120102
Comilation issue fix for ARDUINO_MAPLE_MINI.  Thanks, Edgar, KC2UEZ

2.2.2016120401
Added keyer_stm32duino.h with function declarations to make ARDUINO_MAPLE_MINI compilation work.  Thanks, Edgar, KC2UEZ

2.2.2016120901
Merged pull request STM32duino compatibilty 30. Thanks, Edgar, KC2UEZ

2.2.2016120902
Fixed bug in command mode when OPTION_WATCHDOG_TIMER is enabled.  Thanks, disneysw.

2.2.2016121001
Support for FUNtronics FK-10 contributed by disneysw. HARDWARE_FK_10 in keyer_hardware.h; files: keyer_pin_settings_fk_10.h, keyer_features_and_options_fk_10.h, keyer_settings_fk_10.h

2.2.2016121201
Additional work on web interface

2.2.2016121202
Additional work on web interface
Mainstreamed FEATURE_HI_PRECISION_LOOP_TIMING code.  No longer an option.  (Need to clean out of keyer_feature_and_options files)

2.2.2017010301
FEATURE_AMERICAN_MORSE - American Morse Code sending mode.  \= command in the CLI switches to American Morse Code https://en.wikipedia.org/wiki/American_Morse_code

2.2.2017011701
FEATURE_LCD1602_N07DH - added include for Wire.h (Thanks, Hjalmar, OZ1JHM)

2.2.2017011702
Pull request 32 https://github.com/k3ng/k3ng_cw_keyer/pull/32 merged which adds FEATURE_SIDETONE_SWITCH.  Also fixed up additional features and pins files. (Thanks, dfannin)

2.2.2017011703
Added OPTION_CW_KEYBOARD_GERMAN (Thanks, Raimo, DL1HTB)

2.2.2017012101
New command mode command R: set serial number

2.2.2017020701
WD9DMP contributed fixes and changes
  Reconciled CLI Command/Memory Macro Help code with front comments and actual code so all commands now display with /?
  Removed unimplemented Memory Macros from CLI Help
  Updated descriptions of CLI Command/Memory Macro functions in help display (some missing serial number lack increment description where present in code)
  Fixed issue where the TX ON/TX Off LCD display state in Command Mode could get out of sync with the actual key_tx state
  Fixed serial numbers not displaying in LCD and CLI when playing back from Macro or CLI command (please check conditional compilations)
  Fixed capialization in HELP display and Status output to be consistent
  Changed "$" at end of non-empty memory contents in CLI status display to "_" to help determine if a trailing space is present.

2.2.2017020702
Fixed typo

2.2.2017021001
Fixed typo - 'include <Keyboard.h>' was commented out (thanks Raimo, DL1HTB)

2017.02.12.01
WD9DMP contributed addition fixes
Changed version number scheme.  The 2.2 really isn't significant anymore.

2017.02.12.02
loop_element_lengths sending_mode code error fixed.  (Thanks, WD9DMP)

2017.02.16.01
Added note: Have a problem with Keyboard.h not found?  See https://github.com/k3ng/k3ng_cw_keyer/issues/35

2017.03.12.01
WD9DMP contribution: Added checks to see that keyer is NOT in command mode before allowing keyboards or CLI to toggle key_tx flag state, otherwise key commands could key transmitter
Added library.properties file to K3NG_PS2Keyboard library to support the Arduino IDE eye candy bloatware Library Manager

2017.03.12.02
Added CTRL-S keystroke to toggle CMOS Superkeyer Timing on and off in FEATURE_PS2_KEYBOARD and FEATURE_USB_KEYBOARD

2017.03.22.01
Commented out include <Keyboard.h> due to unexplained compilation error in Arduino 1.8.1

2017.03.30.01
FEATURE_4x4_KEYPAD and FEATURE_3x4_KEYPAD code contributed by Jack, W0XR

2017.04.19.01
Minor change in keyer.h to prevent errors with some versions of Arduino IDE when compiling USB HID features

2017.04.19.02
OPTION_CMOS_SUPER_KEYER_IAMBIC_B_TIMING_ON_BY_DEFAULT and two code fixes contributed by Raimo, DL1HTB, thanks!

2017.04.22.01
Webserver About screen now handles millis() uptime rollover
Bug fix in loop_element_lengths and Internet Linking functionality UDP packet handling

2017.04.27.01
Added bounds checking for void speed_set()

2017.05.03.01
FEATURE_TRAINING_COMMAND_LINE_INTERFACE
First release of Wordsworth training functionality

2017.05.05.01
keyer_training_text_czech.h contributed by Martin, OK1RR
Czech language support for Wordsworth training: OPTION_WORDSWORTH_CZECH

2017.05.06.01
Lots of new functionality in FEATURE_TRAINING_COMMAND_LINE_INTERFACE
keyer_training_text_norsk.h content contributed by Karl, LA3FY
Norwegian language support for Wordsworth training: OPTION_WORDSWORTH_NORSK

2017.05.09.01
FEATURE_TRAINING_COMMAND_LINE_INTERFACE - fixed issue with carriage returns and line feeds causing menus to reprint

2017.05.09.02
Updated FEATURE_4x4_KEYPAD and FEATURE_3x4_KEYPAD to allow memory stacking

2017.05.12.01
Fixed bug with \< and \> commands and carriage returns, and now handle serial number sending through the send buffer rather than direct sending
Fixed issue with non-English characters in Wordsworth by implementing OPTION_NON_ENGLISH_EXTENSIONS within Wordsworth

2017.05.12.02
Added DEBUG_MEMORY_LOCATIONS

2017.05.13.01
Improved reading of serial receive buffer in serial_program_memory to facilitate programming of large memories.  Related parameter: serial_program_memory_buffer_size

2017.05.13.02
Added random code group practice

2017.05.14.01
Optimization of serial_program_memory()

2017.06.03.01
Fixed a bug I introduced back in version 2017.05.12.01 or so with memory serial number macros not playing in right sequence (Thanks, Fred, VK2EFL)

2017.06.03.02
Added OPTION_SIDETONE_DIGITAL_OUTPUT_NO_SQUARE_WAVE which changes the sidetone line to go high/low rather than output square wave, for driving an external audio amplifier

2017.06.14.02
Fixed command line interface bug with /> and /< commands and carriage returns

2017.06.28.01
Fixed bug with \T command when FEATURE_BUTTONS is not activated. (Thanks, Павел Бирюков)

2017.06.28.02
Keyer now reports rotary encoder speed changes in K1EL Winkey emulation (Thanks, Marc-Andre, VE2EVN)

2017.07.24.01
Fixed keypad asterisk and pound definitions (Thanks, Fred, VK2EFL)

2017.07.31.01
Fixed bug with memory macro \X not switching to transmitters 4, 5, or 6 (Thanks, Larry, DL6YY)

2018.01.05.01
When entering into program memory mode in command mode, a beep is now emitted rather than a dit
Implemented CLI Receive / Transmit Echo Practice  (\K E)

2018.01.06.01
Enhancements to CLI CW Training module

2018.01.13.01
O command in command mode, keyboard input, and CLI enhanced to cycle through sidetone on / off / paddle only; code provided by Marc-Andre, VE2EVN

2018.01.13.02
Improvements to LCD display in practice modes; code provided by Fred, VK2EFL
Minor tweaks to handle LCD displays with lesser number of columns
Bug fixes involving practice modes and garbage left in paddle_echo_buffer

2018.01.14.01
Added \/ (backslash slash) CLI command for Paged Help
Added /@ CLI command for Mill Mode
ESC in CLI will now dump type ahead buffer and stop memory repeat, just like \\
Added /} CLI command to set potentiometer range

2018.01.25.01
Fixed bug in FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING

2018.01.28.01
Added carriage return and newline to the beginning of several CLI command responses
Add command mode command M - set command mode WPM (command mode now has a speed setting independent of regular keyer speed)

2018.01.29.01
Working on FEATURE_CLI_EXPERT_MENU and FEATURE_SD_CARD_SUPPORT
CLI status now shows speed potentiometer range

2018.02.01.01
Changed Toggle cw send echo CLI command from \: to \;
Deprecated FEATURE_CLI_EXPERT_MENU
Working on Extended CLI Commands /:
Added OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS
Extended CLI Commands
  eepromdump
  saveeeprom <filename>
  loadeeprom <filename>
  printlog
  clearlog
  ls <directory>
  cat <filename>
Added serial support for ARDUINO_AVR_LEONARDO_ETH

2018.02.01.02
Fixed bug with dit_buffer_off and dah_buffer_off not being initialized from eeprom settings at boot up (Thanks, YU7MW)

2018.02.05.01
Typo fix: ifdef defined(__AVR__) (Thanks, Glen, N1XF https://github.com/k3ng/k3ng_cw_keyer/issues/19)

2018.02.05.02
Fixed https://github.com/k3ng/k3ng_cw_keyer/issues/40 (Thanks, Glen, N1XF)

2018.02.07.01
Added support for 8 column LCD displays

2018.02.25.01
  FEATURE_SIDETONE_SWITCH switch line is now set for internal pullup so it won't cause a problem if left floating

2018.03.04.01
Changed wpm_command_mode from uint8_t to unsigned int
Fixed minor bug with junk left in paddle echo buffer after exiting command mode

2018.03.04.02
Added OPTION_DFROBOT_LCD_COMMAND_BUTTONS to use this board https://www.dfrobot.com/wiki/index.php/Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009) with FEATURE_BUTTONS

2018.03.08.01
Additional OPTION_DFROBOT_LCD_COMMAND_BUTTONS code and corresponding DEBUG_BUTTONS code

2018.03.08.02
Added OPTION_EXCLUDE_MILL_MODE

2018.03.11.01
New feature! FEATURE_SEQUENCER  Wiki: https://github.com/k3ng/k3ng_cw_keyer/wiki/383-Feature:-Sequencer
define sequencer_1_pin 0
define sequencer_2_pin 0
define sequencer_3_pin 0
define sequencer_4_pin 0
define sequencer_5_pin 0
define ptt_input_pin 0
define sequencer_pins_active_state HIGH
define sequencer_pins_inactive_state LOW
define ptt_line_active_state HIGH
define ptt_line_inactive_state LOW
define tx_key_line_active_state HIGH
define tx_key_line_inactive_state LOW
define ptt_input_pin_active_state LOW
define ptt_input_pin_inactive_state HIGH
New commands:
  \:pl <tx_number> <time_in_mS> - set PTT lead time
  \:pt <tx_number> <time_in_mS> - set PTT tail time
  \:pa <sequencer_number> <time_in_mS> - PTT active to sequencer active time
  \:pi <sequencer_number> <time_in_mS> - PTT inactive to sequencer inactive time
  \:timing - show all current timing settings
PTT lead and tail times are now stored in EEPROM and setable at runtime with extended commands \:pl and \:pt
Additional documentation: https://github.com/k3ng/k3ng_cw_keyer/wiki/225-Sidetone,-PTT,-and-TX-Key-Lines

2018.03.14.01
FEATURE_LCD_FABO_PCF8574 - Added support for FaBo LCD https://github.com/FaBoPlatform/FaBoLCD-PCF8574-Library

2018.03.16.01
Fixed compile error involving lcd_string (Thanks, Jeff, N0MII)

2018.03.23.01
Bug with automatic sending interruption fixed (Thanks, Larry, F6FVY)

2018.03.23.02
Fixed compilation bug with FEATURE_PTT_INTERLOCK when FEATURE_WINKEY_EMULATION was disabled

2018.03.24.01
Support for ARDUINO_MAPLE_MINI contributed by Marcin SP5IOU
HARDWARE_MAPLE_MINI hardware profile in keyer_hardware.h

2018.03.29.01
Support for ARDUINO_GENERIC_STM32F103C (Blue pill boards) contributed by Marcin SP5IOU
HARDWARE_GENERIC_STM32F103C hardware profile in keyer_hardware.h
How to deal with those boards with Arduino: https://www.techshopbd.com/uploads/product_document/STM32bluepillarduinoguide(1).pdf

2018.03.30.01
tx_inhibit and tx_pause pins implemented for use with contest station interlock controllers.  Documentation: https://github.com/k3ng/k3ng_cw_keyer/wiki/225-Sidetone,-PTT,-and-TX-Key-Lines#tx-inhibit-and-pause

2018.03.31.01
Now have OPTION_WINKEY_2_HOST_CLOSE_NO_SERIAL_PORT_RESET activated in feature files by default.

2018.04.07.01
Improved tx_pause when buffer or memory sending is paused mid-character

2018.04.15.01
Added HARDWARE_MORTTY

2018.04.16.01
Added OPTION_WINKEY_BLINK_PTT_ON_HOST_OPEN - visual cue that Winkey HOST OPEN has occurred

2018.04.20.01
FEATURE_WINKEY_EMULATION - Now clear manual ptt invoke upon host open, host close, and 0A commands

2018.04.22.01
Added OPTION_BLINK_HI_ON_PTT - on units that lack a sidetone speaker, this will blink HI on the PTT line on boot up
Fixed issue in keyer_pin_settings_mortty.h
Added TX Inhibit and TX Pause status in Command Line Interface Status \S command

2018.04.23.01
OPTION_KEEP_PTT_KEYED_WHEN_CHARS_BUFFERED - when Winkeyer Pause command is received, PTT is now de-asserted until Pause is cleared

2018.04.29.01
Deprecated OPTION_KEEP_PTT_KEYED_WHEN_CHARS_BUFFERED.  Winkey PINCONFIG PTT bit now sets / unsets ptt_buffer_hold_active
New CLI command \" to activate/deactivate PTT Hold Active When Characters Buffered functionality

2018.05.04.01
Winkey Emulation - minor addition to filtering of values echoed from send_char()

2018.05.05.01
Winkey Emulation - minor bug fix with handling of PTT tail time setting.  Also added support in Admin Get Values command to report PTT lead and tail time

2018.05.08.01
Fixed bug in CLI with multiple backspaces / backspaces exceeding number of characters in buffer locking up the keyer (Thanks, WF3T)

2018.05.10.01
Removed OPTION_N1MM_WINKEY_TAB_BUG_WORKAROUND.  The bug appears to be gone when testing a recent version of N1MM+.

2018.05.17.01
Updated to better handle STM32 board compilation (Thanks, Marcin, SP5IOU)

2018.05.28.01
Addressed potential issue with random pauses when using Winkey emulation with Writelog and Wintest: Changed check_for_dirty_configuration so it won't write to eeprom when there are characters buffered or PTT is active.  Also, Winkey Pinconfig command no longer sets config_dirty.

2018.05.31.01
Fixed design flaw with ptt_input_pin and manual PTT invoke commands not working independently (Thanks, Mek, SQ3RX)

2018.07.15.01
Added FEATURE_LCD_8BIT for controlling standard LCD displays with 8 data lines

2018.08.03.01
Fixed bug FEATURE_FARNSWORTH that was inadvertently introduced with command mode speed feature (Thanks, Jim, W5LA)

2018.08.04.01
Added additional checking of serial port while sending automatic CW in order to better interrupt character sending (Thanks, Max, NG7M)
Added OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW to disable this additional checking if desired or for troubleshooting

2018.08.13.01
More accurate Farnsworth timing; code contributed by Jim, W5LA

2018.08.21.01
Merged pull request https://github.com/k3ng/k3ng_cw_keyer/pull/50
HARDWARE_YAACWK contributed by Federico Pietro Briata, IZ1GLG <federico@briata.org>

2018.08.21.02
Different Farnsworth timing calculation.  Introduced farnsworth_timing_calibration in settings files.

2018.08.23.01
Fixed bug with Farnsworth timing not occurring during intercharacter time, however now overall WPM timing not right...

2018.08.25.01
More work on Farnsworth timing.  The timing appears correct now with PARIS testing, however using farnsworth_timing_calibration = 0.35
Now allow /M0 command to disable Farnsworth

2018.08.30.01
Think we got Farnsworth timing right now.  Thanks, Jim, W5LA !

2018.10.17.01
PTT lead and tail times, and sequencer times can now be set up to 65,535 mS
Updated help text with extended commands

2018.10.17.02
Fixed bug in K1EL Winkeyer Emulation paddle echo

2018.10.17.03
Improved potentiometer noise immunity, added potentiometer_reading_threshold in settings (Thanks, Wolf, DK7OB)
Fixed non-optimal potentiometer speed change comparison (Thanks, Wolf, DK7OB)

2018.10.19.01
Enabling OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW by default.  There appear to be lock ups caused by the serial port checking while sending functionality.  Investigating.

2018.10.21.01
Fixed Funtronics FK-10 LCD pin definitions that were broken in 2018.07.15.01 (Thanks, Jeff, N0MII )

2018.11.09.01
Fixed bug with K1EL Winkey emulation with Admin Get Values PTT Hang Time value returned (Thanks, Dariusz, SP2MKI)
Improved reporting of K1EL Winkey emulation PTT tail time and also now have tail time change dynamically with WPM changes to better follow specification
Fixed bug in Beacon Mode where dit and dah paddle would interrupt beacon code
Fixed bug with K1EL Winkey emulation with dead op watchdog enabling / disabling, and reporting (Thanks, Dariusz, SP2MKI)
K1EL Winkey emulation PINCONFIG and Winkeyer Mode commands now write to eeprom

2018.11.09.02
CLI Status now shows paddle and straight key echo state

2018.12.25.01
Fixed potential bug in sleep functionality timing

2019.02.05.01
Fixed bug in command mode K command when in ultimatic mode (Thanks, Rich)
Under Development: FEATURE_SINEWAVE_SIDETONE_USING_TIMER_1 and FEATURE_SINEWAVE_SIDETONE_USING_TIMER_3 in keyer_features_and_options_test.h

2019.02.05.02
Improvement in how K1EL Winkey emulation buffered speed command speed changing and clearing is handled

2019.04.06.01
OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW has been flipped and changed to OPTION_ENABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW_MAY_CAUSE_PROBLEMS
Fixed some compiler warnings

2019.04.07.01
Fixed additional compiler warnings

2019.04.27.01
FEATURE_DISPLAY - fixed issue with cpm label and FunKeyer.  (Thanks, Fred, VK2EFL)
Fixed bug introduced in version 2019.02.05.01 with not being able to switch between CLI and Winkey at startup using command button when FEATURE_COMMAND_LINE_INTERFACE and FEATURE_WINKEY_EMULATION both compiled in (Thanks, Dave, G8PGO)

2019.04.27.02
Merge of pull request 59 https://github.com/k3ng/k3ng_cw_keyer/pull/59 - HARDWARE_K5BCQ added.  (Thanks, woodjrx)

2019.04.27.03
Merge of pull request 51 https://github.com/k3ng/k3ng_cw_keyer/pull/51 - Yaacwk dev (Thanks, federicobriata)

2019.04.27.04
Merge of pull request 60 https://github.com/k3ng/k3ng_cw_keyer/pull/60 - Add support for generic PCF8574 based I2C display (Thanks, W6IPA)

2019.04.27.05
Fixed bug with I2C displays and \+ memory macros with pauses in between prosigned characters (Thanks, Fred, VK2EFL)

2019.04.28.01
Implemented asynchronous EEPROM writes

2019.04.29.01
Fixed bug introduced in 2019.04.27.05 with display of second prosign character (Thanks, Fred, VK2EFL)

2019.05.03.01
Merged pull request https://github.com/k3ng/k3ng_cw_keyer/pull/61  (Thanks, W6IPA)
Merged pull request https://github.com/k3ng/k3ng_cw_keyer/pull/62  (Thanks, W6IPA)
Merged pull request https://github.com/k3ng/k3ng_cw_keyer/pull/63  (Thanks, W6IPA)
New hardware profile: HARDWARE_MEGAKEYER  https://github.com/w6ipa/megakeyer    (Thanks, W6IPA) {needs documented}

2019.05.03.02
Added potentiometer_enable_pin {needs documented}
Merged pull request https://github.com/k3ng/k3ng_cw_keyer/pull/64  (Thanks, W6IPA)

2019.05.15.01
Merged pull request https://github.com/k3ng/k3ng_cw_keyer/pull/65 (Thanks, federicobriata); yaacwk FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
Merged pull request https://github.com/k3ng/k3ng_cw_keyer/pull/67 (Thanks, OK1CDJ); New hardware profile: HARDWARE_OPENCWKEYER_MK2 https://github.com/ok1cdj/OpenCWKeyerMK2
Merged pull request https://github.com/k3ng/k3ng_cw_keyer/pull/66 (Thanks, woodjrx); Last update for K5BCQ

2019.05.16.01
Fixed issue with factory reset functionality and asynchronous EEPROM write feature (Thanks, Fred, VK2EFL)
Relocated sidetone_hz_limit_low and sidetone_hz_limit_high setting from ino file to settings.h files (Thanks, Fred, VK2EFL) {needs documented}

2019.05.17.01
service_async_eeprom_write(): Changed EEPROM.write to EEPROM.update to lessen wear and tear on EEPROM and also reduce writing time.  (Each EEPROM.write = 3.3 mS)

2019.05.17.02
OPTION_DIRECT_PADDLE_PIN_READS_UNO (Thanks, Fred, VK2EFL) {needs documented}
OPTION_SAVE_MEMORY_NANOKEYER now does direct pin reads rather than digitalRead (Thanks, Fred, VK2EFL)
FEATURE_SD_CARD_SUPPORT - Rolled out SD card support to main keyer_features_and_options.h files {needs documented}

2019.05.28.01
FEATURE_WINKEY_EMULATION - fixed prosign lock up issue with Win-Test (Thanks, Bob, N6TV)

2019.05.29.01
FEATURE_WINKEY_EMULATION - fixed issues with paddle echo (Thanks, Bob, N6TV)
Settings - winkey_paddle_echo_buffer_decode_time_factor changed to winkey_paddle_echo_buffer_decode_timing_factor
Fixed keyer_pin_settings_nanokeyer_rev_*.h to include potentiometer_enable_pin (https://github.com/k3ng/k3ng_cw_keyer/issues/68) (Thanks, rificity)
convert_cw_number_to_ascii() was returning exclamation and not comma (Thanks, W6IPA)
FEATURE_AMERICAN_MORSE - fixed errant submitted change in send_char() (Thanks, Sverre, LA3ZA)

2019.06.18.01
Fixed bug with OPTION_SAVE_MEMORY_NANOKEYER and reading left (dit) paddle

2019.08.18.01
Fixed logic issue with W INKEY_CANCEL_BUFFERED_SPEED_COMMAND that may arise in Logger32

2019.08.18.02
Fixed issue with OPTION_BLINK_HI_ON_PTT (Thanks, Bob, WO6W)

2019.08.18.03
Merged pull request 75 https://github.com/k3ng/k3ng_cw_keyer/pull/75 - New HW addition - PIC32 Pinguino (Thanks, IZ3GME)

2019.08.18.04
Merged pull request 73 https://github.com/k3ng/k3ng_cw_keyer/pull/73 - code change to allow the speed input device encoder or potentiometer to change the command mode speed when in Command Mode (Thanks VK2EFL and KG6HUM)

2019.10.23.01
Added HARDWARE_MORTTY_REGULAR, HARDWARE_MORTTY_REGULAR_WITH_POTENTIOMETER, HARDWARE_MORTTY_SO2R, HARDWARE_MORTTY_SO2R_WITH_POTENTIOMETER
(Going to depricate Morrty preconfigurations directory shortly...)

2019.10.23.02
https://github.com/k3ng/k3ng_cw_keyer/issues/70  Thanks, SP9RQA

2019.10.23.03
Merged Pull 71 - re-factor analog button functions to support multiple button lines https://github.com/k3ng/k3ng_cw_keyer/pull/71 Thanks, W6IPA

2019.10.23.04
Fixed bug with contest wordspace in K1EL Winkey emulation setmode command.  Thanks, Paul K1XM
OPTION_WINKEY_SEND_VERSION_ON_HOST_CLOSE - Made this an option that is disabled by default.

2019.10.24.01
Fixed issue in winkey_unbuffered_speed_command
Fixed some compiler warnings
Fixed compiler redefine errors with Mortty hardware profiles

2019.11.07.01
Merged Pull Request 77 (https://github.com/k3ng/k3ng_cw_keyer/pull/77) Allow for 12 buttons + command - (Thanks, W6IPA)
Manually merged Pull Request 76 (https://github.com/k3ng/k3ng_cw_keyer/pull/76) Fix case sensitive include - (Thanks, Daniele, IU5HKX)

2019.11.07.02
Manual merge of contributed S02R code in progress.  Details later.  No functionality added or big fixes.

2019.11.08.01
Improved LCD screen refreshes; may improve performance with I2C LCD displays

2019.11.08.02
Fixed bug with character displaying during memory playing; bug introduced with improved LCD screen refreshes

2019.11.08.03
Completed manual merge of contributed code for HARDWARE_YCCC_SO2R_MINI .  Testing in progress.  (Thanks, Paul, K1XM)
Includes FEATURE_SO2R_BASE, FEATURE_SO2R_SWITCHES, FEATURE_SO2R_ANTENNA

2019.11.13.01
Various code compilation warning cleanups.  (Thanks, Paul, K1XM)

2019.12.07.01
Updated version number for multiple merged pull requests
  https://github.com/k3ng/k3ng_cw_keyer/pull/78 - Custom startup text - Thanks, Fred, VK2EFL
  https://github.com/k3ng/k3ng_cw_keyer/pull/80 - FK-11 Support - Thanks, Ben git1k2
  https://github.com/k3ng/k3ng_cw_keyer/pull/82 - Additional display info - Thanks, Fred, VK2EFL
Fixed errant text at line 7293 from merge of pull request 82

2019.12.07.02
Added OPTION_PERSONALIZED_STARTUP_SCREEN and custom_startup_field to feature and settings files of all hardware profiles (from pull request 78)

2019.12.07.03
Resolved conflict and merged pull request 83 - Paddle direction parameter change ( https://github.com/k3ng/k3ng_cw_keyer/pull/83/) Thanks, Fred, VK2EFL
Added OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION  to feature and settings files of all hardware profiles (OPTION from pull request 83)

2019.12.07.04
Resolved conflict and merged pull request 84 - Code to support the correct answer and wrong answer LEDs when progressive 5 character practice is invoked from the CLI ( https://github.com/k3ng/k3ng_cw_keyer/pull/84/) Thanks, Fred, VK2EFL

2019.12.07.05
Resolved conflict and merged pull request 85 - Command mode display memory ( https://github.com/k3ng/k3ng_cw_keyer/pull/85/) Thanks, Fred, VK2EFL
Github conflict resolution tool nuked about 2000 lines at the end on ino file.  Fixed that.  GRRRRRRR
Added OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE from pull request 85 to all hardware profile feature files
Updated all pin settings files so that correct_answer_led and wrong_answer_led are always defined

2019.12.16.01
Fixed bug with K1EL Winkeyer emulation cancel buffered speed command (Thanks, Drew, N7DA)

2019.12.17.01
Fixed bug with K1EL Winkeyer emulation with SO2R operation and errant CW being sent after switching radios

2020.02.04.01
Fixed bug with handling of K1EL Winkeyer emulation and handling of PINCONFIG bit 0 and PTT lead time (Thanks, Bill, K1GQ)

2020.02.04.02
Renabled serial port checking during CW sending specifically when automatic sending is happening
OPTION_ENABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW_MAY_CAUSE_PROBLEMS has been depricated
OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW added

2020.02.10.01
Added OPTION_WINKEY_PINCONFIG_PTT_CONTROLS_PTT_HOLD - Instead of normal K1EL Winkeyer PINCONFIG PTT bit 0 behavior (activating/deactivating PTT) have this bit control PTT hold when characters are buffered  (Thanks, Bill, K1GQ)

2020.02.13.01
Enabling OPTION_WINKEY_PINCONFIG_PTT_CONTROLS_PTT_HOLD by default with YCCC SO2R Mini hardware profile while we continue to troubleshoot issue involving PTT line, SO2R Mini footswitch, and K1EL Winkey emulation PINCONFIG PTT bit 0

2020.02.18.01
Fix for YCCC SO2R Mini issue involving PTT line, SO2R Mini footswitch, and K1EL Winkey emulation PINCONFIG PTT bit 0 (Thanks, K1GC and JH5GHM)

2020.02.18.02
Corrected fix for YCCC SO2R Mini issue involving PTT line, SO2R Mini footswitch, and K1EL Winkey emulation PINCONFIG PTT bit 0 (Thanks, K1GC and JH5GHM)
In K1EL Winkey emulation character echo is now sent after character CW is sent

2020.03.06.01
Merged pull request 92 - Fix button logic and add test https://github.com/k3ng/k3ng_cw_keyer/pull/92  (Thanks W6IPA)

2020.03.07.01
Added OPTION_WINKEY_PINCONFIG_PTT_CONTROLS_PTT_LINE - K1EL Winkeyer PTT setting activates/deactivates PTT line rather than controls buffered character PTT hold
TinyKeyer - OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW enabled by default for TinyKeyer (keyer_features_and_options_tinykeyer.h)

2020.03.07.02
Command Line Interface - Added \] to disable and enable PTT

2020.03.07.03
Fixed bug with \] and \U interaction (Thanks SV5FRI)
Added \] to serial help

2020.03.08.01
Fixed another bug with \] and \U interaction (Thanks SV5FRI)

2020.03.10.01
Merged pull request 94 - HARDWARE_GENERIC_STM32F103C - Fixed error : call of overloaded 'noTone(<anonymous enum>)' is ambiguous (Thanks 7m4mon)
Merged pull request 93 - Option to disable ultimatic to save space (Thanks, W6IPA)

2020.04.13.01
Fixed compilation error when LCD display is enabled without FEATURE_MEMORIES (Thanks Nigel M0NDE)

2020.04.14.01
Support for FlashAsEEPROM (Thanks Phil M0VSE)

2020.04.14.02
Support for FlashAsEEPROM, take two; ARDUINO_SAMD_VARIANT_COMPLIANCE support (Thanks Phil M0VSE)

2020.04.14.03
Support for FlashAsEEPROM, take three; ARDUINO_SAMD_VARIANT_COMPLIANCE support (Thanks Phil M0VSE)
Added DEBUG_EEPROM_READ_SETTINGS

2020.04.15.01
Support for FlashAsEEPROM, take four; ARDUINO_SAMD_VARIANT_COMPLIANCE support (Thanks Phil M0VSE)

2020.04.21.01
FEATURE_BEACON_SETTING
Command Line Interface: \_ Command - Beacon Mode at Boot Up Enable / Disable (requires FEATURE_BEACON_SETTING)
{Need to update wiki}

2020.04.24.01
Added to settings files: command_mode_acknowledgement_character 'E'

2020.04.25.01
-....- now echoes as dash "-"  (-...- is double dash / equals =)
Added Command Mode command: -  Enable / disable PTT Line

2020.04.26.01
memory_area_end is now automagically calculated at runtime and is no longer in settings files

2020.05.27.01
The Paddle Echo timing factor is now in eeprom and can be set with the \:pf ### extended CLI command ( https://github.com/k3ng/k3ng_cw_keyer/wiki/310-Feature:-Command-Line-Interface#cli-commands )
Autospace now has a configurable timing factor which can be set with the \:af ### extended CLI command
cw_echo_timing_factor setting has been deprecated and replaced with default_cw_echo_timing_factor
default_autospace_timing_factor setting created
memory_area_start is now automagically calculated
FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT created which has customizable CW feedback messages for most command mode commands
New settings in settings files for FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT ( https://github.com/k3ng/k3ng_cw_keyer/wiki/320-Feature:-Command-Mode#feedback )

2020.06.03.01
Fixed issue (hopefully) with memory_area_start automatic cacluation
Fixed issue of command_mode_acknowledgement_character not used for command acknowledgement when FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT is disabled

2020.06.03.02
In FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT added setting command_t_tune_mode for T (tune) command
Without FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT, setting command_mode_acknowledgement_character now is used for T (tune) command
Added setting FEATURE_ETHERNET_DNS {8,8,8,8} to FEATURE_WEB_SERVER and FEATURE_INTERNET_LINK (FEATURE_ETHERNET)

2020.06.03.03
Fixed issue with paddle interruption of stacked memories not being consistent (Thanks, Marcin SP5IOU)
\S memory macro now prints space on CLI and LCD display

2020.06.13.01
HARDWARE_GENERIC_STM32F103C - hard code EEPROM length to account for length() method not being available on this platform

2020.06.14.01
Added [ character as prosign AS for K1EL Winkeyer / N1MM+ compatibility (Thanks, Mark WH7W)

2020.07.01.01
Pull request 98 - Support for configuring the side tone line states - merged.  (https://github.com/k3ng/k3ng_cw_keyer/pull/98) (Thanks, Costin Stroie)
New settings: sidetone_line_active_state, sidetone_line_inactive_state

2020.07.04.01
Added OPTION_WINKEY_PROSIGN_COMPATIBILITY for additional character mappings to support K1EL Winkey emulation prosigns

2020.07.07.01
Merge of pull request 99 - Add support for using the NewTone library (https://github.com/k3ng/k3ng_cw_keyer/pull/99) (Thanks, Costin Stroie)
  Added support for configuring the sidetone line states HIGH and LOW.
  Use the NewTone instead of the standard tone library (~1k smaller code).
  Included the NewTone library by Tim Eckel <teckel@leethost.com>
Added FEATURE_WEB_SERVER and FEATURE_INTERNET_LINK to all features and options files

2020.07.17.01
Merge of pull request 100 - Command buttons 1-3 are not working with ARDUINO_GENERIC_STM32F103C (https://github.com/k3ng/k3ng_cw_keyer/pull/100) (Thanks, 7m4mon)

2020.07.26.01
Extended CLI commands now work with linefeed line terminations in addition to carriage return; Issue 101 (https://github.com/k3ng/k3ng_cw_keyer/issues/101), (Thanks, devcpu)

2020.08.17.01
FEATURE_WINKEY_EMULATION - fixed bug that adversly affected operation with Logger32

2020.08.21.01
FEATURE_COMMAND_BUTTONS is now called FEATURE_BUTTONS
Command mode is now broken out into its own feature, FEATURE_COMMAND_MODE

2020.08.22.01
Minor tweak in check_buttons()

2020.08.23.01
Added FEATURE_LCD_I2C_FDEBRABANDER
Added settings
  lcd_i2c_address_mathertel_PCF8574 0x27             // I2C address of display for FEATURE_LCD_MATHERTEL_PCF8574
  lcd_i2c_address_fdebrander_lcd 0x27                // I2C address of display for FEATURE_LCD_I2C_FDEBRABANDER
  lcd_i2c_address_ydv1_lcd 0x27                      // I2C address of display for FEATURE_LCD_YDv1
  lcd_i2c_address_sainsmart_lcd 0x27                 // I2C address of display for FEATURE_LCD_SAINSMART_I2C

2020.08.24.01
In pin settings files clarified the function of pins for FEATURE_CW_DECODER & OPTION_CW_DECODER_GOERTZEL_AUDIO_DETECTOR
Updated CW Decoder Wiki Page https://github.com/k3ng/k3ng_cw_keyer/wiki/385-Feature:-CW-Decoder

2020.08.28.02
Merged pull request 103 Change personalized startup operation ( https://github.com/k3ng/k3ng_cw_keyer/pull/103 )  (Thanks, VK2EFL)

2020.08.29.01
Implemented fix for memories not halting after a paddle press ( https://groups.io/g/radioartisan/message/13500 ) (Thanks, Gary, AF8A, for code )

2020.11.01.01
Fixed issues with FEATURE_WEB_SERVER and FEATURE_INTERNET_LINK when compiled with main features and settings files.

2021.01.24.01
Added the \:comp extended command to change keying compensation in the CLI
keying_compensation is now stored in eeprom
NOTE: increasing keying compensation above 35 mS at ~24 WPM causes wonkiness.  Probably need to add code to limit this value based on current WPM

2021.01.25.01
Keying compensation now displayed in \S status CLI command
Added Q command to FEATURE_COMMAND_MODE: Adjust keying compensation (left paddle = increase, right paddle = decrease)
FEATURE_COMMAND_LINE_INTERFACE \:comp command now displays warning if keying compensation setting is probably too high

2021.03.10.01
Merged Pull Request 108 https://github.com/k3ng/k3ng_cw_keyer/pull/108 from VK2EFL
  Adds to the \S CLI listing a display of the current tx's lead and tail times, plus the hang time (in wordspace units) plus the memory repeat time.
Merged Pull Request 109 https://github.com/k3ng/k3ng_cw_keyer/pull/109 from VK2EFL
  Adds the option of a memory repeat time between repeated playing of memory 1 when in beacon mode.
  Adds the option of having the PTT tail time added to the PTT at the end of each playing of memory 1 when in beacon mode.

2021.03.20.01
Updated version number for merging of Pull Request 110 https://github.com/k3ng/k3ng_cw_keyer/pull/110 from FrugalGuy (Ron, KO4RON)
  Adds FEATURE_LCD_BACKLIGHT_AUTO_DIM

2021.07.17.01
Added pins pin_sending_mode_automatic and pin_sending_mode_manual which go HIGH for automatic and manual sending modes

2021.12.16.01
Merged pull request 114 https://github.com/k3ng/k3ng_cw_keyer/pull/114 Support for STM32F1 boards - Thanks, 7m4mon
Merged pull request 107 https://github.com/k3ng/k3ng_cw_keyer/pull/107 Conditional section for LGT8FX8P EEPROM handling - Thanks, Jindřich Vavruška
Merged pull request 115 https://github.com/k3ng/k3ng_cw_keyer/pull/115 Redraw screen when exiting command mode - Thanks, VK2EFL
Merged pull request 117 https://github.com/k3ng/k3ng_cw_keyer/pull/117 Support for LCD type: I2C 1602 with backlight using TwiLiquidCrystal library - Thanks, billyf

2021.12.16.02
Merged pull request 86 https://github.com/k3ng/k3ng_cw_keyer/pull/86 SOS Prosign - Thanks, VK2EFL

2021.12.16.03
Merged pull request 118 https://github.com/k3ng/k3ng_cw_keyer/pull/118 OLED SSD1306 Support - Thanks, F5MDY

2021.12.17.01
Merged pull request 119 https://github.com/k3ng/k3ng_cw_keyer/pull/119/ Definable startup text (define HI_TEXT) - Thanks, ON6ZQ

2022.01.28.01
Added compiler macro to error out if paddle_left or paddle_right is defined as 0 (disabled)

2023.09.20.2100
Updates to allow compilation for Raspberry Pi Pico
Reports of my death have been greatly exagerrated

2023.09.21.0033
Additional updates for Raspberry Pi Pico compilation

2023.09.25.2346
Fixed HI_TEXT compilation warning

2023.09.27.2146
Experimenting with FEATURE_DUAL_MODE_KEYER_AND_TINYFSK

2023.09.28.1624
Put OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE back into all hardware profiles and enabling by default.  Not having it activated makes N1MM CQ Repeat paddle sending deactivation not work.

2023.09.28.2326
Added HARDWARE_MORTTY_PICO_OVER_USB

2023.09.29.1326
HARDWARE_MORTTY_PICO_OVER_USB pins file correction

2023.09.29.2043
FEATURE_WINKEY_EMULATION: corrected pot_full_scale_reading

2023.10.01.1410
FEATURE_DUAL_MODE_KEYER_AND_TINYFSK: eeprom should be working now
Raspberry Pi Pico now supported for everything except PS2 keyboard and sleep

2023.10.03.2351
More work on FEATURE_DUAL_MODE_KEYER_AND_TINYFSK

2023.10.04.2255
FEATURE_DUAL_MODE_KEYER_AND_TINYFSK: fixed issue with eeprom config load

2023.10.06.1053
FEATURE_INTERNET_LINK: Fix lock up related to initialization order (Thanks, SM3GSJ Roger)

2023.10.09.2308
FEATURE_WINKEY_EMULATION: Now expect three parameters from deprecated Paddle A2D command

2023.10.28.2304
FEATURE_AUDIOPWMSINEWAVE for Raspberry Pi Pico

2024.02.17.1400
Fixed issues found by swalberg ( https://github.com/k3ng/k3ng_cw_keyer/commit/e79277672f4c04dfeeef5bfb9c82e384b59f32c4#r134909644 ).  Thanks!

2024.02.17.1600
OPTION_WORDSWORTH_POLISH - Polish CW training text from Piotr, SP2BPD.  Thanks!
Straight key capability for CW training Piotr, SP2BPD.  Thanks!

2024.02.27.2314
HARDWARE_MORTTY_PICO_OVER_USB - Updated for Mortty v5

2024.02.28.0239
FEATURE_MORTTY_SPEEDPOT_BOOT_FUNCTION

2024.03.15.1354
FEATURE_WINKEY_EMULATION: Changed potentiometer behavior to work correctly with N1MM+ pot settings

2024.03.20.2239
tx_inhibit: unkey PTT when tx_inhibit goes active

qwerty

Documentation: https://github.com/k3ng/k3ng_cw_keyer/wiki

Support: https://groups.io/g/radioartisan  ( Please do not email K3NG directly for support.  Thanks )

YouTube Channel: https://www.youtube.com/channel/UC5o8UM1-heT5kJbwnJRkUYg

This code is currently maintained for and compiled with Arduino 1.8.x.  Your mileage may vary with other versions.

ATTENTION: LIBRARY FILES MUST BE PUT IN LIBRARIES DIRECTORIES AND NOT THE INO SKETCH DIRECTORY !!!!

FOR EXAMPLE:

K3NG_PS2Keyboard.h, K3NG_PS2Keyboard.cpp ----->  \Arduino\Sketchbook\libraries\K3NG_PS2Keyboard\
Goertz.h, Goertz.cpp ------------------------>   \Arduino\Sketchbook\libraries\Goertz\

"Make good code and share it with friends."

If you offer a hardware kit using this software, show your appreciation by sending the author a complimentary kit or a bottle of bourbon ;-)
