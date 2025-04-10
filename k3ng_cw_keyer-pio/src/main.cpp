#include <Arduino.h>

#define CODE_VERSION "2024.03.20.2239"

#define eeprom_magic_number 41 // you can change this number to have the unit re-initialize EEPROM

#include <stdio.h>
#include "keyer_hardware.h"

#include <config.h>
#include <misc.h>
#include <ptt.h>
#include <winkey.h>
#include <check.h>
#include <subroutines.h>

/*---------------------------------------------------------------------------------------------------------


 “What we do for ourselves dies with us. What we do for others and the world remains and is immortal.”

― Albert Pike


---------------------------------------------------------------------------------------------------------*/

void setup()
{

#if defined(FEATURE_DUAL_MODE_KEYER_AND_TINYFSK)
      check_run_tinyfsk_pin(); // read Mortty+v5 CW<->RTTY slide switch
      if (runTinyFSK)
      {
            TinyFSKsetup();
      }
      else
      {
#endif

#if defined(FEATURE_MORTTY_SPEEDPOT_BOOT_FUNCTION)
            mortty_speedpot_boot_function();
#endif

            initialize_pins();
            // initialize_serial_ports();        // Goody - this is available for testing startup issues
            // initialize_debug_startup();       // Goody - this is available for testing startup issues
            initialize_keyer_state();
            initialize_potentiometer();
            initialize_rotary_encoder();
            initialize_default_modes();
            initialize_watchdog();
            initialize_ethernet_variables();
#if defined(DEBUG_EEPROM_READ_SETTINGS)
            initialize_serial_ports();
#endif
            check_eeprom_for_initialization();
            check_for_beacon_mode();
            check_for_debug_modes();
            initialize_analog_button_array();
#if !defined(DEBUG_EEPROM_READ_SETTINGS)
            initialize_serial_ports();
#endif
            initialize_ps2_keyboard();
            initialize_usb();
            initialize_cw_keyboard();
            initialize_ethernet();
            initialize_udp();
            initialize_web_server();
            initialize_display();
            initialize_sd_card();
            initialize_debug_startup();

#if defined(FEATURE_DUAL_MODE_KEYER_AND_TINYFSK)
      } // if (runTinyFSK)
#endif

#if defined(FEATURE_MIDI)
      midi_setup();
#endif

      initialize_audiopwmsinewave();
}

// --------------------------------------------------------------------------------------------

void loop()
{

      // this is where the magic happens

#if defined(FEATURE_DUAL_MODE_KEYER_AND_TINYFSK)
      if (runTinyFSK)
      {
            TinyFSKloop();
      }
      else
      {
#endif

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
            wdt_reset();
#endif // OPTION_WATCHDOG_TIMER

#if defined(FEATURE_BEACON) && defined(FEATURE_MEMORIES)
            if (keyer_machine_mode == BEACON)
            {
                  delay(201); // an odd duration delay before we enter BEACON mode
#ifdef OPTION_BEACON_MODE_MEMORY_REPEAT_TIME
                  unsigned int time_to_delay = configuration.memory_repeat_time - configuration.ptt_tail_time[configuration.current_tx - 1];
#endif // OPTION_BEACON_MODE_MEMORY_REPEAT_TIME

                  while (keyer_machine_mode == BEACON)
                  { // if we're in beacon mode, just keep playing memory 1
                        if (!send_buffer_bytes)
                        {
                              add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
                              add_to_send_buffer(0);
                        }
                        service_send_buffer(PRINTCHAR);
#ifdef OPTION_BEACON_MODE_PTT_TAIL_TIME
                        delay(configuration.ptt_tail_time[configuration.current_tx - 1]);      // after memory 1 has played, this holds the PTT line active for the ptt tail time of the current tx
                        check_ptt_tail();                                                      // this resets things so that the ptt line will go high during the next playout
                        digitalWrite(configuration.current_ptt_line, ptt_line_inactive_state); // forces the ptt line of the current tx to be inactive
#endif                                                                                         // OPTION_BEACON_MODE_PTT_TAIL_TIME

#ifdef FEATURE_SERIAL
                        check_serial();
#endif

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
                        wdt_reset();
#endif // OPTION_WATCHDOG_TIMER

#ifdef OPTION_BEACON_MODE_MEMORY_REPEAT_TIME
                        if (time_to_delay > 0)
                              delay(time_to_delay); // this provdes a delay between successive playouts of the memory contents
#endif                                              // OPTION_BEACON_MODE_MEMORY_REPEAT_TIME
                  } // end while (keyer_machine_mode == BEACON)
            } // end if (keyer_machine_mode == BEACON)
#endif // defined(FEATURE_BEACON) && defined(FEATURE_MEMORIES)

#if defined(FEATURE_BEACON_SETTING)
            service_beacon();
#endif

            if (keyer_machine_mode == KEYER_NORMAL)
            {
#ifdef FEATURE_BUTTONS
                  check_buttons();
#endif
                  check_paddles();
                  service_dit_dah_buffers();

#if defined(FEATURE_SERIAL)
                  check_serial();
                  check_paddles();
                  service_dit_dah_buffers();
#endif

                  service_send_buffer(PRINTCHAR);
                  check_ptt_tail();

#ifdef FEATURE_POTENTIOMETER
                  check_potentiometer();
#endif

#ifdef FEATURE_ROTARY_ENCODER
                  check_rotary_encoder();
#endif

#ifdef FEATURE_PS2_KEYBOARD
                  check_ps2_keyboard();
#endif

#if defined(FEATURE_USB_KEYBOARD) || defined(FEATURE_USB_MOUSE)
                  service_usb();
#endif

                  check_for_dirty_configuration();

#ifdef FEATURE_DEAD_OP_WATCHDOG
                  check_for_dead_op();
#endif

#ifdef FEATURE_MEMORIES
                  check_memory_repeat();
#endif

#ifdef FEATURE_DISPLAY
                  check_paddles();
                  service_send_buffer(PRINTCHAR);
                  service_display();
#endif

#ifdef FEATURE_CW_DECODER
                  service_cw_decoder();
#endif

#ifdef FEATURE_LED_RING
                  update_led_ring();
#endif

#if defined(FEATURE_SLEEP) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
                  check_sleep();
#endif

#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
                  check_backlight();
#endif

#ifdef FEATURE_PTT_INTERLOCK
                  service_ptt_interlock();
#endif

#ifdef FEATURE_PADDLE_ECHO
                  service_paddle_echo();
#endif

#ifdef FEATURE_STRAIGHT_KEY
                  service_straight_key();
#endif

#if defined(FEATURE_COMPETITION_COMPRESSION_DETECTION)
                  service_competition_compression_detection();
#endif

#if defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE) && defined(FEATURE_WINKEY_EMULATION)
                  service_winkey_breakin();
#endif

#if defined(FEATURE_ETHERNET)
                  check_for_network_restart();
#if defined(FEATURE_WEB_SERVER)
                  service_web_server();
#endif
#if defined(FEATURE_INTERNET_LINK)
                  service_udp_send_buffer();
                  service_udp_receive();
                  service_internet_link_udp_receive_buffer();
#endif
#endif

#ifdef FEATURE_SIDETONE_SWITCH
                  check_sidetone_switch();
#endif

#if defined(FEATURE_4x4_KEYPAD) || defined(FEATURE_3x4_KEYPAD)
                  service_keypad();
#endif

#ifdef FEATURE_SD_CARD_SUPPORT
                  service_sd_card();
#endif

#ifdef FEATURE_SEQUENCER
                  check_sequencer_tail_time();
#endif

#ifdef FEATURE_SO2R_SWITCHES
                  so2r_switches();
#endif

                  service_async_eeprom_write();
            }

            service_sending_pins();

            service_millis_rollover();

#if defined(FEATURE_DUAL_MODE_KEYER_AND_TINYFSK)
      } // if (runTinyFSK){
#endif
}
