// Subroutines --------------------------------------------------------------------------------------------

// Are you a radio artisan ?

#if defined(FEATURE_MORTTY_SPEEDPOT_BOOT_FUNCTION)
void mortty_speedpot_boot_function()
{

  int releaseLevel = 1; // Mortty_v5 feature: releaseLevel display blink

  check_run_tinyfsk_pin();              // set which mode to run
  digitalWrite(pin_keyer_running, LOW); // both LEDs off
  digitalWrite(pin_rtty_running, LOW);  // both LEDs off
  if (analogRead(potentiometer) < 33)
  { // Speed Pot at or near MIN of 0
    // Mortty_v5 feature: blinking LEDs represent releaseLevel
    // blink both CW&RTTY LEDs simultaneously - user counts releaseLevel
    delay(1000);
    for (int loopCount = 1; loopCount <= releaseLevel; loopCount++)
    {
      digitalWrite(pin_keyer_running, HIGH);
      digitalWrite(pin_rtty_running, HIGH);
      delay(500);
      digitalWrite(pin_keyer_running, LOW);
      digitalWrite(pin_rtty_running, LOW);
      delay(500);
    }
  }
  else
  {
    if (analogRead(potentiometer) > 984)
    { // Speed Pot at or near MAX of 1023
      // Mortty_v5 feature: update firmware without disassembly
      // bootLoader mounts Mortty storage in File I/O mode
      delay(1000);
      digitalWrite(pin_keyer_running, HIGH);
      digitalWrite(pin_rtty_running, LOW);
      for (int loopCount = 1; loopCount < 50; loopCount++)
      {
        // notify the user - blink the rear=panel CW & RTTY LEDs
        digitalWrite(pin_keyer_running, (!digitalRead(pin_keyer_running))); // toggle
        digitalWrite(pin_rtty_running, (!digitalRead(pin_rtty_running)));   // toggle
        delay(100);
        if (analogRead(potentiometer) < 200)
        {                              // user turned speed pot - Speed Pot at or near MIN of 0
          rp2040.rebootToBootloader(); // reboot the RP2040 as File I/O mounted drive
        }
      }
    }
  }
  digitalWrite(pin_keyer_running, LOW); // both LEDs off
  digitalWrite(pin_rtty_running, LOW);  // both LEDs off
  delay(1000);
}
#endif

//-------------------------------------------------------------------------------------------------------

void initialize_audiopwmsinewave()
{

#if defined(FEATURE_AUDIOPWMSINEWAVE) // Raspberry Pi Pico hardware only
  // // SIDETONE -  define the PWM ouput pin, frequency and dutyCycle
  RPI_PICO_Timer timerSidetone(0);
  sidetoneCarrierHz = 40000; // frequency in Hz - 1000 = 1 kilohertz. even multiple of 20 slices
  sidetoneDutyCycle = 50000; // dutyCycle = real_dutyCycle * 1000, dutyCycle 50% = 50000;  USE 5?4? DIGITS: "0000" == zero
  PWM_Sidetone = new RP2040_PWM(pin_Sidetone, sidetoneCarrierHz, false);
  if (PWM_Sidetone)
  {
    //   Serial.print("Defining PWM_Sidetone OK, = ");
    //   Serial.print(sidetoneCarrierHz);
    //   Serial.println(" slices per each Sidetone audio cycle");
    PWM_Sidetone->setPWM_Int(pin_Sidetone, sidetoneCarrierHz, sidetoneDutyCycle);
    isSidetoneON = false; // launch with sideTone == OFF
  }
  else
  {
    Serial.println("Defining PWM_Sidetone instance FAIL");
  } // end of Sidetone definition ---------------
#endif
}

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_DUAL_MODE_KEYER_AND_TINYFSK)
void check_run_tinyfsk_pin()
{

  if (pin_run_tinyfsk)
  {
    if (pin_rtty_running)
    {
      pinMode(pin_rtty_running, OUTPUT);
    }
    if (pin_keyer_running)
    {
      pinMode(pin_keyer_running, OUTPUT);
    }
    pinMode(pin_run_tinyfsk, INPUT_PULLUP);
    if (digitalRead(pin_run_tinyfsk) == LOW)
    {
      runTinyFSK = 1;
      if (pin_rtty_running)
      {
        digitalWrite(pin_rtty_running, HIGH);
      }
      if (pin_keyer_running)
      {
        digitalWrite(pin_keyer_running, LOW);
      }
    }
    else
    {
      runTinyFSK = 0;
      if (pin_rtty_running)
      {
        digitalWrite(pin_rtty_running, LOW);
      }
      if (pin_keyer_running)
      {
        digitalWrite(pin_keyer_running, HIGH);
      }
    }
  }
}
#endif

//-------------------------------------------------------------------------------------------------------

void service_sending_pins()
{

  static byte last_sending_mode = 255;

  if ((sending_mode == AUTOMATIC_SENDING) || (sending_mode == AUTOMATIC_SENDING_INTERRUPTED))
  {
    if (last_sending_mode != AUTOMATIC_SENDING)
    {
      if (pin_sending_mode_automatic)
      {
        digitalWrite(pin_sending_mode_automatic, HIGH);
      }
      if (pin_sending_mode_manual)
      {
        digitalWrite(pin_sending_mode_manual, LOW);
      }
    }
  }
  if ((sending_mode == MANUAL_SENDING) || (sending_mode == UNDEFINED_SENDING))
  {
    if (last_sending_mode != MANUAL_SENDING)
    {
      if (pin_sending_mode_automatic)
      {
        digitalWrite(pin_sending_mode_automatic, LOW);
      }
      if (pin_sending_mode_manual)
      {
        digitalWrite(pin_sending_mode_manual, HIGH);
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_BEACON_SETTING)
void service_beacon()
{
  if ((configuration.beacon_mode_on_boot_up) && (!send_buffer_bytes))
  {
    serial_play_memory(0);
  }
}
#endif // FEATURE_BEACON_SETTING

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_MEMORIES
int memory_start(byte memory_number)
{
  return (memory_area_start + (memory_number * ((memory_area_end - memory_area_start) / number_of_memories)));
}
#endif

//---------------------------------------------------------------------

#ifdef FEATURE_MEMORIES
int memory_end(byte memory_number)
{
  return (memory_start(memory_number) - 1 + ((memory_area_end - memory_area_start) / number_of_memories));
}
#endif

//---------------------------------------------------------------------

void check_button0()
{
#ifdef FEATURE_BUTTONS
  if (analogbuttonread(0))
  {
    button0_buffer = 1;
  }
#endif
}

//---------------------------------------------------------------------

void command_speed_set(int wpm_set)
{
  if ((wpm_set > 0) && (wpm_set < 1000))
  {
    configuration.wpm_command_mode = wpm_set;
    config_dirty = 1;

#ifdef FEATURE_DISPLAY
    lcd_center_print_timed("Cmd Spd " + String(configuration.wpm_command_mode) + " wpm", 0, default_display_msg_delay);
#endif // FEATURE_DISPLAY
  } // end if
} // end command_speed_set

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_POTENTIOMETER
void check_potentiometer()
{
#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering check_potentiometer"));
#endif

  static unsigned long last_pot_check_time = 0;

  if ((configuration.pot_activated || potentiometer_always_on) && ((millis() - last_pot_check_time) > potentiometer_check_interval_ms))
  {
    last_pot_check_time = millis();
    if ((potentiometer_enable_pin) && (digitalRead(potentiometer_enable_pin) == HIGH))
    {
      return;
    }
    byte pot_value_wpm_read = pot_value_wpm();
    if (((abs(pot_value_wpm_read - last_pot_wpm_read) * 10) > (potentiometer_change_threshold * 10)))
    {
#ifdef DEBUG_POTENTIOMETER
      debug_serial_port->print(F("check_potentiometer: speed change: "));
      debug_serial_port->print(pot_value_wpm_read);
      debug_serial_port->print(F(" analog read: "));
      debug_serial_port->println(analogRead(potentiometer));
#endif
#ifdef FEATURE_WINKEY_EMULATION
      if (keyer_machine_mode == KEYER_COMMAND_MODE)
      {
        command_speed_set(pot_value_wpm_read);
      }
      else
      {
        if (!winkey_host_open)
        {
          speed_set(pot_value_wpm_read);
        }
      }
      if ((primary_serial_port_mode == SERIAL_WINKEY_EMULATION) && (winkey_host_open))
      {
        winkey_port_write(((pot_value_wpm_read - pot_wpm_low_value) | 128), 0);
        winkey_last_unbuffered_speed_wpm = configuration.wpm;
      }
#else
      if (keyer_machine_mode == KEYER_COMMAND_MODE)
      {
        command_speed_set(pot_value_wpm_read);
      }
      else
      {
        speed_set(pot_value_wpm_read);
      }
#endif
      last_pot_wpm_read = pot_value_wpm_read;
#ifdef FEATURE_SLEEP
      last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
      last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
    }
  }
}

#endif
//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_MEMORIES
byte play_memory(byte memory_number)
{

  unsigned int jump_back_to_y = 9999;
  byte jump_back_to_memory_number = 255;
  static byte prosign_flag = 0;
  static byte prosign_before_flag = 0;
  byte eeprom_byte_read = 0;
  byte pause_sending_buffer_backspace = 0;

  play_memory_prempt = 0;

#if defined(OPTION_PROSIGN_SUPPORT)
  byte eeprom_temp = 0;
  static char *prosign_temp = 0;
#endif

  if (memory_number > (number_of_memories - 1))
  {
    boop();
    return 0;
  }

#ifdef DEBUG_PLAY_MEMORY
  debug_serial_port->print(F("play_memory: called with memory_number:"));
  debug_serial_port->println(memory_number);
#endif

#ifdef FEATURE_MEMORY_MACROS
  byte eeprom_byte_read2;
  int z;
  byte input_error;
  byte delay_result = 0;
  int int_from_macro;
#endif // FEATURE_MEMORY_MACROS

  button0_buffer = 0;

  if (keyer_machine_mode == KEYER_NORMAL)
  {
#if defined(FEATURE_SERIAL)
#ifdef FEATURE_WINKEY_EMULATION
    if (primary_serial_port_mode != SERIAL_WINKEY_EMULATION)
    {
      primary_serial_port->println();
    }
#else
    primary_serial_port->println();
#endif

#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
    secondary_serial_port->println();
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
#endif
  }

  for (int y = (memory_start(memory_number)); (y < (memory_end(memory_number) + 1)); y++)
  {

    service_tx_inhibit_and_pause();

    if (keyer_machine_mode == KEYER_NORMAL)
    {
#ifdef FEATURE_POTENTIOMETER
      check_potentiometer();
#endif

#ifdef FEATURE_ROTARY_ENCODER
      check_rotary_encoder();
#endif // FEATURE_ROTARY_ENCODER

#ifdef FEATURE_PS2_KEYBOARD
      check_ps2_keyboard();
#endif

      check_button0();

#ifdef FEATURE_DISPLAY
      service_display();
#endif
    }

#if defined(FEATURE_SERIAL)
    check_serial();
#endif

    if ((play_memory_prempt == 0) && (pause_sending_buffer == 0))
    {

      pause_sending_buffer_backspace = 0;

      eeprom_byte_read = EEPROM.read(y);
      if (eeprom_byte_read < 255)
      {

#ifdef DEBUG_PLAY_MEMORY
        debug_serial_port->println(F("\n\nplay_memory:\r"));
        debug_serial_port->print(F("    Memory number:"));
        debug_serial_port->println(memory_number);
        debug_serial_port->print(F("    EEPROM location:"));
        debug_serial_port->println(y);
        debug_serial_port->print(F("    eeprom_byte_read:"));
        debug_serial_port->println(eeprom_byte_read);
#endif

        if (eeprom_byte_read != 92)
        { // do we have a backslash?
          if (keyer_machine_mode == KEYER_NORMAL)
          {

#if defined(OPTION_PROSIGN_SUPPORT)
            eeprom_temp = eeprom_byte_read;
            if ((eeprom_temp > PROSIGN_START) && (eeprom_temp < PROSIGN_END))
            {
              prosign_temp = convert_prosign(eeprom_temp);
            }
#endif // OPTION_PROSIGN_SUPPORT

#if defined(FEATURE_SERIAL)
#ifndef FEATURE_WINKEY_EMULATION
            if (!cw_send_echo_inhibit)
            {
#if defined(OPTION_PROSIGN_SUPPORT)
              if ((eeprom_temp > PROSIGN_START) && (eeprom_temp < PROSIGN_END))
              {
                primary_serial_port->print(prosign_temp[0]);
                primary_serial_port->print(prosign_temp[1]);
                if (strlen(prosign_temp) == 3)
                  primary_serial_port->print(prosign_temp[2]);
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
                secondary_serial_port->print(prosign_temp[0]);
                secondary_serial_port->print(prosign_temp[1]);
                if (strlen(prosign_temp) == 3)
                  secondary_serial_port->print(prosign_temp[2]);
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
              }
              else
              {
                primary_serial_port->write(eeprom_byte_read);
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
                secondary_serial_port->write(eeprom_byte_read);
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
              }
#else
              primary_serial_port->write(eeprom_byte_read);
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
              secondary_serial_port->write(eeprom_byte_read);
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
#endif // OPTION_PROSIGN_SUPPORT
            }
#else // FEATURE_WINKEY_EMULATION
            if (((primary_serial_port_mode == SERIAL_WINKEY_EMULATION) && (winkey_paddle_echo_activated) && (winkey_host_open)) || (primary_serial_port_mode != SERIAL_WINKEY_EMULATION))
            {

#if defined(OPTION_PROSIGN_SUPPORT)
              if ((eeprom_temp > PROSIGN_START) && (eeprom_temp < PROSIGN_END))
              {
                winkey_port_write(prosign_temp[0], 0);
                winkey_port_write(prosign_temp[1], 0);
              }
              else
              {
                winkey_port_write(eeprom_byte_read, 0);
              }
#else
              winkey_port_write(eeprom_byte_read, 0);
#endif // OPTION_PROSIGN_SUPPORT
            }
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
#if defined(OPTION_PROSIGN_SUPPORT)
            if ((eeprom_temp > PROSIGN_START) && (eeprom_temp < PROSIGN_END))
            {
              secondary_serial_port->print(prosign_temp[0]);
              secondary_serial_port->print(prosign_temp[1]);
            }
            else
            {
              secondary_serial_port->write(eeprom_byte_read);
            }
#else
            secondary_serial_port->write(eeprom_byte_read);
#endif // OPTION_PROSIGN_SUPPORT
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT

#endif // FEATURE_WINKEY_EMULATION
#endif // FEATURE_SERIAL

#ifdef FEATURE_DISPLAY
            if (lcd_send_echo)
            {
#if defined(OPTION_PROSIGN_SUPPORT)
              if ((eeprom_temp > PROSIGN_START) && (eeprom_temp < PROSIGN_END))
              {
                display_scroll_print_char(prosign_temp[0]);
                display_scroll_print_char(prosign_temp[1]);
              }
              else
              {
                if (prosign_flag)
                {
                  display_scroll_print_char(eeprom_byte_read);
                  display_scroll_print_char(EEPROM.read(y + 1));
                  prosign_before_flag = 1;
                }
                else
                {
                  if (prosign_before_flag)
                  {
                    prosign_before_flag = 0;
                  }
                  else
                  {
                    display_scroll_print_char(eeprom_byte_read);
                  }
                }
              }
#else
              if (prosign_flag)
              {
                display_scroll_print_char(eeprom_byte_read);
                display_scroll_print_char(EEPROM.read(y + 1));
                prosign_before_flag = 1;
              }
              else
              {
                if (prosign_before_flag)
                {
                  prosign_before_flag = 0;
                }
                else
                {
                  display_scroll_print_char(eeprom_byte_read);
                }
              }
#endif
              service_display();
            }
#endif // FEATURE_DISPLAY
          }
          if (prosign_flag)
          {
            send_char(eeprom_byte_read, OMIT_LETTERSPACE);
            prosign_flag = 0;
          }
          else
          {
            send_char(eeprom_byte_read, KEYER_NORMAL); // no - play the character
          }
        }
        else
        {      // yes - we have a backslash command ("macro")
          y++; // get the next memory byte
#ifdef FEATURE_MEMORY_MACROS
          if (y < (memory_end(memory_number) + 1))
          {
            eeprom_byte_read = EEPROM.read(y); // memory macros (backslash commands)
            switch (eeprom_byte_read)
            {
            case 48: // 0 - jump to memory 10
              eeprom_byte_read = 58;
            case 49: // 1 - jump to memory 1
            case 50: // 2 - jump to memory 2
            case 51: // 3 - jump to memory 3
            case 52: // 4 - jump to memory 4
            case 53: // 5 - jump to memory 5
            case 54: // 6 - jump to memory 6
            case 55: // 7 - jump to memory 7
            case 56: // 8 - jump to memory 8
            case 57: // 9 - jump to memory 9
              if (number_of_memories > (eeprom_byte_read - 49))
              {
                memory_number = (eeprom_byte_read - 49);
                y = ((memory_start(memory_number)) - 1);
                if (keyer_machine_mode == KEYER_NORMAL)
                {
                  primary_serial_port->println();
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
                  secondary_serial_port->println();
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
                }
              }
              break;
            case 'I': // insert memory #
              y++;
              if (y < (memory_end(memory_number) + 1))
              { // get the next byte
                eeprom_byte_read = EEPROM.read(y);
                if (number_of_memories > (eeprom_byte_read - 49))
                {
                  jump_back_to_y = y;
                  jump_back_to_memory_number = memory_number;
                  memory_number = (eeprom_byte_read - 49);
                  y = ((memory_start(memory_number)) - 1);
                  if (keyer_machine_mode == KEYER_NORMAL)
                  {
                    primary_serial_port->println();
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
                    secondary_serial_port->println();
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
                  }
                }
              }
              break;

            case 'S': // insert space
              send_char(' ', KEYER_NORMAL);
              primary_serial_port->print(' ');
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
              secondary_serial_port->print(' ');
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
#ifdef FEATURE_DISPLAY
              if (lcd_send_echo)
              {
                display_scroll_print_char(' ');
              }
#endif // FEATURE_DISPLAY
              break;

            case 'X': // X - switch transmitter
              y++;
              if (y < (memory_end(memory_number) + 1))
              {
                eeprom_byte_read2 = EEPROM.read(y);
                if ((eeprom_byte_read2 > 48) && (eeprom_byte_read2 < 55))
                {
                  switch (eeprom_byte_read2)
                  {
                  case 49:
                    switch_to_tx_silent(1);
                    break;
                  case 50:
                    if ((ptt_tx_2) || (tx_key_line_2))
                    {
                      switch_to_tx_silent(2);
                    }
                    break;
                  case 51:
                    if ((ptt_tx_3) || (tx_key_line_3))
                    {
                      switch_to_tx_silent(3);
                    }
                    break;
                  case 52:
                    if ((ptt_tx_4) || (tx_key_line_4))
                    {
                      switch_to_tx_silent(4);
                    }
                    break;
                  case 53:
                    if ((ptt_tx_5) || (tx_key_line_5))
                    {
                      switch_to_tx_silent(5);
                    }
                    break;
                  case 54:
                    if ((ptt_tx_6) || (tx_key_line_6))
                    {
                      switch_to_tx_silent(6);
                    }
                    break;
                  }
                }
              }
              break; // case X

            case 'C': // C - play serial number with cut numbers T and N, then increment
              send_serial_number(1, 1, 0);
              break;

            case 'D': // D - delay for ### seconds
              int_from_macro = 0;
              z = 100;
              input_error = 0;
              for (int x = 1; x < 4; x++)
              {
                y++;
                if (y < (memory_end(memory_number) + 1))
                {
                  eeprom_byte_read2 = EEPROM.read(y);
                  if ((eeprom_byte_read2 > 47) && (eeprom_byte_read2 < 58))
                  { // ascii 48-57 = "0" - "9")
                    int_from_macro = int_from_macro + ((eeprom_byte_read2 - 48) * z);
                    z = z / 10;
                  }
                  else
                  {
                    x = 4; // error - exit
                    input_error = 1;
                    y--; // go back one so we can at least play the errant character
                  }
                }
                else
                {
                  x = 4;
                  input_error = 1;
                }
              }
              if (input_error != 1)
              { // do the delay
                delay_result = memory_nonblocking_delay(int_from_macro * 1000);
              }
              if (delay_result)
              { // if a paddle or button0 was hit during the delay, exit
                return 0;
              }
              break; // case D

            case 'E': // E - play serial number, then increment
              send_serial_number(0, 1, 0);
              break;

            case 'F': // F - change sidetone frequency
              int_from_macro = 0;
              z = 1000;
              input_error = 0;
              for (int x = 1; x < 5; x++)
              {
                y++;
                if (y < (memory_end(memory_number) + 1))
                {
                  eeprom_byte_read2 = EEPROM.read(y);
                  if ((eeprom_byte_read2 > 47) && (eeprom_byte_read2 < 58))
                  { // ascii 48-57 = "0" - "9")
                    int_from_macro = int_from_macro + ((eeprom_byte_read2 - 48) * z);
                    z = z / 10;
                  }
                  else
                  {
                    x = 5; // error - exit
                    input_error = 1;
                    y--; // go back one so we can at least play the errant character
                  }
                }
                else
                {
                  x = 4;
                  input_error = 1;
                }
              }
              if ((input_error != 1) && (int_from_macro > sidetone_hz_limit_low) && (int_from_macro < sidetone_hz_limit_high))
              {
                configuration.hz_sidetone = int_from_macro;
#ifdef FEATURE_DISPLAY
                if (LCD_COLUMNS < 9)
                  lcd_center_print_timed(String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
                else
                  lcd_center_print_timed("Sidetone " + String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
#endif // FEATURE_DISPLAY
              }
              break;
              if ((input_error != 1) && (int_from_macro > sidetone_hz_limit_low) && (int_from_macro < sidetone_hz_limit_high))
              {
                configuration.hz_sidetone = int_from_macro;
              }
              break;

            case 'H': // H - Switch to Hell
              char_send_mode = HELL;
              break;

            case 'L': // L - Switch to CW
              char_send_mode = CW;
              break;

            case 'N': // N - decrement serial number (do not play)
              serial_number--;
              break;

            case '+': // + - Prosign
              prosign_flag = 1;
              break;

            case 'Q': // Q - QRSS mode and set dit length to ##
              int_from_macro = 0;
              z = 10;
              input_error = 0;
              for (int x = 1; x < 3; x++)
              {
                y++;
                if (y < (memory_end(memory_number) + 1))
                {
                  eeprom_byte_read2 = EEPROM.read(y);
                  if ((eeprom_byte_read2 > 47) && (eeprom_byte_read2 < 58))
                  { // ascii 48-57 = "0" - "9")
                    int_from_macro = int_from_macro + ((eeprom_byte_read2 - 48) * z);
                    z = z / 10;
                  }
                  else
                  {
                    x = 4; // error - exit
                    input_error = 1;
                    y--; // go back one so we can at least play the errant character
                  }
                }
                else
                {
                  x = 4;
                  input_error = 1;
                }
              }
              if (input_error == 0)
              {
                speed_mode = SPEED_QRSS;
                qrss_dit_length = int_from_macro;
                // calculate_element_length();
              }
              break; // case Q

            case 'R': // R - regular speed mode
              speed_mode = SPEED_NORMAL;
              // calculate_element_length();
              break;

            case 'T': // T - transmit for ### seconds
              int_from_macro = 0;
              z = 100;
              input_error = 0;
              for (int x = 1; x < 4; x++)
              {
                y++;
                if (y < (memory_end(memory_number) + 1))
                {
                  eeprom_byte_read2 = EEPROM.read(y);
                  if ((eeprom_byte_read2 > 47) && (eeprom_byte_read2 < 58))
                  { // ascii 48-57 = "0" - "9")
                    int_from_macro = int_from_macro + ((eeprom_byte_read2 - 48) * z);
                    z = z / 10;
                  }
                  else
                  {
                    x = 4; // error - exit
                    input_error = 1;
                    y--; // go back one so we can at least play the errant character
                  }
                }
                else
                {
                  x = 4;
                  input_error = 1;
                }
              }
              sending_mode = AUTOMATIC_SENDING;
              if (input_error != 1)
              { // go ahead and transmit
                tx_and_sidetone_key(1);
                delay_result = memory_nonblocking_delay(int_from_macro * 1000);
                tx_and_sidetone_key(0);
              }
              if (delay_result)
              { // if a paddle or button0 was hit during the delay, exit
                return 0;
              }
              break; // case T

            case 'U': // U - turn on PTT
              manual_ptt_invoke = 1;
              ptt_key();
              break;

            case 'V': // V - turn off PTT
              manual_ptt_invoke = 0;
              ptt_unkey();
              break;

            case 'W': // W - change speed to ### WPM
              int_from_macro = 0;
              z = 100;
              input_error = 0;
              for (int x = 1; x < 4; x++)
              {
                y++;
                if (y < (memory_end(memory_number) + 1))
                {
                  eeprom_byte_read2 = EEPROM.read(y);
                  if ((eeprom_byte_read2 > 47) && (eeprom_byte_read2 < 58))
                  { // ascii 48-57 = "0" - "9")
                    int_from_macro = int_from_macro + ((eeprom_byte_read2 - 48) * z);
                    z = z / 10;
                  }
                  else
                  {
                    x = 4; // error - exit
                    input_error = 1;
                    y--; // go back one so we can at least play the errant character
                  }
                }
                else
                {
                  x = 4;
                  input_error = 1;
                }
              }
              if (input_error != 1)
              {
                speed_mode = SPEED_NORMAL;
                speed_set(int_from_macro);
              }
              break; // case W

            case 'Y': // Y - Relative WPM change (positive)
              y++;
              if ((y < (memory_end(memory_number) + 1)) && (speed_mode == SPEED_NORMAL))
              {
                eeprom_byte_read2 = EEPROM.read(y);
                if ((eeprom_byte_read2 > 47) && (eeprom_byte_read2 < 58))
                { // ascii 48-57 = "0" - "9")
                  speed_set(configuration.wpm + eeprom_byte_read2 - 48);
                }
                else
                {
                  y--; // go back one so we can at least play the errant character
                }
              }
              else
              {
              }
              break; // case Y

            case 'Z': // Z - Relative WPM change (positive)
              y++;
              if ((y < (memory_end(memory_number) + 1)) && (speed_mode == SPEED_NORMAL))
              {
                eeprom_byte_read2 = EEPROM.read(y);
                if ((eeprom_byte_read2 > 47) && (eeprom_byte_read2 < 58))
                { // ascii 48-57 = "0" - "9")
                  speed_set(configuration.wpm - (eeprom_byte_read2 - 48));
                }
                else
                {
                  y--; // go back one so we can at least play the errant character
                }
              }
              else
              {
              }
              break; // case Z
            }
          }
#endif // FEATURE_MEMORY_MACROS
        }
        if (keyer_machine_mode != BEACON)
        {
#ifdef FEATURE_STRAIGHT_KEY
          if ((dit_buffer) || (dah_buffer) || (button0_buffer) || (digitalRead(pin_straight_key) == STRAIGHT_KEY_ACTIVE_STATE))
          { // exit if the paddle or button0 was hit
            dit_buffer = 0;
            dah_buffer = 0;
            button0_buffer = 0;
            repeat_memory = 255;
#ifdef FEATURE_BUTTONS
            while (analogbuttonread(0))
            {
            }
#endif
            return 0;
          }
#else // FEATURE_STRAIGHT_KEY
          if ((dit_buffer) || (dah_buffer) || (button0_buffer))
          { // exit if the paddle or button0 was hit
            dit_buffer = 0;
            dah_buffer = 0;
            button0_buffer = 0;
            repeat_memory = 255;
#ifdef FEATURE_BUTTONS
            while (analogbuttonread(0))
            {
            }
#endif
            return 0;
          }
#endif // FEATURE_STRAIGHT_KEY
        }
      }
      else
      {
        if (y == (memory_start(memory_number)))
        { // memory is totally empty - do a boop
          repeat_memory = 255;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("MemEmpty", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Memory empty", 0, default_display_msg_delay);
          }
#else
          boop();
#endif
        }

        // if we had an inserted memory, jump back to the original one
        if (/*(y== (memory_end(memory_number)+1)) &&*/ (jump_back_to_y < 9999) && (jump_back_to_memory_number < 255))
        {
#ifdef DEBUG_PLAY_MEMORY
          debug_serial_port->print(F("\nplay_memory: jump back to original memory:"));
          debug_serial_port->println(jump_back_to_memory_number);
#endif
          y = jump_back_to_y;
          memory_number = jump_back_to_memory_number;
          jump_back_to_y = 9999;
          jump_back_to_memory_number = 255;
        }
        else
        {

          return 0;
        }
      }
    }
    else
    {
      if (pause_sending_buffer == 0)
      {
        y = (memory_end(memory_number) + 1); // we got a play_memory_prempt flag, exit out
      }
      else
      {
        y--; // we're in a pause mode, so sit and spin awhile
        if ((y > (memory_start(memory_number)) && (!pause_sending_buffer_backspace)))
        {
          y--;
          pause_sending_buffer_backspace = 1;
        }
        check_ptt_tail();
#if defined(FEATURE_SEQUENCER)
        check_sequencer_tail_time();
#endif
      }
    }

    last_memory_repeat_time = millis();
#ifdef DEBUG_PLAY_MEMORY
    debug_serial_port->println(F("\nplay_memory: reset last_memory_repeat_time"));
    debug_serial_port->print("y: ");
    debug_serial_port->print(y);
    debug_serial_port->print("\tmemory_number: ");
    debug_serial_port->print(memory_number);
    debug_serial_port->print("\tmemory_end: ");
    debug_serial_port->print(memory_end(memory_number));
    debug_serial_port->print("\tjump_back_to_y: ");
    debug_serial_port->print(jump_back_to_y);
    debug_serial_port->print("\tjump_back_to_memory_number: ");
    debug_serial_port->println(jump_back_to_memory_number);
#endif

    // if we had an inserted memory, jump back to the original one
    /*
    if ((y== (memory_end(memory_number)+1)) && (jump_back_to_y < 99999) && (jump_back_to_memory_number < 255)) {
      primary_serial_port->print(F("\nplay_memory: jump back to original memory:"));
      primary_serial_port->println(jump_back_to_memory_number);
      y = jump_back_to_y;
      memory_number = jump_back_to_memory_number;
      jump_back_to_y = 99999;
      jump_back_to_memory_number = 255;
    }
    */

  } // for (int y = (memory_start(memory_number)); (y < (memory_end(memory_number)+1)); y++)
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_COMPETITION_COMPRESSION_DETECTION)
void service_competition_compression_detection()
{

  static byte compression_detection_indicator_on = 0;
  static unsigned long last_compression_check_time = 0;

  if ((millis() - last_compression_check_time) > 1000)
  {
    float time_average = 0;
    if (time_array_index == COMPETITION_COMPRESSION_DETECTION_ARRAY_SIZE)
    {
      for (int i = 0; i < COMPETITION_COMPRESSION_DETECTION_ARRAY_SIZE; i++)
      {
        time_average = time_average + time_array[i];
      }
      time_average = time_average / COMPETITION_COMPRESSION_DETECTION_ARRAY_SIZE;
      if (time_average < ((1200 / configuration.wpm) * COMPETITION_COMPRESSION_DETECTION_AVERAGE_ALARM_THRESHOLD))
      {
        if (!compression_detection_indicator_on)
        {
          compression_detection_indicator_on = 1;
          digitalWrite(compression_detection_pin, HIGH);
#if defined(DEBUG_FEATURE_COMPETITION_COMPRESSION_DETECTION)
          debug_serial_port->print("service_competition_compression_detection: time_array: ");
          for (int i = 0; i < COMPETITION_COMPRESSION_DETECTION_ARRAY_SIZE; i++)
          {
            debug_serial_port->print(time_array[i]);
            debug_serial_port->print(" ");
          }
          debug_serial_port->print("\n\rservice_competition_compression_detection: COMPRESSION DETECTION ON  average: ");
          debug_serial_port->println(time_average);
#endif
        }
      }
      else
      {
        if (compression_detection_indicator_on)
        {
          compression_detection_indicator_on = 0;
          digitalWrite(compression_detection_pin, LOW);
#if defined(DEBUG_FEATURE_COMPETITION_COMPRESSION_DETECTION)
          debug_serial_port->print("service_competition_compression_detection: time_array: ");
          for (int i = 0; i < COMPETITION_COMPRESSION_DETECTION_ARRAY_SIZE; i++)
          {
            debug_serial_port->print(time_array[i]);
            debug_serial_port->print(" ");
          }
          debug_serial_port->print("\n\rservice_competition_compression_detection: COMPRESSION DETECTION OFF  average: ");
          debug_serial_port->println(time_average);
#endif
        }
      }
    }
    last_compression_check_time = millis();
  }
}
#endif // FEATURE_COMPETITION_COMPRESSION_DETECTION

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_4x4_KEYPAD) || defined(FEATURE_3x4_KEYPAD)
void service_keypad()
{

  // Code contributed by Jack, W0XR

  char key = kpd.getKey();

  if (key)
  { // Check for a valid key.

#if defined(DEBUG_KEYPAD_SERIAL)
    debug_serial_port->print("service_keypad: key:");
    debug_serial_port->println(key);
#endif

    switch (key)
    {
    case '1':
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(mem1);
      // play_memory(mem1); //MEMORY 1
      break;
    case '2':
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(mem2);
      // play_memory(mem2); //MEMORY 2
      break;
    case '3':
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(mem3);
      // play_memory(mem3); //MEMORY 3
      break;
    case '4':
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(mem4);
      // play_memory(mem4); //MEMORY 4
      break;
    case '5':
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(mem5);
      // play_memory(mem5); //MEMORY 5
      break;
    case '6':
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(mem6);
      // play_memory(mem6); //MEMORY 6
      break;
    case '7':
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(mem7);
      // play_memory(mem7); //MEMORY 7
      break;
    case '8':
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(mem8);
      // play_memory(mem8); //MEMORY 8
      break;
    case '9':
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(mem9);
      // play_memory(mem9); //MEMORY 9
      break;
    case '0':
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(mem10);
      // play_memory(mem10); //MEMORY 10
      break;
    case '#':
      beep_boop();
      break;
    case '*':
      beep_boop();
      break;
    case 'A':
      beep_boop();
      break;
    case 'B':
      beep_boop();
      break;
#ifdef FEATURE_COMMAND_MODE
    case 'C':
      command_mode();
      break;
#endif
    case 'D':
      beep_boop();
      break;
    }

  } // if(key)

} // service_keypad()
#endif // defined(FEATURE_4x4_KEYPAD) || defined(FEATURE_3x4_KEYPAD)

//-------------------------------------------------------------------------------------------------------

#ifdef DEBUG_MEMORYCHECK
void memorycheck()
{
  void *HP = malloc(4);
  if (HP)
    free(HP);

  unsigned long free = (unsigned long)SP - (unsigned long)HP;

  //  port_to_use->print("Heap=");
  //  port_to_use->println((unsigned long)HP,HEX);
  //  port_to_use->print("Stack=");
  //  port_to_use->println((unsigned long)SP,HEX);
  //  port_to_use->print("Free Memory = ");
  //  port_to_use->print((unsigned long)free,HEX);
  //  port_to_use->print("  ");

  // if (free > 2048) {
  //   free = 0;
  // }

  if (primary_serial_port_mode == SERIAL_CLI)
  {
    port_to_use->print((unsigned long)free, DEC);
    port_to_use->println(F(" bytes free"));
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_STRAIGHT_KEY)
long service_straight_key()
{

  long decode_character = 0;
  static byte last_straight_key_state = 0;

  if (digitalRead(pin_straight_key) == STRAIGHT_KEY_ACTIVE_STATE)
  {
    if (!last_straight_key_state)
    {
      sending_mode = MANUAL_SENDING;
      tx_and_sidetone_key(1);
      last_straight_key_state = 1;
#ifdef FEATURE_MEMORIES
      clear_send_buffer();
      repeat_memory = 255;
#endif
    }
  }
  else
  {
    if (last_straight_key_state)
    {
      sending_mode = MANUAL_SENDING;
      tx_and_sidetone_key(0);
      last_straight_key_state = 0;
    }
  }

#if defined(FEATURE_STRAIGHT_KEY_DECODE)
  static unsigned long last_transition_time = 0;
  static unsigned long last_decode_time = 0;
  static byte last_state = 0;
  static int decode_elements[16]; // this stores received element lengths in mS (positive = tone, minus = no tone)
  static byte decode_element_pointer = 0;
  static float decode_element_tone_average = 0;
  static float decode_element_no_tone_average = 0;
  static int no_tone_count = 0;
  static int tone_count = 0;
  byte decode_it_flag = 0;
  int element_duration = 0;
  static float decoder_wpm = configuration.wpm;
  static byte space_sent = 0;
#if defined(FEATURE_COMMAND_LINE_INTERFACE) && defined(FEATURE_STRAIGHT_KEY_ECHO)
  static byte screen_column = 0;
  static int last_printed_decoder_wpm = 0;
#endif

#if defined(FEATURE_CW_COMPUTER_KEYBOARD)
  static byte cw_keyboard_no_space = 0;
  char cw_keyboard_character_to_send;
  static byte cw_keyboard_backspace_flag = 0;
#endif // defined(FEATURE_CW_COMPUTER_KEYBOARD)

  if (last_transition_time == 0)
  {
    if (last_straight_key_state == 1)
    { // is this our first tone?
      last_transition_time = millis();
      last_state = 1;
#ifdef FEATURE_SLEEP
      last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
      last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
    }
    else
    {

      if ((last_decode_time > 0) && (!space_sent) && ((millis() - last_decode_time) > ((1200 / decoder_wpm) * CW_DECODER_SPACE_PRINT_THRESH)))
      { // should we send a space?
#if defined(FEATURE_SERIAL) && defined(FEATURE_STRAIGHT_KEY_ECHO)
#ifdef FEATURE_COMMAND_LINE_INTERFACE
        primary_serial_port->write(32);
        screen_column++;
#endif // FEATURE_COMMAND_LINE_INTERFACE
#endif // FEATURE_SERIAL
#ifdef FEATURE_DISPLAY
        display_scroll_print_char(' ');
#endif // FEATURE_DISPLAY
        space_sent = 1;

#if defined(FEATURE_CW_COMPUTER_KEYBOARD)
        if (!cw_keyboard_no_space)
        {
          Keyboard.write(' ');
#ifdef DEBUG_CW_COMPUTER_KEYBOARD
          debug_serial_port->println("service_straight_key: Keyboard.write: <space>");
#endif // DEBUG_CW_COMPUTER_KEYBOARD
        }
        cw_keyboard_no_space = 0;
#endif // defined(FEATURE_CW_COMPUTER_KEYBOARD)

      } // should we send a space?
    }
  }
  else
  {
    if (last_straight_key_state != last_state)
    {
      // we have a transition
      element_duration = millis() - last_transition_time;
      if (element_duration > CW_DECODER_NOISE_FILTER)
      { // filter out noise
        if (last_straight_key_state == 1)
        {                                                                    // we have a tone
          decode_elements[decode_element_pointer] = (-1 * element_duration); // the last element was a space, so make it negative
          no_tone_count++;
          if (decode_element_no_tone_average == 0)
          {
            decode_element_no_tone_average = element_duration;
          }
          else
          {
            decode_element_no_tone_average = (element_duration + decode_element_no_tone_average) / 2;
          }
          decode_element_pointer++;
          last_state = 1;
        }
        else
        {                                                             // we have no tone
          decode_elements[decode_element_pointer] = element_duration; // the last element was a tone, so make it positive
          tone_count++;
          if (decode_element_tone_average == 0)
          {
            decode_element_tone_average = element_duration;
          }
          else
          {
            decode_element_tone_average = (element_duration + decode_element_tone_average) / 2;
          }
          last_state = 0;
          decode_element_pointer++;
        }
        last_transition_time = millis();
        if (decode_element_pointer == 16)
        {
          decode_it_flag = 1;
        } // if we've filled up the array, go ahead and decode it
      }
    }
    else
    {
      // no transition
      element_duration = millis() - last_transition_time;
      if (last_state == 0)
      {
        // we're still high (no tone) - have we reached character space yet?
        // if ((element_duration > (decode_element_no_tone_average * 2.5)) || (element_duration > (decode_element_tone_average * 2.5))) {
        if (element_duration > (float(1200 / decoder_wpm) * CW_DECODER_SPACE_DECODE_THRESH))
        {
          decode_it_flag = 1;
        }
      }
      else
      {
        // have we had tone for an outrageous amount of time?
      }
    }
  }

  if (decode_it_flag)
  { // are we ready to decode the element array?

    // adjust the decoder wpm based on what we got

    if ((no_tone_count > 0) && (tone_count > 1))
    {
      if (decode_element_no_tone_average > 0)
      {
        if (abs((1200 / decode_element_no_tone_average) - decoder_wpm) < 5)
        {
          decoder_wpm = (decoder_wpm + (1200 / decode_element_no_tone_average)) / 2;
        }
        else
        {
          if (abs((1200 / decode_element_no_tone_average) - decoder_wpm) < 10)
          {
            decoder_wpm = (decoder_wpm + decoder_wpm + (1200 / decode_element_no_tone_average)) / 3;
          }
          else
          {
            if (abs((1200 / decode_element_no_tone_average) - decoder_wpm) < 20)
            {
              decoder_wpm = (decoder_wpm + decoder_wpm + decoder_wpm + (1200 / decode_element_no_tone_average)) / 4;
            }
          }
        }
      }
    }

#ifdef DEBUG_FEATURE_STRAIGHT_KEY_ECHO
    if (abs(decoder_wpm - last_printed_decoder_wpm) > 0.9)
    {
      debug_serial_port->print("<");
      debug_serial_port->print(int(decoder_wpm));
      debug_serial_port->print(">");
      last_printed_decoder_wpm = decoder_wpm;
    }
#endif // DEBUG_FEATURE_STRAIGHT_KEY_ECHO

    for (byte x = 0; x < decode_element_pointer; x++)
    {
      if (decode_elements[x] > 0)
      { // is this a tone element?
        // we have no spaces to time from, use the current wpm
        if ((decode_elements[x] / (1200 / decoder_wpm)) < 2.1)
        {                                                 // changed from 1.3 to 2.1 2015-05-12
          decode_character = (decode_character * 10) + 1; // we have a dit
        }
        else
        {
          decode_character = (decode_character * 10) + 2; // we have a dah
        }
      }
#ifdef DEBUG_FEATURE_STRAIGHT_KEY_ECHO
      debug_serial_port->print(F("service_straight_key: decode_elements["));
      debug_serial_port->print(x);
      debug_serial_port->print(F("]: "));
      debug_serial_port->println(decode_elements[x]);
#endif // DEBUG_FEATURE_STRAIGHT_KEY_ECHO
    }

#ifdef DEBUG_FEATURE_STRAIGHT_KEY_ECHO
    debug_serial_port->print(F("service_straight_key: decode_element_tone_average: "));
    debug_serial_port->println(decode_element_tone_average);
    debug_serial_port->print(F("service_straight_key: decode_element_no_tone_average: "));
    debug_serial_port->println(decode_element_no_tone_average);
    debug_serial_port->print(F("service_straight_key: decode_element_no_tone_average wpm: "));
    debug_serial_port->println(1200 / decode_element_no_tone_average);
    debug_serial_port->print(F("service_straight_key: decoder_wpm: "));
    debug_serial_port->println(decoder_wpm);
    debug_serial_port->print(F("service_straight_key: decode_character: "));
    debug_serial_port->println(decode_character);
#endif // DEBUG_FEATURE_STRAIGHT_KEY_ECHO

#if defined(OPTION_PROSIGN_SUPPORT)
    byte cw_ascii_temp = convert_cw_number_to_ascii(decode_character);
    static char *prosign_char = (char *)"";

    if ((cw_ascii_temp > PROSIGN_START) && (cw_ascii_temp < PROSIGN_END))
    { // if we have a prosign code, convert it to chars
      prosign_char = convert_prosign(cw_ascii_temp);
      cw_ascii_temp = 0;
    }

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && defined(FEATURE_STRAIGHT_KEY_ECHO)
    if (cli_straight_key_echo)
    {
      if (cw_ascii_temp)
      {
        primary_serial_port->write(cw_ascii_temp);
      }
      else
      {
        primary_serial_port->write(prosign_char[0]);
        primary_serial_port->write(prosign_char[1]);
      }
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
      if (cw_ascii_temp)
      {
        secondary_serial_port->write(cw_ascii_temp);
      }
      else
      {
        secondary_serial_port->write(prosign_char[0]);
        secondary_serial_port->write(prosign_char[1]);
      }
#endif
      screen_column++;
    }
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)

#if defined(FEATURE_DISPLAY) && defined(FEATURE_STRAIGHT_KEY_ECHO)
    if (cli_straight_key_echo)
    {
      if (cw_ascii_temp)
      {
        display_scroll_print_char(cw_ascii_temp);
      }
      else
      {
        display_scroll_print_char(prosign_char[0]);
        display_scroll_print_char(prosign_char[1]);
      }
    }
#endif // FEATURE_DISPLAY

#else // OPTION_PROSIGN_SUPPORT

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && defined(FEATURE_STRAIGHT_KEY_ECHO)
    if (cli_straight_key_echo)
    {
      primary_serial_port->write(convert_cw_number_to_ascii(decode_character));
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
      secondary_serial_port->write(convert_cw_number_to_ascii(decode_character));
#endif
      screen_column++;
    }
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)

#if defined(FEATURE_DISPLAY) && defined(FEATURE_STRAIGHT_KEY_ECHO)
    if (cli_straight_key_echo)
    {
      display_scroll_print_char(convert_cw_number_to_ascii(decode_character));
    }
#endif // FEATURE_DISPLAY

#endif // OPTION_PROSIGN_SUPPORT

#if defined(FEATURE_CW_COMPUTER_KEYBOARD)
    switch (decode_character)
    {
    case 111111:
    case 1111111:
    case 11111111:
    case 111111111:
      Keyboard.write(KEY_BACKSPACE); // backspace
      cw_keyboard_no_space = 1;
      break;
    case 222222:
    case 2222222:
    case 22222222:
    case 222222222:
      Keyboard.write(32); // space
      no_space = 1;
      break;
    case 1212: // prosign AA
      Keyboard.write(KEY_RETURN);
      cw_keyboard_no_space = 1;
      break;
    case 211222: // prosign DO
      Keyboard.write(KEY_CAPS_LOCK);
#ifdef OPTION_CW_KEYBOARD_CAPSLOCK_BEEP
      if (cw_keyboard_capslock_on)
      {
        beep();
        delay(100);
        boop();
        cw_keyboard_capslock_on = 0;
      }
      else
      {
        boop();
        beep();
        cw_keyboard_capslock_on = 1;
      }
#endif // OPTION_CW_KEYBOARD_CAPSLOCK_BEEP
      cw_keyboard_no_space = 1;
      break;

#ifdef OPTION_CW_KEYBOARD_ITALIAN // courtesy of Giorgio IZ2XBZ
    case 122121:                  // "@"
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.write(59);
      Keyboard.releaseAll();
      break;
    case 112211: // "?"
      Keyboard.write(95);
      break;
    case 11221: // "!"
      Keyboard.write(33);
      break;
    case 21121: // "/"
      Keyboard.write(38);
      break;
    case 21112: // "=" or "BT"
      Keyboard.write(41);
      break;
    case 12212: // à
      Keyboard.write(39);
      break;
    case 11211: // è
      Keyboard.write(91);
      break;
    case 12221: // ì
      Keyboard.write(61);
      break;
    case 2221: // ò
      Keyboard.write(59);
      break;
    case 1122: // ù
      Keyboard.write(92);
      break;
    case 21221: // (
      Keyboard.write(42);
      break;
    case 212212: // )
      Keyboard.write(40);
      break;
    case 12111: // &
      Keyboard.write(94);
      break;
    case 222111: //:
      Keyboard.write(62);
      break;
    case 212121: //;
      Keyboard.write(60);
      break;
    case 12121: //+
      Keyboard.write(93);
      break;
    case 211112: // -
      Keyboard.write(47);
      break;
#endif // OPTION_CW_KEYBOARD_ITALIAN

    default:
      cw_keyboard_character_to_send = convert_cw_number_to_ascii(decode_character);
      if ((cw_keyboard_character_to_send > 64) && (cw_keyboard_character_to_send < 91))
      {
        cw_keyboard_character_to_send = cw_keyboard_character_to_send + 32;
      }
      if (cw_keyboard_character_to_send == '*')
      {
        cw_keyboard_no_space = 1;
#ifdef OPTION_UNKNOWN_CHARACTER_ERROR_TONE
        boop();
#endif // OPTION_UNKNOWN_CHARACTER_ERROR_TONE
      }
      else
      {
        if (!((cw_keyboard_backspace_flag) && ((decode_character == 1) || (decode_character == 11) || (decode_character == 111) || (decode_character == 1111) || (decode_character == 11111))))
        {
          Keyboard.write(char(cw_keyboard_character_to_send));
        }
        cw_keyboard_backspace_flag = 0;
      }
      break;

    } // switch (decode_character)

#ifdef DEBUG_CW_COMPUTER_KEYBOARD
    debug_serial_port->print("service_straight_key: Keyboard.write: ");
    debug_serial_port->write(character_to_send);
    debug_serial_port->println();
#endif // DEBUG_CW_COMPUTER_KEYBOARD

#endif // defined(FEATURE_CW_COMPUTER_KEYBOARD)

    // reinitialize everything
    last_transition_time = 0;
    last_decode_time = millis();
    decode_element_pointer = 0;
    decode_element_tone_average = 0;
    decode_element_no_tone_average = 0;
    space_sent = 0;
    no_tone_count = 0;
    tone_count = 0;

  } // if (decode_it_flag)

#if defined(FEATURE_SERIAL) && defined(FEATURE_STRAIGHT_KEY_ECHO)
#ifdef FEATURE_COMMAND_LINE_INTERFACE
  if ((screen_column > CW_DECODER_SCREEN_COLUMNS) && (cli_straight_key_echo))
  {
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
    secondary_serial_port->println();
#else
    primary_serial_port->println();
#endif
    screen_column = 0;
  }
#endif // FEATURE_COMMAND_LINE_INTERFACE
#endif // FEATURE_SERIAL

#endif // FEATURE_STRAIGHT_KEY_DECODE

  return (decode_character);
}
#endif // FEATURE_STRAIGHT_KEY

//-------------------------------------------------------------------------------------------------------

void initialize_cw_keyboard()
{

#ifdef FEATURE_CW_COMPUTER_KEYBOARD
  Keyboard.begin();
#endif // FEATURE_CW_COMPUTER_KEYBOARD
}

//-------------------------------------------------------------------------------------------------------

#ifdef ARDUINO_SAM_DUE

/*

This code from http://forum.arduino.cc/index.php?topic=136500.0

*/

// timers TC0 TC1 TC2   channels 0-2 ids 0-2  3-5  6-8     AB 0 1
// use TC1 channel 0

#define TONE_TIMER TC1
#define TONE_CHNL 0
#define TONE_IRQ TC3_IRQn

// TIMER_CLOCK4   84MHz/128 with 16 bit counter give 10 Hz to 656KHz

static uint8_t pinEnabled[PINS_COUNT];
static uint8_t TCChanEnabled = 0;
static boolean pin_state = false;
static Tc *chTC = TONE_TIMER;
static uint32_t chNo = TONE_CHNL;

volatile static int32_t toggle_count;
static uint32_t tone_pin;

void toneDUE(uint32_t ulPin, uint32_t frequency, int32_t duration = 0)
{

  // frequency (in hertz) and duration (in milliseconds)

  const uint32_t rc = VARIANT_MCK / 256 / frequency;
  tone_pin = ulPin;
  toggle_count = 0; // strange  wipe out previous duration
  if (duration > 0)
  {
    toggle_count = 2 * frequency * duration / 1000;
  }
  else
  {
    toggle_count = -1;
  }
  if (!TCChanEnabled)
  {
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk((uint32_t)TONE_IRQ);
    TC_Configure(chTC, chNo, TC_CMR_TCCLKS_TIMER_CLOCK4 | TC_CMR_WAVE | // Waveform mode
                                 TC_CMR_WAVSEL_UP_RC);                  // Counter running up and reset when equals to RC
    chTC->TC_CHANNEL[chNo].TC_IER = TC_IER_CPCS;                        // RC compare interrupt
    chTC->TC_CHANNEL[chNo].TC_IDR = ~TC_IER_CPCS;
    NVIC_EnableIRQ(TONE_IRQ);
    TCChanEnabled = 1;
  }
  if (!pinEnabled[ulPin])
  {
    pinMode(ulPin, OUTPUT);
    pinEnabled[ulPin] = 1;
  }
  TC_Stop(chTC, chNo);
  TC_SetRC(chTC, chNo, rc); // set frequency
  TC_Start(chTC, chNo);
}

void noToneDUE(uint32_t ulPin)
{

  TC_Stop(chTC, chNo);      // stop timer
  digitalWrite(ulPin, LOW); // no signal on pin
}

// timer ISR  TC1 ch 0
void TC3_Handler(void)
{

  TC_GetStatus(TC1, 0);
  if (toggle_count != 0)
  {
    // toggle pin  TODO  better
    digitalWrite(tone_pin, pin_state = !pin_state);
    if (toggle_count > 0)
      toggle_count--;
  }
  else
  {
    noTone(tone_pin);
  }
}
#endif
/*
//sp5iou 2018/0329 This is already in stm32 SDK standard library
#elif defined(ARDUINO_MAPLE_MINI) || defined(ARDUINO_GENERIC_STM32F103C) //HARDWARE_ARDUINO_DUE
//This code from http://www.stm32duino.com/viewtopic.php?t=496

///////////////////////////////////////////////////////////////////////
//
// tone(pin,frequency[,duration]) generate a tone on a given pin
//
// noTone(pin)                    switch of the tone on the pin
//
///////////////////////////////////////////////////////////////////////

//#include "Arduino.h"
//#include <HardwareTimer.h>

#ifndef TONE_TIMER
  #define TONE_TIMER 2
#endif

HardwareTimer tone_timer(TONE_TIMER);

bool tone_state = true;             // last pin state for toggling
short tone_pin = -1;                // pin for outputting sound
short tone_freq = 444;              // tone frequency (0=pause)
unsigned tone_micros = 500000/444;  // tone have wave time in usec
int tone_counts = 0;                // tone duration in units of half waves

// timer hander for tone with no duration specified,
// will keep going until noTone() is called
void tone_handler_1(void) {
  tone_state = !tone_state;
  digitalWrite(tone_pin,tone_state);
}

// timer hander for tone with a specified duration,
// will stop automatically when duration time is up.
void tone_handler_2(void) {   // check duration
  if(tone_freq>0){
   tone_state = !tone_state;
   digitalWrite(tone_pin,tone_state);
  }
  if(!--tone_counts){
   tone_timer.pause();
   pinMode(tone_pin, INPUT);
  }
}

//  play a tone on given pin with given frequency and optional duration in msec
void tone(uint8_t pin, unsigned short freq, unsigned duration = 0) {
  tone_pin = pin;
  tone_freq = freq;
  tone_micros = 500000/(freq>0?freq:1000);
  tone_counts = 0;

  tone_timer.pause();

  if(freq >= 0){
    if(duration > 0)tone_counts = ((long)duration)*1000/tone_micros;
    pinMode(tone_pin, OUTPUT);

    // set timer to half period in microseconds
    tone_timer.setPeriod(tone_micros);

    // Set up an interrupt on channel 1
    tone_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
    tone_timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
    tone_timer.attachCompare1Interrupt(tone_counts?tone_handler_2:tone_handler_1);

    // Refresh the tone timer
    tone_timer.refresh();

    // Start the timer counting
    tone_timer.resume();
  } else {
    pinMode(tone_pin, INPUT);
  }
}

// disable tone on specified pin, if any
void noTone(uint8_t pin){
  tone(pin,-1);
}


#endif //ARDUINO_MAPLE_MINI / ARDUINO_SAM_DUE
*/

//-------------------------------------------------------------------------------------------------------

/*   Sleep code prior to 2016-01-18
#ifdef FEATURE_SLEEP
void wakeup() {
  detachInterrupt(0);
}
#endif //FEATURE_SLEEP
*/

#if defined(FEATURE_SLEEP) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO) // Code contributed by Graeme, ZL2APV 2016-01-18
void wakeup()
{
  sleep_disable();
  detachInterrupt(0);
} // end of wakeup

ISR(PCINT1_vect)
{
  PCICR = 0; // cancel pin change interrupts
  sleep_disable();
} // end of ISR (PCINT1_vect)

ISR(PCINT2_vect)
{
  PCICR = 0; // turn off all pin change interrupt ports
  sleep_disable();
} // end of ISR (PCINT2_vect)
#endif // FEATURE_SLEEP

//-------------------------------------------------------------------------------------------------------
/*  Sleep code prior to 2016-01-18
#ifdef FEATURE_SLEEP
void check_sleep(){

  if ((millis() - last_activity_time) > (go_to_sleep_inactivity_time*60000)){

    if (config_dirty) {  // force a configuration write to EEPROM if the config is dirty
      last_config_write = 0;
      check_for_dirty_configuration();
    }

    attachInterrupt(0, wakeup, LOW);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    #ifdef DEBUG_SLEEP
    debug_serial_port->println(F("check_sleep: entering sleep"));
    delay(1000);
    #endif //DEBUG_SLEEP

    sleep_mode();

    // shhhhh! we are asleep here !!

    sleep_disable();
    last_activity_time = millis();

    #ifdef DEBUG_SLEEP
    debug_serial_port->println(F("check_sleep: I'm awake!"));
    #endif //DEBUG_SLEEP
  }


}
#endif //FEATURE_SLEEP
*/

#if defined(FEATURE_SLEEP) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO) // Code contributed by Graeme, ZL2APV  2016-01-18
void check_sleep()
{

  if ((millis() - last_activity_time) > ((unsigned long)go_to_sleep_inactivity_time * 60000))
  {

    if (config_dirty)
    { // force a configuration write to EEPROM if the config is dirty
      last_config_write = 0;
      check_for_dirty_configuration();
    }

    byte old_ADCSRA = ADCSRA;
    // disable ADC to save power
    ADCSRA = 0;

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    // Do not interrupt before we go to sleep, or the ISR will detach interrupts and we won't wake.
    noInterrupts();

    // will be called when pin D2, D5 or A1 goes low
    attachInterrupt(0, wakeup, FALLING);
    EIFR = bit(INTF0);     // clear flag for interrupt 0
    PCIFR = 0;             // Clear all pin change flags
    PCICR = 0b00000110;    // Turn on ports C and D only
    PCMSK2 = bit(PCINT21); // Turn on pin D5
    PCMSK1 = bit(PCINT9);  // Turn on pin A1

// turn off brown-out enable in software
// BODS must be set to one and BODSE must be set to zero within four clock cycles
#if !defined(__AVR_ATmega2560__)
    MCUCR = bit(BODS) | bit(BODSE);
    // The BODS bit is automatically cleared after three clock cycles
    MCUCR = bit(BODS);
#endif

#ifdef DEBUG_SLEEP
    debug_serial_port->println(F("check_sleep: entering sleep"));
    delay(1000);
#endif // DEBUG_SLEEP

    if (keyer_awake)
    {
      digitalWrite(keyer_awake, KEYER_AWAKE_PIN_ASLEEP_STATE);
    }

    interrupts();
    sleep_cpu();

    // shhhhh! we are asleep here !!

    // An interrupt on digital 2 will call the wake() interrupt service routine
    // and then return us to here while a change on D5 or A1 will vector to their
    // interrupt handler and also return to here.

    detachInterrupt(0);
    PCICR = 0;  // Turn off all ports
    PCMSK2 = 0; // Turn off pin D5
    PCMSK1 = 0; // Turn off pin A1

    ADCSRA = old_ADCSRA; // re-enable ADC conversion

    if (keyer_awake)
    {
      digitalWrite(keyer_awake, KEYER_AWAKE_PIN_AWAKE_STATE);
    }

    last_activity_time = millis();
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
    last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM

#ifdef DEBUG_SLEEP
    debug_serial_port->println(F("check_sleep: I'm awake!"));
#endif // DEBUG_SLEEP
  }
}
#endif // FEATURE_SLEEP

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
void check_backlight()
{

  static unsigned long last_bl_check = 0;

  if (millis() - last_bl_check < 1000)
    return; // not time-critical

  last_bl_check = millis();

  if ((last_bl_check - last_active_time) > ((unsigned long)dim_backlight_inactive_time * 60000))
  {

#ifdef DEBUG_BACKLIGHT
    debug_serial_port->println(F("check_backlight: I'm asleep!"));
#endif // DEBUG_BACKLIGHT

    if (keyer_power_led)
    {
      analogWrite(keyer_power_led, keyer_power_led_asleep_duty);
    }
    lcd.noBacklight();
  }
  else
  {

#ifdef DEBUG_BACKLIGHT
    debug_serial_port->println(F("check_backlight: I'm awake!"));
#endif // DEBUG_BACKLIGHT

    if (keyer_power_led)
    {
      analogWrite(keyer_power_led, keyer_power_led_awake_duty);
    }
    lcd.backlight();
  }
}
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_DISPLAY
void service_display()
{

#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering service_display"));
#endif

#define SCREEN_REFRESH_IDLE 0
#define SCREEN_REFRESH_INIT 1
#define SCREEN_REFRESH_IN_PROGRESS 2

  static byte x = 0;
  static byte y = 0;

  static byte screen_refresh_status = SCREEN_REFRESH_IDLE;

  if (screen_refresh_status == SCREEN_REFRESH_INIT)
  {
    lcd.setCursor(0, 0);
    y = 0;
    x = 0;
    screen_refresh_status = SCREEN_REFRESH_IN_PROGRESS;
    return;
  }

  if (screen_refresh_status == SCREEN_REFRESH_IN_PROGRESS)
  {
    if (x > lcd_scroll_buffer[y].length())
    {
      y++;
      if (y >= LCD_ROWS)
      {
        screen_refresh_status = SCREEN_REFRESH_IDLE;
        lcd_scroll_buffer_dirty = 0;
        return;
      }
      else
      {
        x = 0;
#ifndef FEATURE_OLED_SSD1306
        lcd.setCursor(0, y);
#else
        lcd.setCursor(0, y * 2);
#endif;
      }
    }
    else
    {
      if (lcd_scroll_buffer[y].charAt(x) > 0)
      {
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
        lcd.backlight();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
        lcd.print(lcd_scroll_buffer[y].charAt(x));
      }
      x++;
    }
  }

  if (screen_refresh_status == SCREEN_REFRESH_IDLE)
  {
    if (lcd_status == LCD_REVERT)
    {
      lcd_status = lcd_previous_status;
      switch (lcd_status)
      {
      case LCD_CLEAR:
        lcd_clear();
        break;
      case LCD_SCROLL_MSG:
        lcd.clear();
        // for (x = 0;x < LCD_ROWS;x++){
        //   //clear_display_row(x);
        //   lcd.setCursor(0,x);
        //   lcd.print(lcd_scroll_buffer[x]);
        // }
        screen_refresh_status = SCREEN_REFRESH_INIT;
        lcd_scroll_flag = 0;
        // lcd_scroll_buffer_dirty = 0;
        break;
      }
    }
    else
    {
      switch (lcd_status)
      {
      case LCD_CLEAR:
        break;
      case LCD_TIMED_MESSAGE:
        if (millis() > lcd_timed_message_clear_time)
        {
          lcd_status = LCD_REVERT;
        }
      case LCD_SCROLL_MSG:
        if (lcd_scroll_buffer_dirty)
        {
          if (lcd_scroll_flag)
          {
            lcd.clear();
            lcd_scroll_flag = 0;
          }
          // for (x = 0;x < LCD_ROWS;x++){
          //   //clear_display_row(x);
          //   lcd.setCursor(0,x);
          //   lcd.print(lcd_scroll_buffer[x]);
          // }
          // lcd_scroll_buffer_dirty = 0;
          screen_refresh_status = SCREEN_REFRESH_INIT;
        }
        break;
      }
    }
  }
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_DISPLAY
void display_scroll_print_char(char charin)
{

  static byte column_pointer = 0;
  static byte row_pointer = 0;
  static byte holding_space = 0;
  byte x = 0;

#ifdef DEBUG_DISPLAY_SCROLL_PRINT_CHAR
  debug_serial_port->print(F("display_scroll_print_char: "));
  debug_serial_port->write(charin);
  debug_serial_port->print(" ");
  debug_serial_port->println(charin);
#endif // DEBUG_DISPLAY_SCROLL_PRINT_CHAR

#ifdef OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS
  switch (byte(charin))
  {
  case 220:
    charin = 0;
    break; // U_umlaut  (D, ...)
  case 214:
    charin = 1;
    break; // O_umlaut  (D, SM, OH, ...)
  case 196:
    charin = 2;
    break; // A_umlaut  (D, SM, OH, ...)
  case 198:
    charin = 3;
    break; // AE_capital (OZ, LA)
  case 216:
    charin = 4;
    break; // OE_capital (OZ, LA)
  case 197:
    charin = 6;
    break; // AA_capital (OZ, LA, SM)
  case 209:
    charin = 7;
    break; // N-tilde (EA)
  }
#endif // OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS

  if (lcd_status != LCD_SCROLL_MSG)
  {
    lcd_status = LCD_SCROLL_MSG;
    lcd.clear();
  }

  if (charin == ' ')
  {
    holding_space = 1;
    return;
  }

  if (holding_space)
  { // ok, I admit this is a hack.  Hold on to spaces and don't scroll until the next char comes in...
    if (column_pointer > (LCD_COLUMNS - 1))
    {
      row_pointer++;
      column_pointer = 0;
      if (row_pointer > (LCD_ROWS - 1))
      {
        for (x = 0; x < (LCD_ROWS - 1); x++)
        {
          lcd_scroll_buffer[x] = lcd_scroll_buffer[x + 1];
        }
        lcd_scroll_buffer[x] = "";
        row_pointer--;
        lcd_scroll_flag = 1;
      }
    }
    if (column_pointer > 0)
    { // don't put a space in the first column
      lcd_scroll_buffer[row_pointer].concat(' ');
      column_pointer++;
    }
    holding_space = 0;
  }

  if (column_pointer > (LCD_COLUMNS - 1))
  {
    row_pointer++;
    column_pointer = 0;
    if (row_pointer > (LCD_ROWS - 1))
    {
      for (x = 0; x < (LCD_ROWS - 1); x++)
      {
        lcd_scroll_buffer[x] = lcd_scroll_buffer[x + 1];
      }
      lcd_scroll_buffer[x] = "";
      row_pointer--;
      lcd_scroll_flag = 1;
    }
  }
  lcd_scroll_buffer[row_pointer].concat(charin);
  column_pointer++;

  lcd_scroll_buffer_dirty = 1;
}

#endif // FEATURE_DISPLAY

//-------------------------------------------------------------------------------------------------------
#ifdef FEATURE_DISPLAY
void lcd_clear()
{
  lcd.clear();
#ifndef FEATURE_OLED_SSD1306
  lcd.noCursor(); // sp5iou 20180328
#endif
  lcd_status = LCD_CLEAR;
}
#endif
//-------------------------------------------------------------------------------------------------------
#ifdef FEATURE_DISPLAY
void lcd_center_print_timed(String lcd_print_string, byte row_number, unsigned int duration)
{
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
  lcd.backlight();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
#ifndef FEATURE_OLED_SSD1306
  lcd.noCursor(); // sp5iou 20180328
#endif

  if (lcd_status != LCD_TIMED_MESSAGE)
  {
    lcd_previous_status = lcd_status;
    lcd_status = LCD_TIMED_MESSAGE;
    lcd.clear();
  }
  else
  {
    clear_display_row(row_number);
  }
#ifndef FEATURE_OLED_SSD1306
  lcd.setCursor(((LCD_COLUMNS - lcd_print_string.length()) / 2), row_number);
#else
  if (lcd_print_string.length() <= LCD_COLUMNS)
    lcd.setCursor(((LCD_COLUMNS - lcd_print_string.length()) / 2) * 11, 2 * row_number);
  else
    lcd.setCursor(0, 2 * row_number);
#endif
  lcd.print(lcd_print_string);
  lcd_timed_message_clear_time = millis() + duration;
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_DISPLAY
void clear_display_row(byte row_number)
{
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
  lcd.backlight();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
#ifndef FEATURE_OLED_SSD1306
  lcd.noCursor(); // sp5iou 20180328
#endif
  for (byte x = 0; x < LCD_COLUMNS; x++)
  {
#ifndef FEATURE_OLED_SSD1306
    lcd.setCursor(x, row_number);
#else
    lcd.setCursor(x * 11, row_number * 2);
#endif
    lcd.print(" ");
  }
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_MEMORIES
void initialize_eeprom_memories()
{
  for (int x = 0; x < number_of_memories; x++)
  {
    EEPROM.write(memory_start(x), 255);
  }
}
#endif

//---------------------------------------------------------------------

void write_settings_to_eeprom(int initialize_eeprom)
{

#if (!defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_ARCH_MBED) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)) || (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024))

  if (initialize_eeprom)
  {
    // configuration.magic_number = eeprom_magic_number;
    EEPROM.write(0, eeprom_magic_number);
#ifdef FEATURE_MEMORIES
    initialize_eeprom_memories();
#endif // FEATURE_MEMORIES
    const byte *p = (const byte *)(const void *)&configuration;
    unsigned int i;
    int ee = 1; // starting point of configuration struct
    for (i = 0; i < sizeof(configuration); i++)
    {
      EEPROM.write(ee++, *p++);
    }
  }
  else
  {
    async_eeprom_write = 1; // initiate an asynchronous eeprom write
  }

#endif //! defined(ARDUINO_SAM_DUE) || (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024))

#if defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO)
#if defined(DEBUG_EEPROM)
  debug_serial_port->println(F("write_settings_to_eeprom: ARDUINO_RASPBERRY_PI_PICO"));
#endif
  EEPROM.write(0, eeprom_magic_number);
  EEPROM.commit();
  EEPROM.put(1, configuration);
  EEPROM.commit();
#endif

  config_dirty = 0;
}

//-------------------------------------------------------------------------------------------------------

void initialize_eeprom()
{

  write_settings_to_eeprom(1);
  // #if defined(FEATURE_SINEWAVE_SIDETONE)
  //   initialize_tonsin();
  // #endif
  beep_boop();
  beep_boop();
  beep_boop();
}

//---------------------------------------------------------------------

void check_for_dirty_configuration()
{
#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering check_for_dirty_configuration"));
#endif

  // if ((config_dirty) && ((millis()-last_config_write)>30000) && (!send_buffer_bytes) && (!ptt_line_activated)) {
  if ((config_dirty) && ((millis() - last_config_write) > eeprom_write_time_ms) && (!send_buffer_bytes) && (!ptt_line_activated) && (!dit_buffer) && (!dah_buffer) && (!async_eeprom_write) && (paddle_pin_read(paddle_left) == HIGH) && (paddle_pin_read(paddle_right) == HIGH))
  {
    write_settings_to_eeprom(0);
    last_config_write = millis();
#ifdef DEBUG_EEPROM
    debug_serial_port->println(F("check_for_dirty_configuration: wrote config\n"));
#endif
  }
}

//-------------------------------------------------------------------------------------------------------
#ifdef FEATURE_MEMORIES
void check_memory_repeat()
{

#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering check_memory_repeat"));
#endif

  if ((repeat_memory < number_of_memories) && ((millis() - last_memory_repeat_time) > configuration.memory_repeat_time))
  {
    add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
    add_to_send_buffer(repeat_memory);
    last_memory_repeat_time = millis();
#ifdef DEBUG_MEMORIES
    debug_serial_port->print(F("check_memory_repeat: added repeat_memory to send buffer\n\r"));
#endif
  }

  if (repeat_memory == 255)
  {
    last_memory_repeat_time = 0;
  }
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_DEAD_OP_WATCHDOG
void check_for_dead_op()

// if the dit or dah paddle is stuck, disable the transmitter line after 100 straight dits or dahs
// go in and out of command mode to clear or just reset the unit

{

#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering check_for_dead_op"));
#endif

  if (dead_op_watchdog_active && ((dit_counter > 100) || (dah_counter > 100)))
  {
    key_tx = 0;
  }
}
#endif
//-------------------------------------------------------------------------------------------------------

#if (defined(FEATURE_PS2_KEYBOARD) || defined(FEATURE_USB_KEYBOARD)) && defined(FEATURE_MEMORIES)

void repeat_memory_msg(byte memory_number)
{

#ifdef FEATURE_MEMORIES
  repeat_memory = memory_number;
#ifdef FEATURE_DISPLAY
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("RptMem" + String(memory_number + 1), 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Repeat Memory " + String(memory_number + 1), 0, default_display_msg_delay);
  }
  service_display();
#endif // FEATURE_DISPLAY
#endif // FEATURE_MEMORIES
}

#endif // defined(FEATURE_PS2_KEYBOARD) || defined(FEATURE_USB_KEYBOARD)

//-------------------------------------------------------------------------------------------------------

void switch_to_tx_silent(byte tx)
{

  switch (tx)
  {
  case 1:
    if ((ptt_tx_1) || (tx_key_line_1))
    {
      configuration.current_ptt_line = ptt_tx_1;
      current_tx_key_line = tx_key_line_1;
      configuration.current_tx = 1;
      config_dirty = 1;
    }
    break;
  case 2:
    if ((ptt_tx_2) || (tx_key_line_2))
    {
      configuration.current_ptt_line = ptt_tx_2;
      current_tx_key_line = tx_key_line_2;
      configuration.current_tx = 2;
      config_dirty = 1;
    }
    break;
  case 3:
    if ((ptt_tx_3) || (tx_key_line_3))
    {
      configuration.current_ptt_line = ptt_tx_3;
      current_tx_key_line = tx_key_line_3;
      configuration.current_tx = 3;
      config_dirty = 1;
    }
    break;
  case 4:
    if ((ptt_tx_4) || (tx_key_line_4))
    {
      configuration.current_ptt_line = ptt_tx_4;
      current_tx_key_line = tx_key_line_4;
      configuration.current_tx = 4;
      config_dirty = 1;
    }
    break;
  case 5:
    if ((ptt_tx_5) || (tx_key_line_5))
    {
      configuration.current_ptt_line = ptt_tx_5;
      current_tx_key_line = tx_key_line_5;
      configuration.current_tx = 5;
      config_dirty = 1;
    }
    break;
  case 6:
    if ((ptt_tx_6) || (tx_key_line_6))
    {
      configuration.current_ptt_line = ptt_tx_6;
      current_tx_key_line = tx_key_line_6;
      configuration.current_tx = 6;
      config_dirty = 1;
    }
    break;
  }
}

//------------------------------------------------------------------

void sidetone_adj(int hz)
{

  if (((configuration.hz_sidetone + hz) > sidetone_hz_limit_low) && ((configuration.hz_sidetone + hz) < sidetone_hz_limit_high))
  {
    configuration.hz_sidetone = configuration.hz_sidetone + hz;
    config_dirty = 1;
#if defined(FEATURE_DISPLAY) && defined(OPTION_MORE_DISPLAY_MSGS)
    if (LCD_COLUMNS < 9)
    {
      lcd_center_print_timed(String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
    }
    else
    {
      lcd_center_print_timed("Sidetone " + String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
    }
#endif
  }
}

//-------------------------------------------------------------------------------------------------------

void adjust_dah_to_dit_ratio(int adjustment)
{

  if ((configuration.dah_to_dit_ratio + adjustment) > 150 && (configuration.dah_to_dit_ratio + adjustment) < 810)
  {
    configuration.dah_to_dit_ratio = configuration.dah_to_dit_ratio + adjustment;
#ifdef FEATURE_DISPLAY
#ifdef OPTION_MORE_DISPLAY_MSGS
    if (LCD_COLUMNS < 9)
    {
      lcd_center_print_timed("DDR:" + String(configuration.dah_to_dit_ratio), 0, default_display_msg_delay);
    }
    else
    {
      lcd_center_print_timed("Dah/Dit: " + String(configuration.dah_to_dit_ratio), 0, default_display_msg_delay);
    }
    service_display();
#endif
#endif
  }

  config_dirty = 1;
}

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_MEMORIES
void program_memory(int memory_number)
{

  if (memory_number > (number_of_memories - 1))
  {
    boop();
    return;
  }

#ifdef FEATURE_DISPLAY
  String lcd_print_string;
  if (LCD_COLUMNS < 9)
  {
    lcd_print_string.concat("PgmMem");
  }
  else
  {
    lcd_print_string.concat("Pgm Memory ");
  }
  lcd_print_string.concat(memory_number + 1);
  lcd_center_print_timed(lcd_print_string, 0, default_display_msg_delay);
#endif

  // send_dit();
  beep();

  byte paddle_hit = 0;
  byte loop1 = 1;
  byte loop2 = 1;
  unsigned long last_element_time = 0;
  int memory_location_index = 0;
  long cwchar = 0;
  byte space_count = 0;

#ifdef FEATURE_MEMORY_MACROS
  byte macro_flag = 0;
#endif // FEATURE_MEMORY_MACROS

#if defined(FEATURE_STRAIGHT_KEY)
  long straight_key_decoded_character = 0;
#endif

  dit_buffer = 0;
  dah_buffer = 0;

#if defined(FEATURE_BUTTONS) && !defined(FEATURE_STRAIGHT_KEY)
  while ((paddle_pin_read(paddle_left) == HIGH) && (paddle_pin_read(paddle_right) == HIGH) && (!analogbuttonread(0)))
  {
  } // loop until user starts sending or hits the button
#endif

#if defined(FEATURE_BUTTONS) && defined(FEATURE_STRAIGHT_KEY)
  while ((paddle_pin_read(paddle_left) == HIGH) && (paddle_pin_read(paddle_right) == HIGH) && (!analogbuttonread(0)) && (digitalRead(pin_straight_key) == HIGH))
  {
  } // loop until user starts sending or hits the button
#endif

  while (loop2)
  {

#ifdef DEBUG_MEMORY_WRITE
    debug_serial_port->println(F("program_memory: entering loop2\r"));
#endif

    cwchar = 0;
    paddle_hit = 0;
    loop1 = 1;

    while (loop1)
    {
      check_paddles();
      if (dit_buffer)
      {
        sending_mode = MANUAL_SENDING;
        send_dit();
        dit_buffer = 0;
        paddle_hit = 1;
        cwchar = (cwchar * 10) + 1;
        last_element_time = millis();
#ifdef DEBUG_MEMORY_WRITE
        debug_serial_port->write(".");
#endif
      }
      if (dah_buffer)
      {
        sending_mode = MANUAL_SENDING;
        send_dah();
        dah_buffer = 0;
        paddle_hit = 1;
        cwchar = (cwchar * 10) + 2;
        last_element_time = millis();
#ifdef DEBUG_MEMORY_WRITE
        debug_serial_port->write("_");
#endif
      }

#if defined(FEATURE_STRAIGHT_KEY)
      straight_key_decoded_character = service_straight_key();
      if (straight_key_decoded_character != 0)
      {
        cwchar = straight_key_decoded_character;
        paddle_hit = 1;
      }
#endif

#if !defined(FEATURE_STRAIGHT_KEY)
      if ((paddle_hit) && (millis() > (last_element_time + (float(600 / configuration.wpm) * length_letterspace))))
      { // this character is over
        loop1 = 0;
      }
#else
      if (((paddle_hit) && (millis() > (last_element_time + (float(600 / configuration.wpm) * length_letterspace)))) || (straight_key_decoded_character != 0))
      { // this character is over
        loop1 = 0;
      }
#endif

      // TODO - need to add something here to handle straight key leading space
#ifdef FEATURE_MEMORY_MACROS
      if ((!macro_flag) && (paddle_hit == 0) && (millis() > (last_element_time + ((float(1200 / configuration.wpm) * configuration.length_wordspace)))) && (space_count < program_memory_limit_consec_spaces))
      { // we have a space
        loop1 = 0;
        cwchar = 9;
        space_count++;
      }
#else
      if ((paddle_hit == 0) && (millis() > (last_element_time + ((float(1200 / configuration.wpm) * configuration.length_wordspace)))) && (space_count < program_memory_limit_consec_spaces))
      { // we have a space
        loop1 = 0;
        cwchar = 9;
        space_count++;
      }
#endif // FEATURE_MEMORY_MACROS

#ifdef FEATURE_BUTTONS
      while (analogbuttonread(0))
      { // hit the button to get out of command mode if no paddle was hit
        loop1 = 0;
        loop2 = 0;
      }
#endif
    } // loop1

    if (cwchar != 9)
    {
      space_count = 0;
    }

    // write the character to memory
    if (cwchar > 0)
    {

#ifdef DEBUG_MEMORY_WRITE
      debug_serial_port->print(F("program_memory: write_character_to_memory"));
      debug_serial_port->print(F(" mem number:"));
      debug_serial_port->print(memory_number);
      debug_serial_port->print(F("  memory_location_index:"));
      debug_serial_port->print(memory_location_index);
      debug_serial_port->print(F("  EEPROM location:"));
      debug_serial_port->print(memory_start(memory_number) + memory_location_index);
      debug_serial_port->print(F("   cwchar:"));
      debug_serial_port->print(cwchar);
      debug_serial_port->print(F("   ascii to write:"));
      debug_serial_port->println(convert_cw_number_to_ascii(cwchar));
#endif

      EEPROM.write((memory_start(memory_number) + memory_location_index), convert_cw_number_to_ascii(cwchar));
      memory_location_index++;

#ifdef FEATURE_MEMORY_MACROS
      if (!macro_flag)
      {
        if (convert_cw_number_to_ascii(cwchar) == '\\')
        {
          macro_flag = 1;
        } // if we got the \ macro character, supress spaces
      }
      else
      {
        if (convert_cw_number_to_ascii(cwchar) == '+')
        { // if we're building a prosign, supress the next two spaces
          macro_flag = 2;
        }
        else
        {
          macro_flag--;
        }
      }
#endif // FEATURE_MEMORY_MACROS
    }

    // are we out of memory locations?
    if ((memory_start(memory_number) + memory_location_index) == memory_end(memory_number))
    {
      loop1 = 0;
      loop2 = 0;
#ifdef DEBUG_MEMORY_WRITE
      debug_serial_port->println(F("program_memory: out of memory location"));
#endif
    }
  }

// write terminating 255 at end
#ifdef DEBUG_MEMORY_WRITE
  debug_serial_port->println(F("program_memory: writing memory termination"));
#endif

  EEPROM.write((memory_start(memory_number) + memory_location_index), 255);

#ifdef OPTION_PROG_MEM_TRIM_TRAILING_SPACES
  for (int x = (memory_location_index - 1); x > 0; x--)
  {
    if (EEPROM.read((memory_start(memory_number) + x)) == 32)
    {
      EEPROM.write((memory_start(memory_number) + x), 255);
    }
    else
    {
      x = 0;
    }
  }
#endif

#ifdef FEATURE_DISPLAY
  lcd_center_print_timed("Done", 0, default_display_msg_delay);
#endif

  play_memory(memory_number);
}
#endif

//---------------------------------------------------------------------

#ifdef FEATURE_PS2_KEYBOARD
void check_ps2_keyboard()
{

#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering check_ps2_keyboard"));
#endif

  static byte keyboard_tune_on = 0;
  static byte ps2_prosign_flag = 0;
  int work_int = 0;
  uint8_t keystroke = 0;

  /* NOTE!!! This entire block of code is repeated again below the #else.  This was done to fix a bug with Notepad++ not
             collapsing code correctly when while() statements are encapsulated in #ifdef/#endifs.                        */

#ifdef FEATURE_MEMORIES
  while ((keyboard.available()) && (play_memory_prempt == 0))
  {

    // read the next key
    keystroke = keyboard.read();

#if defined(DEBUG_PS2_KEYBOARD)
    debug_serial_port->print("check_ps2_keyboard: keystroke: ");
    debug_serial_port->println(keystroke, DEC);
#endif // DEBUG_PS2_KEYBOARD

#ifdef FEATURE_SLEEP
    last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
    last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM

    if (ps2_keyboard_mode == PS2_KEYBOARD_NORMAL)
    {
      switch (keystroke)
      {
      case PS2_PAGEUP:
        sidetone_adj(20);
        break;
      case PS2_PAGEDOWN:
        sidetone_adj(-20);
        break;
      case PS2_RIGHTARROW:
        adjust_dah_to_dit_ratio(int(configuration.dah_to_dit_ratio / 10));
        break;
      case PS2_LEFTARROW:
        adjust_dah_to_dit_ratio(-1 * int(configuration.dah_to_dit_ratio / 10));
        break;
      case PS2_UPARROW:
        speed_set(configuration.wpm + 1);
        break;
      case PS2_DOWNARROW:
        speed_set(configuration.wpm - 1);
        break;
      case PS2_HOME:
        configuration.dah_to_dit_ratio = initial_dah_to_dit_ratio;
        key_tx = 1;
        config_dirty = 1;
#ifdef FEATURE_DISPLAY
#ifdef OPTION_MORE_DISPLAY_MSGS
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("DfltRtio", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Default ratio", 0, default_display_msg_delay);
        }
        service_display();
#endif
#endif
        break;
      case PS2_TAB:
        if (pause_sending_buffer)
        {
          pause_sending_buffer = 0;
#ifdef FEATURE_DISPLAY
#ifdef OPTION_MORE_DISPLAY_MSGS
          lcd_center_print_timed("Resume", 0, default_display_msg_delay);
#endif
#endif
        }
        else
        {
          pause_sending_buffer = 1;
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("Pause", 0, default_display_msg_delay);
#endif
        }
        break; // pause

      case PS2_SCROLL: // Prosign next two characters
        ps2_prosign_flag = 1;
#ifdef FEATURE_DISPLAY
#ifdef OPTION_MORE_DISPLAY_MSGS
        lcd_center_print_timed("Prosign", 0, default_display_msg_delay);
#endif
#endif
        break;

#ifdef FEATURE_MEMORIES
      case PS2_F1:
        ps2_usb_keyboard_play_memory(0);
        break;
      case PS2_F2:
        ps2_usb_keyboard_play_memory(1);
        break;
      case PS2_F3:
        ps2_usb_keyboard_play_memory(2);
        break;
#ifndef OPTION_SAVE_MEMORY_NANOKEYER
      case PS2_F4:
        ps2_usb_keyboard_play_memory(3);
        break;
      case PS2_F5:
        ps2_usb_keyboard_play_memory(4);
        break;
      case PS2_F6:
        ps2_usb_keyboard_play_memory(5);
        break;
      case PS2_F7:
        ps2_usb_keyboard_play_memory(6);
        break;
      case PS2_F8:
        ps2_usb_keyboard_play_memory(7);
        break;
      case PS2_F9:
        ps2_usb_keyboard_play_memory(8);
        break;
      case PS2_F10:
        ps2_usb_keyboard_play_memory(9);
        break;
      case PS2_F11:
        ps2_usb_keyboard_play_memory(10);
        break;
      case PS2_F12:
        ps2_usb_keyboard_play_memory(11);
        break;
#endif // OPTION_SAVE_MEMORY_NANOKEYER
      case PS2_F1_ALT:
        if (number_of_memories > 0)
        {
          repeat_memory_msg(0);
        }
        break;
      case PS2_F2_ALT:
        if (number_of_memories > 1)
        {
          repeat_memory_msg(1);
        }
        break;
      case PS2_F3_ALT:
        if (number_of_memories > 2)
        {
          repeat_memory_msg(2);
        }
        break;
#ifndef OPTION_SAVE_MEMORY_NANOKEYER
      case PS2_F4_ALT:
        if (number_of_memories > 3)
        {
          repeat_memory_msg(3);
        }
        break;
      case PS2_F5_ALT:
        if (number_of_memories > 4)
        {
          repeat_memory_msg(4);
        }
        break;
      case PS2_F6_ALT:
        if (number_of_memories > 5)
        {
          repeat_memory_msg(5);
        }
        break;
      case PS2_F7_ALT:
        if (number_of_memories > 6)
        {
          repeat_memory_msg(6);
        }
        break;
      case PS2_F8_ALT:
        if (number_of_memories > 7)
        {
          repeat_memory_msg(7);
        }
        break;
      case PS2_F9_ALT:
        if (number_of_memories > 8)
        {
          repeat_memory_msg(8);
        }
        break;
      case PS2_F10_ALT:
        if (number_of_memories > 9)
        {
          repeat_memory_msg(9);
        }
        break;
      case PS2_F11_ALT:
        if (number_of_memories > 10)
        {
          repeat_memory_msg(10);
        }
        break;
      case PS2_F12_ALT:
        if (number_of_memories > 11)
        {
          repeat_memory_msg(11);
        }
        break;
#endif // OPTION_SAVE_MEMORY_NANOKEYER
#endif // FEATURE_MEMORIES

      case PS2_DELETE:
        if (send_buffer_bytes)
        {
          send_buffer_bytes--;
        }
        break;
      case PS2_ESC: // clear the serial send buffer and a bunch of other stuff
        if (manual_ptt_invoke)
        {
          manual_ptt_invoke = 0;
          ptt_unkey();
        }
        if (keyboard_tune_on)
        {
          sending_mode = MANUAL_SENDING;
          tx_and_sidetone_key(0);
          keyboard_tune_on = 0;
        }
        if (pause_sending_buffer)
        {
          pause_sending_buffer = 0;
        }
        clear_send_buffer();
#ifdef FEATURE_MEMORIES
        // clear_memory_button_buffer();
        play_memory_prempt = 1;
        repeat_memory = 255;
#endif
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Abort", 0, default_display_msg_delay);
#endif
        break;

#ifdef FEATURE_MEMORIES
      case PS2_F1_SHIFT:
        ps2_keyboard_program_memory(0);
        break;

      case PS2_F2_SHIFT:
        ps2_keyboard_program_memory(1);
        break;

      case PS2_F3_SHIFT:
        ps2_keyboard_program_memory(2);
        break;

#ifndef OPTION_SAVE_MEMORY_NANOKEYER
      case PS2_F4_SHIFT:
        ps2_keyboard_program_memory(3);
        break;

      case PS2_F5_SHIFT:
        ps2_keyboard_program_memory(4);
        break;

      case PS2_F6_SHIFT:
        ps2_keyboard_program_memory(5);
        break;

      case PS2_F7_SHIFT:
        ps2_keyboard_program_memory(6);
        break;

      case PS2_F8_SHIFT:
        ps2_keyboard_program_memory(7);
        break;

      case PS2_F9_SHIFT:
        ps2_keyboard_program_memory(8);
        break;

      case PS2_F10_SHIFT:
        ps2_keyboard_program_memory(9);
        break;

      case PS2_F11_SHIFT:
        ps2_keyboard_program_memory(10);
        break;

      case PS2_F12_SHIFT:
        ps2_keyboard_program_memory(11);
        break;
#endif // OPTION_SAVE_MEMORY_NANOKEYER
#endif // FEATURE_MEMORIES

#ifndef OPTION_SAVE_MEMORY_NANOKEYER
      case PS2_INSERT: // send serial number and increment
        put_serial_number_in_send_buffer();
        serial_number++;
        break;

      case PS2_END: // send serial number no increment
        put_serial_number_in_send_buffer();
        break;

      case PS2_BACKSPACE_SHIFT: // decrement serial number
        serial_number--;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("SN " + String(serial_number), 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Serial: " + String(serial_number), 0, default_display_msg_delay);
        }
#endif
        break;

#endif // OPTION_SAVE_MEMORY_NANOKEYER

      case PS2_LEFT_ALT:
#ifdef DEBUG_PS2_KEYBOARD
        debug_serial_port->println("PS2_LEFT_ALT\n");
#endif
        break;

      case PS2_A_CTRL:
        configuration.keyer_mode = IAMBIC_A;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Iambic A", 0, default_display_msg_delay);
#endif

        config_dirty = 1;
        break;

      case PS2_B_CTRL:
        configuration.keyer_mode = IAMBIC_B;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Iambic B", 0, default_display_msg_delay);
#endif
        config_dirty = 1;
        break;

      case PS2_C_CTRL:
        configuration.keyer_mode = SINGLE_PADDLE;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("SnglePdl", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Single Paddle", 0, default_display_msg_delay);
        }
#endif
        config_dirty = 1;
        break;

#ifndef OPTION_NO_ULTIMATIC
      case PS2_D_CTRL:
        configuration.keyer_mode = ULTIMATIC;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Ultimatc", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Ultimatic", 0, default_display_msg_delay);
        }
#endif
        config_dirty = 1;
        break;
#endif // OPTION_NO_ULTIMATIC
#ifndef OPTION_SAVE_MEMORY_NANOKEYER
      case PS2_E_CTRL:
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("EnterSN", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Enter Serial #", 0, default_display_msg_delay);
        }
#else
        boop_beep();
#endif
        work_int = ps2_keyboard_get_number_input(4, 0, 10000);
        if (work_int > 0)
        {
          serial_number = work_int;
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#else
          beep();
#endif
        }
        break;
#endif // OPTION_SAVE_MEMORY_NANOKEYER

      case PS2_G_CTRL:
        configuration.keyer_mode = BUG;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Bug", 0, default_display_msg_delay);
#endif
        config_dirty = 1;
        break;

#ifdef FEATURE_HELL
      case PS2_H_CTRL:
        if (char_send_mode == CW)
        {
          char_send_mode = HELL;
          beep();
        }
        else
        {
          char_send_mode = CW;
          beep();
        }
        break;
#endif // FEATURE_HELL

      case PS2_I_CTRL:
        if (key_tx && keyer_machine_mode != KEYER_COMMAND_MODE)
        { // Added check that keyer is NOT in command mode or keyer might be enabled for paddle commands (WD9DMP)
          key_tx = 0;
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX off", 0, default_display_msg_delay);
#endif
        }
        else if (!key_tx && keyer_machine_mode != KEYER_COMMAND_MODE)
        { // Added check that keyer is NOT in command mode or keyer might be enabled for paddle commands (WD9DMP)
          key_tx = 1;
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX on", 0, default_display_msg_delay);
#endif
        }
        break;

#ifdef FEATURE_FARNSWORTH
      case PS2_M_CTRL:
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Frnswrth", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Farnsworth WPM", 0, default_display_msg_delay);
        }
#else
        boop_beep();
#endif
        work_int = ps2_keyboard_get_number_input(3, -1, 1000);
        if (work_int > -1)
        {
          configuration.wpm_farnsworth = work_int;
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#else
          beep();
#endif
          config_dirty = 1;
        }

        break;
#endif // FEATURE_FARNSWORTH

      case PS2_N_CTRL:
        if (configuration.paddle_mode == PADDLE_NORMAL)
        {
          configuration.paddle_mode = PADDLE_REVERSE;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("Pdl Rev", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Paddle Reverse", 0, default_display_msg_delay);
          }
#endif
        }
        else
        {
          configuration.paddle_mode = PADDLE_NORMAL;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("Pdl Norm", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Paddle Normal", 0, default_display_msg_delay);
          }
#endif
        }
        config_dirty = 1;
        break;

      case PS2_O_CTRL: // CTRL-O - cycle through sidetone modes on, paddle only and off - New code Marc-Andre, VE2EVN
        if (configuration.sidetone_mode == SIDETONE_PADDLE_ONLY)
        {
          configuration.sidetone_mode = SIDETONE_OFF;
          boop();
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("ST Off", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Sidetone Off", 0, default_display_msg_delay);
          }
#endif
        }
        else if (configuration.sidetone_mode == SIDETONE_ON)
        {
          configuration.sidetone_mode = SIDETONE_PADDLE_ONLY;
          beep();
          delay(200);
          beep();
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("ST Pdl O", 0, default_display_msg_delay);
          }
          if (LCD_COLUMNS > 19)
          {
            lcd_center_print_timed("Sidetone Paddle Only", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Sidetone", 0, default_display_msg_delay);
            lcd_center_print_timed("Paddle Only", 1, default_display_msg_delay);
          }
#endif
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("ST On", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Sidetone On", 0, default_display_msg_delay);
          }
#endif
          configuration.sidetone_mode = SIDETONE_ON;
          beep();
        }
        config_dirty = 1;
        break;

#if defined(FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING)
      case PS2_S_CTRL:
        if (configuration.cmos_super_keyer_iambic_b_timing_on)
        {
          configuration.cmos_super_keyer_iambic_b_timing_on = 0;
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("CMOS Superkeyer Off", 0, default_display_msg_delay);
#endif
        }
        else
        {
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("CMOS Superkeyer On", 0, default_display_msg_delay);
#endif
          configuration.cmos_super_keyer_iambic_b_timing_on = 1;
        }
        config_dirty = 1;
        break;
#endif // FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING

      case PS2_T_CTRL:
#ifdef FEATURE_MEMORIES
        repeat_memory = 255;
#endif
        if (keyboard_tune_on)
        {
          sending_mode = MANUAL_SENDING;
          tx_and_sidetone_key(0);
          keyboard_tune_on = 0;
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#endif // FEATURE_DISPLAY
        }
        else
        {
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("Tune", 0, default_display_msg_delay);
#endif
          sending_mode = MANUAL_SENDING;
          tx_and_sidetone_key(1);
          keyboard_tune_on = 1;
        }
        break;

      case PS2_U_CTRL:
        if (ptt_line_activated)
        {
          manual_ptt_invoke = 0;
          ptt_unkey();
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#endif // FEATURE_DISPLAY
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("PTTInvk", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("PTT Invoke", 0, default_display_msg_delay);
          }
#endif
          manual_ptt_invoke = 1;
          ptt_key();
        }
        break;

      case PS2_W_CTRL:
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("WPM Adj", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("WPM Adjust", 0, default_display_msg_delay);
        }
#else
        boop_beep();
#endif
        work_int = ps2_keyboard_get_number_input(3, 0, 1000);
        if (work_int > 0)
        {
          speed_set(work_int);
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#else
          beep();
#endif
          config_dirty = 1;
        }
        break;

      case PS2_F1_CTRL:
        switch_to_tx_silent(1);
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("TX 1", 0, default_display_msg_delay);
#endif
        break;

      case PS2_F2_CTRL:
        if ((ptt_tx_2) || (tx_key_line_2))
        {
          switch_to_tx_silent(2);
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX 2", 0, default_display_msg_delay);
#endif
        }
        break;
#ifndef OPTION_SAVE_MEMORY_NANOKEYER
      case PS2_F3_CTRL:
        if ((ptt_tx_3) || (tx_key_line_3))
        {
          switch_to_tx_silent(3);
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX 3", 0, default_display_msg_delay);
#endif
        }
        break;

      case PS2_F4_CTRL:
        if ((ptt_tx_4) || (tx_key_line_4))
        {
          switch_to_tx_silent(4);
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX 4", 0, default_display_msg_delay);
#endif
        }
        break;

      case PS2_F5_CTRL:
        if ((ptt_tx_5) || (tx_key_line_5))
        {
          switch_to_tx_silent(5);
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX 5", 0, default_display_msg_delay);
#endif
        }
        break;

      case PS2_F6_CTRL:
        if ((ptt_tx_6) || (tx_key_line_6))
        {
          switch_to_tx_silent(6);
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX 6", 0, default_display_msg_delay);
#endif
        }
        break;
#endif // OPTION_SAVE_MEMORY_NANOKEYER

#ifdef FEATURE_AUTOSPACE
      case PS2_Z_CTRL:
        if (configuration.autospace_active)
        {
          configuration.autospace_active = 0;
          config_dirty = 1;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("AutoSOff", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Autospace Off", 0, default_display_msg_delay);
          }
#endif
        }
        else
        {
          configuration.autospace_active = 1;
          config_dirty = 1;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("AutoSpOn", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Autospace On", 0, default_display_msg_delay);
          }
#endif
        }
        break;
#endif

      default:
        if ((keystroke > 31) && (keystroke < 255 /*123*/))
        {
          if (ps2_prosign_flag)
          {
            add_to_send_buffer(SERIAL_SEND_BUFFER_PROSIGN);
            ps2_prosign_flag = 0;
          }
          keystroke = uppercase(keystroke);
          add_to_send_buffer(keystroke);
#ifdef FEATURE_MEMORIES
          repeat_memory = 255;
#endif
        }
        break;
      }
    }
    else
    {
    }
  } // while ((keyboard.available()) && (play_memory_prempt == 0))

#else // FEATURE_MEMORIES --------------------------------------------------------------------

  while (keyboard.available())
  {

    // read the next key
    keystroke = keyboard.read();

#ifdef FEATURE_SLEEP
    last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
    last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM

    if (ps2_keyboard_mode == PS2_KEYBOARD_NORMAL)
    {
      switch (keystroke)
      {
      case PS2_PAGEUP:
        sidetone_adj(20);
        break;
      case PS2_PAGEDOWN:
        sidetone_adj(-20);
        break;
      case PS2_RIGHTARROW:
        adjust_dah_to_dit_ratio(int(configuration.dah_to_dit_ratio / 10));
        break;
      case PS2_LEFTARROW:
        adjust_dah_to_dit_ratio(-1 * int(configuration.dah_to_dit_ratio / 10));
        break;
      case PS2_UPARROW:
        speed_set(configuration.wpm + 1);
        break;
      case PS2_DOWNARROW:
        speed_set(configuration.wpm - 1);
        break;
      case PS2_HOME:
        configuration.dah_to_dit_ratio = initial_dah_to_dit_ratio;
        key_tx = 1;
        config_dirty = 1;
#ifdef FEATURE_DISPLAY
#ifdef OPTION_MORE_DISPLAY_MSGS
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("DfltRtio", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Default ratio", 0, default_display_msg_delay);
        }
        service_display();
#endif
#endif
        break;
      case PS2_TAB:
        if (pause_sending_buffer)
        {
          pause_sending_buffer = 0;
#ifdef FEATURE_DISPLAY
#ifdef OPTION_MORE_DISPLAY_MSGS
          lcd_center_print_timed("Resume", 0, default_display_msg_delay);
#endif
#endif
        }
        else
        {
          pause_sending_buffer = 1;
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("Pause", 0, default_display_msg_delay);
#endif
        }
        break; // pause

      case PS2_SCROLL: // Prosign next two characters
        ps2_prosign_flag = 1;
#ifdef FEATURE_DISPLAY
#ifdef OPTION_MORE_DISPLAY_MSGS
        lcd_center_print_timed("Prosign", 0, default_display_msg_delay);
#endif
#endif
        break;

#ifdef FEATURE_MEMORIES
      case PS2_F1:
        ps2_usb_keyboard_play_memory(0);
        break;
      case PS2_F2:
        ps2_usb_keyboard_play_memory(1);
        break;
      case PS2_F3:
        ps2_usb_keyboard_play_memory(2);
        break;
      case PS2_F4:
        ps2_usb_keyboard_play_memory(3);
        break;
      case PS2_F5:
        ps2_usb_keyboard_play_memory(4);
        break;
      case PS2_F6:
        ps2_usb_keyboard_play_memory(5);
        break;
      case PS2_F7:
        ps2_usb_keyboard_play_memory(6);
        break;
      case PS2_F8:
        ps2_usb_keyboard_play_memory(7);
        break;
      case PS2_F9:
        ps2_usb_keyboard_play_memory(8);
        break;
      case PS2_F10:
        ps2_usb_keyboard_play_memory(9);
        break;
      case PS2_F11:
        ps2_usb_keyboard_play_memory(10);
        break;
      case PS2_F12:
        ps2_usb_keyboard_play_memory(11);
        break;
      case PS2_F1_ALT:
        if (number_of_memories > 0)
        {
          repeat_memory_msg(0);
        }
        break;
      case PS2_F2_ALT:
        if (number_of_memories > 1)
        {
          repeat_memory_msg(1);
        }
        break;
      case PS2_F3_ALT:
        if (number_of_memories > 2)
        {
          repeat_memory_msg(2);
        }
        break;
      case PS2_F4_ALT:
        if (number_of_memories > 3)
        {
          repeat_memory_msg(3);
        }
        break;
      case PS2_F5_ALT:
        if (number_of_memories > 4)
        {
          repeat_memory_msg(4);
        }
        break;
      case PS2_F6_ALT:
        if (number_of_memories > 5)
        {
          repeat_memory_msg(5);
        }
        break;
      case PS2_F7_ALT:
        if (number_of_memories > 6)
        {
          repeat_memory_msg(6);
        }
        break;
      case PS2_F8_ALT:
        if (number_of_memories > 7)
        {
          repeat_memory_msg(7);
        }
        break;
      case PS2_F9_ALT:
        if (number_of_memories > 8)
        {
          repeat_memory_msg(8);
        }
        break;
      case PS2_F10_ALT:
        if (number_of_memories > 9)
        {
          repeat_memory_msg(9);
        }
        break;
      case PS2_F11_ALT:
        if (number_of_memories > 10)
        {
          repeat_memory_msg(10);
        }
        break;
      case PS2_F12_ALT:
        if (number_of_memories > 11)
        {
          repeat_memory_msg(11);
        }
        break;
#endif

      case PS2_DELETE:
        if (send_buffer_bytes)
        {
          send_buffer_bytes--;
        }
        break;
      case PS2_ESC: // clear the serial send buffer and a bunch of other stuff
        if (manual_ptt_invoke)
        {
          manual_ptt_invoke = 0;
          ptt_unkey();
        }
        if (keyboard_tune_on)
        {
          sending_mode = MANUAL_SENDING;
          tx_and_sidetone_key(0);
          keyboard_tune_on = 0;
        }
        if (pause_sending_buffer)
        {
          pause_sending_buffer = 0;
        }
        clear_send_buffer();
#ifdef FEATURE_MEMORIES
        // clear_memory_button_buffer();
        play_memory_prempt = 1;
        repeat_memory = 255;
#endif
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Abort", 0, default_display_msg_delay);
#endif
        break;

#ifdef FEATURE_MEMORIES
      case PS2_F1_SHIFT:
        ps2_keyboard_program_memory(0);
        break;

      case PS2_F2_SHIFT:
        ps2_keyboard_program_memory(1);
        break;

      case PS2_F3_SHIFT:
        ps2_keyboard_program_memory(2);
        break;

      case PS2_F4_SHIFT:
        ps2_keyboard_program_memory(3);
        break;

      case PS2_F5_SHIFT:
        ps2_keyboard_program_memory(4);
        break;

      case PS2_F6_SHIFT:
        ps2_keyboard_program_memory(5);
        break;

      case PS2_F7_SHIFT:
        ps2_keyboard_program_memory(6);
        break;

      case PS2_F8_SHIFT:
        ps2_keyboard_program_memory(7);
        break;

      case PS2_F9_SHIFT:
        ps2_keyboard_program_memory(8);
        break;

      case PS2_F10_SHIFT:
        ps2_keyboard_program_memory(9);
        break;

      case PS2_F11_SHIFT:
        ps2_keyboard_program_memory(10);
        break;

      case PS2_F12_SHIFT:
        ps2_keyboard_program_memory(11);
        break;
#endif // FEATURE_MEMORIES

      case PS2_INSERT: // send serial number and increment
        put_serial_number_in_send_buffer();
        serial_number++;
        break;

      case PS2_END: // send serial number no increment
        put_serial_number_in_send_buffer();
        break;

      case PS2_BACKSPACE_SHIFT: // decrement serial number
        serial_number--;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("SN " + String(serial_number), 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Serial: " + String(serial_number), 0, default_display_msg_delay);
        }
#endif
        break;

      case PS2_LEFT_ALT:
#ifdef DEBUG_PS2_KEYBOARD
        debug_serial_port->println("PS2_LEFT_ALT\n");
#endif
        break;

      case PS2_A_CTRL:
        configuration.keyer_mode = IAMBIC_A;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Iambic A", 0, default_display_msg_delay);
#endif

        config_dirty = 1;
        break;

      case PS2_B_CTRL:
        configuration.keyer_mode = IAMBIC_B;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Iambic B", 0, default_display_msg_delay);
#endif
        config_dirty = 1;
        break;

      case PS2_C_CTRL:
        configuration.keyer_mode = SINGLE_PADDLE;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Sngl Pdl", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Single Paddle", 0, default_display_msg_delay);
        }
#endif
        config_dirty = 1;
        break;

#ifndef OPTION_NO_ULTIMATIC
      case PS2_D_CTRL:
        configuration.keyer_mode = ULTIMATIC;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Ultimatc", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Ultimatic", 0, default_display_msg_delay);
        }
#endif
        config_dirty = 1;
        break;
#endif // OPTION_NO_ULTIMATIC

      case PS2_E_CTRL:
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Enter SN", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Enter Serial #", 0, default_display_msg_delay);
        }
#else
        boop_beep();
#endif
        work_int = ps2_keyboard_get_number_input(4, 0, 10000);
        if (work_int > 0)
        {
          serial_number = work_int;
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#else
          beep();
#endif
        }
        break;

      case PS2_G_CTRL:
        configuration.keyer_mode = BUG;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Bug", 0, default_display_msg_delay);
#endif
        config_dirty = 1;
        break;

      case PS2_H_CTRL:
#ifdef FEATURE_HELL
        if (char_send_mode == CW)
        {
          char_send_mode = HELL;
          beep();
        }
        else
        {
          char_send_mode = CW;
          beep();
        }
#endif // FEATURE_HELL
        break;

      case PS2_I_CTRL:
        if (key_tx && keyer_machine_mode != KEYER_COMMAND_MODE)
        { // Added check that keyer is NOT in command mode or keyer might be enabled for paddle commands (WD9DMP-1)
          key_tx = 0;
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX Off", 0, default_display_msg_delay);
#endif
        }
        else if (!key_tx && keyer_machine_mode != KEYER_COMMAND_MODE)
        { // Added check that keyer is NOT in command mode or keyer might be enabled for paddle commands (WD9DMP-1)
          key_tx = 1;
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX On", 0, default_display_msg_delay);
#endif
        }
        break;

      case PS2_M_CTRL:
#ifdef FEATURE_FARNSWORTH
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Frnswrth", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Farnsworth WPM", 0, default_display_msg_delay);
        }
#else
        boop_beep();
#endif
        work_int = ps2_keyboard_get_number_input(3, -1, 1000);
        if (work_int > -1)
        {
          configuration.wpm_farnsworth = work_int;
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#else
          beep();
#endif
          config_dirty = 1;
        }
#endif
        break;

      case PS2_N_CTRL:
        if (configuration.paddle_mode == PADDLE_NORMAL)
        {
          configuration.paddle_mode = PADDLE_REVERSE;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("Pdl Rev", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Paddle Reverse", 0, default_display_msg_delay);
          }
#endif
        }
        else
        {
          configuration.paddle_mode = PADDLE_NORMAL;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("Pdl Norm", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Paddle Normal", 0, default_display_msg_delay);
          }
#endif
        }
        config_dirty = 1;
        break;

      case PS2_O_CTRL: // CTRL-O - cycle through sidetone modes on, paddle only and off - New code Marc-Andre, VE2EVN
        if (configuration.sidetone_mode == SIDETONE_PADDLE_ONLY)
        {
          configuration.sidetone_mode = SIDETONE_OFF;
          boop();
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("ST Off", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Sidetone Off", 0, default_display_msg_delay);
          }
#endif
        }
        else if (configuration.sidetone_mode == SIDETONE_ON)
        {
          configuration.sidetone_mode = SIDETONE_PADDLE_ONLY;
          beep();
          delay(200);
          beep();
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("ST Pdl O", 0, default_display_msg_delay);
          }
          if (LCD_COLUMNS > 19)
          {
            lcd_center_print_timed("Sidetone Paddle Only", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Sidetone", 0, default_display_msg_delay);
            lcd_center_print_timed("Paddle Only", 1, default_display_msg_delay);
          }
#endif
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("ST On", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Sidetone On", 0, default_display_msg_delay);
          }
#endif
          configuration.sidetone_mode = SIDETONE_ON;
          beep();
        }
        config_dirty = 1;
        break;

      case PS2_T_CTRL:
#ifdef FEATURE_MEMORIES
        repeat_memory = 255;
#endif
        if (keyboard_tune_on)
        {
          sending_mode = MANUAL_SENDING;
          tx_and_sidetone_key(0);
          keyboard_tune_on = 0;
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#endif // FEATURE_DISPLAY
        }
        else
        {
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("Tune", 0, default_display_msg_delay);
#endif
          sending_mode = MANUAL_SENDING;
          tx_and_sidetone_key(1);
          keyboard_tune_on = 1;
        }
        break;

      case PS2_U_CTRL:
        if (ptt_line_activated)
        {
          manual_ptt_invoke = 0;
          ptt_unkey();
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#endif // FEATURE_DISPLAY
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("PTT Invk", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("PTT Invoke", 0, default_display_msg_delay);
          }
#endif
          manual_ptt_invoke = 1;
          ptt_key();
        }
        break;

      case PS2_W_CTRL:
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("WPM Adj", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("WPM Adjust", 0, default_display_msg_delay);
        }
#else
        boop_beep();
#endif
        work_int = ps2_keyboard_get_number_input(3, 0, 1000);
        if (work_int > 0)
        {
          speed_set(work_int);
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#else
          beep();
#endif
          config_dirty = 1;
        }
        break;

      case PS2_F1_CTRL:
        switch_to_tx_silent(1);
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("TX 1", 0, default_display_msg_delay);
#endif
        break;

      case PS2_F2_CTRL:
        if ((ptt_tx_2) || (tx_key_line_2))
        {
          switch_to_tx_silent(2);
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX 2", 0, default_display_msg_delay);
#endif
        }
        break;

      case PS2_F3_CTRL:
        if ((ptt_tx_3) || (tx_key_line_3))
        {
          switch_to_tx_silent(3);
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX 3", 0, default_display_msg_delay);
#endif
        }
        break;

      case PS2_F4_CTRL:
        if ((ptt_tx_4) || (tx_key_line_4))
        {
          switch_to_tx_silent(4);
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX 4", 0, default_display_msg_delay);
#endif
        }
        break;

      case PS2_F5_CTRL:
        if ((ptt_tx_5) || (tx_key_line_5))
        {
          switch_to_tx_silent(5);
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX 5", 0, default_display_msg_delay);
#endif
        }
        break;

      case PS2_F6_CTRL:
        if ((ptt_tx_6) || (tx_key_line_6))
        {
          switch_to_tx_silent(6);
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX 6", 0, default_display_msg_delay);
#endif
        }
        break;

#ifdef FEATURE_AUTOSPACE
      case PS2_Z_CTRL:
        if (configuration.autospace_active)
        {
          configuration.autospace_active = 0;
          config_dirty = 1;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("AutoSOff", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Autospace Off", 0, default_display_msg_delay);
          }
#endif
        }
        else
        {
          configuration.autospace_active = 1;
          config_dirty = 1;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("AutoS On", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Autospace On", 0, default_display_msg_delay);
          }
#endif
        }
        break;
#endif

      default:
        if ((keystroke > 31) && (keystroke < 255 /*123*/))
        {
          if (ps2_prosign_flag)
          {
            add_to_send_buffer(SERIAL_SEND_BUFFER_PROSIGN);
            ps2_prosign_flag = 0;
          }
          keystroke = uppercase(keystroke);
          add_to_send_buffer(keystroke);
#ifdef FEATURE_MEMORIES
          repeat_memory = 255;
#endif
        }
        break;
      }
    }
    else
    {
    }
  } // while (keyboard.available())
#endif // FEATURE_MEMORIES
}
#endif // FEATURE_PS2_KEYBOARD

//-------------------------------------------------------------------------------------------------------
#if (defined(FEATURE_PS2_KEYBOARD) || defined(FEATURE_USB_KEYBOARD)) && defined(FEATURE_MEMORIES)
void ps2_usb_keyboard_play_memory(byte memory_number)
{

  if (memory_number < number_of_memories)
  {
    add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
    add_to_send_buffer(memory_number);
#ifdef FEATURE_MEMORIES
    repeat_memory = 255;
#endif // FEATURE_MEMORIES
  }
}
#endif // defined(FEATURE_PS2_KEYBOARD) || defined(FEATURE_USB_KEYBOARD)
//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_PS2_KEYBOARD) && defined(FEATURE_MEMORIES)
void ps2_keyboard_program_memory(byte memory_number)
{

  char keystroke;
  byte looping = 1;
  byte error = 0;
  int temp_memory_index = 0;
  byte temp_memory[(memory_end(memory_number) - memory_start(memory_number) + 1)];
  int x;
  String keyboard_string;

#ifdef FEATURE_DISPLAY
  String lcd_string;
  if (LCD_COLUMNS < 9)
  {
    lcd_string = "Pgm Mem";
  }
  else
  {
    lcd_string = "Program Memory";
  }
#endif

  if (memory_number > (number_of_memories - 1))
  {
    boop();
    return;
  }

#ifdef FEATURE_DISPLAY
  if (memory_number < 9)
  {
    lcd_string.concat(' ');
  }
  lcd_string.concat(memory_number + 1);
  lcd_center_print_timed(lcd_string, 0, default_display_msg_delay);
#else
  boop_beep();
#endif
  repeat_memory = 255;
  while (looping)
  {
    while (keyboard.available() == 0)
    {
      if (keyer_machine_mode == KEYER_NORMAL)
      { // might as well do something while we're waiting
        check_paddles();
        service_dit_dah_buffers();
      }
    }
    keystroke = keyboard.read();
#ifdef DEBUG_PS2_KEYBOARD
    debug_serial_port->println(keystroke, DEC);
#endif
    if (keystroke == 13)
    { // did we get a carriage return?
      looping = 0;
    }
    else
    {
      if (keystroke == PS2_BACKSPACE)
      {
        if (temp_memory_index)
        {
          temp_memory_index--;
#ifdef FEATURE_DISPLAY
          keyboard_string = keyboard_string.substring(0, keyboard_string.length() - 1);
          lcd_center_print_timed(keyboard_string, 1, default_display_msg_delay);
#endif
        }
      }
      else
      {
        if (keystroke == PS2_ESC)
        {
          looping = 0;
          error = 1;
        }
        else
        {
          keystroke = uppercase(keystroke);
#ifdef FEATURE_DISPLAY
          keyboard_string.concat(char(keystroke));
          if (keyboard_string.length() > LCD_COLUMNS)
          {
            lcd_center_print_timed(keyboard_string.substring((keyboard_string.length() - LCD_COLUMNS)), 1, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed(keyboard_string, 1, default_display_msg_delay);
          }
#endif
          temp_memory[temp_memory_index] = keystroke;
          temp_memory_index++;
          if (temp_memory_index > (memory_end(memory_number) - memory_start(memory_number)))
          {
            looping = 0;
          }
        }
      }
    }
  } // while (looping)

  if (error)
  {
#ifdef FEATURE_DISPLAY
    lcd_status = LCD_REVERT;
#else
    boop();
#endif
  }
  else
  {
    for (x = 0; x < temp_memory_index; x++)
    { // write to memory
      EEPROM.write((memory_start(memory_number) + x), temp_memory[x]);
      if ((memory_start(memory_number) + x) == memory_end(memory_number))
      { // are we at last memory location?
        x = temp_memory_index;
      }
    }
    // write terminating 255
    EEPROM.write((memory_start(memory_number) + x), 255);
#ifdef FEATURE_DISPLAY
    lcd_center_print_timed("Done", 0, default_display_msg_delay);
#else
    beep();
#endif
  }
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_PS2_KEYBOARD

int ps2_keyboard_get_number_input(byte places, int lower_limit, int upper_limit)
{

  byte looping = 1;
  byte error = 0;
  byte numberindex = 0;
  int numbers[6];
  char keystroke;
  String keyboard_string;

#ifdef FEATURE_MEMORIES
  repeat_memory = 255;
#endif

  while (looping)
  {
    if (keyboard.available() == 0)
    { // wait for the next keystroke
      if (keyer_machine_mode == KEYER_NORMAL)
      { // might as well do something while we're waiting
        check_paddles();
        service_dit_dah_buffers();
        service_send_buffer(PRINTCHAR);

        check_ptt_tail();
#ifdef FEATURE_POTENTIOMETER
        if (configuration.pot_activated)
        {
          check_potentiometer();
        }
#endif

#ifdef FEATURE_SIDETONE_SWITCH
        check_sidetone_switch();
#endif

#ifdef FEATURE_ROTARY_ENCODER
        check_rotary_encoder();
#endif // FEATURE_ROTARY_ENCODER
      }
    }
    else
    {
      keystroke = keyboard.read();
      if ((keystroke > 47) && (keystroke < 58))
      { // ascii 48-57 = "0" - "9")
        numbers[numberindex] = keystroke;
        numberindex++;
#ifdef FEATURE_DISPLAY
        keyboard_string.concat(String(keystroke - 48));
        lcd_center_print_timed(keyboard_string, 1, default_display_msg_delay);
#endif
        if (numberindex > places)
        {
          looping = 0;
          error = 1;
        }
      }
      else
      {
        if (keystroke == PS2_BACKSPACE)
        {
          if (numberindex)
          {
            numberindex--;
#ifdef FEATURE_DISPLAY
            keyboard_string = keyboard_string.substring(0, keyboard_string.length() - 1);
            lcd_center_print_timed(keyboard_string, 1, default_display_msg_delay);
#endif
          }
        }
        else
        {
          if (keystroke == PS2_ENTER)
          { // carriage return - get out
            looping = 0;
          }
          else
          { // bogus input - error out
            looping = 0;
            error = 1;
          }
        }
      }
    }
  }
  if (error)
  {
    boop();
    return (-1);
  }
  else
  {
    int y = 1;
    int return_number = 0;
    for (int x = (numberindex - 1); x >= 0; x = x - 1)
    {
      return_number = return_number + ((numbers[x] - 48) * y);
      y = y * 10;
    }
    if ((return_number > lower_limit) && (return_number < upper_limit))
    {
      return (return_number);
    }
    else
    {
      boop();
      return (-1);
    }
  }
}
#endif

//-------------------------------------------------------------------------------------------------------
#if (defined(FEATURE_PS2_KEYBOARD) || defined(FEATURE_USB_KEYBOARD)) && !defined(OPTION_SAVE_MEMORY_NANOKEYER)
void put_serial_number_in_send_buffer()
{

  String serial_number_string;

#ifdef FEATURE_MEMORIES
  repeat_memory = 255;
#endif

  serial_number_string = String(serial_number, DEC);
  if ((serial_number_string.length() < 3) && (serial_leading_zeros))
  {
    if (serial_cut_numbers)
    {
      add_to_send_buffer('T');
    }
    else
    {
      add_to_send_buffer('0');
    }
  }
  if ((serial_number_string.length() == 1) && (serial_leading_zeros))
  {
    if (serial_cut_numbers)
    {
      add_to_send_buffer('T');
    }
    else
    {
      add_to_send_buffer('0');
    }
  }
  for (byte a = 0; a < serial_number_string.length(); a++)
  {
    if ((serial_number_string[a] == '0') && (serial_cut_numbers))
    {
      add_to_send_buffer('T');
    }
    else
    {
      if ((serial_number_string[a] == '9') && (serial_cut_numbers))
      {
        add_to_send_buffer('N');
      }
      else
      {
        add_to_send_buffer(serial_number_string[a]);
      }
    }
  }
}
#endif // defined(FEATURE_PS2_KEYBOARD) || defined(FEATURE_USB_KEYBOARD)

//-------------------------------------------------------------------------------------------------------

#ifdef DEBUG_CAPTURE_COM_PORT
void debug_capture()
{

  byte serial_byte_in;
  int x = 1022;

  while (primary_serial_port->available() == 0)
  {
  } // wait for first byte
  serial_byte_in = primary_serial_port->read();
  primary_serial_port->write(serial_byte_in);
  // if ((serial_byte_in > 47) or (serial_byte_in = 20)) { primary_serial_port->write(serial_byte_in); }  // echo back
  if (serial_byte_in == '~')
  {
    debug_capture_dump(); // go into dump mode if we get a tilde
  }
  else
  {
    EEPROM.write(x, serial_byte_in);
    x--;
    while (x > 400)
    {
      if (primary_serial_port->available() > 0)
      {
        serial_byte_in = primary_serial_port->read();
        EEPROM.write(x, serial_byte_in);
        EEPROM.write(x - 1, 255);
        send_dit();
        x--;
        primary_serial_port->write(serial_byte_in);
        // if ((serial_byte_in > 47) or (serial_byte_in = 20)) { primary_serial_port->write(serial_byte_in); }  // echo back
      }
    }
  }

  while (1)
  {
  }
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef DEBUG_CAPTURE_COM_PORT
void debug_capture_dump()
{
  byte eeprom_byte_in;

  for (int x = 1022; x > (1022 - 100); x--)
  {
    eeprom_byte_in = EEPROM.read(x);
    if (eeprom_byte_in < 255)
    {
      primary_serial_port->print(eeprom_byte_in, BYTE);
    }
    else
    {
      x = 0;
    }
  }
  primary_serial_port->println("\n");
  for (int x = 1022; x > (1022 - 100); x--)
  {
    eeprom_byte_in = EEPROM.read(x);
    if (eeprom_byte_in < 255)
    {
      primary_serial_port->print(eeprom_byte_in, HEX);
      primary_serial_port->write("   :");
      primary_serial_port->println(eeprom_byte_in, BYTE);
    }
    else
    {
      x = 0;
    }
  }

  while (1)
  {
  }
}
#endif

//-------------------------------------------------------------------------------------------------------
#ifdef FEATURE_ROTARY_ENCODER
int chk_rotary_encoder()
{

  static unsigned long timestamp[5];

  unsigned char pinstate = (digitalRead(rotary_pin2) << 1) | digitalRead(rotary_pin1);
  state = ttable[state & 0xf][pinstate];
  unsigned char result = (state & 0x30);
  if (result)
  {
    timestamp[0] = timestamp[1]; // Encoder step timer
    timestamp[1] = timestamp[2];
    timestamp[2] = timestamp[3];
    timestamp[3] = timestamp[4];
    timestamp[4] = millis();

    unsigned long elapsed_time = (timestamp[4] - timestamp[0]); // Encoder step time difference for 10's step

    if (result == DIR_CW)
    {
      if (elapsed_time < 250)
      {
        return 2;
      }
      else
      {
        return 1;
      };
    }
    if (result == DIR_CCW)
    {
      if (elapsed_time < 250)
      {
        return -2;
      }
      else
      {
        return -1;
      };
    }
  }
  return 0;
}

void check_rotary_encoder()
{

  int step = chk_rotary_encoder();

  if (step != 0)
  {
    if (keyer_machine_mode == KEYER_COMMAND_MODE)
      speed_change_command_mode(step);
    else
      speed_change(step);

#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
    last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM

// Start of Winkey Speed change mod for Rotary Encoder -- VE2EVN
#ifdef FEATURE_WINKEY_EMULATION
    if ((primary_serial_port_mode == SERIAL_WINKEY_EMULATION) && (winkey_host_open))
    {
      winkey_port_write(((configuration.wpm - pot_wpm_low_value) | 128), 0);
      winkey_last_unbuffered_speed_wpm = configuration.wpm;
    }
#endif
    // End of Winkey Speed change mod for Rotary Encoder -- VE2EVN

  } // if (step != 0)
}
#endif // FEATURE_ROTARY_ENCODER
//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_SIDETONE_SWITCH
void check_sidetone_switch()
{
  static unsigned long lastcheck = 0;

  if (millis() - lastcheck < 250)
    return;

  lastcheck = millis();

  int ss_read = sidetone_switch_value();

  if ((ss_read == HIGH) && ((configuration.sidetone_mode == SIDETONE_ON) ||
                            (configuration.sidetone_mode == SIDETONE_PADDLE_ONLY)))
  {
    return;
  }
  if ((ss_read == LOW) && configuration.sidetone_mode == SIDETONE_OFF)
  {
    return;
  }
  config_dirty = 1;
#ifdef FEATURE_SLEEP
  last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
  last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM

  if (ss_read == HIGH)
  {
    configuration.sidetone_mode = SIDETONE_ON;
    return;
  }
  if (ss_read == LOW)
  {
    configuration.sidetone_mode = SIDETONE_OFF;
    return;
  }
}

int sidetone_switch_value()
{
  return digitalRead(SIDETONE_SWITCH);
}

#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_HELL
void hell_test()
{
  for (byte h = 65; h < 91; h++)
  {
    transmit_hell_char(h);
  }
  transmit_hell_char('0');
  transmit_hell_char('1');
  transmit_hell_char('2');
  transmit_hell_char('3');
  transmit_hell_char('4');
  transmit_hell_char('5');
  transmit_hell_char('6');
  transmit_hell_char('7');
  transmit_hell_char('8');
  transmit_hell_char('9');
  transmit_hell_char('+');
  transmit_hell_char('-');
  transmit_hell_char('?');
  transmit_hell_char('/');
  transmit_hell_char('.');
  transmit_hell_char(',');
  transmit_hell_char('!'); // sp5iou
  //  transmit_hell_char('‘');  // this causes compiler warning; unicode character or something?
  transmit_hell_char('=');
  transmit_hell_char(')');
  transmit_hell_char('(');
  transmit_hell_char(':');
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_HELL
void transmit_hell_char(byte hellchar)
{

  // blank column
  for (byte w = 0; w < 14; w++)
  {
    transmit_hell_pixel(0);
  }

  if ((hellchar > 64) && (hellchar < 91))
  { // A - Z
    hellchar = ((hellchar - 65) * 9);
    transmit_hell_pixels(hell_font1, hellchar);
  }
  else
  {
    if ((hellchar > 47) && (hellchar < 58))
    { // 0 - 9
      hellchar = ((hellchar - 48) * 9);
      transmit_hell_pixels(hell_font2, hellchar);
    }
    else
    {
      switch (hellchar)
      {
      case '+':
        hellchar = 0;
        break;
      case '-':
        hellchar = 1;
        break;
      case '?':
        hellchar = 2;
        break;
      case '/':
        hellchar = 3;
        break;
      case '.':
        hellchar = 4;
        break;
      case ',':
        hellchar = 5;
        break;
        //        case '‘': hellchar = 6; break;  // this causes compiler warning; unicode character or something?
      case '=':
        hellchar = 7;
        break;
      case ')':
        hellchar = 8;
        break;
      case '(':
        hellchar = 9;
        break;
      case ':':
        hellchar = 10;
        break;
      default:
        hellchar = 11;
        break;
      }
      hellchar = hellchar * 9;
      transmit_hell_pixels(hell_font3, hellchar);
    }
  }

  // blank column
  for (byte w = 0; w < 14; w++)
  {
    transmit_hell_pixel(0);
  }
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_HELL
void transmit_hell_pixels(const char *hell_pixels, byte hellchar)
// void transmit_hell_pixels (prog_uchar* hell_pixels, byte hellchar)
{

  for (byte x = 0; x < 9; x++)
  {
    for (int y = 7; y > -1; y--)
    {
      if ((x < 8) || ((x == 8) && (y > 1)))
      { // drop the last 2 bits in byte 9
        if (bitRead(pgm_read_byte(hell_pixels + hellchar + x), y))
        {
          transmit_hell_pixel(1);
        }
        else
        {
          transmit_hell_pixel(0);
        }
      }
    }
  }
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_HELL
void transmit_hell_pixel(byte hellbit)
{
  sending_mode = AUTOMATIC_SENDING;
  *if (hellbit)
  {
    tx_and_sidetone_key(1);
  }
  else
  {
    tx_and_sidetone_key(0);
  }
  delayMicroseconds(hell_pixel_microseconds);
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_MEMORIES
void put_memory_button_in_buffer(byte memory_number_to_put_in_buffer)
{

  if (memory_number_to_put_in_buffer < number_of_memories)
  {
#ifdef DEBUG_MEMORIES
    debug_serial_port->print(F("put_memory_button_in_buffer: memory_number_to_put_in_buffer:"));
    debug_serial_port->println(memory_number_to_put_in_buffer, DEC);
#endif
    repeat_memory = 255;
    if ((millis() - last_memory_button_buffer_insert) > 400)
    { // don't do another buffer insert if we just did one - button debounce
#ifdef FEATURE_WINKEY_EMULATION
      if (winkey_sending && winkey_host_open)
      {
        winkey_port_write(0xc0 | winkey_sending | winkey_xoff, 0);
        winkey_interrupted = 1;
      }
#endif

      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(memory_number_to_put_in_buffer);
      last_memory_button_buffer_insert = millis();
    }
  }
  else
  {
#ifdef DEBUG_MEMORIES
    debug_serial_port->println(F("put_memory_button_in_buffer: bad memory_number_to_put_in_buffer"));
#endif
  }
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_SEQUENCER
void check_sequencer_tail_time()
{

  if (sequencer_ptt_inactive_time)
  {
    if (sequencer_1_pin)
    {
      if (sequencer_1_active && ((millis() - sequencer_ptt_inactive_time) >= configuration.ptt_inactive_to_sequencer_inactive_time[0]))
      {
        digitalWrite(sequencer_1_pin, sequencer_pins_inactive_state);
        sequencer_1_active = 0;
      }
    }
    if (sequencer_2_pin)
    {
      if (sequencer_2_active && ((millis() - sequencer_ptt_inactive_time) >= configuration.ptt_inactive_to_sequencer_inactive_time[1]))
      {
        digitalWrite(sequencer_2_pin, sequencer_pins_inactive_state);
        sequencer_2_active = 0;
      }
    }
    if (sequencer_3_pin)
    {
      if (sequencer_3_active && ((millis() - sequencer_ptt_inactive_time) >= configuration.ptt_inactive_to_sequencer_inactive_time[2]))
      {
        digitalWrite(sequencer_3_pin, sequencer_pins_inactive_state);
        sequencer_3_active = 0;
      }
    }
    if (sequencer_4_pin)
    {
      if (sequencer_4_active && ((millis() - sequencer_ptt_inactive_time) >= configuration.ptt_inactive_to_sequencer_inactive_time[3]))
      {
        digitalWrite(sequencer_4_pin, sequencer_pins_inactive_state);
        sequencer_4_active = 0;
      }
    }
    if (sequencer_5_pin)
    {
      if (sequencer_5_active && ((millis() - sequencer_ptt_inactive_time) >= configuration.ptt_inactive_to_sequencer_inactive_time[4]))
      {
        digitalWrite(sequencer_5_pin, sequencer_pins_inactive_state);
        sequencer_5_active = 0;
      }
    }
  }

  if (!sequencer_1_active && !sequencer_2_active && !sequencer_3_active && !sequencer_4_active && !sequencer_5_active)
  {
    sequencer_ptt_inactive_time = 0;
  }
}
#endif // FEATURE_SEQUENCER

//-------------------------------------------------------------------------------------------------------

void service_async_eeprom_write()
{

  // This writes one byte out to EEPROM each time it is called

#if !defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_ARCH_MBED) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO) || (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024))

  static byte last_async_eeprom_write_status = 0;
  static int ee = 0;
  static unsigned int i = 0;
  static const byte *p;

  if ((async_eeprom_write) && (!send_buffer_bytes) && (!ptt_line_activated) && (!dit_buffer) && (!dah_buffer) && (paddle_pin_read(paddle_left) == HIGH) && (paddle_pin_read(paddle_right) == HIGH))
  {
    if (last_async_eeprom_write_status)
    { // we have an asynchronous write to eeprom in progress

#if defined(_BOARD_PIC32_PINGUINO_) || defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
      if (EEPROM.read(ee) != *p)
      {
        EEPROM.write(ee, *p);
      }
      ee++;
      p++;
#else
      EEPROM.update(ee++, *p++);
#endif

      if (i < sizeof(configuration))
      {
#if defined(DEBUG_ASYNC_EEPROM_WRITE)
        debug_serial_port->print(F("service_async_eeprom_write: write: "));
        debug_serial_port->println(i);
#endif
        i++;
      }
      else
      { // we're done
        async_eeprom_write = 0;
        last_async_eeprom_write_status = 0;
#if defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
        EEPROM.commit();
#endif

#if defined(DEBUG_ASYNC_EEPROM_WRITE)
        debug_serial_port->println(F("service_async_eeprom_write: complete"));
#endif
      }
    }
    else
    { // we don't have one in progress - initialize things

      p = (const byte *)(const void *)&configuration;
      ee = 1; // starting point of configuration struct
      i = 0;
      last_async_eeprom_write_status = 1;
#if defined(DEBUG_ASYNC_EEPROM_WRITE)
      debug_serial_port->println(F("service_async_eeprom_write: init"));
#endif
    }
  }

#endif // #if !defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_ARCH_MBED)|| (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024))
}

//-------------------------------------------------------------------------------------------------------

int read_settings_from_eeprom()
{

  // returns 0 if eeprom had valid settings, returns 1 if eeprom needs initialized

#if defined(DEBUG_FORCE_RESET)
  return 1;
#endif

#if !defined(ARDUINO_SAM_DUE) || (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024))

#if defined(DEBUG_EEPROM_READ_SETTINGS)
  debug_serial_port->println(F("read_settings_from_eeprom: start"));
#endif

  if (EEPROM.read(0) == eeprom_magic_number)
  {

#if defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO)
#if defined(DEBUG_EEPROM_READ_SETTINGS)
    debug_serial_port->println(F("read_settings_from_eeprom: ARDUINO_RASPBERRY_PI_PICO"));
#endif
    EEPROM.get(1, configuration);
#else
    byte *p = (byte *)(void *)&configuration;
    unsigned int i;
    int ee = 1; // starting point of configuration struct
    for (i = 0; i < sizeof(configuration); i++)
    {
#if defined(DEBUG_EEPROM_READ_SETTINGS)
      debug_serial_port->print(F("read_settings_from_eeprom: read: i:"));
      debug_serial_port->print(i);
      debug_serial_port->print(F(":"));
      debug_serial_port->print(EEPROM.read(ee));
      debug_serial_port->println();
#endif
      *p++ = EEPROM.read(ee++);
    }
#endif

#ifndef FEATURE_SO2R_BASE
    switch_to_tx_silent(configuration.current_tx);
#endif

    config_dirty = 0;

#if defined(DEBUG_EEPROM_READ_SETTINGS)
    debug_serial_port->println(F("read_settings_from_eeprom: read complete"));
#endif
    return 0;
  }
  else
  {
#if defined(DEBUG_EEPROM_READ_SETTINGS)
    debug_serial_port->println(F("read_settings_from_eeprom: eeprom needs initialized"));
#endif
    return 1;
  }

#endif // #if !defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_ARCH_MBED)|| (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024))

#if defined(DEBUG_EEPROM_READ_SETTINGS)
  debug_serial_port->println(F("read_settings_from_eeprom: bypassed read - no eeprom"));
#endif

  return 1;
}

//-------------------------------------------------------------------------------------------------------

void speed_change_command_mode(int change)
{
  if (((configuration.wpm_command_mode + change) > wpm_limit_low) && ((configuration.wpm_command_mode + change) < wpm_limit_high))
  {
    configuration.wpm_command_mode = configuration.wpm_command_mode + change;
    config_dirty = 1;
  }

#ifdef FEATURE_DISPLAY
  lcd_center_print_timed(String(configuration.wpm_command_mode) + " wpm", 0, default_display_msg_delay);
#endif
}

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_DISPLAY
void lcd_center_print_timed_wpm()
{

#if defined(OPTION_ADVANCED_SPEED_DISPLAY)
  lcd_center_print_timed(String(configuration.wpm) + " wpm - " + (configuration.wpm * 5) + " cpm", 0, default_display_msg_delay);
  lcd_center_print_timed(String(1200 / configuration.wpm) + ":" + (((1200 / configuration.wpm) * configuration.dah_to_dit_ratio) / 100) + "ms 1:" + (float(configuration.dah_to_dit_ratio) / 100.00), 1, default_display_msg_delay);
#else
  lcd_center_print_timed(String(configuration.wpm) + " wpm", 0, default_display_msg_delay);
#endif
}
#endif
//-------------------------------------------------------------------------------------------------------

long get_cw_input_from_user(unsigned int exit_time_milliseconds)
{

  byte looping = 1;
  byte paddle_hit = 0;
  long cw_char = 0;
  unsigned long last_element_time = 0;
  byte button_hit = 0;
  unsigned long entry_time = millis();

  while (looping)
  {

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    wdt_reset();
#endif // OPTION_WATCHDOG_TIMER

#ifdef FEATURE_POTENTIOMETER
    if (configuration.pot_activated)
    {
      check_potentiometer();
    }
#endif

#ifdef FEATURE_ROTARY_ENCODER
    check_rotary_encoder();
#endif // FEATURE_ROTARY_ENCODER

    check_paddles();

    if (dit_buffer)
    {
      sending_mode = MANUAL_SENDING;
      send_dit();
      dit_buffer = 0;
      paddle_hit = 1;
      cw_char = (cw_char * 10) + 1;
      last_element_time = millis();
    }
    if (dah_buffer)
    {
      sending_mode = MANUAL_SENDING;
      send_dah();
      dah_buffer = 0;
      paddle_hit = 1;
      cw_char = (cw_char * 10) + 2;
      last_element_time = millis();
    }
    if ((paddle_hit) && (millis() > (last_element_time + (float(600 / configuration.wpm) * length_letterspace))))
    {
#ifdef DEBUG_GET_CW_INPUT_FROM_USER
      debug_serial_port->println(F("get_cw_input_from_user: hit length_letterspace"));
#endif
      looping = 0;
    }

    if ((!paddle_hit) && (exit_time_milliseconds) && ((millis() - entry_time) > exit_time_milliseconds))
    { // if we were passed an exit time and no paddle was hit, blow out of here
      return 0;
    }

#ifdef FEATURE_BUTTONS
    while (analogbuttonread(0))
    { // hit the button to get out of command mode if no paddle was hit
      looping = 0;
      button_hit = 1;
    }
#endif

#if defined(FEATURE_SERIAL)
    check_serial();
#endif

  } // while (looping)

  if (button_hit)
  {
#ifdef DEBUG_GET_CW_INPUT_FROM_USER
    debug_serial_port->println(F("get_cw_input_from_user: button_hit exit 9"));
#endif
    return 9;
  }
  else
  {
#ifdef DEBUG_GET_CW_INPUT_FROM_USER
    debug_serial_port->print(F("get_cw_input_from_user: exiting cw_char:"));
    debug_serial_port->println(cw_char);
#endif
    return cw_char;
  }
}

//-------------------------------------------------------------------------------------------------------
#ifdef FEATURE_COMMAND_MODE

void send_chars(char *buffer)
{

  int x = 0;

  while (buffer[x] != 0)
  {
    send_char(buffer[x++], 0);
  }
}

#endif

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_COMMAND_MODE) && !defined(OPTION_SIDETONE_DIGITAL_OUTPUT_NO_SQUARE_WAVE)
void command_sidetone_freq_adj()
{

  byte looping = 1;

#ifdef FEATURE_DISPLAY
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed(String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Sidetone " + String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
  }
#endif

  while (looping)
  {
    tone(sidetone_line, configuration.hz_sidetone);
    if (paddle_pin_read(paddle_left) == LOW)
    {
#ifdef FEATURE_DISPLAY
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
      sidetone_adj(-5);
#else
      sidetone_adj(5);
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
      if (LCD_COLUMNS < 9)
      {
        lcd_center_print_timed(String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
      }
      else
      {
        lcd_center_print_timed("Sidetone " + String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
      }
#else
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
      sidetone_adj(-1);
#else
      sidetone_adj(1);
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
#endif // FEATURE_DISPLAY
      delay(10);
    }
    if (paddle_pin_read(paddle_right) == LOW)
    {
#ifdef FEATURE_DISPLAY
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
      sidetone_adj(5);
#else
      sidetone_adj(-5);
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
      if (LCD_COLUMNS < 9)
      {
        lcd_center_print_timed(String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
      }
      else
      {
        lcd_center_print_timed("Sidetone " + String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
      }
#else
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
      sidetone_adj(1);
#else
      sidetone_adj(-1);
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
#endif // FEATURE_DISPLAY
      delay(10);
    }
    while ((paddle_pin_read(paddle_left) == LOW && paddle_pin_read(paddle_right) == LOW) || (analogbuttonread(0)))
    { // if paddles are squeezed or button0 pressed - exit
      looping = 0;
    }

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    wdt_reset();
#endif // OPTION_WATCHDOG_TIMER
  }
  while (paddle_pin_read(paddle_left) == LOW || paddle_pin_read(paddle_right) == LOW || analogbuttonread(0))
  {
  } // wait for all lines to go high
  noTone(sidetone_line);
}
#endif // FEATURE_COMMAND_MODE

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_COMMAND_MODE
void command_dah_to_dit_ratio_adjust()
{

  byte looping = 1;

#ifdef FEATURE_DISPLAY
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("Adj DTDR", 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Adj dah to dit", 0, default_display_msg_delay);
  }
#endif

  while (looping)
  {
    send_dit();
    send_dah();
    if (paddle_pin_read(paddle_left) == LOW)
    {
      adjust_dah_to_dit_ratio(10);
    }
    if (paddle_pin_read(paddle_right) == LOW)
    {
      adjust_dah_to_dit_ratio(-10);
    }
    while ((paddle_pin_read(paddle_left) == LOW && paddle_pin_read(paddle_right) == LOW) || (analogbuttonread(0)))
    { // if paddles are squeezed or button0 pressed - exit
      looping = 0;
    }

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    wdt_reset();
#endif // OPTION_WATCHDOG_TIMER
  }
  while (paddle_pin_read(paddle_left) == LOW || paddle_pin_read(paddle_right) == LOW || analogbuttonread(0))
  {
  } // wait for all lines to go high
  dit_buffer = 0;
  dah_buffer = 0;
}
#endif // FEATURE_COMMAND_MODE

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_COMMAND_MODE
void command_weighting_adjust()
{

  byte looping = 1;

#ifdef FEATURE_DISPLAY
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("Weight", 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Adj weighting", 0, default_display_msg_delay);
  }
#endif

  while (looping)
  {
    send_dit();
    send_dah();
    if (paddle_pin_read(paddle_left) == LOW)
    {
      configuration.weighting = configuration.weighting + 1;
      if (configuration.weighting > 90)
      {
        configuration.weighting = 90;
      }
    }
    if (paddle_pin_read(paddle_right) == LOW)
    {
      configuration.weighting = configuration.weighting - 1;
      if (configuration.weighting < 10)
      {
        configuration.weighting = 10;
      }
    }
    while ((paddle_pin_read(paddle_left) == LOW && paddle_pin_read(paddle_right) == LOW) || (analogbuttonread(0)))
    { // if paddles are squeezed or button0 pressed - exit
      looping = 0;
    }

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    wdt_reset();
#endif // OPTION_WATCHDOG_TIMER
  }
  while (paddle_pin_read(paddle_left) == LOW || paddle_pin_read(paddle_right) == LOW || analogbuttonread(0))
  {
  } // wait for all lines to go high
  dit_buffer = 0;
  dah_buffer = 0;
}
#endif // FEATURE_COMMAND_MODE

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_COMMAND_MODE
void command_keying_compensation_adjust()
{

  byte looping = 1;

#ifdef FEATURE_DISPLAY
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("Key Comp", 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Adj Keying Comp", 0, default_display_msg_delay);
  }
#endif

  while (looping)
  {
    send_dit();
    send_dah();
    if (paddle_pin_read(paddle_left) == LOW)
    {
      if (configuration.keying_compensation < 255)
      {
        configuration.keying_compensation++;
      }
    }
    if (paddle_pin_read(paddle_right) == LOW)
    {
      if (configuration.keying_compensation > 0)
      {
        configuration.keying_compensation--;
      }
    }
    while ((paddle_pin_read(paddle_left) == LOW && paddle_pin_read(paddle_right) == LOW) || (analogbuttonread(0)))
    { // if paddles are squeezed or button0 pressed - exit
      looping = 0;
    }

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    wdt_reset();
#endif // OPTION_WATCHDOG_TIMER
  }
  while (paddle_pin_read(paddle_left) == LOW || paddle_pin_read(paddle_right) == LOW || analogbuttonread(0))
  {
  } // wait for all lines to go high
  dit_buffer = 0;
  dah_buffer = 0;
  config_dirty = 1;
}
#endif // FEATURE_COMMAND_MODE

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_COMMAND_MODE
void command_speed_mode(byte mode)
{

  byte looping = 1;
#ifndef FEATURE_DISPLAY
  static char c[4];
#endif
  // static String wpm_string;

  if (mode == COMMAND_SPEED_MODE_KEYER_WPM)
  {
#ifdef FEATURE_DISPLAY
    if (LCD_COLUMNS < 9)
    {
      lcd_center_print_timed("AdjSpeed", 0, default_display_msg_delay);
    }
    else
    {
      lcd_center_print_timed("Adjust Speed", 0, default_display_msg_delay);
    }
#endif
    keyer_machine_mode = KEYER_COMMAND_MODE_SPEED_OVERRIDE;
  }
  else
  {
#ifdef FEATURE_DISPLAY
    if (LCD_COLUMNS < 9)
    {
      lcd_center_print_timed("CmdSpeed", 0, default_display_msg_delay);
    }
    else
    {
      lcd_center_print_timed("Cmd Mode Speed", 0, default_display_msg_delay);
    }
#endif
  }

  while (looping)
  {
    send_dit();
    if ((paddle_pin_read(paddle_left) == LOW))
    {
      if (mode == COMMAND_SPEED_MODE_KEYER_WPM)
      {
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
        speed_change(-1);
#else
        speed_change(1);
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
      }
      else
      {
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
        speed_change_command_mode(-1);
#else
        speed_change_command_mode(1);
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
      } // endif (mode == COMMAND_SPEED_MODE_KEYER_WPM)
    } // end while looping
    if ((paddle_pin_read(paddle_right) == LOW))
    {
      if (mode == COMMAND_SPEED_MODE_KEYER_WPM)
      {
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
        speed_change(1);
#else
        speed_change(-1);
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
      }
      else
      {
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
        speed_change_command_mode(1);
#else
        speed_change_command_mode(-1);
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
      }
    }
    while ((paddle_pin_read(paddle_left) == LOW && paddle_pin_read(paddle_right) == LOW) || (analogbuttonread(0))) // if paddles are squeezed or button0 pressed - exit
    {
      looping = 0;
    }

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    wdt_reset();
#endif // OPTION_WATCHDOG_TIMER
  }

  keyer_machine_mode = KEYER_COMMAND_MODE;

  dit_buffer = 0;
  dah_buffer = 0;

  while (paddle_pin_read(paddle_left) == LOW || paddle_pin_read(paddle_right) == LOW || analogbuttonread(0))
  {
  } // wait for all lines to go high
#ifndef FEATURE_DISPLAY
  // announce speed in CW
  if (mode == COMMAND_SPEED_MODE_KEYER_WPM)
  {
    sprintf(c, "%d", configuration.wpm);
  }
  else
  {
    sprintf(c, "%d", configuration.wpm_command_mode);
  }
  send_char(' ', KEYER_NORMAL);
  send_char(c[0], KEYER_NORMAL);
  send_char(c[1], KEYER_NORMAL);
#endif
}
#endif // FEATURE_COMMAND_MODE

//------------------------------------------------------------------

#if defined(FEATURE_MEMORIES) && defined(FEATURE_COMMAND_MODE)
void command_program_memory()
{
  int cw_char;
  cw_char = get_cw_input_from_user(0); // get another cw character from the user to find out which memory number
#ifdef DEBUG_COMMAND_MODE
  debug_serial_port->print(F("command_program_memory: cw_char: "));
  debug_serial_port->println(cw_char);
#endif
  if (cw_char > 0)
  {
    if ((cw_char == 12222) && (number_of_memories > 9))
    {                                                                    // we have a 1, this could be 1 or 1x
      cw_char = get_cw_input_from_user((1200 / configuration.wpm) * 14); // give the user some time to enter a second digit
      switch (cw_char)
      {
      case 0:
        program_memory(0);
        break; // we didn't get anything, it's a 1
      case 22222:
        program_memory(9);
        break;
      case 12222:
        program_memory(10);
        break;
      case 11222:
        program_memory(11);
        break;
      case 11122:
        program_memory(12);
        break;
      case 11112:
        program_memory(13);
        break;
      case 11111:
        program_memory(14);
        break;
      case 21111:
        program_memory(15);
        break;
      default:
        send_char('?', KEYER_NORMAL);
        break;
      }
    }
    else
    {
      switch (cw_char)
      {
      case 12222:
        program_memory(0);
        break; // 1 = memory 0
      case 11222:
        program_memory(1);
        break;
      case 11122:
        program_memory(2);
        break;
      case 11112:
        program_memory(3);
        break;
      case 11111:
        program_memory(4);
        break;
      case 21111:
        program_memory(5);
        break;
      case 22111:
        program_memory(6);
        break;
      case 22211:
        program_memory(7);
        break;
      case 22221:
        program_memory(8);
        break;
      // case 22222: program_memory(9); break;
      default:
        send_char('?', KEYER_NORMAL);
        break;
      }
    }
  }
}
#endif // FEATURE_COMMAND_MODE

//---------------------------------------------------------------------

#ifdef FEATURE_MEMORIES
void command_set_mem_repeat_delay()
{

  byte character_count = 0;
  ;
  int cw_char = 0;
  byte number_sent = 0;
  unsigned int repeat_value = 0;
  byte error_flag = 0;

  for (character_count = 0; character_count < 4; character_count++)
  {
    cw_char = get_cw_input_from_user(0);
    number_sent = (convert_cw_number_to_ascii(cw_char) - 48);
    if ((number_sent >= 0) && (number_sent < 10))
    {
      repeat_value = (repeat_value * 10) + number_sent;
    }
    else
    { // we got a bad value
      error_flag = 1;
      character_count = 5;
    }
  }

  if (error_flag)
  {
    boop();
  }
  else
  {
    configuration.memory_repeat_time = repeat_value;
    config_dirty = 1;
    beep();
  }
}
#endif // FEATURE_MEMORIES

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_COMMAND_MODE
void command_tuning_mode()
{

  byte looping = 1;
  byte latched = 0;

#ifdef FEATURE_DISPLAY
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("TuneMode", 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Tune Mode", 0, default_display_msg_delay);
  }
#endif

#if !defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
  send_char(command_mode_acknowledgement_character, 0);
#else
  send_chars((char *)command_t_tune_mode);
#endif

  key_tx = 1;
  while (looping)
  {

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    wdt_reset();
#endif // OPTION_WATCHDOG_TIMER

    if (paddle_pin_read(paddle_left) == LOW)
    {
      sending_mode = MANUAL_SENDING;
      tx_and_sidetone_key(1);
      ptt_key();
      latched = 0;
    }
    else
    {
      if (paddle_pin_read(paddle_left) == HIGH && latched == 0)
      {
        sending_mode = MANUAL_SENDING;
        tx_and_sidetone_key(0);
        ptt_unkey();
      }
    }

    if (paddle_pin_read(paddle_right) == LOW && latched == 0)
    {
      latched = 1;
      sending_mode = MANUAL_SENDING;
      tx_and_sidetone_key(1);
      ptt_key();
      while ((paddle_pin_read(paddle_right) == LOW) && (paddle_pin_read(paddle_left) == HIGH))
      {
        delay(10);
      }
    }
    else
    {
      if ((paddle_pin_read(paddle_right) == LOW) && (latched))
      {
        latched = 0;
        sending_mode = MANUAL_SENDING;
        tx_and_sidetone_key(0);
        ptt_unkey();
        while ((paddle_pin_read(paddle_right) == LOW) && (paddle_pin_read(paddle_left) == HIGH))
        {
          delay(10);
        }
      }
    }
    if ((analogbuttonread(0)) || ((paddle_pin_read(paddle_left) == LOW) && (paddle_pin_read(paddle_right) == LOW)))
    { // if paddles are squeezed or button0 pressed - exit
      looping = 0;
    }
  }
  sending_mode = MANUAL_SENDING;
  tx_and_sidetone_key(0);
  ptt_unkey();
  while (paddle_pin_read(paddle_left) == LOW || paddle_pin_read(paddle_right) == LOW || analogbuttonread(0))
  {
  } // wait for all lines to go high
  key_tx = 0;
  send_dit();
  dit_buffer = 0;
  dah_buffer = 0;
}
#endif // FEATURE_COMMAND_MODE

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_COMMAND_MODE)
void command_set_serial_number()
{

  byte character_count = 0;
  ;
  int cw_char = 0;
  byte number_sent = 0;
  unsigned int repeat_value = 0;
  byte error_flag = 0;

  for (character_count = 0; character_count < 4; character_count++)
  {
    cw_char = get_cw_input_from_user(0);
    number_sent = (convert_cw_number_to_ascii(cw_char) - 48);
    if ((number_sent >= 0) && (number_sent < 10))
    {
      repeat_value = (repeat_value * 10) + number_sent;
    }
    else
    { // we got a bad value
      error_flag = 1;
      character_count = 5;
    }
  }

  if (error_flag)
  {
    boop();
  }
  else
  {
    serial_number = repeat_value;
    // config_dirty = 1;
    beep();
  }
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_COMMAND_MODE
void command_mode()
{

  keyer_machine_mode = KEYER_COMMAND_MODE;

#ifdef DEBUG_COMMAND_MODE
  debug_serial_port->println(F("command_mode: entering"));
#endif

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
  wdt_disable();
#endif // OPTION_WATCHDOG_TIMER

  char weight_deci[] = "00";
  byte looping;
  byte button_that_was_pressed = 0;
  byte paddle_hit = 0;
  unsigned long last_element_time = 0;
  unsigned long cw_char;
  byte stay_in_command_mode = 1;
  byte speed_mode_before = speed_mode;
  speed_mode = SPEED_NORMAL; // put us in normal speed mode (life is too short to do command mode in QRSS)
  byte keyer_mode_before = configuration.keyer_mode;
  char c[4];
  if ((configuration.keyer_mode != IAMBIC_A) && (configuration.keyer_mode != IAMBIC_B) && (configuration.keyer_mode != ULTIMATIC))
  {
    // if ((configuration.keyer_mode != IAMBIC_A) && (configuration.keyer_mode != IAMBIC_B)) {
    configuration.keyer_mode = IAMBIC_B; // we got to be in iambic mode (life is too short to make this work in bug mode)
  }

  // command_mode_disable_tx = 0;  //Removed disable TX state every time Command Mode is entered - now set to actual key_tx status on CM entry (WD9DMP)

  boop_beep();
#ifdef command_mode_active_led
  if (command_mode_active_led)
  {
    digitalWrite(command_mode_active_led, HIGH);
  }
#endif // command_mode_active_led

#ifdef FEATURE_DISPLAY
  lcd_clear();
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("Cmd Mode", 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Command Mode", 0, default_display_msg_delay);
  }
#endif

#if defined(FEATURE_WINKEY_EMULATION) && defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE)
  winkey_breakin_status_byte_inhibit = 1;
#endif

  while (stay_in_command_mode)
  {
    cw_char = 0;
    looping = 1;
    while (looping)
    {
      int8_t button_temp = button_array.Pressed();

#ifdef FEATURE_DISPLAY
      service_display();
#endif

#ifdef FEATURE_POTENTIOMETER
      if (configuration.pot_activated)
      {
        check_potentiometer();
      }
#endif

#ifdef FEATURE_ROTARY_ENCODER
      check_rotary_encoder();
#endif // FEATURE_ROTARY_ENCODER

      check_paddles();

      if (dit_buffer)
      {
        sending_mode = MANUAL_SENDING;
        send_dit();
        dit_buffer = 0;
        paddle_hit = 1;
        cw_char = (cw_char * 10) + 1;
        last_element_time = millis();
      }
      if (dah_buffer)
      {
        sending_mode = MANUAL_SENDING;
        send_dah();
        dah_buffer = 0;
        paddle_hit = 1;
        cw_char = (cw_char * 10) + 2;
        last_element_time = millis();
      }
      if ((paddle_hit) && (millis() > (last_element_time + (float(600 / configuration.wpm) * length_letterspace))))
      {
#ifdef DEBUG_GET_CW_INPUT_FROM_USER
        debug_serial_port->println(F("get_cw_input_from_user: hit length_letterspace"));
#endif
        looping = 0;
      }
      if (button_temp >= 0)
      { // check for a button press
        looping = 0;
        cw_char = 9;
        delay(50);
        button_that_was_pressed = button_temp;
        while (button_array.Held(button_that_was_pressed))
        {
        }
      }

#if defined(FEATURE_SERIAL)
      configuration.keyer_mode = keyer_mode_before;
      check_serial();
      if ((configuration.keyer_mode != IAMBIC_A) && (configuration.keyer_mode != IAMBIC_B) && (configuration.keyer_mode != ULTIMATIC) && (configuration.keyer_mode != SINGLE_PADDLE))
      {
        configuration.keyer_mode = IAMBIC_B;
      }
#endif

    } // while (looping)
    // end new code

#ifdef DEBUG_COMMAND_MODE
    debug_serial_port->print(F("command_mode: cwchar: "));
    debug_serial_port->println(cw_char);
#endif
    if (cw_char > 0)
    { // do the command
      switch (cw_char)
      {
      case 12: // A - Iambic mode
        configuration.keyer_mode = IAMBIC_A;
        keyer_mode_before = IAMBIC_A;
        configuration.dit_buffer_off = 0;
        configuration.dah_buffer_off = 0;
        config_dirty = 1;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Iambic A", 0, default_display_msg_delay);
#endif
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
        send_chars((char *)command_a_iambic_a);
#else
        send_char(command_mode_acknowledgement_character, 0);
#endif
        break;

      case 2111: // B - Iambic mode
        configuration.keyer_mode = IAMBIC_B;
        keyer_mode_before = IAMBIC_B;
        configuration.dit_buffer_off = 0;
        configuration.dah_buffer_off = 0;
        config_dirty = 1;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Iambic B", 0, default_display_msg_delay);
#endif
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
        send_chars((char *)command_b_iambic_b);
#else
        send_char(command_mode_acknowledgement_character, 0);
#endif
        break;

      case 2121: // C - Single paddle mode
        configuration.keyer_mode = SINGLE_PADDLE;
        keyer_mode_before = SINGLE_PADDLE;
        config_dirty = 1;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Sngl Pdl", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Single Paddle", 0, default_display_msg_delay);
        }
#endif
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
        send_chars((char *)command_c_single_paddle);
#else
        send_char(command_mode_acknowledgement_character, 0);
#endif
        break;
      case 1: // E - announce spEed
#ifdef FEATURE_DISPLAY
#if defined(OPTION_ADVANCED_SPEED_DISPLAY)
        lcd_center_print_timed("Speed", 0, default_display_msg_delay);
        lcd_center_print_timed(String(configuration.wpm) + " wpm - " + (configuration.wpm * 5) + " cpm ", 1, default_display_msg_delay);
        if (LCD_ROWS > 2)
        {
          lcd_center_print_timed(String(1200 / configuration.wpm) + ":" + (((1200 / configuration.wpm) * configuration.dah_to_dit_ratio) / 100) + "ms 1:" + (float(configuration.dah_to_dit_ratio) / 100.00), 2, default_display_msg_delay);
        }
#else  // OPTION_ADVANCED_SPEED_DISPLAY_DISPLAY
        lcd_center_print_timed("Speed " + String(configuration.wpm) + " wpm", 0, default_display_msg_delay);
#endif // OPTION_ADVANCED_SPEED_DISPLAY_DISPLAY
#endif // FEATURE_DISPLAY
        delay(250);
        sprintf(c, "%d", configuration.wpm);
        send_char(c[0], KEYER_NORMAL);
        send_char(c[1], KEYER_NORMAL);
        break;
#ifndef OPTION_NO_ULTIMATIC
      case 211: // D - Ultimatic mode
        configuration.keyer_mode = ULTIMATIC;
        keyer_mode_before = ULTIMATIC;
        configuration.dit_buffer_off = 1;
        configuration.dah_buffer_off = 1;
        config_dirty = 1;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Ultimatc", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Ultimatic", 0, default_display_msg_delay);
        }
#endif
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
        send_chars((char *)command_d_ultimatic);
#else
        send_char(command_mode_acknowledgement_character, 0);
#endif
        break;
#endif // OPTION_NO_ULTIMATIC
#if !defined(OPTION_SIDETONE_DIGITAL_OUTPUT_NO_SQUARE_WAVE)
      case 1121:
        command_sidetone_freq_adj();
        break; // F - adjust sidetone frequency
#endif

      case 221: // G - switch to buG mode
        configuration.keyer_mode = BUG;
        keyer_mode_before = BUG;
        config_dirty = 1;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Bug", 0, default_display_msg_delay);
#endif
        send_char(command_mode_acknowledgement_character, 0);
        break;

      case 1111: // H - set weighting and dah to dit ratio to defaults
        configuration.weighting = default_weighting;
        configuration.dah_to_dit_ratio = initial_dah_to_dit_ratio;
        config_dirty = 1;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Dflt W+R", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Dflt Wght & Ratio", 0, default_display_msg_delay);
        }
#endif
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
        send_chars((char *)command_h_weight_dit_dah_ratio_default);
#else
        send_char(command_mode_acknowledgement_character, 0);
#endif
        break;
      case 11: // I - toggle TX enable / disable
        if (command_mode_disable_tx)
        {
          command_mode_disable_tx = 0;
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX On", 0, default_display_msg_delay);
#endif
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
          send_chars((char *)command_i_tx_on);
#endif
        }
        else
        {
          command_mode_disable_tx = 1;
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("TX Off", 0, default_display_msg_delay);
#endif
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
          send_chars((char *)command_i_tx_off);
#endif
        }
#if !defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
        send_char(command_mode_acknowledgement_character, 0);
#endif
        break;
      case 1222:
        command_dah_to_dit_ratio_adjust();
        break; // J - dah to dit ratio adjust
#ifndef OPTION_NO_ULTIMATIC
      case 212: // K - turn dit and dah buffers on and off
        if (configuration.keyer_mode == ULTIMATIC)
        {
          // send_char('O',KEYER_NORMAL);
          if (configuration.dit_buffer_off)
          {
            configuration.dit_buffer_off = 0;
            configuration.dah_buffer_off = 0;
#ifdef FEATURE_DISPLAY
            if (LCD_COLUMNS < 9)
            {
              lcd_center_print_timed("BffersOn", 0, default_display_msg_delay);
            }
            if (LCD_COLUMNS > 17)
            {
              lcd_center_print_timed("Dit Dah Buffers On", 0, default_display_msg_delay);
            }
            else
            {
              lcd_center_print_timed("Buffers On", 0, default_display_msg_delay);
            }
#endif
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
            send_chars((char *)command_k_dit_dah_buffers_on);
#else
            send_char(command_mode_acknowledgement_character, 0);
#endif
          }
          else
          {
            configuration.dit_buffer_off = 1;
            configuration.dah_buffer_off = 1;
#ifdef FEATURE_DISPLAY
            if (LCD_COLUMNS < 9)
            {
              lcd_center_print_timed("BffrsOff", 0, default_display_msg_delay);
            }
            if (LCD_COLUMNS > 18)
            {
              lcd_center_print_timed("Dit Dah Buffers Off", 0, default_display_msg_delay);
            }
            else
            {
              lcd_center_print_timed("Buffers Off", 0, default_display_msg_delay);
            }
#endif
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
            send_chars((char *)command_k_dit_dah_buffers_off);
#else
            send_char(command_mode_acknowledgement_character, 0);
#endif
          }
        }
        else
        {
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("Error", 0, default_display_msg_delay);
#endif
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
          send_chars((char *)command_error);
#else
          boop();
#endif
        }
        break;
#endif // OPTION_NO_ULTIMATIC

      case 1211:
        command_weighting_adjust();
        break; // L - weight adjust

      case 22: // M - Set command mode WPM
        command_speed_mode(COMMAND_SPEED_MODE_COMMAND_MODE_WPM);
        break;

#ifdef FEATURE_MEMORIES
      case 1221:
        command_program_memory();
        break; // P - program a memory
#endif         // FEATURE_MEMORIES  Acknowledgement: LA3ZA fixed!
      case 21: // N - paddle mode toggle
        if (configuration.paddle_mode == PADDLE_NORMAL)
        {
          configuration.paddle_mode = PADDLE_REVERSE;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("Pdl Rev", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Paddle Reverse", 0, default_display_msg_delay);
          }
#endif // FEATURE_DISPLAY
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
          send_chars((char *)command_n_paddle_reverse);
#endif
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("Pdl Norm", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Paddle Normal", 0, default_display_msg_delay);
          }
#endif // FEATURE_DISPLAY
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
          send_chars((char *)command_n_paddle_normal);
#endif
          configuration.paddle_mode = PADDLE_NORMAL;
        }
        config_dirty = 1;
#if !defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
        send_char(command_mode_acknowledgement_character, 0);
#endif
        break;
      case 222: // O - cycle through sidetone modes on, paddle only and off - enhanced by Marc-Andre, VE2EVN
        if (configuration.sidetone_mode == SIDETONE_PADDLE_ONLY)
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("ST Off", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Sidetone Off", 0, default_display_msg_delay);
          }
#endif
#ifdef DEBUG_COMMAND_MODE
          debug_serial_port->println(F("command_mode: SIDETONE_OFF"));
#endif
          configuration.sidetone_mode = SIDETONE_OFF;
#if !defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
          boop();
#else
          send_chars((char *)command_o_sidetone_off);
#endif
        }
        else if (configuration.sidetone_mode == SIDETONE_ON)
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("ST Pdl O", 0, default_display_msg_delay);
          }
          if (LCD_COLUMNS > 19)
          {
            lcd_center_print_timed("Sidetone Paddle Only", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Sidetone", 0, default_display_msg_delay);
            lcd_center_print_timed("Paddle Only", 1, default_display_msg_delay);
          }
#endif
#ifdef DEBUG_COMMAND_MODE
          debug_serial_port->println(F("command_mode: SIDETONE_PADDLE_ONLY"));
#endif
          configuration.sidetone_mode = SIDETONE_PADDLE_ONLY;
#if !defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
          beep();
          delay(200);
          beep();
#else
          send_chars((char *)command_o_sidetone_paddle_only);
#endif
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("ST On", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Sidetone On", 0, default_display_msg_delay);
          }
#endif
#ifdef DEBUG_COMMAND_MODE
          debug_serial_port->println(F("command_mode: SIDETONE_ON"));
#endif
          configuration.sidetone_mode = SIDETONE_ON;
#if !defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
          beep();
#else
          send_chars((char *)command_o_sidetone_on);
#endif
        }
        config_dirty = 1;
        break;
      case 2212: // Q - set keying compensation
        command_keying_compensation_adjust();
        break;

      case 121:
        command_set_serial_number();
        break; // R - Set serial number

      case 2:
        command_tuning_mode();
        break; // T - tuning mode
#ifdef FEATURE_POTENTIOMETER
      case 1112: // V - toggle pot active
        if (configuration.pot_activated)
        {
          configuration.pot_activated = 0;
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
          send_chars((char *)command_v_potentiometer_off);
#endif
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS > 14)
          {
            lcd_center_print_timed("Pot Deactivated", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Pot Off", 0, default_display_msg_delay);
          }
#endif
        }
        else
        {
          configuration.pot_activated = 1;
#if defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
          send_chars((char *)command_v_potentiometer_on);
#endif
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS > 13)
          {
            lcd_center_print_timed("Pot Activated", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Pot On", 0, default_display_msg_delay);
          }
#endif
        }
        config_dirty = 1;
#if !defined(FEATURE_COMMAND_MODE_ENHANCED_CMD_ACKNOWLEDGEMENT)
        send_char(command_mode_acknowledgement_character, 0);
#endif
        break;
#endif

      case 122: // W - change wpm
        command_speed_mode(COMMAND_SPEED_MODE_KEYER_WPM);
        break;

#ifdef FEATURE_MEMORIES
      case 2122:
        command_set_mem_repeat_delay();
        break; // Y - set memory repeat delay
#endif

      case 2112:
        stay_in_command_mode = 0;
        break; // X - exit command mode
#ifdef FEATURE_AUTOSPACE
      case 2211: // Z - Autospace
        if (configuration.autospace_active)
        {
          configuration.autospace_active = 0;
          config_dirty = 1;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("AutoSOff", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Autospace Off", 0, default_display_msg_delay);
          }
          send_char(command_mode_acknowledgement_character, 0);
#else
          send_char('O', KEYER_NORMAL);
          send_char('F', KEYER_NORMAL);
          send_char('F', KEYER_NORMAL);
#endif
        }
        else
        {
          configuration.autospace_active = 1;
          config_dirty = 1;
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS < 9)
          {
            lcd_center_print_timed("AutoS On", 0, default_display_msg_delay);
          }
          else
          {
            lcd_center_print_timed("Autospace On", 0, default_display_msg_delay);
          }
          send_char(command_mode_acknowledgement_character, 0);
#else
          send_char('O', KEYER_NORMAL);
          send_char('N', KEYER_NORMAL);
#endif
        }
        break;
#endif
        //        #ifdef FEATURE_MEMORIES
        //          case 12222: play_memory(0); break;
        //          case 11222: play_memory(1); break;
        //          case 11122: play_memory(2); break;
        //          case 11112: play_memory(3); break;
        //          case 11111: play_memory(4); break;
        //        #endif

#ifdef FEATURE_MEMORIES
      case 12222:
        if (number_of_memories > 0)
        {
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("Memory #1", 0, default_display_msg_delay);
#ifdef OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE
          command_display_memory(0);
#endif // OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE
#endif // FEATURE_DISPLAY
          play_memory(0);
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS > 17)
            lcd_center_print_timed("Invalid memory #1", 0, default_display_msg_delay);
          else if (LCD_COLUMNS > 13)
            lcd_center_print_timed("Invalid mem #1", 0, default_display_msg_delay);
#endif // FEATURE_DISPLAY
          beep_boop();
        }
        break;

      case 11222:
        if (number_of_memories > 1)
        {
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("Memory #2", 0, default_display_msg_delay);
#ifdef OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE
          command_display_memory(1);
#endif // OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE
#endif // FEATURE_DISPLAY
          play_memory(1);
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS > 17)
            lcd_center_print_timed("Invalid memory #2", 0, default_display_msg_delay);
          else if (LCD_COLUMNS > 13)
            lcd_center_print_timed("Invalid mem #2", 0, default_display_msg_delay);
#endif // FEATURE_DISPLAY
          beep_boop();
        }
        break;

      case 11122:
        if (number_of_memories > 2)
        {
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("Memory #3", 0, default_display_msg_delay);
#ifdef OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE
          command_display_memory(2);
#endif // OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE
#endif // FEATURE_DISPLAY
          play_memory(2);
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS > 17)
            lcd_center_print_timed("Invalid memory #3", 0, default_display_msg_delay);
          else if (LCD_COLUMNS > 13)
            lcd_center_print_timed("Invalid mem #3", 0, default_display_msg_delay);
#endif // FEATURE_DISPLAY
          beep_boop();
        }
        break;

      case 11112:
        if (number_of_memories > 3)
        {
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("Memory #4", 0, default_display_msg_delay);
#ifdef OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE
          command_display_memory(3);
#endif // OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE
#endif // FEATURE_DISPLAY
          play_memory(3);
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS > 17)
            lcd_center_print_timed("Invalid memory #4", 0, default_display_msg_delay);
          else if (LCD_COLUMNS > 13)
            lcd_center_print_timed("Invalid mem #4", 0, default_display_msg_delay);
#endif // FEATURE_DISPLAY
          beep_boop();
        }
        break;

      case 11111:
        if (number_of_memories > 4)
        {
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("Memory #5", 0, default_display_msg_delay);
#ifdef OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE
          command_display_memory(4);
#endif // OPTION_DISPLAY_MEMORY_CONTENTS_COMMAND_MODE
#endif // FEATURE_DISPLAY
          play_memory(4);
        }
        else
        {
#ifdef FEATURE_DISPLAY
          if (LCD_COLUMNS > 17)
            lcd_center_print_timed("Invalid memory #5", 0, default_display_msg_delay);
          else if (LCD_COLUMNS > 13)
            lcd_center_print_timed("Invalid mem #5", 0, default_display_msg_delay);
#endif // FEATURE_DISPLAY
          beep_boop();
        }
        break;
#endif // FEATURE_MEMORIES

      case 21112: // - : enable / disable PTT
        if (configuration.ptt_disabled)
        {
          configuration.ptt_disabled = 0;
        }
        else
        {
          configuration.ptt_disabled = 1;
        }
        config_dirty = 1;
        send_char(command_mode_acknowledgement_character, 0);
        break;

      case 121212:
        send_char(75, KEYER_NORMAL);
        send_char(51, KEYER_NORMAL);
        send_char(78, KEYER_NORMAL);
        send_char(71, KEYER_NORMAL);
        send_char(32, KEYER_NORMAL);
        send_char(55, KEYER_NORMAL);
        send_char(51, KEYER_NORMAL);
        send_char(32, KEYER_NORMAL);
        send_char(69, KEYER_NORMAL);
        send_char(69, KEYER_NORMAL);
        break;

#ifdef FEATURE_ALPHABET_SEND_PRACTICE // enhanced by Fred, VK2EFL
      case 111:                       // S - Alphabet Send Practice
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("SendPrct", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Send Practice", 0, default_display_msg_delay);
          if (LCD_ROWS > 1)
          {
            lcd_center_print_timed("Cmd button to exit", 1, default_display_msg_delay);
          }
        }
#endif
        beep();
        command_alphabet_send_practice();
        stay_in_command_mode = 0;
        break;
#endif // FEATURE_ALPHABET_SEND_PRACTICE

#ifdef FEATURE_COMMAND_MODE_PROGRESSIVE_5_CHAR_ECHO_PRACTICE
      case 112: // U - 5 Character Echo Practice
        command_progressive_5_char_echo_practice();
        stay_in_command_mode = 0;
        break;
#endif // FEATURE_COMMAND_MODE_PROGRESSIVE_5_CHAR_PRACTICE

      case 112211: // ? - status
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Status", 0, default_display_msg_delay);
        lcd_center_print_timed("wpm  " + String(configuration.wpm), 1, default_display_msg_delay);
        delay(250);
#endif // FEATURE_DISPLAY

        sprintf(c, "%d", configuration.wpm);
        send_char(c[0], KEYER_NORMAL);
        send_char(c[1], KEYER_NORMAL);
        send_char(' ', KEYER_NORMAL);

        switch (keyer_mode_before)
        {
        case IAMBIC_A:
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("mode  Iambic A", 1, default_display_msg_delay);
          delay(250);
#endif // FEATURE_DISPLAY
          send_char('A', KEYER_NORMAL);
          break;
        case IAMBIC_B:
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("mode  Iambic B", 1, default_display_msg_delay);
          delay(250);
#endif // FEATURE_DISPLAY
          send_char('B', KEYER_NORMAL);
          break;
        case SINGLE_PADDLE:
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("mode  Single Pdl", 1, default_display_msg_delay);
          delay(250);
#endif // FEATURE_DISPLAY
          send_char('S', KEYER_NORMAL);
          break;

#ifndef OPTION_NO_ULTIMATIC
        case ULTIMATIC:
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("mode  Ultimatic", 1, default_display_msg_delay);
          delay(250);
#endif // FEATURE_DISPLAY
          send_char('U', KEYER_NORMAL);
          break;
#endif // OPTION_NO_ULTIMATIC
        case BUG:
#ifdef FEATURE_DISPLAY
          lcd_center_print_timed("mode  Bug", 1, default_display_msg_delay);
          delay(250);
#endif // FEATURE_DISPLAY
          send_char('G', KEYER_NORMAL);
          break;
        } // switch(keyer_mode_before)
        send_char(' ', KEYER_NORMAL);
        send_char(' ', KEYER_NORMAL);

#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("weighting  " + String(configuration.weighting), 1, default_display_msg_delay);
        delay(250);
#endif // FEATURE_DISPLAY
        sprintf(c, "%d", configuration.weighting);
        send_char(c[0], KEYER_NORMAL);
        send_char(c[1], KEYER_NORMAL);
        send_char(' ', KEYER_NORMAL);

#ifdef FEATURE_DISPLAY
        strcpy(weight_deci, "00"); // reset the two character array
        if ((configuration.dah_to_dit_ratio % 100) != 0)
        {                                                                       // test if it is X.00
          weight_deci[0] = ((configuration.dah_to_dit_ratio % 100) / 10) + '0'; // for cases where decimal part is 10 to 99
          weight_deci[1] = (configuration.dah_to_dit_ratio % 10) + '0';         // get the single digit units part
        } // end if ((configuration.dah_to_dit_ratio % 100) != 0)
        lcd_center_print_timed("dah:dit  " + String(configuration.dah_to_dit_ratio / 100) + "." + weight_deci, 1, default_display_msg_delay);
        delay(250);
#endif // FEATURE_DISPLAY
        sprintf(c, "%d", configuration.dah_to_dit_ratio);
        send_char(c[0], KEYER_NORMAL);
        send_char('.', KEYER_NORMAL);
        send_char(c[1], KEYER_NORMAL);
        send_char(c[2], KEYER_NORMAL);
        send_char(' ', KEYER_NORMAL);
        break;

      case 9: // button was hit
              // Serial.print("Button - ");
              // Serial.println(button_that_was_pressed);

#if defined(FEATURE_MEMORIES)
        if (button_that_was_pressed == 0)
        { // button 0 was hit - exit
          stay_in_command_mode = 0;
        }
        else
        {
          program_memory(button_that_was_pressed - 1); // a button other than 0 was pressed - program a memory
        }
#else
        stay_in_command_mode = 0;
#endif
        break;
      default: // unknown command, send a ?
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("???", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Unknown command", 0, default_display_msg_delay);
        }
#endif
        send_char('?', KEYER_NORMAL);
        break;
      }
    }
  }
  beep_boop();
#if defined(FEATURE_WINKEY_EMULATION) && defined(OPTION_WINKEY_SEND_BREAKIN_STATUS_BYTE)
  winkey_breakin_status_byte_inhibit = 0;
#endif

#ifdef command_mode_active_led
  if (command_mode_active_led)
  {
    digitalWrite(command_mode_active_led, LOW);
  }
#endif // command_mode_active_led

  keyer_machine_mode = KEYER_NORMAL;
  // configuration.wpm = speed_wpm_before;
  speed_mode = speed_mode_before; // go back to whatever speed mode we were in before
  configuration.keyer_mode = keyer_mode_before;

#ifdef DEBUG_COMMAND_MODE
  if (command_mode_disable_tx)
  {
    debug_serial_port->print(F("command_mode: command_mode_disable_tx set"));
  }
#endif // DEBUG_COMMAND_MODE

#if defined(FEATURE_PADDLE_ECHO)
  paddle_echo_buffer = 0;
#endif

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
  wdt_enable(WDTO_4S);
#endif // OPTION_WATCHDOG_TIMER

#ifdef FEATURE_DISPLAY
  lcd.clear();
  for (int x = 0; x < LCD_ROWS; x++)
  { // as we exit, redraw the display that we had before we went into Command Mode
    lcd.setCursor(0, x);
    lcd.print(lcd_scroll_buffer[x]);
  }
#endif // FEATURE_DISPLAY
}
#endif // FEATURE_COMMAND_MODE

//-------------------------------------------------------------------------------------------------------

void command_display_memory(byte memory_number)
{

#if defined(FEATURE_DISPLAY) && defined(FEATURE_MEMORIES)
  byte eeprom_byte_read = 0;
  char memory_char[LCD_COLUMNS]; // an array of char to hold the retrieved memory from EEPROM
  int j;
  int fill_char; // a flag that is set if we need to fill the char array with spaces

  j = 0;
  fill_char = 0;
  for (int y = (memory_start(memory_number)); y < (memory_start(memory_number)) + LCD_COLUMNS; y++)
  {
    eeprom_byte_read = EEPROM.read(y); // read memory characters from EEPROM
    if (eeprom_byte_read == 255)
      fill_char = 1; // if it is the 'end of stored memory' character set a flag
    if (!fill_char)
      memory_char[j] = eeprom_byte_read; // save the retrieved character in the character array
    else
      memory_char[j] = ' '; // else fill the rest of the array with spaces
    j++;                    // move to the next character to be stored in the array
  } // end for
  lcd_center_print_timed(memory_char, 1, default_display_msg_delay); // write the retrieved char array to line 2 of LCD display
#endif                                                               // FEATURE_DISPLAY
} // end command_display_memory

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_COMMAND_MODE_PROGRESSIVE_5_CHAR_ECHO_PRACTICE) && defined(FEATURE_COMMAND_MODE)
void command_progressive_5_char_echo_practice()
{

  byte loop1 = 1;
  byte loop2 = 0;
  byte x = 0;
  byte user_send_loop = 0;
  String cw_to_send_to_user(10);
  char incoming_char = ' ';
  String user_sent_cw = "";
  byte paddle_hit = 0;
  unsigned long last_element_time = 0;
  unsigned long cw_char;
  byte speed_mode_before = speed_mode;
  byte keyer_mode_before = configuration.keyer_mode;
  byte progressive_step_counter;
  byte practice_mode;
  char word_buffer[10];

  speed_mode = SPEED_NORMAL; // put us in normal speed mode
  if ((configuration.keyer_mode != IAMBIC_A) && (configuration.keyer_mode != IAMBIC_B))
  {
    configuration.keyer_mode = IAMBIC_B; // we got to be in iambic mode (life is too short to make this work in bug mode)
  }
  randomSeed(millis());

#ifdef FEATURE_DISPLAY // enhanced by Fred, VK2EFL
  lcd_clear();
  if (LCD_COLUMNS > 17)
  {
    lcd_center_print_timed("Receive / Transmit", 0, default_display_msg_delay);
    lcd_center_print_timed("5 Char Echo Practice", 1, default_display_msg_delay);
    if (LCD_ROWS > 2)
    {
      lcd_center_print_timed("Cmd button to exit", 2, default_display_msg_delay);
    }
  }
  else
  {
    if (LCD_COLUMNS < 9)
    {
      lcd_center_print_timed("RXTX 5Ch", 0, default_display_msg_delay);
      if (LCD_ROWS > 1)
      {
        lcd_center_print_timed("EchoPrct", 1, default_display_msg_delay);
      }
    }
    else
    {
      lcd_center_print_timed("RX / TX 5 Char", 0, default_display_msg_delay);
      if (LCD_ROWS > 1)
      {
        lcd_center_print_timed("Echo Practice", 1, default_display_msg_delay);
      }
    }
  }
  service_display();
#else
  send_char('E', 0);
  send_char('C', 0);
  send_char('H', 0);
  send_char('O', 0);
  send_char(' ', 0);
  send_char(' ', 0);
  send_char(' ', 0);
  beep();
  beep();
#endif

  while (loop1)
  {
    // if (practice_mode_called == ECHO_MIXED){
    //   practice_mode = random(ECHO_2_CHAR_WORDS,ECHO_QSO_WORDS+1);
    // } else {
    //   practice_mode = practice_mode_called;
    // }

    // progressive_step_counter = 255;

    // switch (practice_mode){
    //   case CALLSIGN_INTERNATIONAL:
    //   case CALLSIGN_US:
    //   case CALLSIGN_EUROPEAN:
    //   case CALLSIGN_CANADA:
    //     cw_to_send_to_user = generate_callsign(practice_mode);
    //     break;
    //   case ECHO_PROGRESSIVE_5:
    cw_to_send_to_user = (char)random(65, 91);
    cw_to_send_to_user.concat((char)random(65, 91));
    cw_to_send_to_user.concat((char)random(65, 91));
    cw_to_send_to_user.concat((char)random(65, 91));
    cw_to_send_to_user.concat((char)random(65, 91));
    progressive_step_counter = 1;
    //     break;
    //   case ECHO_2_CHAR_WORDS:
    //     //word_index = random(0,s2_size);  // min parm is inclusive, max parm is exclusive
    //     strcpy_P(word_buffer, (char*)pgm_read_word(&(s2_table[random(0,s2_size)])));
    //     cw_to_send_to_user = word_buffer;
    //     break;
    //   case ECHO_3_CHAR_WORDS:
    //     //word_index = random(0,s3_size);  // min parm is inclusive, max parm is exclusive
    //     strcpy_P(word_buffer, (char*)pgm_read_word(&(s3_table[random(0,s3_size)])));
    //     cw_to_send_to_user = word_buffer;
    //     break;
    //   case ECHO_4_CHAR_WORDS:
    //     //word_index = random(0,s4_size);  // min parm is inclusive, max parm is exclusive
    //     strcpy_P(word_buffer, (char*)pgm_read_word(&(s4_table[random(0,s4_size)])));
    //     cw_to_send_to_user = word_buffer;
    //     break;
    //   case ECHO_NAMES:
    //     //word_index = random(0,name_size);  // min parm is inclusive, max parm is exclusive
    //     strcpy_P(word_buffer, (char*)pgm_read_word(&(name_table[random(0,name_size)])));
    //     cw_to_send_to_user = word_buffer;
    //     break;
    //   case ECHO_QSO_WORDS:
    //     //word_index = random(0,qso_size);  // min parm is inclusive, max parm is exclusive
    //     strcpy_P(word_buffer, (char*)pgm_read_word(&(qso_table[random(0,qso_size)])));
    //     cw_to_send_to_user = word_buffer;
    //     break;
    // } //switch (practice_mode)

    loop2 = 1;
    while (loop2)
    {
      user_send_loop = 1;
      user_sent_cw = "";
      cw_char = 0;
      x = 0;

      // send the CW to the user
      while ((x < (cw_to_send_to_user.length())) && (x < progressive_step_counter))
      {
        send_char(cw_to_send_to_user[x], KEYER_NORMAL);
        // test
        // port_to_use->print(cw_to_send_to_user[x]);
        //
        x++;
      }
      // port_to_use->println();

      while (user_send_loop)
      {
        // get their paddle input

#ifdef FEATURE_DISPLAY
        service_display();
#endif

#ifdef FEATURE_POTENTIOMETER
        if (configuration.pot_activated)
        {
          check_potentiometer();
        }
#endif

#ifdef FEATURE_ROTARY_ENCODER
        check_rotary_encoder();
#endif // FEATURE_ROTARY_ENCODER

        check_paddles();

        if (dit_buffer)
        {
          sending_mode = MANUAL_SENDING;
          send_dit();
          dit_buffer = 0;
          paddle_hit = 1;
          cw_char = (cw_char * 10) + 1;
          last_element_time = millis();
        }
        if (dah_buffer)
        {
          sending_mode = MANUAL_SENDING;
          send_dah();
          dah_buffer = 0;
          paddle_hit = 1;
          cw_char = (cw_char * 10) + 2;
          last_element_time = millis();
        }

        // have we hit letterspace time (end of a letter?)
        if ((paddle_hit) && (millis() > (last_element_time + (float(600 / configuration.wpm) * length_letterspace))))
        {
#ifdef DEBUG_PRACTICE_CMD_MODE
          debug_serial_port->println(F("command_progressive_5_char_echo_practice: user_send_loop: hit length_letterspace"));
#endif
          incoming_char = convert_cw_number_to_ascii(cw_char);
          // port_to_use->print(incoming_char);
          user_sent_cw.concat(incoming_char);
          cw_char = 0;
          paddle_hit = 0;
          // TODO - print it to serial and lcd
        }

        // do we have all the characters from the user? - if so, get out of user_send_loop
        if ((user_sent_cw.length() >= cw_to_send_to_user.length()) || ((progressive_step_counter < 255) && (user_sent_cw.length() == progressive_step_counter)))
        {
          user_send_loop = 0;
          // port_to_use->println();
        }

        // does the user want to exit?
        while (analogbuttonread(0))
        {
          user_send_loop = 0;
          loop1 = 0;
          loop2 = 0;
          if (correct_answer_led)
            digitalWrite(correct_answer_led, LOW); // clear the LEDs as we exit
          if (wrong_answer_led)
            digitalWrite(wrong_answer_led, LOW);
        }

      } // while (user_send_loop)

      if (loop1 && loop2)
      {
        if (progressive_step_counter < 255)
        { // we're in progressive mode
          if (user_sent_cw.substring(0, progressive_step_counter) == cw_to_send_to_user.substring(0, progressive_step_counter))
          { // we get here if the character entered is correct
            if (progressive_step_counter < 6)
            { // if the step counter is less than 6 then we have not finished the 5 character string
              if (correct_answer_led)
                digitalWrite(correct_answer_led, HIGH); // set the correct answer LED high
              if (wrong_answer_led)
                digitalWrite(wrong_answer_led, LOW); // clear the wrong answer LED
              beep();                                // and beep
#ifdef FEATURE_DISPLAY
              if (LCD_COLUMNS > 17)
                lcd_center_print_timed("Success! " + String(progressive_step_counter) + " correct", 0, default_display_msg_delay);
              else
                lcd_center_print_timed("Success!", 0, default_display_msg_delay);
#endif // FEATURE_DISPLAY
              send_char(' ', 0);
              send_char(' ', 0);
              progressive_step_counter++;
            } // end if (progressive_step_counter < 6)
            if (progressive_step_counter == 6)
            { // we get here if the five character string is correct
              loop2 = 0;
              if (correct_answer_led)
                digitalWrite(correct_answer_led, HIGH); // set the correct answer LED high
#ifdef FEATURE_DISPLAY
              lcd_center_print_timed("Success!", 0, default_display_msg_delay);
              if (LCD_COLUMNS > 17)
                lcd_center_print_timed("5 characters correct", 1, default_display_msg_delay);
              if (LCD_COLUMNS < 17)
                lcd_center_print_timed("5 char correct", 1, default_display_msg_delay);
#endif                                        // FEATURE_DISPLAY
              unsigned int NEWtone = 400;     // the initial tone frequency for the tone sequence
              unsigned int TONEduration = 50; // define the duration of each tone element in the tone sequence to drive a speaker
              for (int k = 0; k < 6; k++)
              {                               // a loop to generate some increasing tones
                tone(sidetone_line, NEWtone); // generate a tone on the speaker pin
                delay(TONEduration);          // hold the tone for the specified delay period
                noTone(sidetone_line);        // turn off the tone
                NEWtone = NEWtone * 1.25;     // calculate a new value for the tone frequency
              } // end for
              send_char(' ', 0);
              send_char(' ', 0);
              if (correct_answer_led)
                digitalWrite(correct_answer_led, LOW); // clear the correct answer LED
            } // end if (progressive_step_counter == 6)
          }
          else
          { // we get here if the character entered is wrong
            if (wrong_answer_led)
              digitalWrite(wrong_answer_led, HIGH); // set the wrong answer LED high
            if (correct_answer_led)
              digitalWrite(correct_answer_led, LOW); // clear the correct answer LED
            boop();
            send_char(' ', 0);
            send_char(' ', 0);
          }
        }
        else
        { // I don't think we ever get here, unless the progressive_step_counter is more than 255, but it is reset as Loop1 starts
          if (user_sent_cw == cw_to_send_to_user)
          { // correct answer
            beep();
            send_char(' ', 0);
            send_char(' ', 0);
            loop2 = 0;
          }
          else
          { // wrong answer
            boop();
            send_char(' ', 0);
            send_char(' ', 0);
          }
        } // if (progressive_step_counter < 255)
      } // if (loop1 && loop2)
    } // loop2
  } // loop1

  speed_mode = speed_mode_before;
  configuration.keyer_mode = keyer_mode_before;
  paddle_echo_buffer = 0;
}
#endif // defined(FEATURE_COMMAND_MODE_PROGRESSIVE_5_CHAR_ECHO_PRACTICE) && defined(FEATURE_COMMAND_MODE)

//-------------------------------------------------------------------------------------------------------

#ifndef FEATURE_DISPLAY
void send_tx()
{

  send_char('T', KEYER_NORMAL);
  send_char('X', KEYER_NORMAL);
}
#endif

//------------------------------------------------------------------

void switch_to_tx(byte tx)
{

#ifdef FEATURE_MEMORIES
  repeat_memory = 255;
#endif

#ifdef FEATURE_DISPLAY
  switch (tx)
  {
  case 1:
    if ((ptt_tx_1) || (tx_key_line_1))
    {
      switch_to_tx_silent(1);
      lcd_center_print_timed("TX 1", 0, default_display_msg_delay);
    }
    break;
  case 2:
    if ((ptt_tx_2) || (tx_key_line_2))
    {
      switch_to_tx_silent(2);
      lcd_center_print_timed("TX 2", 0, default_display_msg_delay);
    }
    break;
  case 3:
    if ((ptt_tx_3) || (tx_key_line_3))
    {
      switch_to_tx_silent(3);
      lcd_center_print_timed("TX 3", 0, default_display_msg_delay);
    }
    break;
  case 4:
    if ((ptt_tx_4) || (tx_key_line_4))
    {
      switch_to_tx_silent(4);
      lcd_center_print_timed("TX 4", 0, default_display_msg_delay);
    }
    break;
  case 5:
    if ((ptt_tx_5) || (tx_key_line_5))
    {
      switch_to_tx_silent(5);
      lcd_center_print_timed("TX 5", 0, default_display_msg_delay);
    }
    break;
  case 6:
    if ((ptt_tx_6) || (tx_key_line_6))
    {
      switch_to_tx_silent(6);
      lcd_center_print_timed("TX 6", 0, default_display_msg_delay);
    }
    break;
  }
#else
  switch (tx)
  {
  case 1:
    if ((ptt_tx_1) || (tx_key_line_1))
    {
      switch_to_tx_silent(1);
      send_tx();
      send_char('1', KEYER_NORMAL);
    }
    break;
  case 2:
    if ((ptt_tx_2) || (tx_key_line_2))
    {
      switch_to_tx_silent(2);
      send_tx();
      send_char('2', KEYER_NORMAL);
    }
    break;
  case 3:
    if ((ptt_tx_3) || (tx_key_line_3))
    {
      switch_to_tx_silent(3);
      send_tx();
      send_char('3', KEYER_NORMAL);
    }
    break;
  case 4:
    if ((ptt_tx_4) || (tx_key_line_4))
    {
      switch_to_tx_silent(4);
      send_tx();
      send_char('4', KEYER_NORMAL);
    }
    break;
  case 5:
    if ((ptt_tx_5) || (tx_key_line_5))
    {
      switch_to_tx_silent(5);
      send_tx();
      send_char('5', KEYER_NORMAL);
    }
    break;
  case 6:
    if ((ptt_tx_6) || (tx_key_line_6))
    {
      switch_to_tx_silent(6);
      send_tx();
      send_char('6', KEYER_NORMAL);
    }
    break;
  }
#endif
}

//------------------------------------------------------------------

void initialize_analog_button_array()
{
#ifdef FEATURE_BUTTONS

#ifdef DEBUG_BUTTONS
  debug_serial_port->print("initialize_analog_button_array: ");
#endif

#if defined(FEATURE_DL2SBA_BANKSWITCH)
  button_array.Add(0, 0);
  button_array.Add(1, 3);
  button_array.Add(2, 2);
  button_array.Add(3, 1);
  button_array.Add(4, 9);
  button_array.Add(5, 8);
  button_array.Add(6, 7);
  button_array.Add(7, 6);
  button_array.Add(8, 5);
  button_array.Add(9, 4);

#elseif defined(OPTION_DFROBOT_LCD_COMMAND_BUTTONS)
  button_array.Add(0, dfrobot_btnSELECT, dfrobot_btnLEFT_analog, dfrobot_btnSELECT_analog);
  button_array.Add(1, dfrobot_btnLEFT, dfrobot_btnDOWN_analog, dfrobot_btnLEFT_analog);
  button_array.Add(2, dfrobot_btnDOWN, dfrobot_btnUP_analog, dfrobot_btnDOWN_analog);
  button_array.Add(3, dfrobot_btnUP, dfrobot_btnRIGHT_analog, dfrobot_btnUP_analog);
  button_array.Add(4, dfrobot_btnRIGHT, 0, dfrobot_btnRIGHT_analog);
#else // FEATURE_DL2SBA_BANKSWITCH
  button_array.AddAll();

#endif // FEATURE_DL2SBA_BANKSWITCH
#endif // FEATURE_BUTTONS
}

//------------------------------------------------------------------

#ifdef FEATURE_BUTTONS
void check_buttons()
{

#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering check_buttons"));
#endif

  static long last_button_action = 0;
  int analogbuttontemp = button_array.Pressed();
  long button_depress_time;
  byte paddle_was_hit = 0;
  byte previous_sidetone_mode = 0;

  if (analogbuttontemp < 0)
  { // no button pressed.
    return;
  }

#ifdef FEATURE_MEMORIES
  repeat_memory = 255;
#endif

  button_depress_time = button_array.last_pressed_ms;

  while (button_array.Held(analogbuttontemp, button_depress_time + 1000))
  {
    if ((paddle_pin_read(paddle_left) == LOW) || (paddle_pin_read(paddle_right) == LOW))
    {
      button_depress_time = 1001; // if button 0 is held and a paddle gets hit, assume we have a hold and shortcut out
    }
  }
  if ((millis() - button_depress_time) < 500)
  { // regular button press
#ifdef FEATURE_COMMAND_MODE
    if (analogbuttontemp == 0)
    {

      command_mode_disable_tx = !key_tx; // Added to sync the Command Mode entry state to actual key_tx state in case changed by CLI or keyboard (WD9DMP)
      key_tx = 0;
      command_mode();
      if (command_mode_disable_tx)
      {
        // key_tx = !store_key_tx; //Inverting pre-command mode state seems to cause Command Mode sync issues (WD9DMP)
        key_tx = 0; // Added this line to explicitly disable key_tx if command_mode_disable_tx is set after exiting Command Mode (WD9DMP)
      }
      else
      {
        key_tx = 1;
      }
    }
#endif // FEATURE_COMMAND_MODE
#ifdef FEATURE_MEMORIES
    if ((analogbuttontemp > 0) && (analogbuttontemp < (number_of_memories + 1)) && ((millis() - button_last_add_to_send_buffer_time) > 400))
    {

#ifdef FEATURE_WINKEY_EMULATION
#ifndef OPTION_WINKEY_2_SUPPORT
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(analogbuttontemp - 1);
#else  // OPTION_WINKEY_2_SUPPORT
      if ((winkey_host_open) && (wk2_mode == 2))
      { // if winkey is open and in wk2 mode, tell it about the button press
        byte winkey_byte_to_send = 0xc8;
        switch (analogbuttontemp)
        {
        case 1:
          winkey_byte_to_send = winkey_byte_to_send | 1;
          break;
        case 2:
          winkey_byte_to_send = winkey_byte_to_send | 2;
          break;
        case 3:
          winkey_byte_to_send = winkey_byte_to_send | 4;
          break;
        case 4:
          winkey_byte_to_send = winkey_byte_to_send | 16;
          break;
        }
        winkey_port_write(winkey_byte_to_send, 0);
        winkey_port_write(0xc8, 0); // tell it that the button is unpressed
      }
      else
      { // otherwise, have the buttons act as normal
        add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
        add_to_send_buffer(analogbuttontemp - 1);
      }
#endif // OPTION_WINKEY_2_SUPPORT
#else  // FEATURE_WINKEY_EMULATION
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(analogbuttontemp - 1);
#endif // FEATURE_WINKEY_EMULATION

      button_last_add_to_send_buffer_time = millis();
#ifdef DEBUG_BUTTONS
      debug_serial_port->print(F("\ncheck_buttons: add_to_send_buffer: "));
      debug_serial_port->println(analogbuttontemp - 1);
#endif // DEBUG_BUTTONS
    }
#endif // ifdef FEATURE_MEMORIES
  }
  else
  { // if ((millis() - button_depress_time) < 500)   -- Button hold down

    if (analogbuttontemp == 0)
    {
      key_tx = 0;
      // do stuff if this is a command button hold down
      while (button_array.Held(analogbuttontemp))
      {
        if (paddle_pin_read(paddle_left) == LOW)
        {
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
          speed_change(-1); // left paddle decrease speed
#else
          speed_change(1); // left paddle increase speed
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
          previous_sidetone_mode = configuration.sidetone_mode;
          configuration.sidetone_mode = SIDETONE_ON;
          sending_mode = MANUAL_SENDING;
          send_dit();
          configuration.sidetone_mode = previous_sidetone_mode;
          // speed_button_cmd_executed = 1;
          dit_buffer = 0;

#ifdef DEBUG_BUTTONS
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
          debug_serial_port->println(F("\ncheck_buttons: speed_change(-1)"));
#else
          debug_serial_port->println(F("\ncheck_buttons: speed_change(1)"));
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
#endif // DEBUG_BUTTONS

#if defined(FEATURE_WINKEY_EMULATION) && defined(FEATURE_POTENTIOMETER)
          if ((primary_serial_port_mode == SERIAL_WINKEY_EMULATION) && (winkey_host_open))
          {
            winkey_port_write(((configuration.wpm - pot_wpm_low_value) | 128), 0);
            winkey_last_unbuffered_speed_wpm = configuration.wpm;
          }
#endif
        }
        if (paddle_pin_read(paddle_right) == LOW)
        {
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
          speed_change(1); // right paddle increase speed
#else
          speed_change(-1); // right paddle decrease speed
#endif
          previous_sidetone_mode = configuration.sidetone_mode;
          configuration.sidetone_mode = SIDETONE_ON;
          sending_mode = MANUAL_SENDING;
          send_dah();
          configuration.sidetone_mode = previous_sidetone_mode;
          dah_buffer = 0;

#ifdef DEBUG_BUTTONS
#ifdef OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
          debug_serial_port->println(F("\ncheck_buttons: speed_change(1)"));
#else
          debug_serial_port->println(F("\ncheck_buttons: speed_change(-1)"));
#endif // OPTION_SWAP_PADDLE_PARAMETER_CHANGE_DIRECTION
#endif // DEBUG_BUTTONS

#if defined(FEATURE_WINKEY_EMULATION) && defined(FEATURE_POTENTIOMETER)
          if ((primary_serial_port_mode == SERIAL_WINKEY_EMULATION) && (winkey_host_open))
          {
            winkey_port_write(((configuration.wpm - pot_wpm_low_value) | 128), 0);
            winkey_last_unbuffered_speed_wpm = configuration.wpm;
          }
#endif
        } // if (paddle_pin_read(paddle_right) == LOW) {
      }
      key_tx = 1;
    } // (analogbuttontemp == 0)
    if ((analogbuttontemp > 0) && (analogbuttontemp < analog_buttons_number_of_buttons))
    {
      while (button_array.Held(analogbuttontemp))
      {
        if (((paddle_pin_read(paddle_left) == LOW) || (paddle_pin_read(paddle_right) == LOW)) && (analogbuttontemp < (number_of_memories + 1)))
        {
#ifdef FEATURE_MEMORIES
          repeat_memory = analogbuttontemp - 1;
          last_memory_repeat_time = 0;
#ifdef DEBUG_BUTTONS
          debug_serial_port->print(F("\ncheck_buttons: repeat_memory:"));
          debug_serial_port->println(repeat_memory);
#endif // DEBUG_BUTTONS
#endif
          paddle_was_hit = 1;
        }
      } // while (button_array.Held(analogbuttontemp)) {

      if (!paddle_was_hit)
      { // if no paddle was hit, this was a button hold to change transmitters
        key_tx = 0;
        previous_sidetone_mode = configuration.sidetone_mode;
        configuration.sidetone_mode = SIDETONE_ON;
        switch_to_tx(analogbuttontemp);
        key_tx = 1;
        configuration.sidetone_mode = previous_sidetone_mode;
      }
    } // if ((analogbuttontemp > 0) && (analogbuttontemp < analog_buttons_number_of_buttons)) {
    //}                                  // button hold
  }
  last_button_action = millis();

#ifdef FEATURE_SLEEP
  last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
  last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
}
#endif // FEATURE_BUTTONS

//-------------------------------------------------------------------------------------------------------

void service_dit_dah_buffers()
{
#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering service_dit_dah_buffers"));
#endif

  if (keyer_machine_mode == BEACON)
  {
    return;
  }

  if (automatic_sending_interruption_time != 0)
  {
    if ((millis() - automatic_sending_interruption_time) > (configuration.paddle_interruption_quiet_time_element_lengths * (1200 / configuration.wpm)))
    {
      automatic_sending_interruption_time = 0;
      sending_mode = MANUAL_SENDING;
    }
    else
    {
      dit_buffer = 0;
      dah_buffer = 0;
      return;
    }
  }

  static byte bug_dah_flag = 0;

#ifdef FEATURE_PADDLE_ECHO
  static unsigned long bug_dah_key_down_time = 0;
#endif // FEATURE_PADDLE_ECHO

  if ((configuration.keyer_mode == IAMBIC_A) || (configuration.keyer_mode == IAMBIC_B) || (configuration.keyer_mode == ULTIMATIC) || (configuration.keyer_mode == SINGLE_PADDLE))
  {
    if ((configuration.keyer_mode == IAMBIC_A) && (iambic_flag) && (paddle_pin_read(paddle_left)) && (paddle_pin_read(paddle_right)))
    {
      iambic_flag = 0;
      dit_buffer = 0;
      dah_buffer = 0;
    }
    else
    {
      if (dit_buffer)
      {
        dit_buffer = 0;
        sending_mode = MANUAL_SENDING;
        send_dit();
      }
      if (dah_buffer)
      {
        dah_buffer = 0;
        sending_mode = MANUAL_SENDING;
        send_dah();
      }
    }
  }
  else
  {
    if (configuration.keyer_mode == BUG)
    {
      if (dit_buffer)
      {
        dit_buffer = 0;
        sending_mode = MANUAL_SENDING;
        send_dit();
      }

      if (dah_buffer)
      {
        dah_buffer = 0;
        if (!bug_dah_flag)
        {
          sending_mode = MANUAL_SENDING;
          tx_and_sidetone_key(1);
          bug_dah_flag = 1;
#ifdef FEATURE_PADDLE_ECHO
          bug_dah_key_down_time = millis();
#endif // FEATURE_PADDLE_ECHO
        }
      }
      else
      {
        if (bug_dah_flag)
        {
          sending_mode = MANUAL_SENDING;
          tx_and_sidetone_key(0);
#ifdef FEATURE_PADDLE_ECHO
          if ((millis() - bug_dah_key_down_time) > (0.5 * (1200.0 / configuration.wpm)))
          {
            if ((millis() - bug_dah_key_down_time) > (2 * (1200.0 / configuration.wpm)))
            {
              paddle_echo_buffer = (paddle_echo_buffer * 10) + 2;
            }
            else
            {
              paddle_echo_buffer = (paddle_echo_buffer * 10) + 1;
            }
            paddle_echo_buffer_decode_time = millis() + (((float)3000.0 / (float)configuration.wpm) * ((float)configuration.cw_echo_timing_factor / (float)100));
          }
#endif // FEATURE_PADDLE_ECHO
          bug_dah_flag = 0;
        }
      }
#ifdef FEATURE_DEAD_OP_WATCHDOG
      dah_counter = 0;
#endif
    }
    else
    {
      if (configuration.keyer_mode == STRAIGHT)
      {
        if (dit_buffer)
        {
          dit_buffer = 0;
          sending_mode = MANUAL_SENDING;
          tx_and_sidetone_key(1);
        }
        else
        {
          sending_mode = MANUAL_SENDING;
          tx_and_sidetone_key(0);
        }
#ifdef FEATURE_DEAD_OP_WATCHDOG
        dit_counter = 0;
#endif
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------------

int uppercase(int charbytein)
{
  if (((charbytein > 96) && (charbytein < 123)) || ((charbytein > 223) && (charbytein < 255)))
  {
    charbytein = charbytein - 32;
  }
  if (charbytein == 158)
  {
    charbytein = 142;
  } // ž -> Ž
  if (charbytein == 154)
  {
    charbytein = 138;
  } // š -> Š

  return charbytein;
}

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_SERIAL)
#ifdef FEATURE_COMMAND_LINE_INTERFACE
void serial_qrss_mode()
{
  byte looping = 1;
  byte incoming_serial_byte;
  byte numbers[4];
  byte numberindex = 0;
  String numberstring;
  byte error = 0;

  while (looping)
  {
    if (primary_serial_port->available() == 0)
    { // wait for the next keystroke
      if (keyer_machine_mode == KEYER_NORMAL)
      { // might as well do something while we're waiting
        check_paddles();
        service_dit_dah_buffers();
        // check_the_memory_buttons();
      }
    }
    else
    {

      incoming_serial_byte = primary_serial_port->read();
      if ((incoming_serial_byte > 47) && (incoming_serial_byte < 58))
      { // ascii 48-57 = "0" - "9")
        numberstring = numberstring + incoming_serial_byte;
        numbers[numberindex] = incoming_serial_byte;
        //        primary_serial_port->write("numberindex:");
        //        primary_serial_port->print(numberindex,DEC);
        //        primary_serial_port->write("     numbers:");
        //        primary_serial_port->println(numbers[numberindex],DEC);
        numberindex++;
        if (numberindex > 2)
        {
          looping = 0;
          error = 1;
        }
      }
      else
      {
        if (incoming_serial_byte == 13)
        { // carriage return - get out
          looping = 0;
        }
        else
        { // bogus input - error out
          looping = 0;
          error = 1;
        }
      }
    }
  }

  if (error)
  {
    primary_serial_port->println(F("Error..."));
    while (primary_serial_port->available() > 0)
    {
      incoming_serial_byte = primary_serial_port->read();
    } // clear out buffer
    return;
  }
  else
  {
    primary_serial_port->print(F("Setting keyer to QRSS Mode. Dit length: "));
    primary_serial_port->print(numberstring);
    primary_serial_port->println(F(" seconds"));
    int y = 1;
    int set_dit_length = 0;
    for (int x = (numberindex - 1); x >= 0; x = x - 1)
    {
      set_dit_length = set_dit_length + ((numbers[x] - 48) * y);
      y = y * 10;
    }
    qrss_dit_length = set_dit_length;
    speed_mode = SPEED_QRSS;
  }
}
#endif
#endif
//-------------------------------------------------------------------------------------------------------

void remove_from_send_buffer()
{

#ifdef FEATURE_WINKEY_EMULATION
  if ((send_buffer_bytes < winkey_xon_threshold) && winkey_xoff && winkey_host_open)
  {
    winkey_xoff = 0;
    winkey_port_write(0xc0 | winkey_sending | winkey_xoff, 0); // send status /XOFF
  }
#endif

  if (send_buffer_bytes)
  {
    send_buffer_bytes--;
  }
  if (send_buffer_bytes)
  {
    for (int x = 0; x < send_buffer_bytes; x++)
    {
      send_buffer_array[x] = send_buffer_array[x + 1];
    }
#if defined(FEATURE_WINKEY_EMULATION) && defined(OPTION_WINKEY_FREQUENT_STATUS_REPORT)
    winkey_port_write(0xc0 | winkey_sending | winkey_xoff, 0);
#endif
  }

#if defined(DEBUG_SERVICE_SEND_BUFFER)
  debug_serial_port->print("remove_from_send_buffer: send_buffer_bytes:");
  debug_serial_port->println(send_buffer_bytes);
  debug_serial_port->print("send_buffer:");
  for (int x = 0; x < send_buffer_bytes; x++)
  {
    debug_serial_port->write(send_buffer_array[x]);
    debug_serial_port->print("[");
    debug_serial_port->print(send_buffer_array[x]);
    debug_serial_port->print("]");
  }
  debug_serial_port->println();
#endif
}

//-------------------------------------------------------------------------------------------------------

void service_send_buffer(byte no_print)
{
  // send one character out of the send buffer

#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering service_send_buffer"));
#endif

  static unsigned long timed_command_end_time;
  static byte timed_command_in_progress = 0;

#if defined(DEBUG_SERVICE_SEND_BUFFER)

  byte no_bytes_flag = 0;

  if (send_buffer_bytes > 0)
  {
    debug_serial_port->print("service_send_buffer: enter:");
    for (int x = 0; x < send_buffer_bytes; x++)
    {
      debug_serial_port->write(send_buffer_array[x]);
      debug_serial_port->print("[");
      debug_serial_port->print(send_buffer_array[x]);
      debug_serial_port->print("]");
    }
    debug_serial_port->println();
  }
  else
  {
    no_bytes_flag = 1;
  }
  Serial.flush();
#endif

  if (service_tx_inhibit_and_pause() == 1)
  {
#if defined(DEBUG_SERVICE_SEND_BUFFER)
    debug_serial_port->println("service_send_buffer: tx_inhib");
#endif
    return;
  }

#ifdef FEATURE_MEMORIES
  play_memory_prempt = 0;
#endif

  if (send_buffer_status == SERIAL_SEND_BUFFER_NORMAL)
  {
    if ((send_buffer_bytes) && (pause_sending_buffer == 0))
    {
#ifdef FEATURE_SLEEP
      last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
      last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM

      if ((send_buffer_array[0] > SERIAL_SEND_BUFFER_SPECIAL_START) && (send_buffer_array[0] < SERIAL_SEND_BUFFER_SPECIAL_END))
      {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
        debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_SPECIAL");
#endif

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_HOLD_SEND)
        {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_HOLD_SEND");
#endif

          send_buffer_status = SERIAL_SEND_BUFFER_HOLD;
          remove_from_send_buffer();
        }

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_HOLD_SEND_RELEASE)
        {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_HOLD_SEND_RELEASE");
#endif

          remove_from_send_buffer();
        }

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_MEMORY_NUMBER)
        {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_MEMORY_NUMBER");
#endif

#ifdef DEBUG_SEND_BUFFER
          debug_serial_port->println(F("service_send_buffer: SERIAL_SEND_BUFFER_MEMORY_NUMBER"));
#endif
#ifdef FEATURE_WINKEY_EMULATION
          if (winkey_sending && winkey_host_open)
          {
#if !defined(OPTION_WINKEY_UCXLOG_SUPRESS_C4_STATUS_BYTE)
            winkey_port_write(0xc0 | winkey_sending | winkey_xoff, 0);
#endif
            winkey_interrupted = 1;
          }
#endif
          remove_from_send_buffer();
          if (send_buffer_bytes)
          {
            if (send_buffer_array[0] < number_of_memories)
            {
#ifdef FEATURE_MEMORIES
              play_memory(send_buffer_array[0]);
#endif
            }
            remove_from_send_buffer();
          }
        }

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_WPM_CHANGE)
        { // two bytes for wpm
// remove_from_send_buffer();
#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_WPM_CHANGE");
#endif
          if (send_buffer_bytes > 2)
          {
#if defined(DEBUG_SERVICE_SEND_BUFFER)
            debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_WPM_CHANGE: send_buffer_bytes>2");
#endif
            remove_from_send_buffer();
#ifdef FEATURE_WINKEY_EMULATION
            if ((winkey_host_open) && (winkey_speed_state == WINKEY_UNBUFFERED_SPEED))
            {
              winkey_speed_state = WINKEY_BUFFERED_SPEED;
#if defined(DEBUG_SERVICE_SEND_BUFFER)
              debug_serial_port->println("service_send_buffer: winkey_speed_state = WINKEY_BUFFERED_SPEED");
#endif
              winkey_last_unbuffered_speed_wpm = configuration.wpm;
            }
#endif
            configuration.wpm = send_buffer_array[0] * 256;
            remove_from_send_buffer();
            configuration.wpm = configuration.wpm + send_buffer_array[0];
            remove_from_send_buffer();

#ifdef FEATURE_LED_RING
            update_led_ring();
#endif // FEATURE_LED_RING
          }
          else
          {
#if defined(DEBUG_SERVICE_SEND_BUFFER)
            debug_serial_port->println("service_send_buffer:SERIAL_SEND_BUFFER_WPM_CHANGE < 2");
#endif
          }

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->print("service_send_buffer: SERIAL_SEND_BUFFER_WPM_CHANGE: exit send_buffer_bytes:");
          debug_serial_port->println(send_buffer_bytes);
#endif
        }

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_TX_CHANGE)
        { // one byte for transmitter #

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_TX_CHANGE");
#endif

          remove_from_send_buffer();
          if (send_buffer_bytes > 1)
          {
// if ((send_buffer_array[0] > 0) && (send_buffer_array[0] < 7)){
//   switch_to_tx_silent(send_buffer_array[0]);
// }
#ifdef FEATURE_SO2R_BASE
            if ((send_buffer_array[0] > 0) && (send_buffer_array[0] < 3))
            {
              if (ptt_line_activated)
              {
                so2r_pending_tx = send_buffer_array[0];
              }
              else
              {
                so2r_tx = send_buffer_array[0];
                so2r_set_tx();
              }
            }
#else
            if ((send_buffer_array[0] > 0) && (send_buffer_array[0] < 7))
            {
              switch_to_tx_silent(send_buffer_array[0]);
            }
#endif // FEATURE_SO2R_BASE

            remove_from_send_buffer();
          }
        }

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_NULL)
        {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_NULL");
#endif

          remove_from_send_buffer();
        }

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_PROSIGN)
        {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_PROSIGN");
#endif

          remove_from_send_buffer();
          if (send_buffer_bytes)
          {
            send_char(send_buffer_array[0], OMIT_LETTERSPACE);
#ifdef FEATURE_WINKEY_EMULATION
            if (winkey_host_open)
            {
              // Must echo back PROSIGN characters sent  N6TV
              winkey_port_write(0xc4 | winkey_sending | winkey_xoff, 0); // N6TV
              winkey_port_write(send_buffer_array[0], 0);                // N6TV
            }
#endif // FEATURE_WINKEY_EMULATION
            remove_from_send_buffer();
          }
          if (send_buffer_bytes)
          {
            send_char(send_buffer_array[0], KEYER_NORMAL);
#ifdef FEATURE_WINKEY_EMULATION
            if (winkey_host_open)
            {
              // Must echo back PROSIGN characters sent  N6TV
              winkey_port_write(0xc4 | winkey_sending | winkey_xoff, 0); // N6TV
              winkey_port_write(send_buffer_array[0], 0);                // N6TV
            }
#endif // FEATURE_WINKEY_EMULATION
            remove_from_send_buffer();
          }
        }

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_TIMED_KEY_DOWN)
        {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_TIMED_KEY_DOWN");
#endif

          remove_from_send_buffer();
          if (send_buffer_bytes)
          {
            send_buffer_status = SERIAL_SEND_BUFFER_TIMED_COMMAND;
            sending_mode = AUTOMATIC_SENDING;
            tx_and_sidetone_key(1);
            timed_command_end_time = millis() + (send_buffer_array[0] * 1000);
            timed_command_in_progress = SERIAL_SEND_BUFFER_TIMED_KEY_DOWN;
            remove_from_send_buffer();
          }
        }

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_TIMED_WAIT)
        {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_TIMED_WAIT");
#endif

          remove_from_send_buffer();
          if (send_buffer_bytes)
          {
            send_buffer_status = SERIAL_SEND_BUFFER_TIMED_COMMAND;
            timed_command_end_time = millis() + (send_buffer_array[0] * 1000);
            timed_command_in_progress = SERIAL_SEND_BUFFER_TIMED_WAIT;
            remove_from_send_buffer();
          }
        }

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_PTT_ON)
        {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_PTT_ON");
#endif

          remove_from_send_buffer();
          manual_ptt_invoke = 1;
          ptt_key();
        }

        if (send_buffer_array[0] == SERIAL_SEND_BUFFER_PTT_OFF)
        {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_PTT_OFF");
#endif

          remove_from_send_buffer();
          manual_ptt_invoke = 0;
          ptt_unkey();
        }
      }
      else
      { // if ((send_buffer_array[0] > SERIAL_SEND_BUFFER_SPECIAL_START) && (send_buffer_array[0] < SERIAL_SEND_BUFFER_SPECIAL_END))

        // #ifdef FEATURE_WINKEY_EMULATION
        //   if ((primary_serial_port_mode == SERIAL_WINKEY_EMULATION) && (winkey_serial_echo) && (winkey_host_open) && (!no_print) && (!cw_send_echo_inhibit)){
        //     #if defined(OPTION_WINKEY_ECHO_7C_BYTE)
        //       if (send_buffer_array[0] > 30) {winkey_port_write(send_buffer_array[0],0);}
        //     #else
        //       if ((send_buffer_array[0]!= 0x7C) && (send_buffer_array[0] > 30)) {winkey_port_write(send_buffer_array[0],0);}
        //     #endif
        //   }
        // #endif //FEATURE_WINKEY_EMULATION

#if defined(FEATURE_COMMAND_LINE_INTERFACE)
        if ((!no_print) && (!cw_send_echo_inhibit))
        {
          if (primary_serial_port_mode == SERIAL_CLI)
          {
            primary_serial_port->write(send_buffer_array[0]);
          };
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
          secondary_serial_port->write(send_buffer_array[0]);
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
          if (send_buffer_array[0] == 13)
          {
            if (primary_serial_port_mode == SERIAL_CLI)
            {
              primary_serial_port->write(10);
            } // if we got a carriage return, also send a line feed
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
            secondary_serial_port->write(10);
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
          }
        }
#endif // FEATURE_COMMAND_LINE_INTERFACE

#ifdef FEATURE_DISPLAY
        if (lcd_send_echo)
        {
          display_scroll_print_char(send_buffer_array[0]);
          service_display();
        }
#endif // FEATURE_DISPLAY

#if defined(DEBUG_SERVICE_SEND_BUFFER)
        debug_serial_port->print("service_send_buffer: send_char:");
        debug_serial_port->write(send_buffer_array[0]);
        debug_serial_port->println();
        Serial.flush();
#endif

        send_char(send_buffer_array[0], KEYER_NORMAL); //****************

#ifdef FEATURE_WINKEY_EMULATION
        if ((primary_serial_port_mode == SERIAL_WINKEY_EMULATION) && (winkey_serial_echo) && (winkey_host_open) && (!no_print) && (!cw_send_echo_inhibit))
        {
#if defined(OPTION_WINKEY_ECHO_7C_BYTE)
          if (send_buffer_array[0] > 30)
          {
            winkey_port_write(send_buffer_array[0], 0);
          }
#else
          if ((send_buffer_array[0] != 0x7C) && (send_buffer_array[0] > 30))
          {
            winkey_port_write(send_buffer_array[0], 0);
          }
#endif
        }
#endif // FEATURE_WINKEY_EMULATION

        if (!pause_sending_buffer)
        {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
          debug_serial_port->println("service_send_buffer: after send_char: remove_from_send_buffer");
          if (no_bytes_flag)
          {
            debug_serial_port->println("service_send_buffer: no_bytes_flag");
          }
          Serial.flush();
#endif

          if (!((send_buffer_array[0] > SERIAL_SEND_BUFFER_SPECIAL_START) && (send_buffer_array[0] < SERIAL_SEND_BUFFER_SPECIAL_END)))
          { // this is a friggin hack to fix something I can't explain with SO2R - Goody 20191217
            remove_from_send_buffer();

#if defined(DEBUG_SERVICE_SEND_BUFFER)
            debug_serial_port->println("service_send_buffer: after send_char: remove_from_send_buffer");
            if (no_bytes_flag)
            {
              debug_serial_port->println("service_send_buffer: no_bytes_flag");
            }
            Serial.flush();
#endif
          }
          else
          {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
            debug_serial_port->println("service_send_buffer: snagged errant remove_from_send_buffer");
            Serial.flush();
#endif
          }
        }
      }
    } // if ((send_buffer_bytes) && (pause_sending_buffer == 0))
  }
  else
  { // if (send_buffer_status == SERIAL_SEND_BUFFER_NORMAL)

    if (send_buffer_status == SERIAL_SEND_BUFFER_TIMED_COMMAND)
    { // we're in a timed command

      if ((timed_command_in_progress == SERIAL_SEND_BUFFER_TIMED_KEY_DOWN) && (millis() > timed_command_end_time))
      {
        sending_mode = AUTOMATIC_SENDING;
        tx_and_sidetone_key(0);
        timed_command_in_progress = 0;
        send_buffer_status = SERIAL_SEND_BUFFER_NORMAL;

#if defined(DEBUG_SERVICE_SEND_BUFFER)
        debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_TIMED_KEY_DOWN->SERIAL_SEND_BUFFER_NORMAL");
#endif
      }

      if ((timed_command_in_progress == SERIAL_SEND_BUFFER_TIMED_WAIT) && (millis() > timed_command_end_time))
      {
        timed_command_in_progress = 0;
        send_buffer_status = SERIAL_SEND_BUFFER_NORMAL;

#if defined(DEBUG_SERVICE_SEND_BUFFER)
        debug_serial_port->println("service_send_buffer: SERIAL_SEND_BUFFER_TIMED_WAIT->SERIAL_SEND_BUFFER_NORMAL");
#endif
      }
    }

    if (send_buffer_status == SERIAL_SEND_BUFFER_HOLD)
    { // we're in a send hold ; see if there's a SERIAL_SEND_BUFFER_HOLD_SEND_RELEASE in the buffer
      if (send_buffer_bytes == 0)
      {
        send_buffer_status = SERIAL_SEND_BUFFER_NORMAL; // this should never happen, but what the hell, we'll catch it here if it ever does happen
      }
      else
      {
        for (int z = 0; z < send_buffer_bytes; z++)
        {
          if (send_buffer_array[z] == SERIAL_SEND_BUFFER_HOLD_SEND_RELEASE)
          {
            send_buffer_status = SERIAL_SEND_BUFFER_NORMAL;
            z = send_buffer_bytes;
          }
        }
      }
    }

  } // if (send_buffer_status == SERIAL_SEND_BUFFER_NORMAL)

  // if the paddles are hit, dump the buffer
  check_paddles();
  if (((dit_buffer || dah_buffer) && (send_buffer_bytes)) && (keyer_machine_mode != BEACON))
  {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
    debug_serial_port->println("service_send_buffer: buffer dump");
#endif

    clear_send_buffer();
    send_buffer_status = SERIAL_SEND_BUFFER_NORMAL;
    dit_buffer = 0;
    dah_buffer = 0;
#ifdef FEATURE_MEMORIES
    repeat_memory = 255;
#endif
#ifdef FEATURE_WINKEY_EMULATION
    if (winkey_sending && winkey_host_open)
    {
      winkey_port_write(0xc2 | winkey_sending | winkey_xoff, 0); // 0xc2 - BREAKIN bit set high
      winkey_interrupted = 1;
    }
#endif
  }
}

//-------------------------------------------------------------------------------------------------------

/*

10 CLS
20 PRINT "HELLO"
30 GOTO 20

*/

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_COMMAND_LINE_INTERFACE
void service_command_line_interface(PRIMARY_SERIAL_CLS *port_to_use)
{

  static byte cli_wait_for_cr_flag = 0;

  if (serial_backslash_command == 0)
  {
    incoming_serial_byte = uppercase(incoming_serial_byte);
    if ((incoming_serial_byte != 92) && (incoming_serial_byte != 27))
    { // we do not have a backslash or ESC

#if !defined(OPTION_EXCLUDE_MILL_MODE)
      if (configuration.cli_mode == CLI_NORMAL_MODE)
      {
        if (cli_prosign_flag)
        {
          add_to_send_buffer(SERIAL_SEND_BUFFER_PROSIGN);
          cli_prosign_flag = 0;
        }
        if (cli_wait_for_cr_to_send_cw)
        {
          if (cli_wait_for_cr_flag == 0)
          {
            if (incoming_serial_byte > 31)
            {
#ifdef DEBUG_CHECK_SERIAL
              port_to_use->println(F("check_serial: add_to_send_buffer(SERIAL_SEND_BUFFER_HOLD_SEND)"));
#endif
              add_to_send_buffer(SERIAL_SEND_BUFFER_HOLD_SEND);
              cli_wait_for_cr_flag = 1;
            }
          }
          else
          {
            if (incoming_serial_byte == 13)
            {
#ifdef DEBUG_CHECK_SERIAL
              port_to_use->println(F("check_serial: add_to_send_buffer(SERIAL_SEND_BUFFER_HOLD_SEND_RELEASE)"));
#endif
              add_to_send_buffer(SERIAL_SEND_BUFFER_HOLD_SEND_RELEASE);
              cli_wait_for_cr_flag = 0;
            }
          }
        }
        add_to_send_buffer(incoming_serial_byte);
      }
      else
      { // configuration.cli_mode != CLI_NORMAL_MODE
        if (configuration.cli_mode == CLI_MILL_MODE_KEYBOARD_RECEIVE)
        {
          port_to_use->write(incoming_serial_byte);
          if (incoming_serial_byte == 13)
          {
            port_to_use->println();
          }
#ifdef FEATURE_SD_CARD_SUPPORT
          sd_card_log("", incoming_serial_byte);
#endif
        }
        else
        { // configuration.cli_mode == CLI_MILL_MODE_PADDLE_SEND
          port_to_use->println();
          port_to_use->println();
          if (incoming_serial_byte == 13)
          {
            port_to_use->println();
          }
          port_to_use->write(incoming_serial_byte);
          configuration.cli_mode = CLI_MILL_MODE_KEYBOARD_RECEIVE;
#ifdef FEATURE_SD_CARD_SUPPORT
          sd_card_log("\r\nRX:", 0);
          sd_card_log("", incoming_serial_byte);
#endif
        }
      } // if (configuration.cli_mode == CLI_NORMAL_MODE)

#else //! defined(OPTION_EXCLUDE_MILL_MODE)

      if (cli_prosign_flag)
      {
        add_to_send_buffer(SERIAL_SEND_BUFFER_PROSIGN);
        cli_prosign_flag = 0;
      }
      if (cli_wait_for_cr_to_send_cw)
      {
        if (cli_wait_for_cr_flag == 0)
        {
          if (incoming_serial_byte > 31)
          {
#ifdef DEBUG_CHECK_SERIAL
            port_to_use->println(F("check_serial: add_to_send_buffer(SERIAL_SEND_BUFFER_HOLD_SEND)"));
#endif
            add_to_send_buffer(SERIAL_SEND_BUFFER_HOLD_SEND);
            cli_wait_for_cr_flag = 1;
          }
        }
        else
        {
          if (incoming_serial_byte == 13)
          {
#ifdef DEBUG_CHECK_SERIAL
            port_to_use->println(F("check_serial: add_to_send_buffer(SERIAL_SEND_BUFFER_HOLD_SEND_RELEASE)"));
#endif
            add_to_send_buffer(SERIAL_SEND_BUFFER_HOLD_SEND_RELEASE);
            cli_wait_for_cr_flag = 0;
          }
        }
      }
      add_to_send_buffer(incoming_serial_byte);

#endif // !defined(OPTION_EXCLUDE_MILL_MODE)

#ifdef FEATURE_MEMORIES
      if ((incoming_serial_byte != 13) && (incoming_serial_byte != 10))
      {
        repeat_memory = 255;
      }
#endif
    }
    else
    { // if ((incoming_serial_byte != 92) && (incoming_serial_byte != 27)) -- we got a backslash or ESC
      if (incoming_serial_byte == 92)
      { // backslash
        serial_backslash_command = 1;
        port_to_use->write(incoming_serial_byte);
      }
      else
      { // escape
        clear_send_buffer();
#ifdef FEATURE_MEMORIES
        play_memory_prempt = 1;
        repeat_memory = 255;
#endif
      }
    }
  }
  else
  { // (serial_backslash_command == 0) -- we already got a backslash
    incoming_serial_byte = uppercase(incoming_serial_byte);
    port_to_use->write(incoming_serial_byte);
    process_serial_command(port_to_use);
    serial_backslash_command = 0;
    port_to_use->println();
  }
}
#endif // FEATURE_COMMAND_LINE_INTERFACE

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_SERIAL_HELP) && defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void print_serial_help(PRIMARY_SERIAL_CLS *port_to_use, byte paged_help)
{

  port_to_use->println(F("\n\rK3NG Keyer Help\n\r"));
  port_to_use->println(F("CLI commands:"));
  port_to_use->println(F("\\#\t\t: Play memory # x")); // Upper case to first letter only(WD9DMP)
  port_to_use->println(F("\\A\t\t: Iambic A"));
  port_to_use->println(F("\\B\t\t: Iambic B"));
  port_to_use->println(F("\\C\t\t: Single paddle")); // Upper case to first letter only(WD9DMP)
#ifndef OPTION_NO_ULTIMATIC
  port_to_use->println(F("\\D\t\t: Ultimatic"));
#endif
  port_to_use->println(F("\\E####\t\t: Set serial number to ####"));
  port_to_use->println(F("\\F####\t\t: Set sidetone to #### hz"));
  port_to_use->println(F("\\G\t\t: Switch to bug mode")); // Upper case to first letter only(WD9DMP)
#ifdef FEATURE_HELL
  port_to_use->println(F("\\H\t\t: Toggle CW / Hell mode"));
#endif
  port_to_use->println(F("\\I\t\t: TX line disable/enable"));
  port_to_use->println(F("\\J###\t\t: Set dah to dit ratio")); // Upper case to first letter only(WD9DMP)
#ifdef FEATURE_TRAINING_COMMAND_LINE_INTERFACE
  port_to_use->println(F("\\K\t\t: Training"));
#endif
  port_to_use->println(F("\\L##\t\t: Set weighting (50 = normal)"));
#ifdef FEATURE_FARNSWORTH
  port_to_use->println(F("\\M###\t\t: Set Farnsworth speed")); // Upper case to first letter only(WD9DMP)
#endif
  if (paged_help)
  {
    serial_page_pause(port_to_use, 10);
  }
  port_to_use->println(F("\\N\t\t: Toggle paddle reverse"));                  // Upper case to first letter only(WD9DMP)
  port_to_use->println(F("\\O\t\t: Toggle sidetone on/off"));                 // Added missing command (WD9DMP)
  port_to_use->println(F("\\Px<string>\t: Program memory #x with <string>")); // Upper case to first letter only(WD9DMP)
  port_to_use->println(F("\\Q#[#]\t\t: Switch to QRSS mode with ## second dit length"));
  port_to_use->println(F("\\R\t\t: Switch to regular speed (wpm) mode"));
  port_to_use->println(F("\\S\t\t: Status report")); // Upper case to first letter only(WD9DMP)
  port_to_use->println(F("\\T\t\t: Tune mode"));
  port_to_use->println(F("\\U\t\t: PTT toggle"));
#ifdef FEATURE_POTENTIOMETER
  port_to_use->println(F("\\V\t\t: Potentiometer activate/deactivate"));
#endif // FEATURE_POTENTIOMETER
  port_to_use->println(F("\\W#[#][#]\t: Change WPM to ###"));
  port_to_use->println(F("\\X#\t\t: Switch to transmitter #"));
  port_to_use->println(F("\\Y#\t\t: Change wordspace to #"));
#ifdef FEATURE_AUTOSPACE
  port_to_use->println(F("\\Z\t\t: Autospace on/off"));
#endif                                                                                                  // FEATURE_AUTOSPACE
  port_to_use->println(F("\\+\t\t: Create prosign"));                                                   // Changed description to match change log at top (WD9DMP)
  port_to_use->println(F("\\!##\t\t: Repeat play memory"));                                             // Added missing command(WD9DMP)
  port_to_use->println(F("\\|####\t\t: Set memory repeat (milliseconds)"));                             // Added missing command(WD9DMP)
  port_to_use->println(F("\\*\t\t: Toggle paddle echo"));                                               // Added missing command(WD9DMP)
  port_to_use->println(F("\\`\t\t: Toggle straight key echo"));                                         // Added missing command(WD9DMP)
  port_to_use->println(F("\\^\t\t: Toggle wait for carriage return to send CW / send CW immediately")); // Added missing command(WD9DMP)
#ifdef FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING
  port_to_use->println(F("\\&\t\t: Toggle CMOS Super Keyer timing on/off")); // Upper case to first letter only(WD9DMP)
  port_to_use->println(F("\\%##\t\t: Set CMOS Super Keyer timing %"));       // Upper case to first letter only(WD9DMP)
#endif                                                                       // FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING
  port_to_use->println(F("\\.\t\t: Toggle dit buffer on/off"));
  port_to_use->println(F("\\-\t\t: Toggle dah buffer on/off"));
  port_to_use->println(F("\\~\t\t: Reset unit"));                                                       // Added missing command(WD9DMP)
  port_to_use->println(F("\\:\t\t: Toggle cw send echo"));                                              // Added missing command(WD9DMP)
  port_to_use->println(F("\\{\t\t: QLF mode on/off"));                                                  // Added missing command(WD9DMP)
  port_to_use->println(F("\\>\t\t: Send serial number, then increment"));                               // Added missing command(WD9DMP)
  port_to_use->println(F("\\<\t\t: Send current serial number"));                                       // Added missing command(WD9DMP)
  port_to_use->println(F("\\(\t\t: Send current serial number in cut numbers"));                        // Added missing command(WD9DMP)
  port_to_use->println(F("\\)\t\t: Send serial number with cut numbers, then increment"));              // Added missing command(WD9DMP)
  port_to_use->println(F("\\[\t\t: Set quiet paddle interruption - 0 to 20 element lengths; 0 = off")); // Added missing command(WD9DMP)
  port_to_use->println(F("\\]\t\t: PTT disable/enable"));
#ifdef FEATURE_AMERICAN_MORSE
  port_to_use->println(F("\\=\t\t: Toggle American Morse mode")); // Added missing command(WD9DMP)
#endif
#ifdef FEATURE_POTENTIOMETER
  port_to_use->println(F("\\}####\t\t: Set Potentiometer range"));
#endif // FEATURE_POTENTIOMETER
#if !defined(OPTION_EXCLUDE_MILL_MODE)
  port_to_use->println(F("\\@\t\t: Mill Mode"));
#endif
  port_to_use->println(F("\\\\\t\t: Empty keyboard send buffer")); // Moved to end of command list (WD9DMP)
  if (paged_help)
  {
    serial_page_pause(port_to_use, 10);
  }
// Memory Macros below (WD9DMP)
#ifdef FEATURE_MEMORY_MACROS
  port_to_use->println(F("\nMemory Macros:"));
  port_to_use->println(F("\\#\t\t: Jump to memory #"));
  port_to_use->println(F("\\C\t\t: Send serial number with cut numbers, then increment")); // Added "then increment" (WD9DMP)
  port_to_use->println(F("\\D###\t\t: Delay for ### seconds"));
  port_to_use->println(F("\\E\t\t: Send serial number, then increment")); // Added "then increment" (WD9DMP)
  port_to_use->println(F("\\F####\t\t: Set sidetone to #### hz"));
#ifdef FEATURE_HELL
  port_to_use->println(F("\\H\t\t: Switch to Hell mode"));
#endif                                                 // FEATURE_HELL
  port_to_use->println(F("\\I\t\t: Insert memory #")); // Added missing macro (WD9DMP)
#ifdef FEATURE_HELL
  port_to_use->println(F("\\L\t\t: Switch to CW (from Hell mode)"));
#endif                                                                       // FEATURE_HELL
  port_to_use->println(F("\\N\t\t: Decrement serial number - do not send")); // Added "do not send" (WD9DMP)
  port_to_use->println(F("\\Q##\t\t: Switch to QRSS with ## second dit length"));
  port_to_use->println(F("\\R\t\t: Switch to regular speed mode"));
  port_to_use->println(F("\\S\t\t: Insert space")); // Added missing macro (WD9DMP)
  port_to_use->println(F("\\T###\t\t: Transmit for ### seconds"));
  port_to_use->println(F("\\U\t\t: Key PTT"));   // Upper case to first letter only(WD9DMP)
  port_to_use->println(F("\\V\t\t: Unkey PTT")); // Upper case to first letter only(WD9DMP)
  port_to_use->println(F("\\W###\t\t: Change WPM to ###"));
  port_to_use->println(F("\\X#\t\t: Switch to transmitter #"));
  port_to_use->println(F("\\Y#\t\t: Increase speed # WPM"));
  port_to_use->println(F("\\Z#\t\t: Decrease speed # WPM"));
  port_to_use->println(F("\\+\t\t: Prosign the next two characters")); // Added "the next two characters" (WD9DMP)
#endif                                                                 // FEATURE_MEMORY_MACROS

#if !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)
  if (paged_help)
  {
    serial_page_pause(port_to_use, 10);
  }
  port_to_use->println(F("\r\n\\:\tExtended CLLI commands"));
  port_to_use->println(F("\t\teepromdump\t\t- do a byte dump of EEPROM for troubleshooting"));
  port_to_use->println(F("\t\tsaveeeprom <filename>\t- store EEPROM in a file"));
  port_to_use->println(F("\t\tloadeeprom <filename> \t- load into EEPROM from a file"));
  port_to_use->println(F("\t\tprintlog\t\t- print the SD card log"));
  port_to_use->println(F("\t\tclearlog\t\t- clear the SD card log"));
  port_to_use->println(F("\t\tls <directory>\t\t- list files in SD card directory"));
  port_to_use->println(F("\t\tcat <filename>\t\t- print filename on SD card"));
  port_to_use->println(F("\t\tpl <transmitter> <mS>\t- Set PTT lead time"));
  port_to_use->println(F("\t\tpt <transmitter> <mS> \t- Set PTT tail time"));
  port_to_use->println(F("\t\tcomp <mS> \t- Set keying compensation time"));
#endif // OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS

  port_to_use->println(F("\r\n\\/\t\t: Paginated help"));
}
#endif
//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void process_serial_command(PRIMARY_SERIAL_CLS *port_to_use)
{

  int user_input_temp = 0;

#ifdef FEATURE_AMERICAN_MORSE
  static int previous_dah_to_dit_ratio = 300;
#endif // FEATURE_AMERICAN_MORSE

  // port_to_use->println();
  switch (incoming_serial_byte)
  {
  case '~':
#if defined(__AVR__)
    asm volatile("jmp 0"); /*wdt_enable(WDTO_30MS); while(1) {} ;*/
#else                      //__AVR__
    setup();
#endif                     //__AVR__
    break;                 // ~ - reset unit
  case '*':                // * - paddle echo on / off
    if (cli_paddle_echo)
    {
      cli_paddle_echo = 0;
    }
    else
    {
      cli_paddle_echo = 1;
    }
    break;
#if defined(FEATURE_STRAIGHT_KEY_ECHO)
  case '`':
    if (cli_straight_key_echo)
    {
      cli_straight_key_echo = 0;
    }
    else
    {
      cli_straight_key_echo = 1;
    }
    break;
#endif // FEATURE_STRAIGHT_KEY_ECHO
  case '+':
    cli_prosign_flag = 1;
    break;
#if defined(FEATURE_SERIAL_HELP)
  case '?':
    print_serial_help(port_to_use, 0);
    break; // ? = print help
  case '/':
    print_serial_help(port_to_use, 1);
    break;  // / = paged help
#endif      // FEATURE_SERIAL_HELP
  case 'A': // A - Iambic A mode
    configuration.keyer_mode = IAMBIC_A;
    configuration.dit_buffer_off = 0;
    configuration.dah_buffer_off = 0;
    config_dirty = 1;
    port_to_use->println(F("\r\nIambic A"));
    break;
  case 'B': // B - Iambic B mode
    configuration.keyer_mode = IAMBIC_B;
    configuration.dit_buffer_off = 0;
    configuration.dah_buffer_off = 0;
    config_dirty = 1;
    port_to_use->println(F("\r\nIambic B"));
    break;
  case 'C': // C - single paddle mode
    configuration.keyer_mode = SINGLE_PADDLE;
    config_dirty = 1;
    port_to_use->println(F("\r\nSingle Paddle"));
    break;

#ifndef OPTION_NO_ULTIMATIC
  case 'D': // D - Ultimatic mode
    configuration.keyer_mode = ULTIMATIC;
    configuration.dit_buffer_off = 1;
    configuration.dah_buffer_off = 1;
    config_dirty = 1;
    port_to_use->println(F("\r\nUltimatic"));
    break;
#endif
  case 'E':
    serial_set_serial_number(port_to_use);
    break; // E - set serial number
  case 'F':
    serial_set_sidetone_freq(port_to_use);
    break; // F - set sidetone frequency
  case 'G':
    configuration.keyer_mode = BUG;
    config_dirty = 1;
    port_to_use->println(F("\r\nBug"));
    break; // G - Bug mode
#ifdef FEATURE_HELL
  case 'H': // H - Hell mode
    if ((char_send_mode == CW) || (char_send_mode == AMERICAN_MORSE))
    {
      char_send_mode = HELL;
      port_to_use->println(F("\r\nHell Mode"));
    }
    else
    {
      char_send_mode = CW;
      port_to_use->println(F("\r\nCW Mode"));
    }
    break;
#endif // FEATURE_HELL
#ifdef FEATURE_AMERICAN_MORSE
  case '=': // = - American Morse
    if ((char_send_mode == CW) || (char_send_mode == HELL))
    {
      char_send_mode = AMERICAN_MORSE;
      port_to_use->println(F("\r\nAmerican Morse Mode"));
      previous_dah_to_dit_ratio = configuration.dah_to_dit_ratio;
      configuration.dah_to_dit_ratio = 200;
    }
    else
    {
      char_send_mode = CW;
      port_to_use->println(F("\r\nInternational CW Mode"));
      configuration.dah_to_dit_ratio = previous_dah_to_dit_ratio;
    }
    break;
#endif // FEATURE_AMERICAN_MORSE

  case 'I': // I - transmit line on/off
    // port_to_use->print(F("\r\nTX o")); //WD9DMP-1
    if (key_tx && keyer_machine_mode != KEYER_COMMAND_MODE)
    { // Added check that keyer is NOT in command mode or keyer might be enabled for paddle commands (WD9DMP-1)
      key_tx = 0;
      port_to_use->println(F("\r\nTX off")); // WD9DMP-1
    }
    else if (!key_tx && keyer_machine_mode != KEYER_COMMAND_MODE)
    { // Added check that keyer is NOT in command mode or keyer might be enabled for paddle commands (WD9DMP-1)
      key_tx = 1;
      port_to_use->println(F("\r\nTX on")); // WD9DMP-1
    }
    else
    {
      port_to_use->print(F("\r\nERROR: Keyer in Command Mode\r\n"));
    } // Print error message if keyer in Command Mode and user tries to change tx line(s) on/off. (WD9DMP-1)
    break;
#ifdef FEATURE_MEMORIES
  case 33:
    repeat_play_memory(port_to_use);
    break; // ! - repeat play
  case 124:
    serial_set_memory_repeat(port_to_use);
    break; // | - set memory repeat time
  case 48:
    serial_play_memory(9);
    break; // 0 - play memory 10
  case 49: // 1-9 - play memory #
  case 50:
  case 51:
  case 52:
  case 53:
  case 54:
  case 55:
  case 56:
  case 57:
    serial_play_memory(incoming_serial_byte - 49);
    break;
  case 80:
    repeat_memory = 255;
    serial_program_memory(port_to_use);
    break; // P - program memory
#endif     // FEATURE_MEMORIES
  case 'Q':
    serial_qrss_mode();
    break; // Q - activate QRSS mode
  case 'R':
    speed_mode = SPEED_NORMAL;
    port_to_use->println(F("\r\nQRSS Off"));
    break; // R - activate regular timing mode
  case 'S':
    serial_status(port_to_use);
    break; // S - Status command
  case 'J':
    serial_set_dit_to_dah_ratio(port_to_use);
    break; // J - dit to dah ratio
#ifdef FEATURE_TRAINING_COMMAND_LINE_INTERFACE
  case 'K':
    serial_cw_practice(port_to_use);
    break; // K - CW practice
#endif     // FEATURE_TRAINING_COMMAND_LINE_INTERFACE
  case 'L':
    serial_set_weighting(port_to_use);
    break;
#ifdef FEATURE_FARNSWORTH
  case 'M':
    serial_set_farnsworth(port_to_use);
    break; // M - set Farnsworth speed
#endif
  case 'N': // N - paddle reverse
    port_to_use->print(F("\r\nPaddles "));
    if (configuration.paddle_mode == PADDLE_NORMAL)
    {
      configuration.paddle_mode = PADDLE_REVERSE;
      port_to_use->println(F("Reversed"));
    }
    else
    {
      configuration.paddle_mode = PADDLE_NORMAL;
      port_to_use->println(F("Normal"));
    }
    config_dirty = 1;
    break;
  case 'O': // O - cycle through sidetone modes on, paddle only and off - enhanced by Marc-Andre, VE2EVN
    port_to_use->print(F("\r\nSidetone "));
    if (configuration.sidetone_mode == SIDETONE_PADDLE_ONLY)
    {
      configuration.sidetone_mode = SIDETONE_OFF;
      boop();
      port_to_use->println(F("Off"));
    }
    else if (configuration.sidetone_mode == SIDETONE_ON)
    {
      configuration.sidetone_mode = SIDETONE_PADDLE_ONLY;
      beep();
      delay(200);
      beep();
      port_to_use->println(F("Paddle Only"));
    }
    else
    {
      configuration.sidetone_mode = SIDETONE_ON;
      beep();
      port_to_use->println(F("On"));
    }
    config_dirty = 1;
    break;

  case 'T': // T - tune
#ifdef FEATURE_MEMORIES
    repeat_memory = 255;
#endif
    serial_tune_command(port_to_use);
    break;
  case 'U':
    port_to_use->print(F("\r\nPTT O"));
    if (ptt_line_activated)
    {
      manual_ptt_invoke = 0;
      ptt_unkey();
      port_to_use->println(F("ff"));
    }
    else
    {
      manual_ptt_invoke = 1;
      configuration.ptt_disabled = 0;
      config_dirty = 1;
      ptt_key();
      port_to_use->println(F("n"));
    }
    break;
#ifdef FEATURE_POTENTIOMETER
  case 'V': // V - toggle pot activation
    port_to_use->print(F("\r\nPotentiometer "));
    configuration.pot_activated = !configuration.pot_activated;
    if (configuration.pot_activated)
    {
      port_to_use->print(F("A"));
    }
    else
    {
      port_to_use->print(F("Dea"));
    }
    port_to_use->println(F("ctivated"));
    config_dirty = 1;
    break;
  case '}':
    serial_set_pot_low_high(port_to_use);
    break;
#endif
  case 'W':
    serial_wpm_set(port_to_use);
    break; // W - set WPM
  case 'X':
    serial_switch_tx(port_to_use);
    break; // X - switch transmitter
  case 'Y':
    serial_change_wordspace(port_to_use);
    break;
#ifdef FEATURE_AUTOSPACE
  case 'Z':
    port_to_use->print(F("\r\nAutospace O"));
    if (configuration.autospace_active)
    {
      configuration.autospace_active = 0;
      config_dirty = 1;
      port_to_use->println(F("ff"));
    }
    else
    {
      configuration.autospace_active = 1;
      config_dirty = 1;
      port_to_use->println(F("n"));
    }
    break;
#endif

  case 92: // \ - double backslash - clear serial send buffer
    clear_send_buffer();
#if !defined(OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW)
    loop_element_lengths_breakout_flag = 0;
#endif
#ifdef FEATURE_MEMORIES
    play_memory_prempt = 1;
    repeat_memory = 255;
#endif
    break;

  case '^': // ^ - toggle send CW send immediately
    if (cli_wait_for_cr_to_send_cw)
    {
      cli_wait_for_cr_to_send_cw = 0;
      port_to_use->println(F("\r\nSend CW immediately"));
    }
    else
    {
      cli_wait_for_cr_to_send_cw = 1;
      port_to_use->println(F("\r\nWait for CR to send CW"));
    }
    break;
#ifdef FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING
  case '&':
    port_to_use->print(F("\r\nCMOS Super Keyer Timing O"));
    if (configuration.cmos_super_keyer_iambic_b_timing_on)
    {
      configuration.cmos_super_keyer_iambic_b_timing_on = 0;
      port_to_use->println(F("ff"));
    }
    else
    {
      configuration.cmos_super_keyer_iambic_b_timing_on = 1;
      port_to_use->println(F("n"));
      configuration.keyer_mode = IAMBIC_B;
    }
    config_dirty = 1;
    break;
  case '%':
    user_input_temp = serial_get_number_input(2, -1, 100, port_to_use, RAISE_ERROR_MSG);
    if ((user_input_temp >= 0) && (user_input_temp < 100))
    {
      configuration.cmos_super_keyer_iambic_b_timing_percent = user_input_temp;
      port_to_use->println(F("\r\nCMOS Super Keyer Timing Set."));
    }
    config_dirty = 1;
    break;
#endif // FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING
  case '.':
    port_to_use->print(F("\r\nDit Buffer O"));
    if (configuration.dit_buffer_off)
    {
      configuration.dit_buffer_off = 0;
      port_to_use->println(F("n"));
    }
    else
    {
      configuration.dit_buffer_off = 1;
      port_to_use->println(F("ff"));
    }
    config_dirty = 1;
    break;
  case '-':
    port_to_use->print(F("\r\nDah Buffer O"));
    if (configuration.dah_buffer_off)
    {
      configuration.dah_buffer_off = 0;
      port_to_use->println(F("n"));
    }
    else
    {
      configuration.dah_buffer_off = 1;
      port_to_use->println(F("ff"));
    }
    config_dirty = 1;
    break;
  case ';':
    if (cw_send_echo_inhibit)
    {
      cw_send_echo_inhibit = 0;
    }
    else
    {
      cw_send_echo_inhibit = 1;
    }
    break;
#ifdef FEATURE_QLF
  case '{':
    port_to_use->print(F("\r\nQLF: O"));
    if (qlf_active)
    {
      qlf_active = 0;
      port_to_use->println(F("ff"));
    }
    else
    {
      qlf_active = 1;
      port_to_use->println(F("n"));
    }
    break;
#endif // FEATURE_QLF

  case '>':
    send_serial_number(0, 1, 1);
    break;
  case '<':
    send_serial_number(0, 0, 1);
    break;
  case '(':
    send_serial_number(1, 0, 1);
    break;
  case ')':
    send_serial_number(1, 1, 1);
    break;
  case '[':
    user_input_temp = serial_get_number_input(2, -1, 21, port_to_use, RAISE_ERROR_MSG);
    if ((user_input_temp >= 0) && (user_input_temp < 21))
    {
      configuration.paddle_interruption_quiet_time_element_lengths = user_input_temp;
      port_to_use->println(F("\r\nPaddle Interruption Quiet Time set."));
    }
    config_dirty = 1;
    break;
#if !defined(OPTION_EXCLUDE_MILL_MODE)
  case '@':
    switch (configuration.cli_mode)
    {
    case CLI_NORMAL_MODE:
      configuration.cli_mode = CLI_MILL_MODE_KEYBOARD_RECEIVE;
      port_to_use->println(F("\r\nMill Mode On"));
      break;
    case CLI_MILL_MODE_PADDLE_SEND:
    case CLI_MILL_MODE_KEYBOARD_RECEIVE:
      configuration.cli_mode = CLI_NORMAL_MODE;
      port_to_use->println(F("\r\nMill Mode Off"));
      break;
    }
    config_dirty = 1;
    break;
#endif // !defined(OPTION_EXCLUDE_MILL_MODE)
  case '"':
    port_to_use->print(F("\r\nPTT Buffered Character Hold O"));
    if (configuration.ptt_buffer_hold_active)
    {
      configuration.ptt_buffer_hold_active = 0;
      port_to_use->println(F("ff"));
    }
    else
    {
      configuration.ptt_buffer_hold_active = 1;
      port_to_use->println(F("n"));
    }
    config_dirty = 1;
    break;
  case ']':
    port_to_use->print(F("\r\nPTT "));
    if (configuration.ptt_disabled)
    {
      configuration.ptt_disabled = 0;
      port_to_use->print(F("En"));
    }
    else
    {
      configuration.ptt_disabled = 1;
      ptt_unkey();
      port_to_use->print(F("Dis"));
    }
    port_to_use->println(F("abled"));
    config_dirty = 1;
    break;

#if defined(FEATURE_BEACON_SETTING)
  case '_':
    port_to_use->print(F("\r\nBeacon Mode At Boot Up "));
    if (!configuration.beacon_mode_on_boot_up)
    {
      configuration.beacon_mode_on_boot_up = 1;
      port_to_use->print(F("En"));
    }
    else
    {
      configuration.beacon_mode_on_boot_up = 0;
      port_to_use->print(F("Dis"));
    }
    port_to_use->println(F("abled"));
    config_dirty = 1;
    break;
#endif

#if !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)
  case ':':
    cli_extended_commands(port_to_use);
    break;
#endif

  default:
    port_to_use->println(F("\r\nUnknown command"));
    break;
  }
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)
void cli_extended_commands(PRIMARY_SERIAL_CLS *port_to_use)
{
  byte incoming_serial_byte = 0;
  byte looping = 1;
  String userinput = "";

  while (looping)
  {
    if (port_to_use->available() == 0)
    { // wait for the next keystroke
      if (keyer_machine_mode == KEYER_NORMAL)
      { // might as well do something while we're waiting
        check_paddles();
        service_dit_dah_buffers();
        service_send_buffer(PRINTCHAR);

        check_ptt_tail();
#ifdef FEATURE_POTENTIOMETER
        if (configuration.pot_activated)
        {
          check_potentiometer();
        }
#endif

#ifdef FEATURE_ROTARY_ENCODER
        check_rotary_encoder();
#endif // FEATURE_ROTARY_ENCODER
      }
    }
    else
    {
      incoming_serial_byte = port_to_use->read();
      port_to_use->write(incoming_serial_byte);
      if ((incoming_serial_byte == 8) || (incoming_serial_byte == 127))
      { // backspace / DEL
        userinput.remove(userinput.length() - 1, 1);
      }
      incoming_serial_byte = uppercase(incoming_serial_byte);
      if ((incoming_serial_byte > 31) && (incoming_serial_byte < 127))
      {
        userinput.concat((char)incoming_serial_byte);
      }
      if (incoming_serial_byte == 13 || incoming_serial_byte == 10)
      { // carriage return - get out
        looping = 0;
      }
    }
  } // while (looping)
#if (!defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_ARCH_MBED) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)) || (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024))
  if (userinput.startsWith("EEPROMDUMP"))
  {
    cli_eeprom_dump(port_to_use);
    return;
  }
#endif // #if !defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_ARCH_MBED)|| (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024))
  if (userinput.startsWith("TIMING"))
  {
    cli_timing_print(port_to_use);
    return;
  }
  if (userinput.startsWith("PL "))
  {
    cli_timing_command(port_to_use, userinput.substring(3), COMMAND_PL);
    return;
  }
  if (userinput.startsWith("PT "))
  {
    cli_timing_command(port_to_use, userinput.substring(3), COMMAND_PT);
    return;
  }
#if defined(FEATURE_SEQUENCER)
  if (userinput.startsWith("PA "))
  {
    cli_timing_command(port_to_use, userinput.substring(3), COMMAND_PA);
    return;
  }
  if (userinput.startsWith("PI "))
  {
    cli_timing_command(port_to_use, userinput.substring(3), COMMAND_PI);
    return;
  }
#endif // FEATURE_SEQUENCER
#if defined(FEATURE_SD_CARD_SUPPORT)
  if (userinput.startsWith("SAVEEEPROM "))
  {
    sd_card_save_eeprom_to_file(port_to_use, userinput.substring(11));
    return;
  }
  if (userinput.startsWith("LOADEEPROM "))
  {
    sd_card_load_eeprom_from_file(port_to_use, userinput.substring(11));
    return;
  }
  if (userinput.startsWith("PRINTLOG"))
  {
    sd_card_print_file(port_to_use, "/keyer/keyer.log");
    return;
  }
  if (userinput.startsWith("CLEARLOG"))
  {
    sd_card_clear_log_file(port_to_use, "/keyer/keyer.log");
    return;
  }
  if (userinput.startsWith("LS "))
  {
    cli_sd_ls_command(port_to_use, userinput.substring(3));
    return;
  }
  if (userinput.startsWith("CAT "))
  {
    sd_card_print_file(port_to_use, userinput.substring(4));
    return;
  }
#endif // defined(FEATURE_SD_CARD_SUPPORT)

#if defined(FEATURE_PADDLE_ECHO)
  if (userinput.startsWith("PF "))
  {
    cli_paddle_echo_factor(port_to_use, userinput.substring(3));
    return;
  }
#endif // defined(FEATURE_PADDLE_ECHO)

#if defined(FEATURE_AUTOSPACE)
  if (userinput.startsWith("AF "))
  {
    cli_autospace_timing_factor(port_to_use, userinput.substring(3));
    return;
  }
#endif // defined(FEATURE_AUTOSPACE)

  if (userinput.startsWith("COMP "))
  {
    cli_keying_compensation(port_to_use, userinput.substring(5));
    return;
  }

  port_to_use->println(F("\r\nError"));
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)
void cli_keying_compensation(PRIMARY_SERIAL_CLS *port_to_use, String command_arguments)
{

  configuration.keying_compensation = command_arguments.toInt();
  config_dirty = 1;
  port_to_use->print(F("\r\nKeying Compensation Set To: "));
  port_to_use->print(configuration.keying_compensation);
  port_to_use->println(F(" mS"));
  if (configuration.keying_compensation > (0.90 * (1200.0 / (float)configuration.wpm)))
  {
    port_to_use->println(F("WARNING: This is setting is probably too high for your current speed setting."));
  }
  config_dirty = 1;
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS) && defined(FEATURE_AUTOSPACE)

/*

Chapter Three


The next day Colin did some research on Walter.   An Internet search yielded his address, not far from the coffee shop where they had met.  On satellite photography the house was visible, and luckily there was street photography which revealed a modest brick house, probably built in the late 50s.  He decided to take a ride to drive by the house, not knowing what purpose it would serve, but he had little else to do to get more information about Walter and this world he had stumbled upon.

Colin made the trek to the area near the coffee house that he had made before, and found his way to the neighborhood of nicely kept homes and tree-lined streets.  Passing by Walter's house slowly he saw the house he had seen in satellite photography.  There was nothing special or noteworthy, except for a sign in the front yard, taped to a maple tree between the sidewalk and street.  "YARD SALE - SATURDAY + SUNDAY 8 AM to 3 PM".  "Perfect" Colin said to himself.  Perhaps he could garner some information from relatives or perhaps even buy some of his old equipment he thought.

Saturday morning he woke up bright and early and drove to the home, arriving about 7:30.  There were now several tables in the driveway with items on them, with two women and several children bringing out items from the house.  Colin parked up the street and verified with a quick glance he had cash in his wallet.  He walked up the short driveway and started browsing at items on the table, trying to look like a normal yard sale person and not a spy on a reconnaissance mission.

"Hello"  Colin looked up from the table and saw a woman greeting him.  "I'm Nicole."

He could see familiar facial features and Walter's eyes which let him know immediately that this was his daughter.   "Hi", he replied.

"Looking for anything in particular?"

"Oh, just radio stuff."  Colin said somewhat nervously.

"Did you know Walter?" she asked.

"I met him once, but I didn't really know him." Colin replied, careful not to reveal too much information.

"There are some of his big radios inside, and we have many books about radio over in that box." pointing to a cardboard box by the garage.  "You're the second person to ask about radio items.  We had someone stop in here yesterday while we were setting up.  He bought all of Walter's homemade electronic boxes and gave us a lot of money."

"Homemade electronics?" Colin asked.

"Yes, he would make a lot of stuff in his workshop.  We used to joke that he was a mad scientist.  We never quite knew what he was making.  He had many silver boxes that he used to hook up to his radios."

"Was the man buying the boxes a ham radio guy?"

"I'm not sure.  He came here with a big wad of cash in his pocket.  He wasn't interested in all at the big radios in the house…. the good stuff that I think is worth a lot of money, or maybe not.  When we saw the cash he had, we though that's really what he was here for.  He was very insistent, almost rude, that we sell him all the silver boxes."

"Hmmm.  Interesting."

"Well, we have a lot of stuff to sell here.  It all has to go.  I'm driving back to Florida next week.  Feel free to go inside and look around.  The door's open."  she said.

"Thanks" Colin said, making a small smile briefly as he walked away and towards the garage.  Glancing towards the backyard he saw a wire antenna strung between the chimney and a maple tree in the backyard.  The box of books had numerous hardcover electronics and communications books from the 60s and 70s, and two cryptography books.  He picked up several books and paged through them, looking for material that could be of use.  One particular book was an entry level college electronics text that, looking at the cover, he had no interest in, however the inside had pencil-written notes of codes and block diagrams in the margins.  Colin kept this book under his arm as he continued to look through the books.

He makes his way inside the house, noticing everything in various states of disarray with furniture obviously missing and boxes everywhere.  He walks freely around the first floor and sees a stairway leading to the attic.  At the one end of the semi-finished attic Colin comes to what was obviously Walter's ham shack.  There are several radios, mostly from the late 70s and early 80s, each well cared for.  Nothing interested Colin as he had plenty of equipment at home and didn't have the need for yet another radio. Colin heard someone coming up the stairs.  It was Nicole, holding a cardboard box.  The box appeared to have been sealed with duct tape but was opened.

"I just found this in the basement.  I'm not sure what it is, but it's some radio stuff I think.  Looks like junk.  I don't know how we missed this box yesterday, but you can have it if you want." said Nicole.  "We'll probably just throw it out as we have so much junk."

Colin glanced at the box and had to withhold a gasp.  His identity string, now committed to memory, was written on the top of the box in big letters in black marker.  Colin could hardly contain himself but knew he had to keep a straight face and not display any signs of surprise or excitement.

"OK" replied Colin.  "I'll go through it and see if there's anything I can put to use."  Colin paid Nicole a dollar for the book and made his way to his car, and placed the box in the trunk.

As Colin leaves the yard sale and drives home, he notices a car following him. At first, he dismisses it as a coincidence, but as he turns onto different streets, the car continues to tail him, never falling behind.  Feeling a growing sense of unease, Colin tries to shake off the pursuing vehicle by taking sudden turns and doubling back on his route, but the car remains hot on his trail.

Realizing that he's being followed intentionally, Colin's heart pounds in his chest as he navigates the streets, searching for a way to lose his pursuer. He knows he can't lead them back to his home, where his family might be at risk.  With adrenaline pumping through his veins, Colin makes a daring move, speeding down narrow alleys and darting through red lights, desperate to evade his unknown assailants. The tension mounts as the chase intensifies, with Colin's every maneuver met by the relentless pursuit of the car behind him.

Finally, with a burst of ingenuity, Colin spots an opportunity to outmaneuver his pursuer. With nerves of steel, he executes a risky maneuver, swerving down a deserted side street and slamming on the brakes, sending the pursuing car careening past him.

Breathless and shaken, Colin watches in relief as the car speeds off. With a shaky sigh of relief, he takes a moment to collect himself before cautiously continuing on his way home, acutely aware of the danger lurking.

As Colin arrives home, he resolves to uncover the truth behind the mysterious car chase and its connection to the secrets he's stumbled upon. Little does he know that his journey is far from over, and the dangers he faces are only just beginning.

*/

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS) && defined(FEATURE_AUTOSPACE)
void cli_autospace_timing_factor(PRIMARY_SERIAL_CLS *port_to_use, String command_arguments)
{

  configuration.autospace_timing_factor = command_arguments.toInt();
  config_dirty = 1;
  port_to_use->print(F("\r\nAutospace Timing Factor Set To: "));
  port_to_use->println((float)configuration.autospace_timing_factor / (float)100);
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS) && defined(FEATURE_AUTOSPACE)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)
void cli_paddle_echo_factor(PRIMARY_SERIAL_CLS *port_to_use, String command_arguments)
{

  configuration.cw_echo_timing_factor = command_arguments.toInt();
  config_dirty = 1;
  port_to_use->print(F("\r\nPaddle Echo Factor Set To: "));
  port_to_use->println((float)configuration.cw_echo_timing_factor / (float)100);
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)
void cli_timing_command(PRIMARY_SERIAL_CLS *port_to_use, String command_arguments, byte command_called)
{

  byte valid_command = 0;
  unsigned int parm1 = 0;
  unsigned int parm2 = 0;
  String temp_string;

  temp_string = command_arguments.substring(0, 1);
  parm1 = temp_string.toInt();
  temp_string = command_arguments.substring(2);
  parm2 = temp_string.toInt();
  if ((command_called == COMMAND_PL) || (command_called == COMMAND_PT))
  {
    if ((parm1 > 0) && (parm1 < 7))
    {
      if (command_called == COMMAND_PL)
      {
        configuration.ptt_lead_time[parm1 - 1] = parm2;
      }
      else
      {
        configuration.ptt_tail_time[parm1 - 1] = parm2;
      }
      valid_command = 1;
    }
  }

#if defined(FEATURE_SEQUENCER)
  if ((command_called == COMMAND_PA) || (command_called == COMMAND_PI))
  {
    if ((parm1 > 0) && (parm1 < 6))
    {
      if (command_called == COMMAND_PA)
      {
        configuration.ptt_active_to_sequencer_active_time[parm1 - 1] = parm2;
      }
      else
      {
        configuration.ptt_inactive_to_sequencer_inactive_time[parm1 - 1] = parm2;
      }
      valid_command = 1;
    }
  }
#endif // FEATURE_SEQUENCER

  if (!valid_command)
  {
    port_to_use->println(F("\r\nError."));
  }
  else
  {
    config_dirty = 1;
  }
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)
//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)
void cli_timing_print(PRIMARY_SERIAL_CLS *port_to_use)
{

  port_to_use->println(F("\r\nTimings (mS)"));
  port_to_use->println(F("\r\nPTT"));
  port_to_use->println(F("TX\tLead\tTail"));
  port_to_use->println(F("--\t----\t----"));
  for (int x = 0; x < 6; x++)
  {
    port_to_use->print(x + 1);
    port_to_use->print("\t");
    port_to_use->print(configuration.ptt_lead_time[x]);
    port_to_use->print("\t");
    port_to_use->println(configuration.ptt_tail_time[x]);
  }
#if defined(FEATURE_SEQUENCER)
  port_to_use->println(F("\r\nSequencer"));
  port_to_use->println(F("#\tPTT Active to Sequencer Active\tPTT Inactive to Sequencer Inactive"));
  port_to_use->println(F("-\t------------------------------\t----------------------------------"));
  for (int x = 0; x < 5; x++)
  {
    port_to_use->print(x + 1);
    port_to_use->print("\t\t\t");
    port_to_use->print(configuration.ptt_active_to_sequencer_active_time[x]);
    port_to_use->print("\t\t\t\t");
    port_to_use->println(configuration.ptt_inactive_to_sequencer_inactive_time[x]);
  }
#endif // FEATURE_SEQUENCER
  port_to_use->println(F("\r\nCommand Hints\r\n"));
  port_to_use->println(F("pl <transmitter> <mS>\tSet PTT lead time"));
  port_to_use->println(F("pt <transmitter> <mS>\tSet PTT tail time"));
#if defined(FEATURE_SEQUENCER)
  port_to_use->println(F("pa <#> <mS>\t\tSet PTT active to Sequencer active time"));
  port_to_use->println(F("pi <#> <mS>\t\tSet PTT inactive to Sequencer inactive time"));
#endif // FEATURE_SEQUENCER
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)

//---------------------------------------------------------------------
#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS) && defined(FEATURE_SD_CARD_SUPPORT)
void cli_sd_ls_command(PRIMARY_SERIAL_CLS *port_to_use, String directory)
{

  port_to_use->println();

  File dir = SD.open(directory);

  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }
    if (entry.isDirectory())
    {
      port_to_use->print("/");
      port_to_use->println(entry.name());
    }
    else
    {
      // files have sizes, directories do not
      port_to_use->print(entry.name());
      port_to_use->print("\t\t");
      port_to_use->println(entry.size(), DEC);
    }
    entry.close();
  }
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)  && defined(FEATURE_SD_CARD_SUPPORT)

//---------------------------------------------------------------------
#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS) && defined(FEATURE_SD_CARD_SUPPORT)
void sd_card_clear_log_file(PRIMARY_SERIAL_CLS *port_to_use, String filename)
{

  if (sd_card_log_state == SD_CARD_LOG_OPEN)
  {
    sdlogfile.close();
  }
  SD.remove(filename);
  sdlogfile = SD.open(filename, FILE_WRITE);
  sd_card_log_state = SD_CARD_LOG_NOT_OPEN;
  if (!sdfile)
  {
    port_to_use->println(F("Unable to open file "));
    sd_card_state = SD_CARD_ERROR_;
    sd_card_log_state = SD_CARD_LOG_ERROR;
  }
  sdlogfile.close();
}
#endif
//---------------------------------------------------------------------
#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS) && defined(FEATURE_SD_CARD_SUPPORT)
void sd_card_print_file(PRIMARY_SERIAL_CLS *port_to_use, String filename)
{

  sdfile = SD.open(filename);
  if (!sdfile)
  {
    port_to_use->print(F("Unable to open file "));
    port_to_use->println(filename);
  }
  else
  {
    port_to_use->println(F("\r\nSTART"));
    while (sdfile.available())
    {
      port_to_use->write(sdfile.read());
    }
    port_to_use->println(F("\r\nEND"));
  }
}
#endif
//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS) && defined(FEATURE_SD_CARD_SUPPORT)
void sd_card_load_eeprom_from_file(PRIMARY_SERIAL_CLS *port_to_use, String filename)
{

  uint8_t eeprom_byte_;
  unsigned int x;

  sdfile = SD.open(filename, FILE_READ);

  if (sdfile)
  {
    port_to_use->print(F("Loading eeprom from "));
    port_to_use->print(filename);
  }
  else
  {
    port_to_use->println(F("Error opening file.  Exiting."));
    return;
  }

  for (x = 0; x < memory_area_end; x++)
  {
    if (sdfile.available())
    {
      EEPROM.write(x, sdfile.read());
      if ((x % 16) == 0)
      {
        port_to_use->print("#");
      }
    }
    else
    {
      x = memory_area_end;
      port_to_use->println(F("\r\nHit end of file before end of eeprom"));
    }
  }

  sdfile.close();
  port_to_use->println(F("\r\nReading settings from eeprom."));
  read_settings_from_eeprom();
  port_to_use->println(F("Done."));
}

#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS) && defined(FEATURE_SD_CARD_SUPPORT)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS) && defined(FEATURE_SD_CARD_SUPPORT)
void sd_card_save_eeprom_to_file(PRIMARY_SERIAL_CLS *port_to_use, String filename)
{

  uint8_t eeprom_byte_in;
  unsigned int x;

  SD.remove(filename);
  sdfile = SD.open(filename, FILE_WRITE);

  if (sdfile)
  {
    port_to_use->print(F("Writing to "));
    port_to_use->print(filename);
  }
  else
  {
    port_to_use->println(F("Error opening file.  Exiting."));
    return;
  }

  for (x = 0; x < memory_area_end; x++)
  {
    eeprom_byte_in = EEPROM.read(x);
    sdfile.write(eeprom_byte_in);
    if ((x % 16) == 0)
    {
      port_to_use->print(".");
    }
  }

  sdfile.close();
  port_to_use->println(F("Done."));
}

#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && d!defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS)
//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && !defined(OPTION_EXCLUDE_EXTENDED_CLI_COMMANDS) && ((!defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_ARCH_MBED) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)) || (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024)))
void cli_eeprom_dump(PRIMARY_SERIAL_CLS *port_to_use)
{

  byte eeprom_byte_in;
  byte y = 0;
  int w = 0;
  int x = 0;

#define EEPROM_DUMP_COLUMNS 32
#define EEPROM_DUMP_LINES 30

  for (x = 0; x < memory_area_end; x++)
  {
    if (y == 0)
    {
      port_to_use->print("\r\n");
      if (x < 0x1000)
      {
        port_to_use->print("0");
      }
      if (x < 0x100)
      {
        port_to_use->print("0");
      }
      if (x < 0x10)
      {
        port_to_use->print("0");
      }
      port_to_use->print(x, HEX);
      port_to_use->print("\t");
    }
    eeprom_byte_in = EEPROM.read(x);
    if (eeprom_byte_in < 0x10)
    {
      port_to_use->print("0");
    }
    port_to_use->print(eeprom_byte_in, HEX);
    port_to_use->write(" ");
    y++;
    if (y > (EEPROM_DUMP_COLUMNS - 1))
    {
      port_to_use->print("\t");
      for (int z = x - y; z < x; z++)
      {
        eeprom_byte_in = EEPROM.read(z);
        if ((eeprom_byte_in > 31) && (eeprom_byte_in < 127))
        {
          port_to_use->write(eeprom_byte_in);
        }
        else
        {
          port_to_use->print(".");
        }
      }
      y = 0;
      w++;
      if (w > EEPROM_DUMP_LINES)
      {
        port_to_use->println(F("\r\nPress enter..."));
        while (!port_to_use->available())
        {
        }
        while (port_to_use->available())
        {
          port_to_use->read();
        }
        w = 0;
      }
    }
  }
  if (y > 0)
  {
    for (int z = (EEPROM_DUMP_COLUMNS - y); z > 0; z--)
    {
      port_to_use->write("   ");
    }
    port_to_use->print("\t");
    for (int z = x - y; z < x; z++)
    {
      eeprom_byte_in = EEPROM.read(z);
      if ((eeprom_byte_in > 31) && (eeprom_byte_in < 127))
      {
        port_to_use->write(eeprom_byte_in);
      }
      else
      {
        port_to_use->print(".");
      }
    }
  }
}
#endif
//---------------------------------------------------------------------
#ifdef FEATURE_PADDLE_ECHO
void service_paddle_echo()
{

#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering service_paddle_echo"));
#endif

  static byte paddle_echo_space_sent = 1;
  byte character_to_send = 0;
  static byte no_space = 0;

#if defined(OPTION_PROSIGN_SUPPORT)
  byte byte_temp = 0;
  static char *prosign_temp = (char *)"";
#endif

#if defined(FEATURE_DISPLAY) && defined(OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS)
  byte ascii_temp = 0;
#endif // defined(FEATURE_DISPLAY) && defined(OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS)

#if defined(FEATURE_CW_COMPUTER_KEYBOARD)
  static byte backspace_flag = 0;
  if (paddle_echo_buffer == 111111)
  {
    paddle_echo_buffer_decode_time = 0;
    backspace_flag = 1;
  } // this is a special hack to make repeating backspace work
#endif // defined(FEATURE_CW_COMPUTER_KEYBOARD)

#ifdef FEATURE_SD_CARD_SUPPORT
  char temp_string[2];
#endif

  if ((paddle_echo_buffer) && (millis() > paddle_echo_buffer_decode_time))
  {

#if defined(FEATURE_CW_COMPUTER_KEYBOARD)
    switch (paddle_echo_buffer)
    {
    case 111111:
    case 1111111:
    case 11111111:
    case 111111111:
      Keyboard.write(KEY_BACKSPACE); // backspace
      no_space = 1;
      break;
#ifdef OPTION_CW_KEYBOARD_GERMAN // DL1HTB changed sign AA for return to BK
    case 2111212:                // return prosign BK
#else
    case 1212: // prosign AA
#endif // OPTION_CW_KEYBOARD_GERMAN // #end DL1HTB changed sign AA for return to BK
      Keyboard.write(KEY_RETURN);
      no_space = 1;
      break;
    case 211222: // prosign DO
      Keyboard.write(KEY_CAPS_LOCK);
#ifdef OPTION_CW_KEYBOARD_CAPSLOCK_BEEP
      if (cw_keyboard_capslock_on)
      {
        beep();
        delay(100);
        boop();
        cw_keyboard_capslock_on = 0;
      }
      else
      {
        boop();
        beep();
        cw_keyboard_capslock_on = 1;
      }
#endif // OPTION_CW_KEYBOARD_CAPSLOCK_BEEP
      no_space = 1;
      break;

#ifdef OPTION_CW_KEYBOARD_ITALIAN // courtesy of Giorgio IZ2XBZ
    case 122121:                  // "@"
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.write(59);
      Keyboard.releaseAll();
      break;
    case 112211: // "?"
      Keyboard.write(95);
      break;
    case 11221: // "!"
      Keyboard.write(33);
      break;
    case 21121: // "/"
      Keyboard.write(38);
      break;
    case 21112: // "=" or "BT"
      Keyboard.write(41);
      break;
    case 12212: // à
      Keyboard.write(39);
      break;
    case 11211: // è
      Keyboard.write(91);
      break;
    case 12221: // ì
      Keyboard.write(61);
      break;
    case 2221: // ò
      Keyboard.write(59);
      break;
    case 1122: // ù
      Keyboard.write(92);
      break;
    case 21221: // (
      Keyboard.write(42);
      break;
    case 212212: // )
      Keyboard.write(40);
      break;
    case 12111: // &
      Keyboard.write(94);
      break;
    case 222111: //:
      Keyboard.write(62);
      break;
    case 212121: //;
      Keyboard.write(60);
      break;
    case 12121: //+
      Keyboard.write(93);
      break;
    case 211112: // -
      Keyboard.write(47);
      break;
#endif // OPTION_CW_KEYBOARD_ITALIAN

#ifdef OPTION_CW_KEYBOARD_GERMAN // DL1HTB added german keyboard mapping
    case 122121:                 // "@"
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.write('q');
      Keyboard.releaseAll();
      break;
    case 112211: // "?"
      Keyboard.write(95);
      break;
    case 11221: // "!"
      Keyboard.write(33);
      break;
    case 21121: // "/"
      Keyboard.write(38);
      break;
    case 222222: // "\"
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.write('-');
      Keyboard.releaseAll();
      // Keyboard.write(92);
      break;
    case 21112: // "=" or "BT"
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.write('0');
      Keyboard.releaseAll();
      break;
    case 1212: // "ä"
      Keyboard.write(39);
      break;
    case 2221: // "ö"
      Keyboard.write(59);
      break;
    case 1122: // "ü"
      Keyboard.write(91);
      break;
    case 2222: // "ch"
      Keyboard.write(99);
      Keyboard.write(104);
      break;
    case 2122: // "y"
      Keyboard.write(122);
      break;
    case 2211: // "z"
      Keyboard.write(121);
      break;
    case 21221: // "("
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.write('8');
      Keyboard.releaseAll();
      break;
    case 212212: // ")"
      Keyboard.write(40);
      break;
    case 12111: // "&" "AS"
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.write('6');
      Keyboard.releaseAll();
      break;
    case 222111: // ":"
      Keyboard.write(62);
      break;
    case 212121: // ";"
      Keyboard.write(60);
      break;
    case 12121: // "+"
      Keyboard.write(93);
      break;
    case 211112: // "-"
      Keyboard.write(47);
      break;
#endif // OPTION_CW_KEYBOARD_GERMAN // #end DL1HTB added german keyboard mapping

    default:
      character_to_send = convert_cw_number_to_ascii(paddle_echo_buffer);
      // if ((character_to_send > 64) && (character_to_send < 91)) {character_to_send = character_to_send + 32;}
      if ((cw_keyboard_capslock_on == 0) && (character_to_send > 64) && (character_to_send < 91))
      {
        character_to_send = character_to_send + 32;
      }
      if (character_to_send == '*')
      {
        no_space = 1;
#ifdef OPTION_UNKNOWN_CHARACTER_ERROR_TONE
        boop();
#endif // OPTION_UNKNOWN_CHARACTER_ERROR_TONE
      }
      else
      {
        if (!((backspace_flag) && ((paddle_echo_buffer == 1) || (paddle_echo_buffer == 11) || (paddle_echo_buffer == 111) || (paddle_echo_buffer == 1111) || (paddle_echo_buffer == 11111))))
        {
          Keyboard.write(char(character_to_send));
        }
        backspace_flag = 0;
      }
      break;
    }
#ifdef DEBUG_CW_COMPUTER_KEYBOARD
    debug_serial_port->print("service_paddle_echo: Keyboard.write: ");
    debug_serial_port->write(character_to_send);
    debug_serial_port->println();
#endif // DEBUG_CW_COMPUTER_KEYBOARD
#endif // defined(FEATURE_CW_COMPUTER_KEYBOARD)

#ifdef FEATURE_DISPLAY
    if (lcd_paddle_echo)
    {
#if defined(OPTION_PROSIGN_SUPPORT)
#ifndef OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS
      byte_temp = convert_cw_number_to_ascii(paddle_echo_buffer);
      if ((byte_temp > PROSIGN_START) && (byte_temp < PROSIGN_END))
      {
        prosign_temp = convert_prosign(byte_temp);
        display_scroll_print_char(prosign_temp[0]);
        display_scroll_print_char(prosign_temp[1]);
        if (strlen(prosign_temp) == 3)
          display_scroll_print_char(prosign_temp[2]);
      }
      else
      {
        display_scroll_print_char(byte(convert_cw_number_to_ascii(paddle_echo_buffer)));
      }
#else  // OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS
      ascii_temp = byte(convert_cw_number_to_ascii(paddle_echo_buffer));
      if ((ascii_temp > PROSIGN_START) && (ascii_temp < PROSIGN_END))
      {
        prosign_temp = convert_prosign(ascii_temp);
        display_scroll_print_char(prosign_temp[0]);
        display_scroll_print_char(prosign_temp[1]);
        if (strlen(prosign_temp) == 3)
          display_scroll_print_char(prosign_temp[2]);
      }
      else
      {
        switch (ascii_temp)
        {
        case 220:
          ascii_temp = 0;
          break; // U_umlaut  (D, ...)
        case 214:
          ascii_temp = 1;
          break; // O_umlaut  (D, SM, OH, ...)
        case 196:
          ascii_temp = 2;
          break; // A_umlaut  (D, SM, OH, ...)
        case 198:
          ascii_temp = 3;
          break; // AE_capital (OZ, LA)
        case 216:
          ascii_temp = 4;
          break; // OE_capital (OZ, LA)
        case 197:
          ascii_temp = 6;
          break; // AA_capital (OZ, LA, SM)
        case 209:
          ascii_temp = 7;
          break; // N-tilde (EA)
        }
        display_scroll_print_char(ascii_temp);
      }
#endif // OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS

#else // ! OPTION_PROSIGN_SUPPORT

#ifndef OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS
      display_scroll_print_char(byte(convert_cw_number_to_ascii(paddle_echo_buffer)));
#else  // OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS
      ascii_temp = byte(convert_cw_number_to_ascii(paddle_echo_buffer));
      switch (ascii_temp)
      {
      case 220:
        ascii_temp = 0;
        break; // U_umlaut  (D, ...)
      case 214:
        ascii_temp = 1;
        break; // O_umlaut  (D, SM, OH, ...)
      case 196:
        ascii_temp = 2;
        break; // A_umlaut  (D, SM, OH, ...)
      case 198:
        ascii_temp = 3;
        break; // AE_capital (OZ, LA)
      case 216:
        ascii_temp = 4;
        break; // OE_capital (OZ, LA)
      case 197:
        ascii_temp = 6;
        break; // AA_capital (OZ, LA, SM)
      case 209:
        ascii_temp = 7;
        break; // N-tilde (EA)
      }
      display_scroll_print_char(ascii_temp);
#endif // OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS
#endif // OPTION_PROSIGN_SUPPORT
    }
#endif // FEATURE_DISPLAY

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
#if defined(OPTION_PROSIGN_SUPPORT)
    byte_temp = convert_cw_number_to_ascii(paddle_echo_buffer);
    if (cli_paddle_echo)
    {
      if ((byte_temp > PROSIGN_START) && (byte_temp < PROSIGN_END))
      {
        primary_serial_port->print(prosign_temp[0]);
        primary_serial_port->print(prosign_temp[1]);
        if (strlen(prosign_temp) == 3)
          primary_serial_port->print(prosign_temp[2]);
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
        secondary_serial_port->print(prosign_temp[0]);
        secondary_serial_port->print(prosign_temp[1]);
        if (strlen(prosign_temp) == 3)
          secondary_serial_port->print(prosign_temp[2]);
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
      }
      else
      {
        if (configuration.cli_mode == CLI_MILL_MODE_KEYBOARD_RECEIVE)
        {
          primary_serial_port->println();
          primary_serial_port->println();
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
          secondary_serial_port->println();
          secondary_serial_port->println();
#endif
#ifdef FEATURE_SD_CARD_SUPPORT
          sd_card_log("\r\nTX:", 0);
#endif
          configuration.cli_mode = CLI_MILL_MODE_PADDLE_SEND;
        }
        primary_serial_port->write(byte_temp);
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
        secondary_serial_port->write(byte_temp);
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
#ifdef FEATURE_SD_CARD_SUPPORT
        sd_card_log("", incoming_serial_byte);
#endif
      }
    }

#else // ! OPTION_PROSIGN_SUPPORT
    if (cli_paddle_echo)
    {
      if (configuration.cli_mode == CLI_MILL_MODE_KEYBOARD_RECEIVE)
      {
        primary_serial_port->println();
        primary_serial_port->println();
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
        secondary_serial_port->println();
        secondary_serial_port->println();
#endif
#ifdef FEATURE_SD_CARD_SUPPORT
        sd_card_log("\r\nTX:", 0);
#endif
        configuration.cli_mode = CLI_MILL_MODE_PADDLE_SEND;
      }
      primary_serial_port->write(byte(convert_cw_number_to_ascii(paddle_echo_buffer)));
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
      secondary_serial_port->write(byte(convert_cw_number_to_ascii(paddle_echo_buffer)));
#endif
#ifdef FEATURE_SD_CARD_SUPPORT
      sd_card_log("", convert_cw_number_to_ascii(paddle_echo_buffer));
#endif
    }
#endif // OPTION_PROSIGN_SUPPORT
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)

    paddle_echo_buffer = 0;
    paddle_echo_buffer_decode_time = millis() + (float(600 / configuration.wpm) * length_letterspace);
    paddle_echo_space_sent = 0;
  }

  // is it time to echo a space?
  if ((paddle_echo_buffer == 0) && (millis() > (paddle_echo_buffer_decode_time + (float(1200 / configuration.wpm) * (configuration.length_wordspace - length_letterspace)))) && (!paddle_echo_space_sent))
  {

#if defined(FEATURE_CW_COMPUTER_KEYBOARD)
    if (!no_space)
    {
      Keyboard.write(' ');
#ifdef DEBUG_CW_COMPUTER_KEYBOARD
      debug_serial_port->println("service_paddle_echo: Keyboard.write: <space>");
#endif // DEBUG_CW_COMPUTER_KEYBOARD
    }
    no_space = 0;
#endif // defined(FEATURE_CW_COMPUTER_KEYBOARD)

#ifdef FEATURE_DISPLAY
    if (lcd_paddle_echo)
    {
      display_scroll_print_char(' ');
    }
#endif // FEATURE_DISPLAY

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
    if (cli_paddle_echo)
    {
      primary_serial_port->write(" ");

#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
      secondary_serial_port->write(" ");
#endif
    }
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)

#ifdef FEATURE_SD_CARD_SUPPORT
    sd_card_log(" ", 0);
#endif

    paddle_echo_space_sent = 1;
  }
}
#endif // FEATURE_PADDLE_ECHO

//---------------------------------------------------------------------
#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && defined(FEATURE_MEMORIES)
void serial_set_memory_repeat(PRIMARY_SERIAL_CLS *port_to_use)
{

  int temp_int = serial_get_number_input(5, -1, 32000, port_to_use, RAISE_ERROR_MSG);
  if (temp_int > -1)
  {
    configuration.memory_repeat_time = temp_int;
    config_dirty = 1;
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && defined(FEATURE_MEMORIES)
void repeat_play_memory(PRIMARY_SERIAL_CLS *port_to_use)
{

  byte memory_number = serial_get_number_input(2, 0, (number_of_memories + 1), port_to_use, RAISE_ERROR_MSG);
#ifdef DEBUG_CHECK_SERIAL
  debug_serial_port->print(F("repeat_play_memory: memory_number:"));
  debug_serial_port->println(memory_number);
#endif // DEBUG_SERIAL
  if (memory_number > -1)
  {
    repeat_memory = memory_number - 1;
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && defined(FEATURE_MEMORIES)

void serial_play_memory(byte memory_number)
{

  if (memory_number < number_of_memories)
  {
    add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
    add_to_send_buffer(memory_number);
    repeat_memory = 255;
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
int serial_get_number_input(byte places, int lower_limit, int upper_limit, PRIMARY_SERIAL_CLS *port_to_use, int raise_error_message)
{
  byte incoming_serial_byte = 0;
  byte looping = 1;
  byte error = 0;
  String numberstring = "";
  byte numberindex = 0;
  int numbers[6];

  while (looping)
  {
    if (port_to_use->available() == 0)
    { // wait for the next keystroke
      if (keyer_machine_mode == KEYER_NORMAL)
      { // might as well do something while we're waiting
        check_paddles();
        service_dit_dah_buffers();
        service_send_buffer(PRINTCHAR);

        check_ptt_tail();
#ifdef FEATURE_POTENTIOMETER
        if (configuration.pot_activated)
        {
          check_potentiometer();
        }
#endif

#ifdef FEATURE_ROTARY_ENCODER
        check_rotary_encoder();
#endif // FEATURE_ROTARY_ENCODER
      }
    }
    else
    {
      incoming_serial_byte = port_to_use->read();
      port_to_use->write(incoming_serial_byte);
      if ((incoming_serial_byte > 47) && (incoming_serial_byte < 58))
      { // ascii 48-57 = "0" - "9")
        numberstring = numberstring + incoming_serial_byte;
        numbers[numberindex] = incoming_serial_byte;
        numberindex++;
        if (numberindex > places)
        {
          looping = 0;
          error = 1;
        }
      }
      else
      {
        if (incoming_serial_byte == 13)
        { // carriage return - get out
          looping = 0;
        }
        else
        { // bogus input - error out
          looping = 0;
          error = 1;
        }
      }
    }
  }
  if (error)
  {
    if (raise_error_message == RAISE_ERROR_MSG)
    {
      port_to_use->println(F("Error..."));
    }
    while (port_to_use->available() > 0)
    {
      incoming_serial_byte = port_to_use->read();
    } // clear out buffer
    return (-1);
  }
  else
  {
    int y = 1;
    int return_number = 0;
    for (int x = (numberindex - 1); x >= 0; x = x - 1)
    {
      return_number = return_number + ((numbers[x] - 48) * y);
      y = y * 10;
    }
    if ((return_number > lower_limit) && (return_number < upper_limit))
    {
      return (return_number);
    }
    else
    {
      if (raise_error_message == RAISE_ERROR_MSG)
      {
        port_to_use->println(F("Error..."));
      }
      return (-1);
    }
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_change_wordspace(PRIMARY_SERIAL_CLS *port_to_use)
{
  int set_wordspace_to = serial_get_number_input(2, 0, 100, port_to_use, RAISE_ERROR_MSG);
  if (set_wordspace_to > 0)
  {
    config_dirty = 1;
    configuration.length_wordspace = set_wordspace_to;
    port_to_use->write("\r\nWordspace set to ");
    port_to_use->println(set_wordspace_to, DEC);
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_switch_tx(PRIMARY_SERIAL_CLS *port_to_use)
{
  int set_tx_to = serial_get_number_input(1, 0, 7, port_to_use, RAISE_ERROR_MSG);
  if (set_tx_to > 0)
  {
    switch (set_tx_to)
    {
    case 1:
      switch_to_tx_silent(1);
      port_to_use->print(F("\r\nSwitching to TX #"));
      port_to_use->println(F("1"));
      break;
    case 2:
      if ((ptt_tx_2) || (tx_key_line_2))
      {
        switch_to_tx_silent(2);
        port_to_use->print(F("\r\nSwitching to TX #"));
      }
      port_to_use->println(F("2"));
      break;
    case 3:
      if ((ptt_tx_3) || (tx_key_line_3))
      {
        switch_to_tx_silent(3);
        port_to_use->print(F("\r\nSwitching to TX #"));
      }
      port_to_use->println(F("3"));
      break;
    case 4:
      if ((ptt_tx_4) || (tx_key_line_4))
      {
        switch_to_tx_silent(4);
        port_to_use->print(F("\r\nSwitching to TX #"));
      }
      port_to_use->println(F("4"));
      break;
    case 5:
      if ((ptt_tx_5) || (tx_key_line_5))
      {
        switch_to_tx_silent(5);
        port_to_use->print(F("\r\nSwitching to TX #"));
      }
      port_to_use->println(F("5"));
      break;
    case 6:
      if ((ptt_tx_6) || (tx_key_line_6))
      {
        switch_to_tx_silent(6);
        port_to_use->print(F("\r\nSwitching to TX #"));
      }
      port_to_use->println(F("6"));
      break;
    }
  }
}
#endif

//---------------------------------------------------------------------
#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_set_dit_to_dah_ratio(PRIMARY_SERIAL_CLS *port_to_use)
{
  int set_ratio_to = serial_get_number_input(4, 99, 1000, port_to_use, DONT_RAISE_ERROR_MSG);
  // if ((set_ratio_to > 99) && (set_ratio_to < 1000)) {
  //   configuration.dah_to_dit_ratio = set_ratio_to;
  //   port_to_use->print(F("Setting dah to dit ratio to "));
  //   port_to_use->println((float(configuration.dah_to_dit_ratio)/100));
  //   config_dirty = 1;
  // }

  if ((set_ratio_to < 100) || (set_ratio_to > 999))
  {
    set_ratio_to = 300;
  }
  configuration.dah_to_dit_ratio = set_ratio_to;
  port_to_use->write("\r\nDah to dit ratio set to ");
  port_to_use->println((float(configuration.dah_to_dit_ratio) / 100));
  config_dirty = 1;
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_set_serial_number(PRIMARY_SERIAL_CLS *port_to_use)
{
  int set_serial_number_to = serial_get_number_input(4, 0, 10000, port_to_use, RAISE_ERROR_MSG);
  if (set_serial_number_to > 0)
  {
    serial_number = set_serial_number_to;
    port_to_use->write("\r\nSetting serial number to ");
    port_to_use->println(serial_number);
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && defined(FEATURE_POTENTIOMETER)
void serial_set_pot_low_high(PRIMARY_SERIAL_CLS *port_to_use)
{
  int serial_get_number = serial_get_number_input(4, 500, 10000, port_to_use, RAISE_ERROR_MSG);
  int low_number = (serial_get_number / 100);
  int high_number = serial_get_number % (int(serial_get_number / 100) * 100);
  if ((low_number < high_number) && (low_number >= wpm_limit_low) && (high_number <= wpm_limit_high))
  {
    port_to_use->print(F("\r\nSetting potentiometer range to "));
    port_to_use->print(low_number);
    port_to_use->print(F(" - "));
    port_to_use->print(high_number);
    port_to_use->println(F(" WPM"));
    pot_wpm_low_value = low_number;
    pot_wpm_high_value = high_number;
  }
  else
  {
    port_to_use->println(F("\nError"));
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_set_sidetone_freq(PRIMARY_SERIAL_CLS *port_to_use)
{
  int set_sidetone_hz = serial_get_number_input(4, (sidetone_hz_limit_low - 1), (sidetone_hz_limit_high + 1), port_to_use, RAISE_ERROR_MSG);
  if ((set_sidetone_hz > sidetone_hz_limit_low) && (set_sidetone_hz < sidetone_hz_limit_high))
  {
    port_to_use->write("\r\nSetting sidetone to ");
    port_to_use->print(set_sidetone_hz, DEC);
    port_to_use->println(F(" hz"));
    configuration.hz_sidetone = set_sidetone_hz;
    config_dirty = 1;
#ifdef FEATURE_DISPLAY
    if (LCD_COLUMNS < 9)
      lcd_center_print_timed(String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
    else
      lcd_center_print_timed("Sidetone " + String(configuration.hz_sidetone) + " Hz", 0, default_display_msg_delay);
#endif // FEATURE_DISPLAY
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_wpm_set(PRIMARY_SERIAL_CLS *port_to_use)
{
  int set_wpm = serial_get_number_input(3, 0, 1000, port_to_use, RAISE_ERROR_MSG);
  if (set_wpm > 0)
  {
    speed_set(set_wpm);
    port_to_use->write("\r\nSetting WPM to ");
    port_to_use->println(set_wpm, DEC);
    config_dirty = 1;
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE) && defined(FEATURE_FARNSWORTH)
void serial_set_farnsworth(PRIMARY_SERIAL_CLS *port_to_use)
{
  int set_farnsworth_wpm = serial_get_number_input(3, -1, 1000, port_to_use, RAISE_ERROR_MSG);
  if ((set_farnsworth_wpm > 0) || (set_farnsworth_wpm == 0))
  {
    configuration.wpm_farnsworth = set_farnsworth_wpm;
    port_to_use->write("\r\nSetting Farnsworth WPM to ");
    port_to_use->println(set_farnsworth_wpm, DEC);
    config_dirty = 1;
  }
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_set_weighting(PRIMARY_SERIAL_CLS *port_to_use)
{
  int set_weighting = serial_get_number_input(2, 9, 91, port_to_use, DONT_RAISE_ERROR_MSG);
  if (set_weighting < 1)
  {
    set_weighting = 50;
  }
  configuration.weighting = set_weighting;
  port_to_use->write("\r\nSetting weighting to ");
  port_to_use->println(set_weighting, DEC);
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_tune_command(PRIMARY_SERIAL_CLS *port_to_use)
{
  byte incoming;

  delay(100);
  while (port_to_use->available() > 0)
  { // clear out the buffer if anything is there
    incoming = port_to_use->read();
  }

  sending_mode = MANUAL_SENDING;
  tx_and_sidetone_key(1);
  port_to_use->println(F("\r\nKeying tx - press a key to unkey"));
#ifdef FEATURE_BUTTONS
  while ((port_to_use->available() == 0) && (!analogbuttonread(0)))
  {
  } // keystroke or button0 hit gets us out of here
#else
  while (port_to_use->available() == 0)
  {
  }
#endif
  while (port_to_use->available() > 0)
  { // clear out the buffer if anything is there
    incoming = port_to_use->read();
  }
  tx_and_sidetone_key(0);
}
#endif

//---------------------------------------------------------------------

#ifdef FEATURE_TRAINING_COMMAND_LINE_INTERFACE

String generate_callsign(byte callsign_mode)
{

  static String callsign(10);
  char nextchar;
  char word_buffer[10];

  callsign = "";

  if (callsign_mode == CALLSIGN_INTERNATIONAL)
  {
    if (random(1, 101) < 96)
    {
      // start with a letter 96% of the time
      nextchar = random(65, 91);
      callsign = callsign + nextchar;
      if (random(1, 101) < 20)
      { // randomly add second prefix letter 20% of the time
        nextchar = random(65, 91);
        callsign = callsign + nextchar;
      }
    }
    else
    {
      // start with a number
      nextchar = random(49, 58); // generate the number
      callsign = callsign + nextchar;
      nextchar = random(65, 91); // must add a letter next
      callsign = callsign + nextchar;
    }
  } // CALLSIGN_INTERNATIONAL

  if (callsign_mode == CALLSIGN_US)
  {
    switch (random(1, 5))
    {
    case 1:
      callsign = "K";
      break;
    case 2:
      callsign = "W";
      break;
    case 3:
      callsign = "N";
      break;
    case 4:
      callsign = "A";
      break;
    }
    if (callsign == "A")
    { // if the first letter is A, we definitely need a second letter before the number
      nextchar = random(65, 91);
      callsign = callsign + nextchar;
    }
    else
    {
      // randomly add a second letter for K, W, N prefixes
      if (random(1, 101) < 51)
      {
        nextchar = random(65, 91);
        callsign = callsign + nextchar;
      }
    }
  } // CALLSIGN_US

  if (callsign_mode == CALLSIGN_CANADA)
  {
    strcpy_P(word_buffer, (char *)pgm_read_word(&(canadian_prefix_table[random(0, canadian_prefix_size)])));
    callsign = word_buffer;
  }

  if (callsign_mode == CALLSIGN_EUROPEAN)
  {

    strcpy_P(word_buffer, (char *)pgm_read_word(&(eu_prefix_table[random(0, eu_prefix_size)])));
    callsign = word_buffer;
  }

  if (callsign_mode != CALLSIGN_CANADA)
  {
    nextchar = random(48, 58); // generate the number
    callsign = callsign + nextchar;
  }

  nextchar = random(65, 91); // generate first letter after number
  callsign = callsign + nextchar;
  if ((random(1, 101) < 40) || (callsign_mode == CALLSIGN_CANADA))
  { // randomly put a second character after the number
    nextchar = random(65, 91);
    callsign = callsign + nextchar;
    if ((random(1, 101) < 96) || (callsign_mode == CALLSIGN_CANADA))
    { // randomly put a third character after the number
      nextchar = random(65, 91);
      callsign = callsign + nextchar;
    }
  }

  if (random(1, 101) < 10)
  { // randomly put a slash something on the end like /QRP or /#
    if (random(1, 101) < 25)
    {
      callsign = callsign + "/QRP";
    }
    else
    {
      nextchar = random(48, 58);
      callsign = callsign + "/" + nextchar;
    }
  }

  return callsign;
}

#endif // FEATURE_TRAINING_COMMAND_LINE_INTERFACE
//---------------------------------------------------------------------
// #if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
// void paqso_practice(PRIMARY_SERIAL_CLS * port_to_use){

//   // VT100 emulation in Linux: screen /dev/ttyACM1 115200 term vt100

//   #define CONTEST_PRACTICE_IDLE 0
//   #define CONTEST_PRACTICE_CQ_SENT 1
//   #define CONTEST_PRACTICE_REPORT_SENT 2

//   #define FIELD_CALLSIGN 0
//   #define FIELD_NR 1
//   #define FIELD_SECTION 2

//   byte overall_state = CONTEST_PRACTICE_IDLE;
//   byte loop1 = 1;
//   byte user_input_buffer[10];
//   byte user_input_buffer_characters = 0;
//   byte incoming_char = 0;
//   byte process_user_input_buffer = 0;
//   unsigned long escape_flag_time = 0;
//   String callsign;
//   String nr;
//   String section;
//   byte cq_answered = 0;
//   unsigned long transition_time = 0;
//   byte current_field = FIELD_CALLSIGN;

//   int previous_sidetone = configuration.hz_sidetone;
//   int previous_wpm = configuration.wpm;
//   int caller_sidetone = 0;
//   int caller_wpm_delta = 0;

//   while (port_to_use->available() > 0) {  // clear out the buffer if anything is there
//     port_to_use->read();
//   }

//   term.init();
//   term.cls();
//   term.position(0,0);
//   term.println(F("\nPA QSO Party Practice\n"));
//   term.println(F("This requires VT100 emulation!\n"));
//   term.println(F("F1 - Call CQ"));
//   term.println(F("F2 - Exchange"));
//   term.println(F("F3 - TU"));
//   term.println(F("Insert - Callsign + Exchange"));
//   term.println(F("\\ - Exit\n"));
//   term.println(F("Callsign  NR  Section"));
//   term.println(F("-------- ---- -------\n\n"));

//   while (loop1){
//     // get user keyboard input
//     if (port_to_use->available()){
//       user_input_buffer[user_input_buffer_characters] = toupper(port_to_use->read());
//       switch(user_input_buffer[user_input_buffer_characters]){
//         case 27: //escape
//           escape_flag_time = millis();
//           user_input_buffer_characters++;
//         case 13: //return
//         case 32: //space
//           process_user_input_buffer = 1;
//           break;
//         case 127:
//         case 8: //backspace
//           if (user_input_buffer_characters > 0){user_input_buffer_characters--;}
//           port_to_use->write(27);
//           port_to_use->write(91);
//           port_to_use->write(49);
//           port_to_use->write(68);
//           break;

//         default:
//           if (!(((user_input_buffer[user_input_buffer_characters-1] == 27) && (user_input_buffer[user_input_buffer_characters] == 79) && (user_input_buffer_characters>0)) ||
//           ((user_input_buffer[user_input_buffer_characters-2] == 27) && (user_input_buffer[user_input_buffer_characters-1] == 79) && (user_input_buffer_characters>1)))){
//             port_to_use->write(user_input_buffer[user_input_buffer_characters]);
//           }
//           user_input_buffer_characters++;
//           break;
//       } //switch(user_input_buffer[user_input_buffer_characters])
//       if (user_input_buffer_characters == 10){process_user_input_buffer = 1;}

//     }//(port_to_use->available())

//     // process user keyboard input
//     if ((process_user_input_buffer) && ((escape_flag_time == 0) || ((millis()-escape_flag_time) > 100))){

//       #ifdef DEBUG_CW_PRACTICE
//       debug_serial_port->print(F("escape_flag_time: process_user_input_buffer user_input_buffer_characters:"));
//       debug_serial_port->println(user_input_buffer_characters);
//       #endif

//       if (user_input_buffer_characters > 0){
//         if (user_input_buffer[0] == '\\'){  // does user want to exit?
//           loop1 = 0;
//         } else {
//           if (user_input_buffer[0] == 27){
//             if (user_input_buffer_characters == 3){
//               if ((user_input_buffer[1] == 79) && (user_input_buffer[2] == 80)) {  //VT100 F1 key
//                 configuration.hz_sidetone = previous_sidetone;
//                 configuration.wpm = previous_wpm;
//                 add_to_send_buffer('C');
//                 add_to_send_buffer('Q');
//                 add_to_send_buffer(' ');
//                 add_to_send_buffer('T');
//                 add_to_send_buffer('E');
//                 add_to_send_buffer('S');
//                 add_to_send_buffer('T');
//                 add_to_send_buffer(' ');
//                 add_to_send_buffer('D');
//                 add_to_send_buffer('E');
//                 add_to_send_buffer(' ');
//                 add_to_send_buffer('K');
//                 add_to_send_buffer('3');
//                 add_to_send_buffer('N');
//                 add_to_send_buffer('G');
//                 overall_state = CONTEST_PRACTICE_CQ_SENT;
//                 transition_time = millis();
//               } //((user_input_buffer[1] == 79) && (user_input_buffer[2] == 80)) VT100 F1 key
//             } //(user_input_buffer_characters == 3)
//             if (user_input_buffer_characters == 4){
//               if ((user_input_buffer[1] == 91) && (user_input_buffer[2] == 50)  && (user_input_buffer[3] == 126)) { //VT100 INS key
//                 for (byte x = 0; x < user_input_buffer_characters; x++) {
//                   add_to_send_buffer(user_input_buffer[x]);
//                 }
//                 add_to_send_buffer(' ');
//                 add_to_send_buffer('0');
//                 add_to_send_buffer('0');
//                 add_to_send_buffer('1');
//                 add_to_send_buffer(' ');
//                 add_to_send_buffer('C');
//                 add_to_send_buffer('A');
//                 add_to_send_buffer('R');
//                 configuration.hz_sidetone = previous_sidetone;
//                 configuration.wpm = previous_wpm;
//                 overall_state = CONTEST_PRACTICE_REPORT_SENT;
//               }
//             } //(user_input_buffer_characters == 4)

//           } else { //(user_input_buffer[0] == 27)

//           // we have a callsign, nr, or section

//             switch(current_field){
//               case FIELD_CALLSIGN:
//                 callsign = "";
//                 for (byte x = 0; x < user_input_buffer_characters; x++) {
//                   callsign.concat(char(user_input_buffer[x]));
//                 }
//                 current_field = FIELD_NR;
//                 break;

//               case FIELD_NR:
//                 nr = "";
//                 for (byte x = 0; x < user_input_buffer_characters; x++) {
//                   nr.concat(char(user_input_buffer[x]));
//                 }
//                 current_field = FIELD_SECTION;
//                 break;

//               case FIELD_SECTION:
//                 section = "";
//                 for (byte x = 0; x < user_input_buffer_characters; x++) {
//                   section.concat(char(user_input_buffer[x]));
//                 }
//                 current_field = FIELD_CALLSIGN;
//                 break;
//             }
//             term.position(13,0);
//             term.print(callsign);
//             term.position(13,9);
//             term.print(nr);
//             term.position(13,14);
//             term.println(section);
//             term.position(15,0);
//             term.print(F("                     "));
//             term.position(15,0);
//           }
//         } //(user_input_buffer[0] == '\\')
//       } //(user_input_buffer_characters > 0)
//       process_user_input_buffer = 0;
//       user_input_buffer_characters = 0;
//       escape_flag_time = 0;
//     } //((process_user_input_buffer) && ((escape_flag_time == 0) || ((millis() -escape_flag_time) > 100)))

//     //do autonomous events
//     service_send_buffer(NOPRINT);

//     switch(overall_state){
//       case CONTEST_PRACTICE_CQ_SENT:
//         if (send_buffer_bytes == 0){
//           if (!cq_answered){
//             if (((millis() - transition_time) > random(250,1500))){  // add some random delay
//               callsign = generate_callsign();
//               caller_sidetone = random(500,1000);
//               configuration.hz_sidetone = caller_sidetone;
//               caller_wpm_delta = random(-5,5);
//               configuration.wpm = configuration.wpm + caller_wpm_delta;
//               for (byte x = 0; x < (callsign.length()); x++) {
//                 add_to_send_buffer(callsign[x]);
//               }
//               cq_answered = 1;
//               transition_time = millis();
//             }
//           } else {  //send it again
//             if ((cq_answered) && ((millis() - transition_time) > random(2000,4000))){
//               configuration.hz_sidetone = caller_sidetone;
//               configuration.wpm = configuration.wpm + caller_wpm_delta;
//               for (byte x = 0; x < (callsign.length()); x++) {
//                 add_to_send_buffer(callsign[x]);
//               }
//               cq_answered++;
//               transition_time = millis();
//             }
//           }
//         } else {
//           transition_time = millis();
//         } //send_buffer_bytes == 0
//         break;  //CONTEST_PRACTICE_CQ_SENT
//     } //switch(overall_state)
//   } //while (loop1)

//   configuration.hz_sidetone = previous_sidetone;
//   configuration.wpm = previous_wpm;
//   send_buffer_bytes = 0;
// }
// #endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_cw_practice(PRIMARY_SERIAL_CLS *port_to_use)
{

  byte menu_loop = 1;
  byte menu_loop2 = 1;
  char incoming_char = ' ';

  check_serial_override = 1;

  byte previous_key_tx_state = key_tx;
  key_tx = 0;

  while (menu_loop)
  {

    while (port_to_use->available() > 0)
    { // clear out the buffer if anything is there
      port_to_use->read();
    }

    port_to_use->println(F("\r\n\nCW Training Menu\n"));
    port_to_use->println(F("C - Receive Practice"));
    port_to_use->println(F("I - Keyboard Interactive Receive Practice"));
    port_to_use->println(F("R - Random Groups Receive Practice"));
    port_to_use->println(F("W - Wordsworth Receive Practice"));
    port_to_use->println(F("E - Receive / Transmit Echo Practice"));
    // port_to_use->println("2 - PA QSO Party");   // Don't think this is working right / wasn't finished - Goody 2017-05-01
    port_to_use->println(F("\nX - Exit\n"));

    menu_loop2 = 1;

    while (menu_loop2)
    {

      if (port_to_use->available())
      {
        incoming_char = port_to_use->read();
        if ((incoming_char != 10) && (incoming_char != 13))
        {
          menu_loop2 = 0;
        }
      }
    }

    incoming_char = toUpperCase(incoming_char);

    switch (incoming_char)
    {
    case 'X':
      menu_loop = 0;
      break;
    case 'C':
      serial_receive_practice_menu(port_to_use, PRACTICE_NON_INTERACTIVE);
      break;
    case 'I':
      serial_receive_practice_menu(port_to_use, PRACTICE_INTERACTIVE);
      break;
    case 'R':
      serial_random_menu(port_to_use);
      break;
    case 'W':
      serial_wordsworth_menu(port_to_use);
      break;
    case 'E':
      serial_receive_transmit_echo_menu(port_to_use);
      break;
      // case '2': paqso_practice(port_to_use); break;
    } // switch(incoming_char)

  } // while(menu_loop)

  port_to_use->println(F("Exited Training module..."));
  check_serial_override = 0;
  key_tx = previous_key_tx_state;
  paddle_echo_buffer = 0;
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_receive_transmit_echo_menu(PRIMARY_SERIAL_CLS *port_to_use)
{

  byte menu_loop = 1;
  byte menu_loop2 = 1;
  char incoming_char = ' ';

  while (menu_loop)
  {

    while (port_to_use->available() > 0)
    { // clear out the buffer if anything is there
      port_to_use->read();
    }

    port_to_use->println(F("\r\n\nReceive / Transmit Echo Practice Menu\n"));
    port_to_use->println(F("I - International Callsigns"));
    port_to_use->println(F("U - US Callsigns"));
    port_to_use->println(F("E - European Callsigns"));
    port_to_use->println(F("C - Canadian Callsigns"));
    port_to_use->println(F("P - Progressive 5 Character Groups"));
    port_to_use->println(F("2 - Two Letter Words"));
    port_to_use->println(F("3 - Three Letter Words"));
    port_to_use->println(F("4 - Four Letter Words"));
    port_to_use->println(F("N - Names"));
    port_to_use->println(F("Q - QSO Words"));
    port_to_use->println(F("M - Mixed\n"));
    port_to_use->println(F("\nX - Exit\n"));

    menu_loop2 = 1;

    while (menu_loop2)
    {
      if (port_to_use->available())
      {
        incoming_char = port_to_use->read();
        if ((incoming_char != 10) && (incoming_char != 13))
        {
          menu_loop2 = 0;
        }
      }
    }

    incoming_char = toUpperCase(incoming_char);

    switch (incoming_char)
    {
    case 'X':
      menu_loop = 0;
      break;
    case 'I':
      receive_transmit_echo_practice(port_to_use, CALLSIGN_INTERNATIONAL);
      break;
    case 'U':
      receive_transmit_echo_practice(port_to_use, CALLSIGN_US);
      break;
    case 'E':
      receive_transmit_echo_practice(port_to_use, CALLSIGN_EUROPEAN);
      break;
    case 'C':
      receive_transmit_echo_practice(port_to_use, CALLSIGN_CANADA);
      break;
    case 'P':
      receive_transmit_echo_practice(port_to_use, ECHO_PROGRESSIVE_5);
      break;
    case '2':
      receive_transmit_echo_practice(port_to_use, ECHO_2_CHAR_WORDS);
      break;
    case '3':
      receive_transmit_echo_practice(port_to_use, ECHO_3_CHAR_WORDS);
      break;
    case '4':
      receive_transmit_echo_practice(port_to_use, ECHO_4_CHAR_WORDS);
      break;
    case 'N':
      receive_transmit_echo_practice(port_to_use, ECHO_NAMES);
      break;
    case 'M':
      receive_transmit_echo_practice(port_to_use, ECHO_MIXED);
      break;
    case 'Q':
      receive_transmit_echo_practice(port_to_use, ECHO_QSO_WORDS);
      break;
    } // switch(incoming_char)

  } // while(menu_loop)

  port_to_use->println(F("Exiting receive / transmit echo practice..."));
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void receive_transmit_echo_practice(PRIMARY_SERIAL_CLS *port_to_use, byte practice_mode_called)
{

  byte loop1 = 1;
  byte loop2 = 0;
  byte x = 0;
  byte user_send_loop = 0;
  String cw_to_send_to_user(10);
  char incoming_char = ' ';
  String user_sent_cw = "";
  byte paddle_hit = 0;
  unsigned long last_element_time = 0;
  unsigned long cw_char;
  byte speed_mode_before = speed_mode;
  byte keyer_mode_before = configuration.keyer_mode;
  byte progressive_step_counter;
  byte practice_mode;
  char word_buffer[10];

  speed_mode = SPEED_NORMAL; // put us in normal speed mode
  if ((configuration.keyer_mode != IAMBIC_A) && (configuration.keyer_mode != IAMBIC_B))
  {
    configuration.keyer_mode = IAMBIC_B; // we got to be in iambic mode (life is too short to make this work in bug mode)
  }

  randomSeed(millis());

#ifdef FEATURE_DISPLAY
  lcd_clear();
  if (LCD_COLUMNS > 17)
  {
    lcd_center_print_timed("Receive / Transmit", 0, default_display_msg_delay);
    lcd_center_print_timed("Echo Practice", 1, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("RX / TX", 0, default_display_msg_delay);
    if (LCD_ROWS > 1)
    {
      if (LCD_COLUMNS < 9)
      {
        lcd_center_print_timed("EchoPrct", 1, default_display_msg_delay);
      }
      else
      {
        lcd_center_print_timed("Echo Practice", 1, default_display_msg_delay);
      }
    }
  }
  service_display();
#endif

  port_to_use->println(F("Receive / Transmit Echo Practice\r\n\r\nCopy the code and send it back using the paddle."));
  port_to_use->println(F("Enter a blackslash \\ to exit.\r\n"));
  while (port_to_use->available() > 0)
  { // clear out the buffer if anything is there
    incoming_char = port_to_use->read();
  }
  port_to_use->print(F("Press enter to start...\r\n"));
  while (port_to_use->available() == 0)
  {
  }
  while (port_to_use->available() > 0)
  { // clear out the buffer if anything is there
    incoming_char = port_to_use->read();
  }

  while (loop1)
  {
    if (practice_mode_called == ECHO_MIXED)
    {
      practice_mode = random(ECHO_2_CHAR_WORDS, ECHO_QSO_WORDS + 1);
    }
    else
    {
      practice_mode = practice_mode_called;
    }

    progressive_step_counter = 255;

    switch (practice_mode)
    {
    case CALLSIGN_INTERNATIONAL:
    case CALLSIGN_US:
    case CALLSIGN_EUROPEAN:
    case CALLSIGN_CANADA:
      cw_to_send_to_user = generate_callsign(practice_mode);
      break;
    case ECHO_PROGRESSIVE_5:
      cw_to_send_to_user = (char)random(65, 91);
      cw_to_send_to_user.concat((char)random(65, 91));
      cw_to_send_to_user.concat((char)random(65, 91));
      cw_to_send_to_user.concat((char)random(65, 91));
      cw_to_send_to_user.concat((char)random(65, 91));
      progressive_step_counter = 1;
      break;
    case ECHO_2_CHAR_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s2_table[random(0, s2_size)])));
      cw_to_send_to_user = word_buffer;
      break;
    case ECHO_3_CHAR_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s3_table[random(0, s3_size)])));
      cw_to_send_to_user = word_buffer;
      break;
    case ECHO_4_CHAR_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s4_table[random(0, s4_size)])));
      cw_to_send_to_user = word_buffer;
      break;
    case ECHO_NAMES:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(name_table[random(0, name_size)])));
      cw_to_send_to_user = word_buffer;
      break;
    case ECHO_QSO_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(qso_table[random(0, qso_size)])));
      cw_to_send_to_user = word_buffer;
      break;

    } // switch (practice_mode)

    loop2 = 1;

    while (loop2)
    {
      user_send_loop = 1;
      user_sent_cw = "";
      cw_char = 0;
      x = 0;

      // send the CW to the user
      while ((x < (cw_to_send_to_user.length())) && (x < progressive_step_counter))
      {
        send_char(cw_to_send_to_user[x], KEYER_NORMAL);
        // test
        port_to_use->print(cw_to_send_to_user[x]);
        //
        x++;
      }
      port_to_use->println();

      while (user_send_loop)
      {
        // get their paddle input

#ifdef FEATURE_DISPLAY
        service_display();
#endif

#ifdef FEATURE_POTENTIOMETER
        if (configuration.pot_activated)
        {
          check_potentiometer();
        }
#endif

#ifdef FEATURE_ROTARY_ENCODER
        check_rotary_encoder();
#endif // FEATURE_ROTARY_ENCODER

        check_paddles();

        if (dit_buffer)
        {
          sending_mode = MANUAL_SENDING;
          send_dit();
          dit_buffer = 0;
          paddle_hit = 1;
          cw_char = (cw_char * 10) + 1;
          last_element_time = millis();
        }
        if (dah_buffer)
        {
          sending_mode = MANUAL_SENDING;
          send_dah();
          dah_buffer = 0;
          paddle_hit = 1;
          cw_char = (cw_char * 10) + 2;
          last_element_time = millis();
        }

        // have we hit letterspace time (end of a letter?)
        if ((paddle_hit) && (millis() > (last_element_time + (float(600 / configuration.wpm) * length_letterspace))))
        {
#ifdef DEBUG_PRACTICE_SERIAL
          debug_serial_port->println(F("receive_transmit_echo_practice: user_send_loop: hit length_letterspace"));
#endif // DEBUG_PRACTICE_SERIAL
          incoming_char = convert_cw_number_to_ascii(cw_char);
          port_to_use->print(incoming_char);
#ifdef FEATURE_DISPLAY
          display_scroll_print_char(incoming_char);
          service_display();
#endif // FEATURE_DISPLAY
          user_sent_cw.concat(incoming_char);
          cw_char = 0;
          paddle_hit = 0;
          // TODO - print it to serial and lcd
        }

// code from Piotr, SP2BPD
#if defined(FEATURE_STRAIGHT_KEY)
        long ext_key = service_straight_key();
        if (ext_key != 0)
        {
          incoming_char = convert_cw_number_to_ascii(ext_key);
          user_sent_cw.concat(incoming_char);
          cw_char = 0;
        }
#endif
        // ------

        // do we have all the characters from the user? - if so, get out of user_send_loop
        if ((user_sent_cw.length() >= cw_to_send_to_user.length()) || ((progressive_step_counter < 255) && (user_sent_cw.length() == progressive_step_counter)))
        {
          user_send_loop = 0;
          port_to_use->println();
#ifdef FEATURE_DISPLAY
          display_scroll_print_char(' ');
          service_display();
#endif
        }

        // does the user want to exit?
        while (port_to_use->available() > 0)
        {
          incoming_char = port_to_use->read();
          user_send_loop = 0;
          loop1 = 0;
          loop2 = 0;
          if (correct_answer_led)
            digitalWrite(correct_answer_led, LOW); // clear the LEDs as we exit
          if (wrong_answer_led)
            digitalWrite(wrong_answer_led, LOW);
        }
#ifdef FEATURE_BUTTONS
        while (analogbuttonread(0))
        { // can exit by pressing the Command Mode button
          user_send_loop = 0;
          loop1 = 0;
          loop2 = 0;
          if (correct_answer_led)
            digitalWrite(correct_answer_led, LOW); // clear the LEDs as we exit
          if (wrong_answer_led)
            digitalWrite(wrong_answer_led, LOW);
        }
#endif // FEATURE_BUTTONS

      } // while (user_send_loop)

      if (loop1 && loop2)
      {

        if (progressive_step_counter < 255)
        { // we're in progressive mode
          if (user_sent_cw.substring(0, progressive_step_counter) == cw_to_send_to_user.substring(0, progressive_step_counter))
          { // characters are correct
            if (correct_answer_led)
              digitalWrite(correct_answer_led, HIGH); // set the correct answer LED high
            if (wrong_answer_led)
              digitalWrite(wrong_answer_led, LOW); // clear the wrong answer LED
            beep();
            send_char(' ', 0);
            send_char(' ', 0);
            progressive_step_counter++;
            if (progressive_step_counter == 6)
            { // all five characters are correct
              loop2 = 0;
              unsigned int NEWtone = 400;     // the initial tone frequency for the tone sequence
              unsigned int TONEduration = 50; // define the duration of each tone element in the tone sequence to drive a speaker
              for (int k = 0; k < 6; k++)
              {                               // a loop to generate some increasing tones
                tone(sidetone_line, NEWtone); // generate a tone on the speaker pin
                delay(TONEduration);          // hold the tone for the specified delay period
                noTone(sidetone_line);        // turn off the tone
                NEWtone = NEWtone * 1.25;     // calculate a new value for the tone frequency
              } // end for
              send_char(' ', 0);
              send_char(' ', 0);
            }
          }
          else
          { // characters are wrong
            if (wrong_answer_led)
              digitalWrite(wrong_answer_led, HIGH); // set the wrong answer LED high
            if (correct_answer_led)
              digitalWrite(correct_answer_led, LOW); // clear the correct answer LED
            boop();
            send_char(' ', 0);
            send_char(' ', 0);
          }
        }
        else
        {
          if (user_sent_cw == cw_to_send_to_user)
          { // we only get here if progressive_step_counter is equal to 255
            beep();
            send_char(' ', 0);
            send_char(' ', 0);
            loop2 = 0;
          }
          else
          {
            boop();
            send_char(' ', 0);
            send_char(' ', 0);
          }
        }
      }

    } // loop2
  } // loop1

  speed_mode = speed_mode_before;
  configuration.keyer_mode = keyer_mode_before;
  paddle_echo_buffer = 0;
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_receive_practice_menu(PRIMARY_SERIAL_CLS *port_to_use, byte practice_mode)
{

  byte menu_loop = 1;
  byte menu_loop2 = 1;
  char incoming_char = ' ';

  while (menu_loop)
  {

    while (port_to_use->available() > 0)
    { // clear out the buffer if anything is there
      port_to_use->read();
    }

    if (practice_mode == PRACTICE_INTERACTIVE)
    {
      port_to_use->println(F("\r\n\nInteractive Receive Practice Menu\n"));
    }
    else
    {
      port_to_use->println(F("\r\n\nReceive Practice Menu\n"));
    }
    port_to_use->println(F("I - International Callsigns"));
    port_to_use->println(F("U - US Callsigns"));
    port_to_use->println(F("E - European Callsigns"));
    port_to_use->println(F("C - Canadian Callsigns"));
    port_to_use->println(F("2 - Two Letter Words"));
    port_to_use->println(F("3 - Three Letter Words"));
    port_to_use->println(F("4 - Four Letter Words"));
    port_to_use->println(F("N - Names"));
    port_to_use->println(F("Q - QSO Words"));
    port_to_use->println(F("M - Mixed Words\n"));
    port_to_use->println(F("\nX - Exit\n"));
    menu_loop2 = 1;

    while (menu_loop2)
    {

      if (port_to_use->available())
      {
        incoming_char = port_to_use->read();
        if ((incoming_char != 10) && (incoming_char != 13))
        {
          menu_loop2 = 0;
        }
      }
    }

    incoming_char = toUpperCase(incoming_char);

    if (practice_mode == PRACTICE_INTERACTIVE)
    {
      switch (incoming_char)
      {
      case 'X':
        menu_loop = 0;
        break;
      case 'I':
        serial_practice_interactive(port_to_use, CALLSIGN_INTERNATIONAL);
        break;
      case 'U':
        serial_practice_interactive(port_to_use, CALLSIGN_US);
        break;
      case 'E':
        serial_practice_interactive(port_to_use, CALLSIGN_EUROPEAN);
        break;
      case 'C':
        serial_practice_interactive(port_to_use, CALLSIGN_CANADA);
        break;
      case '2':
        serial_practice_interactive(port_to_use, PRACTICE_2_CHAR_WORDS);
        break;
      case '3':
        serial_practice_interactive(port_to_use, PRACTICE_3_CHAR_WORDS);
        break;
      case '4':
        serial_practice_interactive(port_to_use, PRACTICE_4_CHAR_WORDS);
        break;
      case 'N':
        serial_practice_interactive(port_to_use, PRACTICE_NAMES);
        break;
      case 'M':
        serial_practice_interactive(port_to_use, PRACTICE_MIXED);
        break;
      case 'Q':
        serial_practice_interactive(port_to_use, PRACTICE_QSO_WORDS);
        break;
      } // switch(incoming_char)
    }
    else
    {
      switch (incoming_char)
      {
      case 'X':
        menu_loop = 0;
        break;
      case 'I':
        serial_practice_non_interactive(port_to_use, CALLSIGN_INTERNATIONAL);
        break;
      case 'U':
        serial_practice_non_interactive(port_to_use, CALLSIGN_US);
        break;
      case 'E':
        serial_practice_non_interactive(port_to_use, CALLSIGN_EUROPEAN);
        break;
      case 'C':
        serial_practice_non_interactive(port_to_use, CALLSIGN_CANADA);
        break;
      case '2':
        serial_practice_non_interactive(port_to_use, PRACTICE_2_CHAR_WORDS);
        break;
      case '3':
        serial_practice_non_interactive(port_to_use, PRACTICE_3_CHAR_WORDS);
        break;
      case '4':
        serial_practice_non_interactive(port_to_use, PRACTICE_4_CHAR_WORDS);
        break;
      case 'N':
        serial_practice_non_interactive(port_to_use, PRACTICE_NAMES);
        break;
      case 'M':
        serial_practice_non_interactive(port_to_use, PRACTICE_MIXED);
        break;
      case 'Q':
        serial_practice_non_interactive(port_to_use, PRACTICE_QSO_WORDS);
        break;
      } // switch(incoming_char)
    }
  } // while(menu_loop)

  port_to_use->println(F("Exiting callsign training..."));
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_set_wordspace_parameters(PRIMARY_SERIAL_CLS *port_to_use, byte mode_select)
{

  byte menu_loop = 1;
  byte menu_loop2 = 1;
  char incoming_char = ' ';
  unsigned int temp_value;

  while (menu_loop)
  {

    while (port_to_use->available() > 0)
    { // clear out the buffer if anything is there
      port_to_use->read();
    }

    switch (mode_select)
    {
    case WORDSWORTH_WORDSPACE:
      port_to_use->print(F("\r\nEnter Wordspace >"));
      break;
    case WORDSWORTH_WPM:
      port_to_use->print(F("\r\nEnter WPM >"));
      break;
    case WORDSWORTH_REPETITION:
      port_to_use->print(F("\r\nEnter Repetition >"));
      break;
    }

    menu_loop2 = 1;
    temp_value = 0;

    while (menu_loop2)
    {

      if (port_to_use->available())
      {
        incoming_char = port_to_use->read();
        if ((incoming_char > 47) && (incoming_char < 58))
        {
          port_to_use->print(incoming_char);
          temp_value = (temp_value * 10) + (incoming_char - 48);
        }
        if (incoming_char == 13)
        { // Enter Key
          menu_loop2 = 0;
        }
      }
    }

    // validate value
    if (temp_value == 0)
    {
      menu_loop = 0; // just blow out if nothing was entered
    }
    else
    {
      if ((temp_value > 0) && (temp_value < 101) && (mode_select == WORDSWORTH_WPM))
      {
        configuration.wpm = temp_value;
        config_dirty = 1;
        menu_loop = 0;
      }
      else
      {
        if ((temp_value > 1) && (temp_value < 13) && (mode_select == WORDSWORTH_WORDSPACE))
        {
          configuration.wordsworth_wordspace = temp_value;
          config_dirty = 1;
          menu_loop = 0;
        }
        else
        {
          if ((temp_value > 0) && (temp_value < 11) && (mode_select == WORDSWORTH_REPETITION))
          {
            configuration.wordsworth_repetition = temp_value;
            config_dirty = 1;
            menu_loop = 0;
          }
          else
          {
            port_to_use->println(F("\r\nOMG that's an invalid value. Try again, OM..."));
          }
        }
      }
    }

  } // while(menu_loop)
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_random_menu(PRIMARY_SERIAL_CLS *port_to_use)
{

  byte menu_loop = 1;
  byte menu_loop2 = 1;
  char incoming_char = ' ';

  while (menu_loop)
  {

    while (port_to_use->available() > 0)
    { // clear out the buffer if anything is there
      port_to_use->read();
    }

    port_to_use->println(F("\r\n\nRandom Code Receive Practice Menu\n"));
    port_to_use->println(F("A - Letter Groups"));
    port_to_use->println(F("1 - Number Groups"));
    port_to_use->println(F("M - Mixed Groups"));
    port_to_use->println(F("\nX - Exit\n"));

    menu_loop2 = 1;

    while (menu_loop2)
    {

      if (port_to_use->available())
      {
        incoming_char = port_to_use->read();
        if ((incoming_char != 10) && (incoming_char != 13))
        {
          menu_loop2 = 0;
        }
      }
    }

    incoming_char = toUpperCase(incoming_char);

    switch (incoming_char)
    {
    case 'A':
      random_practice(port_to_use, RANDOM_LETTER_GROUPS, 5);
      break;
    case '1':
      random_practice(port_to_use, RANDOM_NUMBER_GROUPS, 5);
      break;
    case 'M':
      random_practice(port_to_use, RANDOM_MIXED_GROUPS, 5);
      break;
    case 'X':
      menu_loop = 0;
      break;
    } // switch(incoming_char)

  } // while(menu_loop)

  port_to_use->println(F("Exiting Random code module..."));
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void random_practice(PRIMARY_SERIAL_CLS *port_to_use, byte random_mode, byte group_size)
{

  byte loop1 = 1;
  byte x = 0;
  byte y = 0;
  char incoming_char = ' ';
  char random_character = 0;

  randomSeed(millis());
  port_to_use->println(F("Random group practice\r\n"));

#ifdef FEATURE_DISPLAY
  lcd_clear();
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("RndGroup", 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Random Group", 0, default_display_msg_delay);
  }
  if (LCD_ROWS > 1)
  {
    lcd_center_print_timed("Practice", 1, default_display_msg_delay);
  }
  service_display();
#endif

  while (port_to_use->available() > 0)
  { // clear out the buffer if anything is there
    incoming_char = port_to_use->read();
  }

  while (loop1)
  {

    switch (random_mode)
    {
    case RANDOM_LETTER_GROUPS:
      random_character = random(65, 91);
      break;
    case RANDOM_NUMBER_GROUPS:
      random_character = random(48, 58);
      break;
    case RANDOM_MIXED_GROUPS:
      random_character = random(65, 101);
      if (random_character > 90)
      {
        random_character = random_character - 43;
      };
      break;
    }

    send_char(random_character, KEYER_NORMAL);
    port_to_use->print(random_character);
#ifdef FEATURE_DISPLAY
    display_scroll_print_char(random_character);
    service_display();
#endif

    x++;

    if (x == group_size)
    {
      send_char(' ', KEYER_NORMAL);
      port_to_use->print(" ");
#ifdef FEATURE_DISPLAY
      display_scroll_print_char(' ');
      service_display();
#endif
      x = 0;
      y++;
    }

    if (y > 4)
    {
      port_to_use->println("");
      y = 0;
    }

    if (port_to_use->available())
    {
      port_to_use->read();
      loop1 = 0;
    }

#ifdef FEATURE_BUTTONS
    while ((paddle_pin_read(paddle_left) == LOW) || (paddle_pin_read(paddle_right) == LOW) || (analogbuttonread(0)))
    {
      loop1 = 0;
    }
#else
    while ((paddle_pin_read(paddle_left) == LOW) || (paddle_pin_read(paddle_right) == LOW))
    {
      loop1 = 0;
    }
#endif // FEATURE_BUTTONS

  } // loop1
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_wordsworth_menu(PRIMARY_SERIAL_CLS *port_to_use)
{

  byte menu_loop = 1;
  byte menu_loop2 = 1;
  char incoming_char = ' ';
  byte effective_wpm_factor[] = {100, 93, 86, 81, 76, 71, 68, 64, 61, 58, 56, 53};

  while (menu_loop)
  {

    while (port_to_use->available() > 0)
    { // clear out the buffer if anything is there
      port_to_use->read();
    }

    port_to_use->println(F("\r\n\nWordsworth Menu\n"));
    port_to_use->println(F("2 - Two Letter Words"));
    port_to_use->println(F("3 - Three Letter Words"));
    port_to_use->println(F("4 - Four Letter Words"));
    port_to_use->println(F("N - Names"));
    port_to_use->println(F("Q - QSO Words"));
    port_to_use->println(F("M - Mixed\n"));
    port_to_use->println(F("O - Set Wordspace"));
    port_to_use->println(F("W - Set WPM"));
    port_to_use->println(F("R - Set Repetition"));
    port_to_use->println(F("\nX - Exit\n"));
    port_to_use->print(F("WPM:"));
    port_to_use->print(configuration.wpm);
    port_to_use->print(F(" Wordspace:"));
    port_to_use->print(configuration.wordsworth_wordspace);
    port_to_use->print(F(" Effective WPM:"));
    port_to_use->print(configuration.wpm * (effective_wpm_factor[configuration.wordsworth_wordspace - 1] / 100.0), 0);
    port_to_use->print(F(" Repetition:"));
    port_to_use->println(configuration.wordsworth_repetition);
    port_to_use->println("\r\n\nEnter choice >");

    menu_loop2 = 1;

    while (menu_loop2)
    {

      if (port_to_use->available())
      {
        incoming_char = port_to_use->read();
        if ((incoming_char != 10) && (incoming_char != 13))
        {
          menu_loop2 = 0;
        }
      }
    }

    incoming_char = toUpperCase(incoming_char);

    switch (incoming_char)
    {
    case '2':
      wordsworth_practice(port_to_use, WORDSWORTH_2_CHAR_WORDS);
      break;
    case '3':
      wordsworth_practice(port_to_use, WORDSWORTH_3_CHAR_WORDS);
      break;
    case '4':
      wordsworth_practice(port_to_use, WORDSWORTH_4_CHAR_WORDS);
      break;
    case 'N':
      wordsworth_practice(port_to_use, WORDSWORTH_NAMES);
      break;
    case 'M':
      wordsworth_practice(port_to_use, WORDSWORTH_MIXED);
      break;
    case 'Q':
      wordsworth_practice(port_to_use, WORDSWORTH_QSO_WORDS);
      break;
    case 'W':
      serial_set_wordspace_parameters(port_to_use, WORDSWORTH_WPM);
      break;
    case 'O':
      serial_set_wordspace_parameters(port_to_use, WORDSWORTH_WORDSPACE);
      break;
    case 'R':
      serial_set_wordspace_parameters(port_to_use, WORDSWORTH_REPETITION);
      break;
    case 'X':
      menu_loop = 0;
      break;
    } // switch(incoming_char)

  } // while(menu_loop)

  port_to_use->println(F("Exiting Wordsworth module..."));
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void wordsworth_practice(PRIMARY_SERIAL_CLS *port_to_use, byte practice_type)
{

  byte loop1 = 1;
  byte loop2;
  byte loop3;
  unsigned int word_index;
  char word_buffer[10];
  byte x;
  byte not_printed;
  byte practice_type_called = practice_type;
  byte repetitions;

  randomSeed(millis());
  port_to_use->println(F("Wordsworth practice...\n"));

#ifdef FEATURE_DISPLAY
  lcd_clear();
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("Wrdswrth", 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Wordsworth", 0, default_display_msg_delay);
  }
  if (LCD_ROWS > 1)
  {
    lcd_center_print_timed("Practice", 1, default_display_msg_delay);
  }
  service_display();
#endif

  while (port_to_use->available() > 0)
  { // clear out the buffer if anything is there
    port_to_use->read();
  }

  while (loop1)
  {

    if (practice_type_called == WORDSWORTH_MIXED)
    {
      practice_type = random(WORDSWORTH_2_CHAR_WORDS, WORDSWORTH_QSO_WORDS + 1);
    }
    else
    {
      practice_type = practice_type_called;
    }

    switch (practice_type)
    {
    case WORDSWORTH_2_CHAR_WORDS:
      word_index = random(0, s2_size); // min parm is inclusive, max parm is exclusive
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s2_table[word_index])));
      break;
    case WORDSWORTH_3_CHAR_WORDS:
      word_index = random(0, s3_size); // min parm is inclusive, max parm is exclusive
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s3_table[word_index])));
      break;
    case WORDSWORTH_4_CHAR_WORDS:
      word_index = random(0, s4_size); // min parm is inclusive, max parm is exclusive
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s4_table[word_index])));
      break;
    case WORDSWORTH_NAMES:
      word_index = random(0, name_size); // min parm is inclusive, max parm is exclusive
      strcpy_P(word_buffer, (char *)pgm_read_word(&(name_table[word_index])));
      break;
    case WORDSWORTH_QSO_WORDS:
      word_index = random(0, qso_size); // min parm is inclusive, max parm is exclusive
      strcpy_P(word_buffer, (char *)pgm_read_word(&(qso_table[word_index])));
      break;
    }

#if defined(DEBUG_WORDSWORTH)
    debug_serial_port->print("wordsworth_practice: word_index:");
    debug_serial_port->println(word_index);
    debug_serial_port->print("wordsworth_practice: word_buffer:");
    debug_serial_port->println(word_buffer);
#endif

    loop3 = 1;
    repetitions = 0;

    while ((loop3) && (repetitions < configuration.wordsworth_repetition))
    { // word sending loop

      loop2 = 1;
      x = 0;

      while (loop2)
      { // character sending loop

#if defined(DEBUG_WORDSWORTH)
        debug_serial_port->print("wordsworth_practice: send_char:");
        debug_serial_port->print(word_buffer[x]);
        debug_serial_port->print(" ");
        debug_serial_port->println((byte)word_buffer[x]);
#endif

        // word_buffer[x] = toUpperCase(word_buffer[x]); // word files should be in CAPS

#if defined(OPTION_NON_ENGLISH_EXTENSIONS)
        if (((byte)word_buffer[x] == 195) || ((byte)word_buffer[x] == 197))
        { // do we have a unicode character?
          x++;
          if ((word_buffer[x] != 0) && (x < 10))
          {
            send_char(convert_unicode_to_send_char_code((byte)word_buffer[x - 1], (byte)word_buffer[x]), KEYER_NORMAL);
#ifdef FEATURE_DISPLAY
            display_scroll_print_char(convert_unicode_to_send_char_code((byte)word_buffer[x - 1], (byte)word_buffer[x]));
            service_display();
#endif
            x++;
          }
        }
        else
        {
          send_char(word_buffer[x], KEYER_NORMAL);
#ifdef FEATURE_DISPLAY
          display_scroll_print_char(word_buffer[x]);
          service_display();
#endif
          x++;
        }
#else // OPTION_NON_ENGLISH_EXTENSIONS
        send_char(word_buffer[x], KEYER_NORMAL);
#ifdef FEATURE_DISPLAY
        display_scroll_print_char(word_buffer[x]);
        service_display();
#endif
        x++;
#endif // OPTION_NON_ENGLISH_EXTENSIONS

        not_printed = 1;

        if ((word_buffer[x] == 0) || (x > 9))
        { // are we at the end of the word?
          loop2 = 0;
          for (int y = 0; y < (configuration.wordsworth_wordspace - 1); y++)
          { // send extra word spaces
            send_char(' ', KEYER_NORMAL);
#ifdef FEATURE_DISPLAY
            display_scroll_print_char(' ');
            service_display();
#endif
            if (((y > ((configuration.wordsworth_wordspace - 1) / 2)) || (configuration.wordsworth_wordspace < 4)) && (not_printed))
            {
              port_to_use->println(word_buffer);
              not_printed = 0;
            }
            if (port_to_use->available())
            {
              port_to_use->read();
              y = 99;
              loop1 = 0;
              loop2 = 0;
              loop3 = 0;
            }
          }
        }

        if (port_to_use->available())
        {
          port_to_use->read();
          loop1 = 0;
          loop2 = 0;
          loop3 = 0;
        }
      } // loop2

      repetitions++;

    } // loop3

  } // loop1
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_practice_interactive(PRIMARY_SERIAL_CLS *port_to_use, byte practice_type_called)
{

  byte loop1 = 1;
  byte loop2 = 0;
  byte x = 0;
  byte serialwaitloop = 0;
  String cw_to_send_to_user(10);
  char incoming_char = ' ';
  String user_entered_cw = "";
  byte practice_type = 0;
  char word_buffer[10];

  randomSeed(millis());

#ifdef FEATURE_DISPLAY
  lcd_clear();
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("IntrctRX", 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Interactive RX", 0, default_display_msg_delay);
  }
  if (LCD_ROWS > 1)
  {
    lcd_center_print_timed("Practice", 1, default_display_msg_delay);
  }
  service_display();
#endif

  port_to_use->println(F("Interactive receive practice\r\n\r\nCopy the code, type it in, and hit ENTER."));
  port_to_use->println(F("If you are using the Arduino serial monitor, select \"Carriage Return\" line ending."));
  port_to_use->println(F("Enter a blackslash \\ to exit.\r\n"));
  while (port_to_use->available() > 0)
  { // clear out the buffer if anything is there
    incoming_char = port_to_use->read();
  }
  port_to_use->print(F("Press enter to start...\r\n"));
  while (port_to_use->available() == 0)
  {
  }
  while (port_to_use->available() > 0)
  { // clear out the buffer if anything is there
    incoming_char = port_to_use->read();
  }

  while (loop1)
  {

    if (practice_type_called == PRACTICE_MIXED)
    {
      practice_type = random(PRACTICE_2_CHAR_WORDS, PRACTICE_QSO_WORDS + 1);
    }
    else
    {
      practice_type = practice_type_called;
    }

    switch (practice_type)
    {
    case CALLSIGN_INTERNATIONAL:
    case CALLSIGN_US:
    case CALLSIGN_EUROPEAN:
    case CALLSIGN_CANADA:
      cw_to_send_to_user = generate_callsign(practice_type);
      break;
    case PRACTICE_2_CHAR_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s2_table[random(0, s2_size)])));
      cw_to_send_to_user = word_buffer;
#ifdef DEBUG_PRACTICE_SERIAL
      debug_serial_port->print("serial_practice_interactive: PRACTICE_2_CHAR_WORDS:");
#endif
      break;
    case PRACTICE_3_CHAR_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s3_table[random(0, s3_size)])));
      cw_to_send_to_user = word_buffer;
#ifdef DEBUG_PRACTICE_SERIAL
      debug_serial_port->print("serial_practice_interactive: PRACTICE_3_CHAR_WORDS:");
#endif
      break;
    case PRACTICE_4_CHAR_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s4_table[random(0, s4_size)])));
      cw_to_send_to_user = word_buffer;
#ifdef DEBUG_PRACTICE_SERIAL
      debug_serial_port->print("serial_practice_interactive: PRACTICE_4_CHAR_WORDS:");
#endif
      break;
    case PRACTICE_NAMES:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(name_table[random(0, name_size)])));
      cw_to_send_to_user = word_buffer;
#ifdef DEBUG_PRACTICE_SERIAL
      debug_serial_port->print("serial_practice_interactive: PRACTICE_NAMES:");
#endif
      break;
    case PRACTICE_QSO_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(qso_table[random(0, qso_size)])));
      cw_to_send_to_user = word_buffer;
#ifdef DEBUG_PRACTICE_SERIAL
      debug_serial_port->print("serial_practice_interactive: PRACTICE_QSO_WORDS:");
#endif
      break;
    } // switch(practice_type)

    loop2 = 1;

    while (loop2)
    {

#if defined(DEBUG_CALLSIGN_PRACTICE_SHOW_CALLSIGN)
      port_to_use->println(callsign);
#endif

      serialwaitloop = 1;
      user_entered_cw = "";
      x = 0;
      while (serialwaitloop)
      {

        if (x < (cw_to_send_to_user.length()))
        {
          send_char(cw_to_send_to_user[x], KEYER_NORMAL);
#ifdef FEATURE_DISPLAY
          display_scroll_print_char(cw_to_send_to_user[x]);
          service_display();
#endif
          x++;
        }

        while (port_to_use->available() > 0)
        {
          incoming_char = port_to_use->read();
          incoming_char = toUpperCase(incoming_char);
          port_to_use->print(incoming_char);
          if (incoming_char == 13)
          {
            serialwaitloop = 0;
          }
          else
          {
            if (incoming_char != 10)
            {
              user_entered_cw = user_entered_cw + incoming_char;
            }
          }
        }
      }

      if (user_entered_cw[0] != '?')
      {
        if ((user_entered_cw[0] == '\\'))
        {
          port_to_use->println(F("Exiting...\n"));
          loop1 = 0;
          loop2 = 0;
        }
        else
        {
          user_entered_cw.toUpperCase(); // the toUpperCase function was modified in 1.0; now it changes string in place
          if (cw_to_send_to_user.compareTo(user_entered_cw) == 0)
          {
            port_to_use->println(F("\nCorrect!"));
#ifdef FEATURE_DISPLAY
            lcd_clear();
            lcd_center_print_timed("Correct!", 0, default_display_msg_delay);
            service_display();
#endif
            loop2 = 0;
          }
          else
          {
            port_to_use->println(F("\nWrong!"));
#ifdef FEATURE_DISPLAY
            lcd_clear();
            lcd_center_print_timed("Wrong!", 0, default_display_msg_delay);
            service_display();
#endif
          }
        }
      }

      delay(100);
#ifdef FEATURE_BUTTONS
      while ((paddle_pin_read(paddle_left) == LOW) || (paddle_pin_read(paddle_right) == LOW) || (analogbuttonread(0)))
      {
        loop1 = 0;
        loop2 = 0;
      }
#else
      while ((paddle_pin_read(paddle_left) == LOW) || (paddle_pin_read(paddle_right) == LOW))
      {
        loop1 = 0;
        loop2 = 0;
      }
#endif // FEATURE_BUTTONS
      delay(10);
    } // loop2
  } // loop1
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_practice_non_interactive(PRIMARY_SERIAL_CLS *port_to_use, byte practice_type_called)
{

  byte loop1 = 1;
  byte loop2;
  byte x;
  String cw_to_send_to_user(10);
  char incoming_char = ' ';
  byte practice_type;
  char word_buffer[10];

  key_tx = 0;
  randomSeed(millis());

#ifdef FEATURE_DISPLAY
  lcd_clear();
  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("Call RX", 0, default_display_msg_delay);
  }
  else
  {
    lcd_center_print_timed("Callsign RX", 0, default_display_msg_delay);
  }
  if (LCD_ROWS > 1)
  {
    lcd_center_print_timed("Practice", 1, default_display_msg_delay);
  }
  service_display();
#endif

  port_to_use->println(F("Callsign receive practice\r\n"));

  while (port_to_use->available() > 0)
  { // clear out the buffer if anything is there
    incoming_char = port_to_use->read();
  }

  while (loop1)
  {

    if (practice_type_called == PRACTICE_MIXED)
    {
      practice_type = random(PRACTICE_2_CHAR_WORDS, PRACTICE_QSO_WORDS + 1);
    }
    else
    {
      practice_type = practice_type_called;
    }

    switch (practice_type)
    {
    case CALLSIGN_INTERNATIONAL:
    case CALLSIGN_US:
    case CALLSIGN_EUROPEAN:
    case CALLSIGN_CANADA:
      cw_to_send_to_user = generate_callsign(practice_type);
      break;
    case PRACTICE_2_CHAR_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s2_table[random(0, s2_size)])));
      cw_to_send_to_user = word_buffer;
#ifdef DEBUG_PRACTICE_SERIAL
      debug_serial_port->print("serial_practice_non_interactive: PRACTICE_2_CHAR_WORDS:");
#endif
      break;
    case PRACTICE_3_CHAR_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s3_table[random(0, s3_size)])));
      cw_to_send_to_user = word_buffer;
#ifdef DEBUG_PRACTICE_SERIAL
      debug_serial_port->print("serial_practice_non_interactive: PRACTICE_3_CHAR_WORDS:");
#endif
      break;
    case PRACTICE_4_CHAR_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(s4_table[random(0, s4_size)])));
      cw_to_send_to_user = word_buffer;
#ifdef DEBUG_PRACTICE_SERIAL
      debug_serial_port->print("serial_practice_non_interactive: PRACTICE_4_CHAR_WORDS:");
#endif
      break;
    case PRACTICE_NAMES:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(name_table[random(0, name_size)])));
      cw_to_send_to_user = word_buffer;
#ifdef DEBUG_PRACTICE_SERIAL
      debug_serial_port->print("serial_practice_non_interactive: PRACTICE_NAMES:");
#endif
      break;
    case PRACTICE_QSO_WORDS:
      strcpy_P(word_buffer, (char *)pgm_read_word(&(qso_table[random(0, qso_size)])));
      cw_to_send_to_user = word_buffer;
#ifdef DEBUG_PRACTICE_SERIAL
      debug_serial_port->print("serial_practice_non_interactive: PRACTICE_QSO_WORDS:");
#endif
      break;
    } // switch(practice_type)

    cw_to_send_to_user = cw_to_send_to_user + "    ";

    loop2 = 1;
    x = 0;

    while ((loop2) && (x < (cw_to_send_to_user.length())))
    {

      send_char(cw_to_send_to_user[x], KEYER_NORMAL);
#ifdef FEATURE_DISPLAY
      display_scroll_print_char(cw_to_send_to_user[x]);
      service_display();
#endif
      x++;

      if (port_to_use->available())
      {
        port_to_use->read();
        loop1 = 0;
        loop2 = 0;
        x = 99;
      }

#ifdef FEATURE_BUTTONS
      while ((paddle_pin_read(paddle_left) == LOW) || (paddle_pin_read(paddle_right) == LOW) || (analogbuttonread(0)))
      {
        loop1 = 0;
        loop2 = 0;
        x = 99;
      }
#else
      while ((paddle_pin_read(paddle_left) == LOW) || (paddle_pin_read(paddle_right) == LOW))
      {
        loop1 = 0;
        loop2 = 0;
        x = 99;
      }
#endif // FEATURE_BUTTONS

    } // loop2

    port_to_use->println(cw_to_send_to_user);

  } // loop1
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_TRAINING_COMMAND_LINE_INTERFACE) && defined(FEATURE_COMMAND_LINE_INTERFACE)

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_status(PRIMARY_SERIAL_CLS *port_to_use)
{

  port_to_use->println();
#if defined(FEATURE_AMERICAN_MORSE)
  if (char_send_mode == AMERICAN_MORSE)
  {
    port_to_use->println(F("American Morse"));
  }
#endif
#if defined(FEATURE_HELL)
  if (char_send_mode == HELL)
  {
    port_to_use->println(F("Hellschreiber"));
  }
#endif
  switch (configuration.keyer_mode)
  {
  case IAMBIC_A:
    port_to_use->print(F("Iambic A"));
    break;
  case IAMBIC_B:
    port_to_use->print(F("Iambic B"));
#ifdef FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING
    port_to_use->print(F(" / CMOS Super Keyer Timing: O"));
    if (configuration.cmos_super_keyer_iambic_b_timing_on)
    {
      port_to_use->print("n ");
      port_to_use->print(configuration.cmos_super_keyer_iambic_b_timing_percent);
      port_to_use->print("%");
    }
    else
    {
      port_to_use->print(F("ff"));
    }
#endif // FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING
    break;
  case BUG:
    port_to_use->print(F("Bug"));
    break;
  case STRAIGHT:
    port_to_use->print(F("Straight Key"));
    break;

#ifndef OPTION_NO_ULTIMATIC
  case ULTIMATIC:
    port_to_use->print(F("Ultimatic "));
    switch (ultimatic_mode)
    {
    // case ULTIMATIC_NORMAL:
    //   port_to_use->print(F("Normal"));
    //   break;
    case ULTIMATIC_DIT_PRIORITY:
      port_to_use->print(F("Dit Priority"));
      break;
    case ULTIMATIC_DAH_PRIORITY:
      port_to_use->print(F("Dah Priority"));
      break;
    }
    break;
#endif // OPTION_NO_ULTIMATIC
  case SINGLE_PADDLE:
    port_to_use->print(F("Single Paddle"));
    break;

    break;
  }
  port_to_use->println();
  port_to_use->print(F("Buffers: Dit O"));
  if (configuration.dit_buffer_off)
  {
    port_to_use->print(F("ff"));
  }
  else
  {
    port_to_use->print(F("n"));
  }
  port_to_use->print(F(" Dah O"));
  if (configuration.dah_buffer_off)
  {
    port_to_use->println(F("ff"));
  }
  else
  {
    port_to_use->println(F("n"));
  }
  if (speed_mode == SPEED_NORMAL)
  {
    port_to_use->print(F("WPM: "));
    port_to_use->println(configuration.wpm, DEC);
    port_to_use->print(F("Command Mode WPM: "));
    port_to_use->println(configuration.wpm_command_mode, DEC);
#ifdef FEATURE_FARNSWORTH
    port_to_use->print(F("Farnsworth WPM: "));
    if (configuration.wpm_farnsworth < configuration.wpm)
    {
      port_to_use->println(F("Disabled")); //(WD9DMP)
    }
    else
    {
      port_to_use->println(configuration.wpm_farnsworth, DEC);
    }
#endif // FEATURE_FARNSWORTH
  }
  else
  {
    port_to_use->print(F("QRSS Mode Activated - Dit Length: "));
    port_to_use->print(qrss_dit_length, DEC);
    port_to_use->println(F(" seconds"));
  }
  port_to_use->print(F("Sidetone: "));
  switch (configuration.sidetone_mode)
  {
  case SIDETONE_ON:
    port_to_use->print(F("On"));
    break; //(WD9DMP)
  case SIDETONE_OFF:
    port_to_use->print(F("Off"));
    break; //(WD9DMP)
  case SIDETONE_PADDLE_ONLY:
    port_to_use->print(F("Paddle Only"));
    break;
  }
  port_to_use->print(" ");
  port_to_use->print(configuration.hz_sidetone, DEC);
  port_to_use->println(" hz");

  // #if defined(FEATURE_SINEWAVE_SIDETONE)
  //   port_to_use->print(F("Sidetone Volume: "));
  //   port_to_use->print(map(configuration.sidetone_volume,sidetone_volume_low_limit,sidetone_volume_high_limit,0,100));
  //   port_to_use->println(F("%"));
  //   port_to_use->println(configuration.sidetone_volume);
  // #endif //FEATURE_SINEWAVE_SIDETONE

#ifdef FEATURE_SIDETONE_SWITCH
  port_to_use->print(F("Sidetone Switch: "));
  port_to_use->println(sidetone_switch_value() ? F("On") : F("Off")); //(WD9DMP)
#endif                                                                // FEATURE_SIDETONE_SWITCH
  port_to_use->print(F("Dah to dit: "));
  port_to_use->println((float(configuration.dah_to_dit_ratio) / 100));
  port_to_use->print(F("Weighting: "));
  port_to_use->println(configuration.weighting, DEC);
  port_to_use->print(F("Keying Compensation: "));
  port_to_use->print(configuration.keying_compensation, DEC);
  port_to_use->println(F(" mS"));
  port_to_use->print(F("Serial Number: "));
  port_to_use->println(serial_number, DEC);
#ifdef FEATURE_POTENTIOMETER
  port_to_use->print(F("Potentiometer WPM: "));
  port_to_use->print(pot_value_wpm(), DEC);
  port_to_use->print(F(" ("));
  if (configuration.pot_activated != 1)
  {
    port_to_use->print(F("Not "));
  }
  port_to_use->print(F("Activated) Range: "));
  port_to_use->print(pot_wpm_low_value);
  port_to_use->print(" - ");
  port_to_use->print(pot_wpm_high_value);
  port_to_use->println(F(" WPM"));
#endif
#ifdef FEATURE_AUTOSPACE
  port_to_use->print(F("Autospace O"));
  if (configuration.autospace_active)
  {
    port_to_use->println(F("n"));
  }
  else
  {
    port_to_use->println(F("ff"));
  }
  port_to_use->print(F("Autospace Timing Factor: "));
  port_to_use->println((float)configuration.autospace_timing_factor / (float)100);
#endif
  port_to_use->print(F("Wordspace: "));
  port_to_use->println(configuration.length_wordspace, DEC);
  port_to_use->print(F("TX: "));
  port_to_use->println(configuration.current_tx);

#ifdef FEATURE_QLF
  port_to_use->print(F("QLF: O"));
  if (qlf_active)
  {
    port_to_use->println(F("n"));
  }
  else
  {
    port_to_use->println(F("ff"));
  }
#endif // FEATURE_QLF

  port_to_use->print(F("Quiet Paddle Interrupt: "));
  if (configuration.paddle_interruption_quiet_time_element_lengths > 0)
  {
    port_to_use->println(configuration.paddle_interruption_quiet_time_element_lengths);
  }
  else
  {
    port_to_use->println(F("Off"));
  }

#if !defined(OPTION_EXCLUDE_MILL_MODE)
  port_to_use->print(F("Mill Mode: O"));
  if (configuration.cli_mode == CLI_NORMAL_MODE)
  {
    port_to_use->println(F("ff"));
  }
  else
  {
    port_to_use->println(F("n"));
  }
#endif // !defined(OPTION_EXCLUDE_MILL_MODE)

  port_to_use->print(F("PTT Buffered Character Hold: O"));
  if (configuration.ptt_buffer_hold_active)
  {
    port_to_use->println(F("n"));
  }
  else
  {
    port_to_use->println(F("ff"));
  }

  if (configuration.ptt_disabled)
  {
    port_to_use->println(F("PTT: Disabled"));
  }

  port_to_use->print(F("TX Inhibit: O"));
  if ((digitalRead(tx_inhibit_pin) == tx_inhibit_pin_active_state))
  {
    port_to_use->println(F("n"));
  }
  else
  {
    port_to_use->println(F("ff"));
  }
  port_to_use->print(F("TX Pause: O"));
  if ((digitalRead(tx_pause_pin) == tx_pause_pin_active_state))
  {
    port_to_use->println(F("n"));
  }
  else
  {
    port_to_use->println(F("ff"));
  }

#if defined(FEATURE_BEACON_SETTING)
  port_to_use->print(F("Beacon Mode At Boot Up: O"));
  if (configuration.beacon_mode_on_boot_up)
  {
    port_to_use->println(F("n"));
  }
  else
  {
    port_to_use->println(F("ff"));
  }
#endif

#if defined(FEATURE_PADDLE_ECHO)
  port_to_use->print(F("Paddle Echo: O"));
  if (cli_paddle_echo)
  {
    port_to_use->println(F("n"));
  }
  else
  {
    port_to_use->println(F("ff"));
  }
  port_to_use->print(F("Paddle Echo Timing Factor: "));
  port_to_use->println((float)configuration.cw_echo_timing_factor / (float)100);
#endif

#if defined(FEATURE_STRAIGHT_KEY_ECHO)
  port_to_use->print(F("Straight Key Echo: O"));
  if (cli_straight_key_echo)
  {
    port_to_use->println(F("n"));
  }
  else
  {
    port_to_use->println(F("ff"));
  }
#endif

  port_to_use->print(F("Tx")); // show the ptt lead time for the current tx
  port_to_use->print(configuration.current_tx);
  port_to_use->print(F(" lead time: "));
  port_to_use->println(configuration.ptt_lead_time[configuration.current_tx - 1]);
  port_to_use->print(F("Tx")); // show the ptt tail time for the current tx
  port_to_use->print(configuration.current_tx);
  port_to_use->print(F(" tail time: "));
  port_to_use->println(configuration.ptt_tail_time[configuration.current_tx - 1]);

  port_to_use->print(F("PTT hang time: ")); // show the hang time
  port_to_use->print(ptt_hang_time_wordspace_units);
  port_to_use->println(F(" wordspace units"));
  port_to_use->print(F("Memory repeat time: ")); // show the memory repeat time
  port_to_use->println(configuration.memory_repeat_time);

#ifdef FEATURE_MEMORIES
  serial_status_memories(port_to_use);
#endif

#ifdef DEBUG_MEMORYCHECK
  memorycheck();
#endif

#ifdef DEBUG_VARIABLE_DUMP
  port_to_use->println(configuration.wpm);
#ifdef FEATURE_FARNSWORTH
  port_to_use->println(configuration.wpm_farnsworth);
#endif // FEATURE_FARNSWORTH
  port_to_use->println(1.0 * (float(configuration.weighting) / 50));
  port_to_use->println(configuration.keying_compensation, DEC);
  port_to_use->println(2.0 - (float(configuration.weighting) / 50));
  port_to_use->println(-1.0 * configuration.keying_compensation);
  port_to_use->println((dit_end_time - dit_start_time), DEC);
  port_to_use->println((dah_end_time - dah_start_time), DEC);
  port_to_use->println(millis(), DEC);
#endif // DEBUG_VARIABLE_DUMP

#ifdef DEBUG_BUTTONS
  for (int x = 0; x < analog_buttons_number_of_buttons; x++)
  {
    port_to_use->print(F("analog_button_array:   "));
    port_to_use->print(x);
    port_to_use->print(F(" button_array_low_limit: "));
    port_to_use->print(button_array_low_limit[x]);
    port_to_use->print(F("  button_array_high_limit: "));
    port_to_use->println(button_array_high_limit[x]);
  }
#endif
  // aaaaaaa
#if defined(FEATURE_ETHERNET)
  port_to_use->print(F("Ethernet: "));
  port_to_use->print(configuration.ip[0]);
  port_to_use->print(F("."));
  port_to_use->print(configuration.ip[1]);
  port_to_use->print(F("."));
  port_to_use->print(configuration.ip[2]);
  port_to_use->print(F("."));
  port_to_use->println(configuration.ip[3]);
#endif

  port_to_use->println(F(">"));
}
#endif

//---------------------------------------------------------------------

#if defined(OPTION_PROSIGN_SUPPORT)
char *convert_prosign(byte prosign_code)
{
  switch (prosign_code)
  {
  case PROSIGN_AA:
    return ((char *)"AA");
    break;
  case PROSIGN_AS:
    return ((char *)"AS");
    break;
  case PROSIGN_BK:
    return ((char *)"BK");
    break;
  case PROSIGN_CL:
    return ((char *)"CL");
    break;
  case PROSIGN_CT:
    return ((char *)"CT");
    break;
  case PROSIGN_KN:
    return ((char *)"KN");
    break;
  case PROSIGN_NJ:
    return ((char *)"NJ");
    break;
  case PROSIGN_SK:
    return ((char *)"SK");
    break;
  case PROSIGN_SN:
    return ((char *)"SN");
    break;
  case PROSIGN_HH:
    return ((char *)"HH");
    break; // iz0rus
  case PROSIGN_SOS:
    send_the_dits_and_dahs("...---...");
    break;
  case PROSIGN_SO:
    return ((char *)"SO");
    break;
#if !defined(OPTION_CW_KEYBOARD_GERMAN) && !defined(OPTION_CW_KEYBOARD_ITALIAN) && !defined(OPTION_PS2_NON_ENGLISH_CHAR_LCD_DISPLAY_SUPPORT)
  case PROSIGN_OS:
    return ((char *)"OS");
    break;
#endif // !defined(OPTION_CW_KEYBOARD_GERMAN) ......
  default:
    return ((char *)"");
    break;
  }
}
#endif // OPTION_PROSIGN_SUPPORT

//---------------------------------------------------------------------

#if defined(FEATURE_MEMORIES) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_status_memories(PRIMARY_SERIAL_CLS *port_to_use)
{
  int last_memory_location;

#if defined(OPTION_PROSIGN_SUPPORT)
  byte eeprom_temp = 0;
  static char *prosign_temp = 0;
#endif

  for (int x = 0; x < number_of_memories; x++)
  {
    last_memory_location = memory_end(x) + 1;
    port_to_use->write("Memory ");
    port_to_use->print(x + 1);
    port_to_use->write(":");
    if (EEPROM.read(memory_start(x)) == 255)
    {
      port_to_use->write("{empty}");
    }
    else
    {
      for (int y = (memory_start(x)); (y < last_memory_location); y++)
      {
        if (EEPROM.read(y) < 255)
        {
#if defined(OPTION_PROSIGN_SUPPORT)
          eeprom_temp = EEPROM.read(y);
          if ((eeprom_temp > PROSIGN_START) && (eeprom_temp < PROSIGN_END))
          {
            prosign_temp = convert_prosign(eeprom_temp);
            port_to_use->print(prosign_temp[0]);
            port_to_use->print(prosign_temp[1]);
            if (strlen(prosign_temp) == 3)
              port_to_use->print(prosign_temp[2]);
          }
          else
          {
            port_to_use->write(eeprom_temp);
          }
#else
          if ((EEPROM.read(y) == 32) && ((EEPROM.read(y + 1) == 255) || ((y + 1) >= last_memory_location)))
          {
            port_to_use->write("_");
          }
          else
          {
            port_to_use->write(EEPROM.read(y));
          }
#endif // OPTION_PROSIGN_SUPPORT
        }
        else
        {
          // port_to_use->write("$");
          y = last_memory_location;
        }
      }
    }

#if defined(DEBUG_MEMORY_LOCATIONS)
    port_to_use->print(F("  start: "));
    port_to_use->print(memory_start(x));
    port_to_use->print(F(" end: "));
    port_to_use->print(memory_end(x));
    port_to_use->print(F(" size: "));
    port_to_use->print(memory_end(x) - memory_start(x));
#endif

    port_to_use->println();
  }

#if defined(DEBUG_MEMORY_LOCATIONS)
  port_to_use->print(F("Config Area end: "));
  port_to_use->println(sizeof(configuration));
#endif
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_MEMORIES) && defined(FEATURE_COMMAND_LINE_INTERFACE)
void serial_program_memory(PRIMARY_SERIAL_CLS *port_to_use)
{

  /*

  CLI memory programming test string

  WE LOVE RADIO AND SMALL COMPUTING DEVICES AND WE COMBINE THE TWO TO AUTOMATE EXPERIMENT OR JUST SEE IF SOMETHING CAN BE DONE WE BELIEVE ITS BETTER TO BUILD SOMETHING WITH OUR OWN HANDS HOWEVER SMALL OR IMPERFECT AND IMPROVE AND EXPAND IT OVER TIME WE SUPPORT EXPERIMENTERS OF ALL LEVELS AND EXCHANGE IDEAS ABOUT AMATEUR RADIO HARDWARE HOMEBREWING AND SOFTWARE DEVELOPMENT

  */

  uint8_t incoming_serial_byte = 0;
  uint8_t memory_number = 0;
  uint8_t looping = 1;
  int memory_index = 0;
  uint8_t memory_number_entered = 0;
  uint8_t memory_data_entered = 0;
  uint8_t error_flag = 0;
  uint8_t memory_1_or_1x_flag = 0;

  uint8_t incoming_serial_byte_buffer[serial_program_memory_buffer_size];
  unsigned int incoming_serial_byte_buffer_size = 0;

  while (looping)
  {
    if (keyer_machine_mode == KEYER_NORMAL)
    { // might as well do something while we're waiting
      check_paddles();
      service_dit_dah_buffers();
    }

    while ((port_to_use->available()) && (incoming_serial_byte_buffer_size < serial_program_memory_buffer_size))
    { // get serial data if available
      incoming_serial_byte_buffer[incoming_serial_byte_buffer_size] = uppercase(port_to_use->read());
      incoming_serial_byte_buffer_size++;
    }

    if (incoming_serial_byte_buffer_size)
    {
      incoming_serial_byte = incoming_serial_byte_buffer[0];
      port_to_use->write(incoming_serial_byte);
      for (unsigned int x = 0; x < (incoming_serial_byte_buffer_size - 1); x++)
      {
        incoming_serial_byte_buffer[x] = incoming_serial_byte_buffer[x + 1];
      }
      incoming_serial_byte_buffer_size--;

      if ((memory_1_or_1x_flag) && ((incoming_serial_byte < 48) || (incoming_serial_byte > 57)))
      { // do we have something other than a number?
        memory_1_or_1x_flag = 0;
        memory_number_entered = 1;
      }

      if (!memory_number_entered)
      {
        if ((incoming_serial_byte > 47) && (incoming_serial_byte < 58))
        { // do we have a number?
          if (memory_1_or_1x_flag)
          {
            memory_number = incoming_serial_byte - 48 + 10;
            memory_1_or_1x_flag = 0;
            memory_number_entered = 1;
          }
          else
          {
            memory_number = incoming_serial_byte - 48;
            if ((memory_number == 1) && (number_of_memories > 9))
            {
              memory_1_or_1x_flag = 1;
            }
            else
            {
              memory_number_entered = 1;
            }
          }
          // memory number out of range check
          if (memory_number > number_of_memories)
          {
            looping = 0;
            error_flag = 1;
          }
        }
        else
        {
          looping = 0;
          error_flag = 1;
        }
      }
      else
      {

        if (incoming_serial_byte == 13)
        { // we got a carriage return
          looping = 0;
          EEPROM.write((memory_start(memory_number - 1) + memory_index), 255);
        }
        else
        { // looking for memory data
          memory_data_entered = 1;
#if !defined(OPTION_SAVE_MEMORY_NANOKEYER)
          while ((port_to_use->available()) && (incoming_serial_byte_buffer_size < serial_program_memory_buffer_size))
          { // get serial data if available
            incoming_serial_byte_buffer[incoming_serial_byte_buffer_size] = uppercase(port_to_use->read());
            incoming_serial_byte_buffer_size++;
          }
#endif
          EEPROM.write((memory_start(memory_number - 1) + memory_index), incoming_serial_byte);
#if !defined(OPTION_SAVE_MEMORY_NANOKEYER)
          while ((port_to_use->available()) && (incoming_serial_byte_buffer_size < serial_program_memory_buffer_size))
          { // get serial data if available
            incoming_serial_byte_buffer[incoming_serial_byte_buffer_size] = uppercase(port_to_use->read());
            incoming_serial_byte_buffer_size++;
          }
#endif
#ifdef DEBUG_EEPROM
          debug_serial_port->print(F("serial_program_memory: wrote "));
          debug_serial_port->print(incoming_serial_byte);
          debug_serial_port->print(F(" to location "));
          debug_serial_port->println((memory_start(memory_number - 1) + memory_index));
#endif
          memory_index++;
          if ((memory_start(memory_number - 1) + memory_index) > (memory_end(memory_number - 1) - 2))
          { // are we at last memory location?
            looping = 0;
            EEPROM.write((memory_start(memory_number - 1) + memory_index), 255);
            port_to_use->println(F("Memory full, truncating."));
          }
        }

      } //
    }
  }

  if ((memory_number_entered) && (memory_data_entered) && (!error_flag))
  {
    port_to_use->print(F("\n\rWrote memory "));
    port_to_use->println(memory_number);
  }
  else
  {
    port_to_use->println(F("\n\rError"));
  }

#if defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
  EEPROM.commit();
#endif
}

#endif

//---------------------------------------------------------------------

#ifdef FEATURE_MEMORIES
byte memory_nonblocking_delay(unsigned long delaytime)
{
  // 2012-04-20 was long starttime = millis();
  unsigned long starttime = millis();

  while ((millis() - starttime) < delaytime)
  {
    check_paddles();
#ifdef FEATURE_BUTTONS
    if (((dit_buffer) || (dah_buffer) || (analogbuttonread(0))) && (keyer_machine_mode != BEACON))
    { // exit if the paddle or button0 was hit
#else
    if (((dit_buffer) || (dah_buffer)) && (keyer_machine_mode != BEACON))
    { // exit if the paddle or button0 was hit
#endif
      dit_buffer = 0;
      dah_buffer = 0;
#ifdef FEATURE_BUTTONS
      while (analogbuttonread(0))
      {
      }
#endif
      return 1;
    }
  }
  return 0;
}

#endif

//---------------------------------------------------------------------

/*

program HelloWorld;

{ HelloWorld version 2019.12.27.02 by K3NG }

VAR
 Callsign:string;

BEGIN
 Write('What is your callsign: ');
 Readln(Callsign);
 WriteLn('Hello ', Callsign);
END.

*/
//---------------------------------------------------------------------

#if defined(FEATURE_MEMORIES) || defined(FEATURE_COMMAND_LINE_INTERFACE)
void display_serial_number_character(char snumchar)
{
#if defined(FEATURE_SERIAL)
#ifdef FEATURE_COMMAND_LINE_INTERFACE
  primary_serial_port->write(snumchar);
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
  secondary_serial_port->write(snumchar);
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
#endif // FEATURE_COMMAND_LINE_INTERFACE
#endif // (FEATURE_SERIAL)
#ifdef FEATURE_DISPLAY
  if (lcd_send_echo)
  {
    display_scroll_print_char(snumchar);
    service_display();
  }
#endif // FEATURE_DISPLAY
}
#endif

//---------------------------------------------------------------------

#if defined(FEATURE_MEMORIES) || defined(FEATURE_COMMAND_LINE_INTERFACE)
void send_serial_number(byte cut_numbers, int increment_serial_number, byte buffered_sending)
{

  String serial_number_string;

  serial_number_string = String(serial_number, DEC);
  if (serial_number_string.length() < 3)
  {
    if (cut_numbers)
    {
      if (buffered_sending)
      {
        add_to_send_buffer('T');
      }
      else
      {
        if (keyer_machine_mode != KEYER_COMMAND_MODE)
        {
          display_serial_number_character('T');
        } // Display the SN as well as play it unless playing back after programming for verification(WD9DMP)
        send_char('T', KEYER_NORMAL);
      }
    }
    else
    {
      if (buffered_sending)
      {
        add_to_send_buffer('0');
      }
      else
      {
        if (keyer_machine_mode != KEYER_COMMAND_MODE)
        {
          display_serial_number_character('0');
        } // Display the SN as well as play it unless playing back after programming for verification(WD9DMP)
        send_char('0', KEYER_NORMAL);
      }
    }
  }
  if (serial_number_string.length() == 1)
  {
    if (cut_numbers)
    {
      if (buffered_sending)
      {
        add_to_send_buffer('T');
      }
      else
      {
        if (keyer_machine_mode != KEYER_COMMAND_MODE)
        {
          display_serial_number_character('T');
        } // Display the SN as well as play it unless playing back after programming for verification(WD9DMP)
        send_char('T', KEYER_NORMAL);
      }
    }
    else
    {
      if (buffered_sending)
      {
        add_to_send_buffer('0');
      }
      else
      {
        if (keyer_machine_mode != KEYER_COMMAND_MODE)
        {
          display_serial_number_character('0');
        } // Display the SN as well as play it unless playing back after programming for verification(WD9DMP)
        send_char('0', KEYER_NORMAL);
      }
    }
  }
  for (unsigned int a = 0; a < serial_number_string.length(); a++)
  {
    if ((serial_number_string[a] == '0') && (cut_numbers))
    {
      if (buffered_sending)
      {
        add_to_send_buffer('T');
      }
      else
      {
        if (keyer_machine_mode != KEYER_COMMAND_MODE)
        {
          display_serial_number_character('T');
        } // Display the SN as well as play it unless playing back after programming for verification(WD9DMP)
        send_char('T', KEYER_NORMAL);
      }
    }
    else
    {
      if ((serial_number_string[a] == '9') && (cut_numbers))
      {
        if (buffered_sending)
        {
          add_to_send_buffer('N');
        }
        else
        {
          if (keyer_machine_mode != KEYER_COMMAND_MODE)
          {
            display_serial_number_character('N');
          } // Display the SN as well as play it unless playing back after programming for verification(WD9DMP)
          send_char('N', KEYER_NORMAL);
        }
      }
      else
      {
        if (buffered_sending)
        {
          add_to_send_buffer(serial_number_string[a]);
        }
        else
        {
          if (keyer_machine_mode != KEYER_COMMAND_MODE)
          {
            display_serial_number_character(serial_number_string[a]);
          } // Display the SN as well as play it unless playing back after programming for verification(WD9DMP)
          send_char(serial_number_string[a], KEYER_NORMAL);
        }
      }
    }
  }

  serial_number = serial_number + increment_serial_number;
}

#endif
// New function below to send serial number character to CLI as well as LCD (WD9DMP)
//---------------------------------------------------------------------

void initialize_pins()
{

  pinMode(paddle_left, INPUT_PULLUP);
  pinMode(paddle_right, INPUT_PULLUP);

#if defined(FEATURE_CAPACITIVE_PADDLE_PINS)
  if (capactive_paddle_pin_inhibit_pin)
  {
    pinMode(capactive_paddle_pin_inhibit_pin, INPUT);
    digitalWrite(capactive_paddle_pin_inhibit_pin, LOW);
  }
#endif // FEATURE_CAPACITIVE_PADDLE_PINS

  if (tx_key_line_1)
  {
    pinMode(tx_key_line_1, OUTPUT);
    digitalWrite(tx_key_line_1, tx_key_line_inactive_state);
  }
  if (tx_key_line_2)
  {
    pinMode(tx_key_line_2, OUTPUT);
    digitalWrite(tx_key_line_2, tx_key_line_inactive_state);
  }
  if (tx_key_line_3)
  {
    pinMode(tx_key_line_3, OUTPUT);
    digitalWrite(tx_key_line_3, tx_key_line_inactive_state);
  }
  if (tx_key_line_4)
  {
    pinMode(tx_key_line_4, OUTPUT);
    digitalWrite(tx_key_line_4, tx_key_line_inactive_state);
  }
  if (tx_key_line_5)
  {
    pinMode(tx_key_line_5, OUTPUT);
    digitalWrite(tx_key_line_5, tx_key_line_inactive_state);
  }
  if (tx_key_line_6)
  {
    pinMode(tx_key_line_6, OUTPUT);
    digitalWrite(tx_key_line_6, tx_key_line_inactive_state);
  }

  if (ptt_tx_1)
  {
    pinMode(ptt_tx_1, OUTPUT);
    digitalWrite(ptt_tx_1, ptt_line_inactive_state);
  }
  if (ptt_tx_2)
  {
    pinMode(ptt_tx_2, OUTPUT);
    digitalWrite(ptt_tx_2, ptt_line_inactive_state);
  }
  if (ptt_tx_3)
  {
    pinMode(ptt_tx_3, OUTPUT);
    digitalWrite(ptt_tx_3, ptt_line_inactive_state);
  }
  if (ptt_tx_4)
  {
    pinMode(ptt_tx_4, OUTPUT);
    digitalWrite(ptt_tx_4, ptt_line_inactive_state);
  }
  if (ptt_tx_5)
  {
    pinMode(ptt_tx_5, OUTPUT);
    digitalWrite(ptt_tx_5, ptt_line_inactive_state);
  }
  if (ptt_tx_6)
  {
    pinMode(ptt_tx_6, OUTPUT);
    digitalWrite(ptt_tx_6, ptt_line_inactive_state);
  }
  if (sidetone_line)
  {
    pinMode(sidetone_line, OUTPUT);
    digitalWrite(sidetone_line, sidetone_line_inactive_state);
  }
  if (tx_key_dit)
  {
    pinMode(tx_key_dit, OUTPUT);
    digitalWrite(tx_key_dit, tx_key_dit_and_dah_pins_inactive_state);
  }
  if (tx_key_dah)
  {
    pinMode(tx_key_dah, OUTPUT);
    digitalWrite(tx_key_dah, tx_key_dit_and_dah_pins_inactive_state);
  }

#ifdef FEATURE_CW_DECODER
  pinMode(cw_decoder_pin, INPUT_PULLUP);
  // pinMode (cw_decoder_pin, INPUT);
  // digitalWrite (cw_decoder_pin, HIGH);

#if defined(OPTION_CW_DECODER_GOERTZEL_AUDIO_DETECTOR)
  digitalWrite(cw_decoder_audio_input_pin, LOW);
  cwtonedetector.init(cw_decoder_audio_input_pin);
#endif // OPTION_CW_DECODER_GOERTZEL_AUDIO_DETECTOR

  if (cw_decoder_indicator)
  {
    pinMode(cw_decoder_indicator, OUTPUT);
    digitalWrite(cw_decoder_indicator, LOW);
  }
#endif // FEATURE_CW_DECODER

#if defined(FEATURE_COMMAND_MODE) && defined(command_mode_active_led)
  if (command_mode_active_led)
  {
    pinMode(command_mode_active_led, OUTPUT);
    digitalWrite(command_mode_active_led, LOW);
  }
#endif // FEATURE_COMMAND_MODE && command_mode_active_led

#ifdef FEATURE_LED_RING
  pinMode(led_ring_sdi, OUTPUT);
  pinMode(led_ring_clk, OUTPUT);
  pinMode(led_ring_le, OUTPUT);
#endif // FEATURE_LED_RING

#ifdef FEATURE_ALPHABET_SEND_PRACTICE
  if (correct_answer_led)
  {
    pinMode(correct_answer_led, OUTPUT);
    digitalWrite(correct_answer_led, LOW);
  }
  if (wrong_answer_led)
  {
    pinMode(wrong_answer_led, OUTPUT);
    digitalWrite(wrong_answer_led, LOW);
  }
#endif // FEATURE_ALPHABET_SEND_PRACTICE

#ifdef FEATURE_PTT_INTERLOCK
  pinMode(ptt_interlock, INPUT_PULLUP);
  // pinMode(ptt_interlock,INPUT);
  // if (ptt_interlock_active_state == HIGH){
  //   digitalWrite(ptt_interlock,LOW);
  // } else {
  //   digitalWrite(ptt_interlock,HIGH);
  // }
#endif // FEATURE_PTT_INTERLOCK

#ifdef FEATURE_STRAIGHT_KEY
  pinMode(pin_straight_key, INPUT);
  if (STRAIGHT_KEY_ACTIVE_STATE == HIGH)
  {
    digitalWrite(pin_straight_key, LOW);
  }
  else
  {
    digitalWrite(pin_straight_key, HIGH);
  }
#endif // FEATURE_STRAIGHT_KEY

#if defined(FEATURE_COMPETITION_COMPRESSION_DETECTION)
  pinMode(compression_detection_pin, OUTPUT);
  digitalWrite(compression_detection_pin, LOW);
#endif // FEATURE_COMPETITION_COMPRESSION_DETECTION

#if defined(FEATURE_SLEEP) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
  if (keyer_awake)
  {
    pinMode(keyer_awake, OUTPUT);
    digitalWrite(keyer_awake, KEYER_AWAKE_PIN_AWAKE_STATE);
  }
#endif // FEATURE_SLEEP

#if defined(FEATURE_LCD_BACKLIGHT_AUTO_DIM)
  if (keyer_power_led)
  {
    pinMode(keyer_power_led, OUTPUT);
    analogWrite(keyer_power_led, keyer_power_led_awake_duty);
  }
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM

#ifdef FEATURE_SIDETONE_SWITCH
  pinMode(SIDETONE_SWITCH, INPUT_PULLUP);
#endif // FEATURE_SIDETONE_SWITCH

#if defined(FEATURE_4x4_KEYPAD) || defined(FEATURE_3x4_KEYPAD)
  pinMode(Row3, INPUT_PULLUP);
  pinMode(Row2, INPUT_PULLUP);
  pinMode(Row1, INPUT_PULLUP);
  pinMode(Row0, INPUT_PULLUP);
  pinMode(Col2, INPUT_PULLUP);
  pinMode(Col1, INPUT_PULLUP);
  pinMode(Col0, INPUT_PULLUP);
#endif

#if defined(FEATURE_4x4_KEYPAD)
  pinMode(Col3, INPUT_PULLUP); // Col3 not used if 3x4 keypad is defined.
#endif

#ifdef FEATURE_SEQUENCER
  if (sequencer_1_pin)
  {
    pinMode(sequencer_1_pin, OUTPUT);
    digitalWrite(sequencer_1_pin, sequencer_pins_inactive_state);
  }
  if (sequencer_2_pin)
  {
    pinMode(sequencer_2_pin, OUTPUT);
    digitalWrite(sequencer_2_pin, sequencer_pins_inactive_state);
  }
  if (sequencer_3_pin)
  {
    pinMode(sequencer_3_pin, OUTPUT);
    digitalWrite(sequencer_3_pin, sequencer_pins_inactive_state);
  }
  if (sequencer_4_pin)
  {
    pinMode(sequencer_4_pin, OUTPUT);
    digitalWrite(sequencer_4_pin, sequencer_pins_inactive_state);
  }
  if (sequencer_5_pin)
  {
    pinMode(sequencer_5_pin, OUTPUT);
    digitalWrite(sequencer_5_pin, sequencer_pins_inactive_state);
  }
#endif // FEATURE_SEQUENCER

#ifdef FEATURE_SO2R_BASE
  if (so2r_tx_1)
  {
    pinMode(so2r_tx_1, OUTPUT);
  }

  if (so2r_tx_2)
  {
    pinMode(so2r_tx_2, OUTPUT);
  }

  if (so2r_rx_1)
  {
    pinMode(so2r_rx_1, OUTPUT);
  }

  if (so2r_rx_1s)
  {
    pinMode(so2r_rx_1s, OUTPUT);
  }

  if (so2r_rx_2)
  {
    pinMode(so2r_rx_2, OUTPUT);
  }

  if (so2r_rx_2s)
  {
    pinMode(so2r_rx_2s, OUTPUT);
  }

  if (so2r_rx_s)
  {
    pinMode(so2r_rx_s, OUTPUT);
  }

  so2r_set_tx();
  so2r_set_rx();

#ifdef FEATURE_SO2R_SWITCHES
  if (so2r_tx_switch)
  {
    pinMode(so2r_tx_switch, INPUT_PULLUP);
  }

  if (so2r_rx1_switch)
  {
    pinMode(so2r_rx1_switch, INPUT_PULLUP);
  }

  if (so2r_rx2_switch)
  {
    pinMode(so2r_rx2_switch, INPUT_PULLUP);
  }
#endif // FEATURE_SO2R_SWITCHES
#endif // FEATURE_SO2R_BASE

  if (ptt_input_pin)
  {
    pinMode(ptt_input_pin, INPUT_PULLUP);
  }

  if (tx_inhibit_pin)
  {
    pinMode(tx_inhibit_pin, INPUT_PULLUP);
  }

  if (tx_pause_pin)
  {
    pinMode(tx_pause_pin, INPUT_PULLUP);
  }

  if (potentiometer_enable_pin)
  {
    pinMode(potentiometer_enable_pin, INPUT);
  }

  if (pin_sending_mode_automatic)
  {
    pinMode(pin_sending_mode_automatic, OUTPUT);
    digitalWrite(pin_sending_mode_automatic, LOW);
  }

  if (pin_sending_mode_manual)
  {
    pinMode(pin_sending_mode_manual, OUTPUT);
    digitalWrite(pin_sending_mode_manual, LOW);
  }

} // initialize_pins()

//---------------------------------------------------------------------

void initialize_debug_startup()
{
#ifdef DEBUG_STARTUP

  serial_status(debug_serial_port);
#if defined(FEATURE_SERIAL)
  debug_serial_port->println(F("FEATURE_SERIAL"));
#endif
#ifdef FEATURE_COMMAND_LINE_INTERFACE
  debug_serial_port->println(F("FEATURE_COMMAND_LINE_INTERFACE"));
#endif
#ifndef OPTION_DO_NOT_SAY_HI
  debug_serial_port->println(F("OPTION_DO_NOT_SAY_HI"));
#endif
#ifdef FEATURE_MEMORIES
  debug_serial_port->println(F("FEATURE_MEMORIES"));
#endif
#ifdef FEATURE_MEMORY_MACROS
  debug_serial_port->println(F("FEATURE_MEMORY_MACROS"));
#endif
#ifdef FEATURE_WINKEY_EMULATION
  debug_serial_port->println(F("FEATURE_WINKEY_EMULATION"));
#endif
#ifdef OPTION_WINKEY_2_SUPPORT
  debug_serial_port->println(F("OPTION_WINKEY_2_SUPPORT"));
#endif
#ifdef FEATURE_BEACON
  debug_serial_port->println(F("FEATURE_BEACON"));
#endif
#ifdef FEATURE_TRAINING_COMMAND_LINE_INTERFACE
  debug_serial_port->println(F("FEATURE_TRAINING_COMMAND_LINE_INTERFACE"));
#endif
#ifdef FEATURE_POTENTIOMETER
  debug_serial_port->println(F("FEATURE_POTENTIOMETER"));
#endif
#if defined(FEATURE_SERIAL_HELP)
  debug_serial_port->println(F("FEATURE_SERIAL_HELP"));
#endif
#ifdef FEATURE_HELL
  debug_serial_port->println(F("FEATURE_HELL"));
#endif
#ifdef FEATURE_AMERICAN_MORSE
  debug_serial_port->println(F("FEATURE_AMERICAN_MORSE"));
#endif
#ifdef FEATURE_PS2_KEYBOARD
  debug_serial_port->println(F("FEATURE_PS2_KEYBOARD"));
#endif
#ifdef FEATURE_DEAD_OP_WATCHDOG
  debug_serial_port->println(F("FEATURE_DEAD_OP_WATCHDOG"));
#endif
#ifdef FEATURE_AUTOSPACE
  debug_serial_port->println(F("FEATURE_AUTOSPACE"));
#endif
#ifdef FEATURE_FARNSWORTH
  debug_serial_port->println(F("FEATURE_FARNSWORTH"));
#endif
#ifdef FEATURE_DL2SBA_BANKSWITCH
  debug_serial_port->println(F("FEATURE_DL2SBA_BANKSWITCH"));
#endif
#ifdef FEATURE_BUTTONS
  debug_serial_port->println(F("FEATURE_BUTTONS"));
#endif
#ifdef FEATURE_COMMAND_MODE
  debug_serial_port->println(F("FEATURE_COMMAND_MODE"));
#endif
#ifdef FEATURE_LCD_4BIT
  debug_serial_port->println(F("FEATURE_LCD_4BIT"));
#endif
#ifdef FEATURE_LCD_8BIT
  debug_serial_port->println(F("FEATURE_LCD_8BIT"));
#endif
  debug_serial_port->println(F("setup: exiting, going into loop"));
#endif // DEBUG_STARTUP
}

//---------------------------------------------------------------------

#ifdef FEATURE_CW_DECODER
void service_cw_decoder()
{

  static unsigned long last_transition_time = 0;
  static unsigned long last_decode_time = 0;
  static byte last_state = HIGH;
  static int decode_elements[16]; // this stores received element lengths in mS (positive = tone, minus = no tone)
  static byte decode_element_pointer = 0;
  static float decode_element_tone_average = 0;
  static float decode_element_no_tone_average = 0;
  static int no_tone_count = 0;
  static int tone_count = 0;
  byte decode_it_flag = 0;
  byte cd_decoder_pin_state = HIGH;

  int element_duration = 0;
  static float decoder_wpm = configuration.wpm;
  long decode_character = 0;
  static byte space_sent = 0;
#ifdef FEATURE_COMMAND_LINE_INTERFACE
  static byte screen_column = 0;
  static int last_printed_decoder_wpm = 0;
#endif

  cd_decoder_pin_state = digitalRead(cw_decoder_pin);

#if defined(OPTION_CW_DECODER_GOERTZEL_AUDIO_DETECTOR)
  if (cwtonedetector.detecttone() == HIGH)
  { // invert states
    cd_decoder_pin_state = LOW;
  }
  else
  {
    cd_decoder_pin_state = HIGH;
  }
#endif

#if defined(DEBUG_CW_DECODER_WITH_TONE)
  if (cd_decoder_pin_state == LOW)
  {
#if defined(GOERTZ_TARGET_FREQ)
    tone(sidetone_line, GOERTZ_TARGET_FREQ);
#else
    tone(sidetone_line, hz_sidetone);
#endif // defined(GOERTZ_TARGET_FREQ)
  }
  else
  {
    noTone(sidetone_line);
  }
#endif // DEBUG_CW_DECODER

  if ((cw_decoder_indicator) && (cd_decoder_pin_state == LOW))
  {
    digitalWrite(cw_decoder_indicator, HIGH);
  }
  else
  {
    digitalWrite(cw_decoder_indicator, LOW);
  }

#ifdef DEBUG_OPTION_CW_DECODER_GOERTZEL_AUDIO_DETECTOR
  static unsigned long last_magnitude_debug_print = 0;
  if ((millis() - last_magnitude_debug_print) > 250)
  {
    // debug_serial_port->print("service_cw_decoder: cwtonedetector magnitude: ");
    // debug_serial_port->print(cwtonedetector.magnitudelimit_low);
    // debug_serial_port->print("\t");
    debug_serial_port->print(cwtonedetector.magnitudelimit);
    debug_serial_port->print("\t");
    debug_serial_port->println(cwtonedetector.magnitude);
    last_magnitude_debug_print = millis();
  }
#endif

  if (last_transition_time == 0)
  {
    if (cd_decoder_pin_state == LOW)
    { // is this our first tone?
      last_transition_time = millis();
      last_state = LOW;

#ifdef FEATURE_SLEEP
      last_activity_time = millis();
#endif // FEATURE_SLEEP
#ifdef FEATURE_LCD_BACKLIGHT_AUTO_DIM
      last_active_time = millis();
#endif // FEATURE_LCD_BACKLIGHT_AUTO_DIM
    }
    else
    {
      if ((last_decode_time > 0) && (!space_sent) && ((millis() - last_decode_time) > ((1200 / decoder_wpm) * CW_DECODER_SPACE_PRINT_THRESH)))
      { // should we send a space?
#if defined(FEATURE_SERIAL)
#ifdef FEATURE_COMMAND_LINE_INTERFACE
        primary_serial_port->write(32);
        screen_column++;
#endif // FEATURE_COMMAND_LINE_INTERFACE
#endif // FEATURE_SERIAL
#ifdef FEATURE_DISPLAY
        display_scroll_print_char(' ');
#endif // FEATURE_DISPLAY
        space_sent = 1;
      }
    }
  }
  else
  {
    if (cd_decoder_pin_state != last_state)
    {
      // we have a transition
      element_duration = millis() - last_transition_time;
      if (element_duration > CW_DECODER_NOISE_FILTER)
      { // filter out noise
        if (cd_decoder_pin_state == LOW)
        {                                                                    // we have a tone
          decode_elements[decode_element_pointer] = (-1 * element_duration); // the last element was a space, so make it negative
          no_tone_count++;
          if (decode_element_no_tone_average == 0)
          {
            decode_element_no_tone_average = element_duration;
          }
          else
          {
            decode_element_no_tone_average = (element_duration + decode_element_no_tone_average) / 2;
          }
          decode_element_pointer++;
          last_state = LOW;
        }
        else
        {                                                             // we have no tone
          decode_elements[decode_element_pointer] = element_duration; // the last element was a tone, so make it positive
          tone_count++;
          if (decode_element_tone_average == 0)
          {
            decode_element_tone_average = element_duration;
          }
          else
          {
            decode_element_tone_average = (element_duration + decode_element_tone_average) / 2;
          }
          last_state = HIGH;
          decode_element_pointer++;
        }
        last_transition_time = millis();
        if (decode_element_pointer == 16)
        {
          decode_it_flag = 1;
        } // if we've filled up the array, go ahead and decode it
      }
    }
    else
    {
      // no transition
      element_duration = millis() - last_transition_time;
      if (last_state == HIGH)
      {
        // we're still high (no tone) - have we reached character space yet?
        // if ((element_duration > (decode_element_no_tone_average * 2.5)) || (element_duration > (decode_element_tone_average * 2.5))) {
        if (element_duration > (float(1200 / decoder_wpm) * CW_DECODER_SPACE_DECODE_THRESH))
        {
          decode_it_flag = 1;
        }
      }
      else
      {
        // have we had tone for an outrageous amount of time?
      }
    }
  }

  if (decode_it_flag)
  { // are we ready to decode the element array?

    // adjust the decoder wpm based on what we got

    if ((no_tone_count > 0) && (tone_count > 1))
    { // NEW

      if (decode_element_no_tone_average > 0)
      {
        if (abs((1200 / decode_element_no_tone_average) - decoder_wpm) < 5)
        {
          decoder_wpm = (decoder_wpm + (1200 / decode_element_no_tone_average)) / 2;
        }
        else
        {
          if (abs((1200 / decode_element_no_tone_average) - decoder_wpm) < 10)
          {
            decoder_wpm = (decoder_wpm + decoder_wpm + (1200 / decode_element_no_tone_average)) / 3;
          }
          else
          {
            if (abs((1200 / decode_element_no_tone_average) - decoder_wpm) < 20)
            {
              decoder_wpm = (decoder_wpm + decoder_wpm + decoder_wpm + (1200 / decode_element_no_tone_average)) / 4;
            }
          }
        }
      }

    } // NEW

#ifdef DEBUG_CW_DECODER_WPM
    if (abs(decoder_wpm - last_printed_decoder_wpm) > 0.9)
    {
      debug_serial_port->print("<");
      debug_serial_port->print(int(decoder_wpm));
      debug_serial_port->print(">");
      last_printed_decoder_wpm = decoder_wpm;
    }
#endif // DEBUG_CW_DECODER_WPM

    for (byte x = 0; x < decode_element_pointer; x++)
    {
      if (decode_elements[x] > 0)
      { // is this a tone element?
        // we have no spaces to time from, use the current wpm
        if ((decode_elements[x] / (1200 / decoder_wpm)) < 2.1 /*1.3*/)
        {                                                 // changed from 1.3 to 2.1 2015-05-12
          decode_character = (decode_character * 10) + 1; // we have a dit
        }
        else
        {
          decode_character = (decode_character * 10) + 2; // we have a dah
        }
      }
#ifdef DEBUG_CW_DECODER
      debug_serial_port->print(F("service_cw_decoder: decode_elements["));
      debug_serial_port->print(x);
      debug_serial_port->print(F("]: "));
      debug_serial_port->println(decode_elements[x]);
#endif // DEBUG_CW_DECODER
    }

#ifdef DEBUG_CW_DECODER
    debug_serial_port->print(F("service_cw_decoder: decode_element_tone_average: "));
    debug_serial_port->println(decode_element_tone_average);
    debug_serial_port->print(F("service_cw_decoder: decode_element_no_tone_average: "));
    debug_serial_port->println(decode_element_no_tone_average);
    debug_serial_port->print(F("service_cw_decoder: decode_element_no_tone_average wpm: "));
    debug_serial_port->println(1200 / decode_element_no_tone_average);
    debug_serial_port->print(F("service_cw_decoder: decoder_wpm: "));
    debug_serial_port->println(decoder_wpm);
    debug_serial_port->print(F("service_cw_decoder: decode_character: "));
    debug_serial_port->println(decode_character);
#endif // DEBUG_CW_DECODER

#if defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)
    primary_serial_port->write(convert_cw_number_to_ascii(decode_character));
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
    secondary_serial_port->write(convert_cw_number_to_ascii(decode_character));
#endif
    screen_column++;
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_COMMAND_LINE_INTERFACE)

#ifdef FEATURE_DISPLAY
    display_scroll_print_char(convert_cw_number_to_ascii(decode_character));
#endif // FEATURE_DISPLAY

    // reinitialize everything
    last_transition_time = 0;
    last_decode_time = millis();
    decode_element_pointer = 0;
    decode_element_tone_average = 0;
    decode_element_no_tone_average = 0;
    space_sent = 0;
    no_tone_count = 0;
    tone_count = 0;
  } // if (decode_it_flag)

#if defined(FEATURE_SERIAL)
#ifdef FEATURE_COMMAND_LINE_INTERFACE
  if (screen_column > CW_DECODER_SCREEN_COLUMNS)
  {
    primary_serial_port->println();
#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
    secondary_serial_port->println();
#endif
    screen_column = 0;
  }
#endif // FEATURE_COMMAND_LINE_INTERFACE
#endif // FEATURE_SERIAL
}

#endif // FEATURE_CW_DECODER

//---------------------------------------------------------------------

void initialize_keyer_state()
{

  key_state = 0;
  key_tx = 1;

  configuration.wpm = initial_speed_wpm;
  pot_wpm_low_value = initial_pot_wpm_low_value;
  configuration.paddle_interruption_quiet_time_element_lengths = default_paddle_interruption_quiet_time_element_lengths;
  configuration.hz_sidetone = initial_sidetone_freq;
  configuration.memory_repeat_time = default_memory_repeat_time;
  configuration.cmos_super_keyer_iambic_b_timing_percent = default_cmos_super_keyer_iambic_b_timing_percent;
  configuration.dah_to_dit_ratio = initial_dah_to_dit_ratio;
  configuration.length_wordspace = default_length_wordspace;
  configuration.weighting = default_weighting;
  configuration.wordsworth_wordspace = default_wordsworth_wordspace;
  configuration.wordsworth_repetition = default_wordsworth_repetition;
  configuration.wpm_farnsworth = initial_speed_wpm;
  configuration.cli_mode = CLI_NORMAL_MODE;
  configuration.wpm_command_mode = initial_command_mode_speed_wpm;
  configuration.ptt_buffer_hold_active = 0;
  configuration.sidetone_volume = sidetone_volume_low_limit + ((sidetone_volume_high_limit - sidetone_volume_low_limit) / 2);
  configuration.ptt_disabled = 0;
  configuration.beacon_mode_on_boot_up = 0;
  configuration.cw_echo_timing_factor = 100 * default_cw_echo_timing_factor;
  configuration.autospace_timing_factor = 100 * default_autospace_timing_factor;
  configuration.keying_compensation = default_keying_compensation;

  configuration.ptt_lead_time[0] = initial_ptt_lead_time_tx1;
  configuration.ptt_tail_time[0] = initial_ptt_tail_time_tx1;
  configuration.ptt_lead_time[1] = initial_ptt_lead_time_tx2;
  configuration.ptt_tail_time[1] = initial_ptt_tail_time_tx2;
#if !defined(OPTION_SAVE_MEMORY_NANOKEYER)
  configuration.ptt_lead_time[2] = initial_ptt_lead_time_tx3;
  configuration.ptt_tail_time[2] = initial_ptt_tail_time_tx3;
  configuration.ptt_lead_time[3] = initial_ptt_lead_time_tx4;
  configuration.ptt_tail_time[3] = initial_ptt_tail_time_tx4;
  configuration.ptt_lead_time[4] = initial_ptt_lead_time_tx5;
  configuration.ptt_tail_time[4] = initial_ptt_tail_time_tx5;
  configuration.ptt_lead_time[5] = initial_ptt_lead_time_tx6;
  configuration.ptt_tail_time[5] = initial_ptt_tail_time_tx6;

  for (int x = 0; x < 5; x++)
  {
    configuration.ptt_active_to_sequencer_active_time[x] = 0;
    configuration.ptt_inactive_to_sequencer_inactive_time[x] = 0;
  }
#endif // OPTION_SAVE_MEMORY_NANOKEYER

#ifndef FEATURE_SO2R_BASE
  switch_to_tx_silent(1);
#endif

#if !defined(FEATURE_DUAL_MODE_KEYER_AND_TINYFSK)
#ifdef __LGT8FX8P__
  /* LGT chip emulates EEPROM at the cost of giving up twice the space in program flash memory.
   * Unfortunately, the last 4 bytes of every 1KB block are read-only. Therefore
   * EEPROM.length() would return 1024 (readable EEPROM size), while EEPROM.size() returns 1020
   * (writable EEPROM size). The following line will give the right figure for LGT.
   */
  memory_area_end = EEPROM.size() - 1;
#elif (!defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_ARCH_MBED) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO) && !defined(HARDWARE_GENERIC_STM32F103C)) || (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024))
  memory_area_end = EEPROM.length() - 1;
#else
#if defined(HARDWARE_GENERIC_STM32F103C)
  memory_area_end = 254;
#else
  memory_area_end = 1024; // not sure if this is a valid assumption
#endif
#endif
#else
#ifdef __LGT8FX8P__
  /* LGT chip emulates EEPROM at the cost of giving up twice the space in program flash memory.
   * Unfortunately, the last 4 bytes of every 1KB block are read-only. Therefore
   * EEPROM.length() would return 1024 (readable EEPROM size), while EEPROM.size() returns 1020
   * (writable EEPROM size). The following line will give the right figure for LGT.
   */
  memory_area_end = EEPROM.size() - 1 - 2;
#elif (!defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_ARCH_MBED) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO) && !defined(HARDWARE_GENERIC_STM32F103C)) || (defined(ARDUINO_SAM_DUE) && defined(FEATURE_EEPROM_E24C1024))
  memory_area_end = EEPROM.length() - 1 - 2;
#else
#if defined(HARDWARE_GENERIC_STM32F103C)
  memory_area_end = 254 - 2;
#else
  memory_area_end = 1024 - 2; // not sure if 1024 is a valid assumption
#endif
#endif
#endif
}

//---------------------------------------------------------------------
void initialize_potentiometer()
{

#ifdef FEATURE_POTENTIOMETER
  pinMode(potentiometer, INPUT);
  pot_wpm_high_value = initial_pot_wpm_high_value;
  last_pot_wpm_read = pot_value_wpm();
  configuration.pot_activated = 1;
#endif
}

//---------------------------------------------------------------------
void initialize_rotary_encoder()
{

#ifdef FEATURE_ROTARY_ENCODER
#ifdef OPTION_ENCODER_ENABLE_PULLUPS
#if defined(ARDUINO_MAPLE_MINI) || defined(ARDUINO_GENERIC_STM32F103C) // sp5iou 20180329
  pinMode(rotary_pin1, INPUT_PULLUP);                                  // sp5iou 20180329
  pinMode(rotary_pin2, INPUT_PULLUP);                                  // sp5iou 20180329
#else                                                                  // (ARDUINO_MAPLE_MINI)||defined(ARDUINO_GENERIC_STM32F103C) //sp5iou 20180329
  pinMode(rotary_pin1, INPUT);
  pinMode(rotary_pin2, INPUT);
  digitalWrite(rotary_pin1, HIGH);
  digitalWrite(rotary_pin2, HIGH);
#endif
#endif // OPTION_ENCODER_ENABLE_PULLUPS
#endif // FEATURE_ROTARY_ENCODER
}

//---------------------------------------------------------------------

void initialize_default_modes()
{

  // setup default modes
  keyer_machine_mode = KEYER_NORMAL;
  configuration.paddle_mode = PADDLE_NORMAL;
  configuration.keyer_mode = IAMBIC_B;
  configuration.sidetone_mode = SIDETONE_ON;

#ifdef initial_sidetone_mode
  configuration.sidetone_mode = initial_sidetone_mode;
#endif

  char_send_mode = CW;

#if defined(FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING) && defined(OPTION_CMOS_SUPER_KEYER_IAMBIC_B_TIMING_ON_BY_DEFAULT) // DL1HTB initialize CMOS Super Keyer if feature is enabled
  configuration.cmos_super_keyer_iambic_b_timing_on = 1;
#endif // FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING // #end DL1HTB initialize CMOS Super Keyer if feature is enabled

  delay(250); // wait a little bit for the caps to charge up on the paddle lines
}

//---------------------------------------------------------------------

void initialize_watchdog()
{

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
  wdt_enable(WDTO_4S);
#endif // OPTION_WATCHDOG_TIMER
}

//---------------------------------------------------------------------

void check_eeprom_for_initialization()
{

#if defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO)
  EEPROM.begin(4096);
#endif

  // do an eeprom reset to defaults if paddles are squeezed
  if (paddle_pin_read(paddle_left) == LOW && paddle_pin_read(paddle_right) == LOW)
  {
    while (paddle_pin_read(paddle_left) == LOW && paddle_pin_read(paddle_right) == LOW)
    {
    }
    initialize_eeprom();
  }

  // read settings from eeprom and initialize eeprom if it has never been written to
  if (read_settings_from_eeprom())
  {
#if defined(HARDWARE_GENERIC_STM32F103C)
    EEPROM.init();   // sp5iou 20180328 to reinitialize / initialize EEPROM
    EEPROM.format(); // sp5iou 20180328 to reinitialize / format EEPROM
#endif
    initialize_eeprom();
  }
}

//---------------------------------------------------------------------

void check_for_beacon_mode()
{

#ifndef OPTION_SAVE_MEMORY_NANOKEYER
  // check for beacon mode (paddle_left == low) or straight key mode (paddle_right == low)
  if (paddle_pin_read(paddle_left) == LOW)
  {
#ifdef FEATURE_BEACON
    keyer_machine_mode = BEACON;
#endif
  }
  else
  {
    if (paddle_pin_read(paddle_right) == LOW)
    {
      configuration.keyer_mode = STRAIGHT;
    }
  }
#endif // OPTION_SAVE_MEMORY_NANOKEYER
}

//---------------------------------------------------------------------

void check_for_debug_modes()
{

#ifdef DEBUG_CAPTURE_COM_PORT
  primary_serial_port->begin(primary_serial_port_baud_rate);
  debug_capture();
#endif

#ifdef DEBUG_HELL_TEST
  hell_test();
#endif
}

//---------------------------------------------------------------------

void initialize_serial_ports()
{

// initialize serial port
#if defined(FEATURE_SERIAL)

#if defined(FEATURE_WINKEY_EMULATION) && defined(FEATURE_COMMAND_LINE_INTERFACE) //--------------------------------------------

#ifdef FEATURE_BUTTONS
  if (analogbuttonread(0))
  {
#ifdef OPTION_PRIMARY_SERIAL_PORT_DEFAULT_WINKEY_EMULATION
    primary_serial_port_mode = SERIAL_CLI;
    primary_serial_port_baud_rate = PRIMARY_SERIAL_PORT_BAUD;
#else
    primary_serial_port_mode = SERIAL_WINKEY_EMULATION;
    primary_serial_port_baud_rate = WINKEY_DEFAULT_BAUD;
#endif // ifndef OPTION_PRIMARY_SERIAL_PORT_DEFAULT_WINKEY_EMULATION
  }
  else
  {
#ifdef OPTION_PRIMARY_SERIAL_PORT_DEFAULT_WINKEY_EMULATION
    primary_serial_port_mode = SERIAL_WINKEY_EMULATION;
    primary_serial_port_baud_rate = WINKEY_DEFAULT_BAUD;
#else
    primary_serial_port_mode = SERIAL_CLI;
    primary_serial_port_baud_rate = PRIMARY_SERIAL_PORT_BAUD;
#endif // ifndef OPTION_PRIMARY_SERIAL_PORT_DEFAULT_WINKEY_EMULATION
  }
  while (analogbuttonread(0))
  {
  }
#else // FEATURE_BUTTONS
#ifdef OPTION_PRIMARY_SERIAL_PORT_DEFAULT_WINKEY_EMULATION
  primary_serial_port_mode = SERIAL_WINKEY_EMULATION;
  primary_serial_port_baud_rate = WINKEY_DEFAULT_BAUD;
#else
  primary_serial_port_mode = SERIAL_CLI;
  primary_serial_port_baud_rate = PRIMARY_SERIAL_PORT_BAUD;
#endif // ifndef OPTION_PRIMARY_SERIAL_PORT_DEFAULT_WINKEY_EMULATION
#endif // FEATURE_BUTTONS
#endif // defined(FEATURE_WINKEY_EMULATION) && defined(FEATURE_COMMAND_LINE_INTERFACE)---------------------------------

#if !defined(FEATURE_WINKEY_EMULATION) && defined(FEATURE_COMMAND_LINE_INTERFACE)
  primary_serial_port_mode = SERIAL_CLI;
  primary_serial_port_baud_rate = PRIMARY_SERIAL_PORT_BAUD;
#endif //! defined(FEATURE_WINKEY_EMULATION) && defined(FEATURE_COMMAND_LINE_INTERFACE)

#if defined(FEATURE_WINKEY_EMULATION) && !defined(FEATURE_COMMAND_LINE_INTERFACE)
  primary_serial_port_mode = SERIAL_WINKEY_EMULATION;
  primary_serial_port_baud_rate = WINKEY_DEFAULT_BAUD;
#endif // defined(FEATURE_WINKEY_EMULATION) && !defined(FEATURE_COMMAND_LINE_INTERFACE)

  primary_serial_port = PRIMARY_SERIAL_PORT;

  primary_serial_port->begin(primary_serial_port_baud_rate);

#ifdef DEBUG_STARTUP
  debug_serial_port->println(F("setup: serial port opened"));
#endif // DEBUG_STARTUP

#if !defined(OPTION_SUPPRESS_SERIAL_BOOT_MSG) && defined(FEATURE_COMMAND_LINE_INTERFACE)
  if (primary_serial_port_mode == SERIAL_CLI)
  {
    primary_serial_port->print(F("\n\rK3NG Keyer Version "));
    primary_serial_port->write(CODE_VERSION);
    primary_serial_port->println();
#if defined(FEATURE_SERIAL_HELP)
    primary_serial_port->println(F("\n\rEnter \\? for help\n"));
#endif
    while (primary_serial_port->available())
    {
      primary_serial_port->read();
    } // clear out buffer at boot
  }
#ifdef DEBUG_MEMORYCHECK
  memorycheck();
#endif // DEBUG_MEMORYCHECK
#endif //! defined(OPTION_SUPPRESS_SERIAL_BOOT_MSG) && defined(FEATURE_COMMAND_LINE_INTERFACE)

#ifdef DEBUG_AUX_SERIAL_PORT
  debug_port = DEBUG_AUX_SERIAL_PORT;
  debug_serial_port->begin(DEBUG_AUX_SERIAL_PORT_BAUD);
  debug_serial_port->print("debug port open ");
  debug_serial_port->println(CODE_VERSION);
#endif // DEBUG_AUX_SERIAL_PORT

#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
#if defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO)
  Serial2.setRX(9);
  Serial2.setTX(8);
  Serial2.begin(SECONDARY_SERIAL_PORT_BAUD);
  // Serial2.println("Hello I'm Serial2");
  secondary_serial_port = &Serial2;
  // secondary_serial_port = SECONDARY_SERIAL_PORT;  // this does not work, not sure why
#else
  secondary_serial_port = SECONDARY_SERIAL_PORT;
  secondary_serial_port->begin(SECONDARY_SERIAL_PORT_BAUD);
#endif
#if !defined(OPTION_SUPPRESS_SERIAL_BOOT_MSG)
  secondary_serial_port->print(F("\n\rK3NG Keyer Version "));
  secondary_serial_port->write(CODE_VERSION);
  secondary_serial_port->println();
#if defined(FEATURE_SERIAL_HELP)
  secondary_serial_port->println(F("\n\rEnter \\? for help\n"));
#endif
  while (secondary_serial_port->available())
  {
    secondary_serial_port->read();
  } // clear out buffer at boot
#endif
#endif // FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT

#ifdef FEATURE_COMMAND_LINE_INTERFACE_ON_SECONDARY_PORT
  debug_serial_port = secondary_serial_port;
#else
  debug_serial_port = primary_serial_port;
#endif

#endif // FEATURE_SERIAL
}

//---------------------------------------------------------------------
void initialize_ps2_keyboard()
{

#ifdef FEATURE_PS2_KEYBOARD

#ifdef OPTION_PS2_KEYBOARD_RESET // code contributed by Bill, W9BEL
  attachInterrupt(1, ps2int_write, FALLING);
  digitalWrite(ps2_keyboard_data, LOW); // pullup off
  pinMode(ps2_keyboard_data, OUTPUT);   // pull clock low
  delay(200);
#endif // OPTION_PS2_KEYBOARD_RESET

  keyboard.begin(ps2_keyboard_data, ps2_keyboard_clock);
#endif // FEATURE_PS2_KEYBOARD
}
//---------------------------------------------------------------------

#if defined(FEATURE_PS2_KEYBOARD) && defined(OPTION_PS2_KEYBOARD_RESET)
void ps2int_write()
{

  // code contributed by Bill, W9BEL
  //----- Called from initialize_ps2_keyboard to reset Mini KBD ---------
  // The ISR for the external interrupt in read mode

  uint8_t buffer[45];
  uint8_t head, tail, writeByte = 255;
  uint8_t curbit = 0, parity = 0, ack = 0;

  if (curbit < 8)
  {
    if (writeByte & 1)
    {
      parity ^= 1;
      digitalWrite(ps2_keyboard_data, HIGH);
    }
    else
      digitalWrite(ps2_keyboard_data, LOW);
    writeByte >>= 1;
  }
  else if (curbit == 8)
  { // parity
    if (parity)
      digitalWrite(ps2_keyboard_data, LOW);
    else
      digitalWrite(ps2_keyboard_data, HIGH);
  }
  else if (curbit == 9)
  {                                        // time to let go
    pinMode(ps2_keyboard_data, INPUT);     // release line
    digitalWrite(ps2_keyboard_data, HIGH); // pullup on
  }
  else
  { // time to check device ACK and hold clock again
    // holdClock();
    digitalWrite(ps2_keyboard_clock, LOW); // pullup off
    pinMode(ps2_keyboard_clock, OUTPUT);   // pull clock low
    ack = !digitalRead(ps2_keyboard_data);
  }
  curbit++;
}
#endif

//---------------------------------------------------------------------

void initialize_display()
{

#ifdef FEATURE_DISPLAY

#ifdef FEATURE_OLED_SSD1306
  Wire.begin();
  Wire.setClock(400000L);
  lcd.begin(&Adafruit128x64, oled_i2c_address_ssd1306);
  lcd.setFont(fixed_bold10x15);
#else
#if defined(FEATURE_LCD_SAINSMART_I2C) || defined(FEATURE_LCD_I2C_FDEBRABANDER)
  lcd.begin();
  lcd.home();
#else
  lcd.begin(LCD_COLUMNS, LCD_ROWS);
#endif
#endif

#ifdef FEATURE_LCD_ADAFRUIT_I2C
  lcd.setBacklight(lcdcolor);
#endif // FEATURE_LCD_ADAFRUIT_I2C

#ifdef FEATURE_LCD_TWILIQUIDCRYSTAL
  lcd.backlight();
#endif
#ifdef FEATURE_LCD_ADAFRUIT_BACKPACK
  lcd.setBacklight(HIGH);
#endif
#ifdef FEATURE_LCD_MATHERTEL_PCF8574
  lcd.setBacklight(HIGH);
#endif

#ifdef OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS // OZ1JHM provided code, cleaned up by LA3ZA
  // Store bit maps, designed using editor at http://omerk.github.io/lcdchargen/

  byte U_umlaut[8] = {B01010, B00000, B10001, B10001, B10001, B10001, B01110, B00000};   // 'Ü'
  byte O_umlaut[8] = {B01010, B00000, B01110, B10001, B10001, B10001, B01110, B00000};   // 'Ö'
  byte A_umlaut[8] = {B01010, B00000, B01110, B10001, B11111, B10001, B10001, B00000};   // 'Ä'
  byte AE_capital[8] = {B01111, B10100, B10100, B11110, B10100, B10100, B10111, B00000}; // 'Æ'
  byte OE_capital[8] = {B00001, B01110, B10011, B10101, B11001, B01110, B10000, B00000}; // 'Ø'
  byte empty[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000};      // empty
  byte AA_capital[8] = {B00100, B00000, B01110, B10001, B11111, B10001, B10001, B00000}; // 'Å'
  byte Ntilde[8] = {B01101, B10010, B00000, B11001, B10101, B10011, B10001, B00000};     // 'Ñ'

  //     upload 8 characters to the lcd
  lcd.createChar(0, U_umlaut);   //     German
  lcd.createChar(1, O_umlaut);   //     German, Swedish
  lcd.createChar(2, A_umlaut);   //     German, Swedish
  lcd.createChar(3, AE_capital); //   Danish, Norwegian
  lcd.createChar(4, OE_capital); //   Danish, Norwegian
  lcd.createChar(5, empty);      //        For some reason this one needs to display nothing - otherwise it will display in pauses on serial interface
  lcd.createChar(6, AA_capital); //   Danish, Norwegian, Swedish
  lcd.createChar(7, Ntilde);     //       Spanish
  lcd.clear();                   // you have to ;o)
#endif                           // OPTION_DISPLAY_NON_ENGLISH_EXTENSIONS

  if (LCD_COLUMNS < 9)
  {
    lcd_center_print_timed("K3NGKeyr", 0, 4000);
  }
  else
  {
    lcd_center_print_timed("K3NG Keyer", 0, 4000);
#ifdef OPTION_PERSONALIZED_STARTUP_SCREEN
    if (LCD_ROWS == 2)
    {
      lcd_center_print_timed(custom_startup_field, 1, 4000); // display the custom field on the second line of the display, maximum field length is the number of columns
    }
    else if (LCD_ROWS > 2)
    {
      lcd_center_print_timed("hi", 1, 4000);                 // display 'hi' on the 2nd line anyway
      lcd_center_print_timed(custom_startup_field, 2, 4000); // display the custom field on the third line of the display, maximum field length is the number of columns
    }
#else
    lcd_center_print_timed("hi", 1, 4000);
#endif // OPTION_PERSONALIZED_STARTUP_SCREEN
    if (LCD_ROWS > 3)
      lcd_center_print_timed("V: " + String(CODE_VERSION), 3, 4000); // display the code version on the fourth line of the display
  }
#endif // FEATURE_DISPLAY

  if (keyer_machine_mode != BEACON)
  {
#ifndef OPTION_DO_NOT_SAY_HI
    // sound out HI
    // store current setting (compliments of DL2SBA - http://dl2sba.com/ )
    byte oldKey = key_tx;
    byte oldSideTone = configuration.sidetone_mode;
    key_tx = 0;
    configuration.sidetone_mode = SIDETONE_ON;

    char hi_text[16];
    strcpy(hi_text, HI_TEXT);
    for (int x = 0; hi_text[x] != 0; x++)
    {
      send_char(hi_text[x], KEYER_NORMAL);
    }

    configuration.sidetone_mode = oldSideTone;
    key_tx = oldKey;
#endif // OPTION_DO_NOT_SAY_HI
#ifdef OPTION_BLINK_HI_ON_PTT
    blink_ptt_dits_and_dahs(".... ..");
#endif
  }
}

//-------------------------------------------------------------------------------------------------------
#if defined(OPTION_BLINK_HI_ON_PTT) || (defined(OPTION_WINKEY_BLINK_PTT_ON_HOST_OPEN) && defined(FEATURE_WINKEY_EMULATION))
void blink_ptt_dits_and_dahs(char const *cw_to_send)
{

  sending_mode = AUTOMATIC_SENDING;

  for (int x = 0; x < strlen(cw_to_send); x++)
  {
    switch (cw_to_send[x])
    {
    case '.':
      ptt_key();
      delay(100);
      ptt_unkey();
      delay(100);
      break;
    case '-':
      ptt_key();
      delay(300);
      ptt_unkey();
      delay(100);
      break;
    case ' ':
      delay(400);
      break;
    }

#if defined(OPTION_WATCHDOG_TIMER) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    wdt_reset();
#endif // OPTION_WATCHDOG_TIMER
  }
}
#endif // defined(OPTION_BLINK_HI_ON_PTT) || (defined(OPTION_WINKEY_BLINK_PTT_ON_HOST_OPEN) && defined(FEATURE_WINKEY_EMULATION))

//---------------------------------------------------------------------
#ifdef FEATURE_USB_KEYBOARD
void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{

#ifdef FEATURE_MEMORIES
  enum usb_kbd_states
  {
    USB_KEYBOARD_NORMAL,
    USB_KEYBOARD_WPM_ADJUST,
    USB_KEYBOARD_FARNS_WPM_ADJUST,
    USB_KEYBOARD_SN_ENTRY,
    USB_KEYBOARD_PROGRAM_MEM
  };
#else
  enum usb_kbd_states
  {
    USB_KEYBOARD_NORMAL,
    USB_KEYBOARD_WPM_ADJUST,
    USB_KEYBOARD_FARNS_WPM_ADJUST,
    USB_KEYBOARD_SN_ENTRY
  };
#endif

#define USB_KEYBOARD_SPECIAL_MODE_TIMEOUT 5000

  static byte usb_keyboard_mode = USB_KEYBOARD_NORMAL;

  static byte user_num_input_places = 0;
  static int user_num_input_lower_limit = 0;
  static int user_num_input_upper_limit = 0;
  static byte user_input_index = 0;
  static byte user_input_array[255];
  static int user_num_input_number_entered = 0;
  byte user_input_process_it = 0;
#ifdef FEATURE_MEMORIES
  static byte usb_keyboard_program_memory = 0;
#endif // FEATURE_MEMORIES
  int x = 0;

#ifdef FEATURE_DISPLAY
  String lcd_string;
#endif

  MODIFIERKEYS modifier;
  *((uint8_t *)&modifier) = mod;

#ifdef DEBUG_USB_KEYBOARD
  debug_serial_port->print(F("KbdRptParser::OnKeyDown: mod:"));
  debug_serial_port->print(mod);
  debug_serial_port->print(" key:");
  debug_serial_port->print(key);
  debug_serial_port->print("\t");
  debug_serial_port->print((modifier.bmLeftCtrl == 1) ? "LeftCtrl" : " ");
  debug_serial_port->print((modifier.bmLeftShift == 1) ? "LeftShift" : " ");
  debug_serial_port->print((modifier.bmLeftAlt == 1) ? "LeftAlt" : " ");
  debug_serial_port->print((modifier.bmLeftGUI == 1) ? "LeftGUI" : " ");
  debug_serial_port->print((modifier.bmRightCtrl == 1) ? "RightCtrl" : " ");
  debug_serial_port->print((modifier.bmRightShift == 1) ? "RightShift" : " ");
  debug_serial_port->print((modifier.bmRightAlt == 1) ? "RightAlt" : " ");
  debug_serial_port->print((modifier.bmRightGUI == 1) ? "RightGUI" : " ");
  debug_serial_port->print("\t");
  PrintHex<uint8_t>(key, 0x80);
  debug_serial_port->println();
#endif // DEBUG_USB_KEYBOARD

  byte usb_keyboard_prosign_flag = 0;
  uint8_t keystroke = OemToAscii(mod, key);
  byte keyboard_tune_on = 0;

#ifdef FEATURE_MEMORIES
  if (usb_keyboard_mode == USB_KEYBOARD_PROGRAM_MEM)
  {

    if ((key == 0x2a) && (user_input_index))
    { // BACKSPACE
      user_input_index--;
#ifdef FEATURE_DISPLAY
      keyboard_string = keyboard_string.substring(0, keyboard_string.length() - 1);
      lcd_center_print_timed(keyboard_string, 1, 0 /*default_display_msg_delay)*/);
#endif
      usb_keyboard_special_mode_start_time = millis();
      return;
    }
    if ((key == 0x28) || (key == 0x58))
    {
      user_input_process_it = 1;
    } // ENTER
    if (key == 0x29)
    { // ESCAPE
#ifdef FEATURE_DISPLAY
      lcd_status = LCD_REVERT;
#else
      boop();
#endif
      user_input_index = 0;
      usb_keyboard_mode = USB_KEYBOARD_NORMAL;
      return;
    }
    if ((keystroke > 31) && (keystroke < 123))
    {
      usb_keyboard_special_mode_start_time = millis();
      keystroke = uppercase(keystroke);
#ifdef FEATURE_DISPLAY
      keyboard_string.concat(char(keystroke));
      if (keyboard_string.length() > LCD_COLUMNS)
      {
        lcd_center_print_timed(keyboard_string.substring((keyboard_string.length() - LCD_COLUMNS)), 1, default_display_msg_delay);
      }
      else
      {
        lcd_center_print_timed(keyboard_string, 1, default_display_msg_delay);
      }
#endif
      user_input_array[user_input_index] = keystroke;
      user_input_index++;
      if (user_input_index > (memory_end(usb_keyboard_program_memory) - memory_start(usb_keyboard_program_memory)))
      {
        user_input_process_it = 1;
      }
#ifdef DEBUG_USB_KEYBOARD
      debug_serial_port->print(F("KbdRptParser::OnKeyDown: user_input_index: "));
      debug_serial_port->println(user_input_index);
#endif // DEBUG_USB_KEYBOARD
    } // if ((keystroke > 31) && (keystroke < 123))
    if (user_input_process_it)
    {
#ifdef DEBUG_USB_KEYBOARD
      debug_serial_port->println(F("KbdRptParser::OnKeyDown: user_input_process_it"));
#endif // DEBUG_USB_KEYBOARD
      for (x = 0; x < user_input_index; x++)
      { // write to memory
        EEPROM.write((memory_start(usb_keyboard_program_memory) + x), user_input_array[x]);
        if ((memory_start(usb_keyboard_program_memory) + x) == memory_end(usb_keyboard_program_memory))
        { // are we at last memory location?
          x = user_input_index;
        }
      }
      // write terminating 255
      EEPROM.write((memory_start(usb_keyboard_program_memory) + x), 255);
#ifdef FEATURE_DISPLAY
      lcd_center_print_timed("Done", 0, default_display_msg_delay);
#else
      beep();
#endif
      user_input_process_it = 0;
      user_input_index = 0;
      usb_keyboard_mode = USB_KEYBOARD_NORMAL;
    } // if (user_input_process_it)
    return;
  } // if (usb_keyboard_mode == USB_KEYBOARD_PROGRAM_MEM)
#endif // FEATURE_MEMORIES

  if ((usb_keyboard_mode == USB_KEYBOARD_WPM_ADJUST) || (usb_keyboard_mode == USB_KEYBOARD_WPM_ADJUST) || (usb_keyboard_mode == USB_KEYBOARD_FARNS_WPM_ADJUST) || (usb_keyboard_mode == USB_KEYBOARD_SN_ENTRY))
  {
    if ((key > 29) && (key < 40))
    { // convert keyboard code to number
      if (key == 39)
      {
        user_input_array[user_input_index] = 0;
#ifdef FEATURE_DISPLAY
        keyboard_string.concat(String(0));
#endif
      }
      else
      {
        user_input_array[user_input_index] = key - 29;
#ifdef FEATURE_DISPLAY
        keyboard_string.concat(String(key - 29));
#endif
      }
#ifdef FEATURE_DISPLAY
      lcd_center_print_timed(keyboard_string, 1, default_display_msg_delay);
#endif
      user_input_index++;
      usb_keyboard_special_mode_start_time = millis();
    }
    else
    { // not a number key, is it a special key?
      if ((key == 0x2a) && (user_input_index))
      { // BACKSPACE
        user_input_index--;
#ifdef FEATURE_DISPLAY
        keyboard_string = keyboard_string.substring(0, keyboard_string.length() - 1);
        lcd_center_print_timed(keyboard_string, 1, default_display_msg_delay);
#endif
      }
      if ((key == 0x28) || (key == 0x58))
      {
        user_input_process_it = 1;
      } // ENTER
      if (key == 0x29)
      { // ESCAPE
        user_input_index = 0;
        usb_keyboard_mode = USB_KEYBOARD_NORMAL;
      }
    }

    if ((user_input_index >= user_num_input_places) || (user_input_process_it))
    { // is the user input ready to be processed?
      user_num_input_number_entered = 0;
      int y = 1;
      for (x = (user_input_index - 1); x >= 0; x--)
      {
        user_num_input_number_entered = user_num_input_number_entered + (user_input_array[x] * y);
        y = y * 10;
      }
      if ((user_num_input_number_entered > user_num_input_lower_limit) && (user_num_input_number_entered < user_num_input_upper_limit))
      {
        switch (usb_keyboard_mode)
        {
        case USB_KEYBOARD_WPM_ADJUST:
          speed_set(user_num_input_number_entered);
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#else
          beep();
#endif
          config_dirty = 1;
          break;
#ifdef FEATURE_FARNSWORTH
        case USB_KEYBOARD_FARNS_WPM_ADJUST:
          configuration.wpm_farnsworth = user_num_input_number_entered;
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#else
          beep();
#endif
          config_dirty = 1;
          break;
#endif // FEATURE_FARNSWORTH
        case USB_KEYBOARD_SN_ENTRY:
          serial_number = user_num_input_number_entered;
#ifdef FEATURE_DISPLAY
          lcd_status = LCD_REVERT;
#else
          beep();
#endif
          break;
        default:
          boop();
          break;
        }
      }
      else
      {
        boop(); // bad user input!
      }
      // reinitialize everything for the next go around
      user_input_index = 0;
      usb_keyboard_mode = USB_KEYBOARD_NORMAL;
    }

    return;
  }

  // grab the keypad / and * for dit and dah paddling
  if (key == 0x54)
  {
    usb_dit = 1;
    return;
  }
  if (key == 0x55)
  {
    usb_dah = 1;
    return;
  }
  if (key == 0x58)
  {
    sending_mode = MANUAL_SENDING;
    tx_and_sidetone_key(1);
    return;
  }

  if ((modifier.bmLeftShift) || (modifier.bmRightShift))
  {
    switch (key)
    {
    case 0x2a: // BACKSPACE - decrement serial number
      serial_number--;
#ifdef FEATURE_DISPLAY
      if (LCD_COLUMNS < 9)
      {
        lcd_center_print_timed("SN " + String(serial_number), 0, default_display_msg_delay);
      }
      else
      {
        lcd_center_print_timed("Serial: " + String(serial_number), 0, default_display_msg_delay);
      }
#endif
      return;
      break;

    } // switch(key)
#ifdef FEATURE_MEMORIES
    if ((key >= 0x3a) && (key <= 0x45))
    {                                           // SHIFT F1-F12 : program memories
      usb_keyboard_program_memory = key - 0x3a; // convert key scan code to memory number; F1 = 0
      if (usb_keyboard_program_memory > (number_of_memories - 1))
      {
        boop();
        return;
      }
      usb_keyboard_special_mode_start_time = millis();
      usb_keyboard_mode = USB_KEYBOARD_PROGRAM_MEM;
#ifdef FEATURE_DISPLAY
      if (LCD_COLUMNS < 9)
      {
        lcd_string = "PgmMem";
      }
      else
      {
        lcd_string = "Program Memory";
      }
      if (usb_keyboard_program_memory < 9)
      {
        lcd_string.concat(' ');
      }
      keyboard_string = "";
      lcd_string.concat(usb_keyboard_program_memory + 1);
      lcd_center_print_timed(lcd_string, 0, default_display_msg_delay);
#else
      boop_beep();
#endif

      repeat_memory = 255;
      return;
    }
#endif // FEATURE_MEMORIES

  } // if ((modifier.bmLeftShift) || (modifier.bmRightShift))

  if ((modifier.bmLeftAlt) || (modifier.bmRightAlt))
  {
    switch (key)
    {
#ifdef FEATURE_MEMORIES
    case 0x3a:
      if (number_of_memories > 0)
      {
        repeat_memory_msg(0);
      }
      return;
      break; // F1
    case 0x3b:
      if (number_of_memories > 1)
      {
        repeat_memory_msg(1);
      }
      return;
      break;
    case 0x3c:
      if (number_of_memories > 2)
      {
        repeat_memory_msg(2);
      }
      return;
      break;
    case 0x3d:
      if (number_of_memories > 3)
      {
        repeat_memory_msg(3);
      }
      return;
      break;
    case 0x3e:
      if (number_of_memories > 4)
      {
        repeat_memory_msg(4);
      }
      return;
      break;
    case 0x3f:
      if (number_of_memories > 5)
      {
        repeat_memory_msg(5);
      }
      return;
      break;
    case 0x40:
      if (number_of_memories > 6)
      {
        repeat_memory_msg(6);
      }
      return;
      break;
    case 0x41:
      if (number_of_memories > 7)
      {
        repeat_memory_msg(7);
      }
      return;
      break;
    case 0x42:
      if (number_of_memories > 8)
      {
        repeat_memory_msg(8);
      }
      return;
      break;
    case 0x43:
      if (number_of_memories > 9)
      {
        repeat_memory_msg(9);
      }
      return;
      break;
    case 0x44:
      if (number_of_memories > 10)
      {
        repeat_memory_msg(10);
      }
      return;
      break;
    case 0x45:
      if (number_of_memories > 11)
      {
        repeat_memory_msg(11);
      }
      return;
      break;
#endif
    } // switch(key)
  } // if ((modifier.bmLeftAlt) || (modifier.bmRightAlt))

  if ((modifier.bmLeftCtrl) || (modifier.bmRightCtrl))
  {
#ifdef DEBUG_USB_KEYBOARD
    debug_serial_port->print(F("KbdRptParser::OnKeyDown: CTRL-"));
    debug_serial_port->println(keystroke);
#endif // DEBUG_USB_KEYBOARD
    switch (key)
    {
    case 0x04: // CTRL-A
      configuration.keyer_mode = IAMBIC_A;
#ifdef FEATURE_DISPLAY
      lcd_center_print_timed("Iambic A", 0, default_display_msg_delay);
#endif
      config_dirty = 1;
      break;

    case 0x05: // CTRL-B
      configuration.keyer_mode = IAMBIC_B;
#ifdef FEATURE_DISPLAY
      lcd_center_print_timed("Iambic B", 0, default_display_msg_delay);
#endif
      config_dirty = 1;
      break;

    case 0x06: // CTRL-C
      configuration.keyer_mode = SINGLE_PADDLE;
#ifdef FEATURE_DISPLAY
      if (LCD_COLUMNS < 9)
      {
        lcd_center_print_timed("SnglePdl", 0, default_display_msg_delay);
      }
      else
      {
        lcd_center_print_timed("Single Paddle", 0, default_display_msg_delay);
      }
#endif
      config_dirty = 1;
      break;

#ifndef OPTION_NO_ULTIMATIC
    case 0x07: // CTRL-D
      configuration.keyer_mode = ULTIMATIC;
#ifdef FEATURE_DISPLAY
      if (LCD_COLUMNS < 9)
      {
        lcd_center_print_timed("Ultimatc", 0, default_display_msg_delay);
      }
      else
      {
        lcd_center_print_timed("Ultimatic", 0, default_display_msg_delay);
      }
#endif
      config_dirty = 1;
      break;
#endif

    case 0x08: // CTRL-E
#ifdef FEATURE_DISPLAY
      if (LCD_COLUMNS < 9)
      {
        lcd_center_print_timed("Entr SN", 0, default_display_msg_delay);
      }
      else
      {
        lcd_center_print_timed("Enter Serial #", 0, default_display_msg_delay);
      }
#else
      boop_beep();
#endif
      usb_keyboard_mode = USB_KEYBOARD_SN_ENTRY;
      user_num_input_places = 4;
      user_num_input_lower_limit = 0;
      user_num_input_upper_limit = 10000;
      usb_keyboard_special_mode_start_time = millis();
      break;

    case 0x0a: // CTRL-G
      configuration.keyer_mode = BUG;
#ifdef FEATURE_DISPLAY
      lcd_center_print_timed("Bug", 0, default_display_msg_delay);
#endif
      config_dirty = 1;
      break;

    case 0x0b: // CTRL-H
#ifdef FEATURE_HELL
      if (char_send_mode == CW)
      {
        char_send_mode = HELL;
        beep();
      }
      else
      {
        char_send_mode = CW;
        beep();
      }
#endif // FEATURE_HELL
      break;

    case 0x0c: // CTRL-I
      if (key_tx && keyer_machine_mode != KEYER_COMMAND_MODE)
      { // Added check that keyer is NOT in command mode or keyer might be enabled for paddle commands (WD9DMP-1)
        key_tx = 0;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("TX Off", 0, default_display_msg_delay);
#endif
      }
      else if (!key_tx && keyer_machine_mode != KEYER_COMMAND_MODE)
      { // Added check that keyer is NOT in command mode or keyer might be enabled for paddle commands (WD9DMP)
        key_tx = 1;
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("TX On", 0, default_display_msg_delay);
#endif
      }
      break;

    case 0x10: // CTRL-M
#ifdef FEATURE_FARNSWORTH
#ifdef FEATURE_DISPLAY
      if (LCD_COLUMNS < 9)
      {
        lcd_center_print_timed("Frns WPM", 0, default_display_msg_delay);
      }
      else
      {
        lcd_center_print_timed("Farnsworth WPM", 0, default_display_msg_delay);
      }
#else
      boop_beep();
#endif
      usb_keyboard_mode = USB_KEYBOARD_FARNS_WPM_ADJUST;
      user_num_input_places = 3;
      user_num_input_lower_limit = -1;
      user_num_input_upper_limit = 1000;
      usb_keyboard_special_mode_start_time = millis();
#endif

      break;

    case 0x11: // CTRL-N
      if (configuration.paddle_mode == PADDLE_NORMAL)
      {
        configuration.paddle_mode = PADDLE_REVERSE;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Pdl Rev", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Paddle Reverse", 0, default_display_msg_delay);
        }
#endif
      }
      else
      {
        configuration.paddle_mode = PADDLE_NORMAL;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("Pdl Norm", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Paddle Normal", 0, default_display_msg_delay);
        }
#endif
      }
      config_dirty = 1;
      break;

    case 0x12: // CTRL-O - cycle through sidetone modes on, paddle only and off - enhanced by Marc-Andre, VE2EVN
      if (configuration.sidetone_mode == SIDETONE_PADDLE_ONLY)
      {
        configuration.sidetone_mode = SIDETONE_OFF;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("ST Off", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Sidetone Off", 0, default_display_msg_delay);
        }
#endif
        boop();
      }
      else if (configuration.sidetone_mode == SIDETONE_ON)
      {
        configuration.sidetone_mode = SIDETONE_PADDLE_ONLY;
        beep();
        delay(200);
        beep();
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("ST Pdl O", 0, default_display_msg_delay);
        }
        if (LCD_COLUMNS > 19)
        {
          lcd_center_print_timed("Sidetone Paddle Only", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Sidetone", 0, default_display_msg_delay);
          lcd_center_print_timed("Paddle Only", 1, default_display_msg_delay);
        }
#endif
      }
      else
      {
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("ST On", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Sidetone On", 0, default_display_msg_delay);
        }
#endif
        configuration.sidetone_mode = SIDETONE_ON;
        beep();
      }
      config_dirty = 1;
      break;

#if defined(FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING)
    case 0x16: // CTRL-S
      if (configuration.cmos_super_keyer_iambic_b_timing_on)
      {
        configuration.cmos_super_keyer_iambic_b_timing_on = 0;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("CMOS Off", 0, default_display_msg_delay);
        }
        if (LCD_COLUMNS > 18)
        {
          lcd_center_print_timed("CMOS Superkeyer Off", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("CMOS SK Off", 0, default_display_msg_delay);
        }
#endif
      }
      else
      {
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("CMOS On", 0, default_display_msg_delay);
        }
        if (LCD_COLUMNS > 17)
        {
          lcd_center_print_timed("CMOS Superkeyer On", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("CMOS SK On", 0, default_display_msg_delay);
        }
#endif
        configuration.cmos_super_keyer_iambic_b_timing_on = 1;
      }
      config_dirty = 1;
      break;
#endif // FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING

    case 0x17: // CTRL-T
#ifdef FEATURE_MEMORIES
      repeat_memory = 255;
#endif
      if (keyboard_tune_on)
      {
        sending_mode = MANUAL_SENDING;
        tx_and_sidetone_key(0);
        keyboard_tune_on = 0;
#ifdef FEATURE_DISPLAY
        lcd_status = LCD_REVERT;
#endif // FEATURE_DISPLAY
      }
      else
      {
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("Tune", 0, default_display_msg_delay);
#endif
        sending_mode = MANUAL_SENDING;
        tx_and_sidetone_key(1);
        keyboard_tune_on = 1;
      }
      break;

    case 0x18: // CTRL-U
      if (ptt_line_activated)
      {
        manual_ptt_invoke = 0;
        ptt_unkey();
#ifdef FEATURE_DISPLAY
        lcd_status = LCD_REVERT;
#endif // FEATURE_DISPLAY
      }
      else
      {
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("PTTInvke", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("PTT Invoke", 0, default_display_msg_delay);
        }
#endif
        manual_ptt_invoke = 1;
        ptt_key();
      }
      break;

    case 0x1a: // CTRL-W
#ifdef FEATURE_DISPLAY
      if (LCD_COLUMNS < 9)
      {
        lcd_center_print_timed("WPM Adj", 0, default_display_msg_delay);
      }
      else
      {
        lcd_center_print_timed("WPM Adjust", 0, default_display_msg_delay);
      }
#else
      boop_beep();
#endif
      usb_keyboard_mode = USB_KEYBOARD_WPM_ADJUST;
      user_num_input_places = 3;
      user_num_input_lower_limit = 0;
      user_num_input_upper_limit = 1000;
      usb_keyboard_special_mode_start_time = millis();
      break;

    case 0x3a: // CTRL-F1
      switch_to_tx_silent(1);
#ifdef FEATURE_DISPLAY
      lcd_center_print_timed("TX 1", 0, default_display_msg_delay);
#endif
      break;

    case 0x3b: // CTRL-F2
      if ((ptt_tx_2) || (tx_key_line_2))
      {
        switch_to_tx_silent(2);
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("TX 2", 0, default_display_msg_delay);
#endif
      }
      break;

    case 0x3c: // CTRL-F3
      if ((ptt_tx_3) || (tx_key_line_3))
      {
        switch_to_tx_silent(3);
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("TX 3", 0, default_display_msg_delay);
#endif
      }
      break;

    case 0x3d: // CTRL-F4
      if ((ptt_tx_4) || (tx_key_line_4))
      {
        switch_to_tx_silent(4);
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("TX 4", 0, default_display_msg_delay);
#endif
      }
      break;

    case 0x3e: // CTRL-F5
      if ((ptt_tx_5) || (tx_key_line_5))
      {
        switch_to_tx_silent(5);
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("TX 5", 0, default_display_msg_delay);
#endif
      }
      break;

    case 0x3f: // CTRL-F6
      if ((ptt_tx_6) || (tx_key_line_6))
      {
        switch_to_tx_silent(6);
#ifdef FEATURE_DISPLAY
        lcd_center_print_timed("TX 6", 0, default_display_msg_delay);
#endif
      }
      break;

#ifdef FEATURE_AUTOSPACE
    case 0x1d: // CTRL-Z
      if (configuration.autospace_active)
      {
        configuration.autospace_active = 0;
        config_dirty = 1;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("AutoSOff", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Autospace Off", 0, default_display_msg_delay);
        }
#endif
      }
      else
      {
        configuration.autospace_active = 1;
        config_dirty = 1;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("AutoS On", 0, default_display_msg_delay);
        }
        else
        {
          lcd_center_print_timed("Autospace On", 0, default_display_msg_delay);
        }
#endif
      }
      break;
#endif
    } // switch(keystroke)
    return;
  } // if ((modifier.bmLeftCtrl) || (modifier.bmRightCtrl))

  // special keys with no modifiers
  switch (key)
  {
  case 0x4b:
  case 0x61:
    sidetone_adj(20);
    return;
    break;
  case 0x4e:
  case 0x5b:
    sidetone_adj(-20);
    return;
    break;
  case 0x4f:
  case 0x5e:
    adjust_dah_to_dit_ratio(int(configuration.dah_to_dit_ratio / 10));
    return;
    break;
  case 0x50:
  case 0x5c:
    adjust_dah_to_dit_ratio(-1 * int(configuration.dah_to_dit_ratio / 10));
    return;
    break;
  case 0x52:
  case 0x60:
    speed_set(configuration.wpm + 1);
    return;
    break;
  case 0x51:
  case 0x5a:
    speed_set(configuration.wpm - 1);
    return;
    break;
  case 0x4a:
  case 0x5f: // HOME
    configuration.dah_to_dit_ratio = initial_dah_to_dit_ratio;
    key_tx = 1;
    config_dirty = 1;
#ifdef FEATURE_DISPLAY
#ifdef OPTION_MORE_DISPLAY_MSGS
    if (LCD_COLUMNS < 9)
    {
      lcd_center_print_timed("DfltRtio", 0, default_display_msg_delay);
    }
    else
    {
      lcd_center_print_timed("Default ratio", 0, default_display_msg_delay);
    }
    service_display();
#endif
#endif
    return;
    break;
  case 0x2b:
  case 0x48: // TAB, PAUSE
    if (pause_sending_buffer)
    {
      pause_sending_buffer = 0;
#ifdef FEATURE_DISPLAY
#ifdef OPTION_MORE_DISPLAY_MSGS
      lcd_center_print_timed("Resume", 0, default_display_msg_delay);
#endif
#endif
    }
    else
    {
      pause_sending_buffer = 1;
#ifdef FEATURE_DISPLAY
      lcd_center_print_timed("Pause", 0, default_display_msg_delay);
#endif
    }
    return;
    break; // pause

  case 0x47: // SCROLL - Prosign next two characters
    usb_keyboard_prosign_flag = 1;
#ifdef FEATURE_DISPLAY
#ifdef OPTION_MORE_DISPLAY_MSGS
    lcd_center_print_timed("Prosign", 0, default_display_msg_delay);
#endif
#endif
    return;
    break;

  case 0x46:
    if (send_buffer_bytes)
    {
      send_buffer_bytes--;
    }
    return;
    break;   // DEL
  case 0x29: // ESC - clear the serial send buffer and a bunch of other stuff
    if (manual_ptt_invoke)
    {
      manual_ptt_invoke = 0;
      ptt_unkey();
    }
    if (keyboard_tune_on)
    {
      sending_mode = MANUAL_SENDING;
      tx_and_sidetone_key(0);
      keyboard_tune_on = 0;
    }
    if (pause_sending_buffer)
    {
      pause_sending_buffer = 0;
    }
    clear_send_buffer();
#ifdef FEATURE_MEMORIES
    // clear_memory_button_buffer();
    play_memory_prempt = 1;
    repeat_memory = 255;
#endif
#ifdef FEATURE_DISPLAY
    lcd_center_print_timed("Abort", 0, default_display_msg_delay);
#endif
    return;
    break;

  case 0x49:
  case 0x62: // INSERT - send serial number and increment
    put_serial_number_in_send_buffer();
    serial_number++;
    return;
    break;

  case 0x4d:
  case 0x59: // END - send serial number no increment
    put_serial_number_in_send_buffer();
    return;
    break;

#ifdef FEATURE_MEMORIES
  case 0x3a:
    ps2_usb_keyboard_play_memory(0);
    return;
    break; // F1
  case 0x3b:
    ps2_usb_keyboard_play_memory(1);
    return;
    break;
  case 0x3c:
    ps2_usb_keyboard_play_memory(2);
    return;
    break;
  case 0x3d:
    ps2_usb_keyboard_play_memory(3);
    return;
    break;
  case 0x3e:
    ps2_usb_keyboard_play_memory(4);
    return;
    break;
  case 0x3f:
    ps2_usb_keyboard_play_memory(5);
    return;
    break;
  case 0x40:
    ps2_usb_keyboard_play_memory(6);
    return;
    break;
  case 0x41:
    ps2_usb_keyboard_play_memory(7);
    return;
    break;
  case 0x42:
    ps2_usb_keyboard_play_memory(8);
    return;
    break;
  case 0x43:
    ps2_usb_keyboard_play_memory(9);
    return;
    break;
  case 0x44:
    ps2_usb_keyboard_play_memory(10);
    return;
    break;
  case 0x45:
    ps2_usb_keyboard_play_memory(11);
    return;
    break;
#endif

  } // switch(key)

  // regular keys
  if (keystroke)
  {
    if ((keystroke > 31) && (keystroke < 123))
    {
      if (usb_keyboard_prosign_flag)
      {
        add_to_send_buffer(SERIAL_SEND_BUFFER_PROSIGN);
        usb_keyboard_prosign_flag = 0;
      }
      keystroke = uppercase(keystroke);
      add_to_send_buffer(keystroke);
#ifdef FEATURE_MEMORIES
      repeat_memory = 255;
#endif
    }
  } // if (keystroke)

  // have we been in a special mode too long?
  if ((usb_keyboard_mode != USB_KEYBOARD_NORMAL) && ((millis() - usb_keyboard_special_mode_start_time) > USB_KEYBOARD_SPECIAL_MODE_TIMEOUT))
  {
    usb_keyboard_mode = USB_KEYBOARD_NORMAL;
    user_input_index = 0;
#ifdef DEBUG_USB_KEYBOARD
    debug_serial_port->println(F("KbdRptParser::OnKeyDown: usb_keyboard_mode timeout"));
#endif // DEBUG_USB_KEYBOARD
    return;
  }
}
#endif // FEATURE_USB_KEYBOARD

//---------------------------------------------------------------------
#ifdef FEATURE_USB_KEYBOARD
void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{

  // grab the keypad / and * for dit and dah paddling
  if (key == 0x54)
  {
    usb_dit = 0;
    return;
  }
  if (key == 0x55)
  {
    usb_dah = 0;
    return;
  }
  if (key == 0x58)
  {
    sending_mode = MANUAL_SENDING;
    tx_and_sidetone_key(0);
    return;
  }
}
#endif // FEATURE_USB_KEYBOARD

//---------------------------------------------------------------------

void initialize_usb()
{

#if defined(FEATURE_USB_KEYBOARD) || defined(FEATURE_USB_MOUSE)
  if (Usb.Init() == -1)
  {
#ifdef DEBUG_USB
    debug_serial_port->println(F("\rinitialize_usb: OSC did not start."));
#endif // DEBUG_USB
    return;
  }
  else
  {
#ifdef DEBUG_USB
    debug_serial_port->println(F("\rinitialize_usb: initializing"));
#endif // DEBUG_USB
  }
  delay(200);
  next_time = millis() + 5000;
#endif // (FEATURE_USB_KEYBOARD) || defined(FEATURE_USB_MOUSE)

#ifdef FEATURE_USB_KEYBOARD
  HidKeyboard.SetReportParser(0, (HIDReportParser *)&KeyboardPrs);
#endif // FEATURE_USB_KEYBOARD

#ifdef FEATURE_USB_MOUSE
  HidMouse.SetReportParser(0, (HIDReportParser *)&MousePrs);
#endif // FEATURE_USB_MOUSE

#if defined(FEATURE_USB_KEYBOARD) || defined(FEATURE_USB_MOUSE)
  unsigned long start_init = millis();
  while ((millis() - start_init) < 2000)
  {
    Usb.Task();
  }
#ifdef DEBUG_USB
  debug_serial_port->println(F("initialize_usb: initialized"));
#endif // DEBUG_USB
#endif // (FEATURE_USB_KEYBOARD) || defined(FEATURE_USB_MOUSE)
}
//---------------------------------------------------------------------
#if defined(FEATURE_USB_KEYBOARD) || defined(FEATURE_USB_MOUSE)
void service_usb()
{

  Usb.Task();
}
#endif // FEATURE_USB_KEYBOARD || FEATURE_USB_MOUSE

//---------------------------------------------------------------------
#ifdef FEATURE_USB_MOUSE
void MouseRptParser::OnMouseMove(MOUSEINFO *mi)
{

  /*
  debug_serial_port->print("dx=");
  debug_serial_port->print(mi->dX, DEC);
  debug_serial_port->print(" dy=");
  debug_serial_port->println(mi->dY, DEC);
  */

  /* this is just me fooling around */

#ifdef OPTION_MOUSE_MOVEMENT_PADDLE
  static int last_dX = 0;
  static int last_dY = 0;
  int current_dX = (mi->dX);
  int current_dY = (mi->dY);

  /* X/Y method - doesn't work too well
  if ((current_dX != last_dX) && (abs(current_dX) > abs(current_dY)) && (abs(current_dX) > 3)){
    dit_buffer = 1;
  }
  if ((current_dY != last_dY)  && (abs(current_dY) > abs(current_dX))  && (abs(current_dY) > 3)){
    dah_buffer = 1;
  }
  */

  /* X only method */
  if ((current_dX != last_dX) && (abs(current_dX) > 8))
  {
    if (current_dX < 0)
    {
      dit_buffer = 1;
    }
    else
    {
      dah_buffer = 1;
    }
  }

  last_dX = current_dX;
  last_dY = current_dY;
#endif // OPTION_MOUSE_MOVEMENT_PADDLE
};

void MouseRptParser::OnLeftButtonUp(MOUSEINFO *mi)
{
  usb_dit = 0;
};
void MouseRptParser::OnLeftButtonDown(MOUSEINFO *mi)
{
  usb_dit = 1;
};
void MouseRptParser::OnRightButtonUp(MOUSEINFO *mi)
{
  usb_dah = 0;
};
void MouseRptParser::OnRightButtonDown(MOUSEINFO *mi)
{
  usb_dah = 1;
};
void MouseRptParser::OnMiddleButtonUp(MOUSEINFO *mi)
{
  sending_mode = MANUAL_SENDING;
  tx_and_sidetone_key(0);
};
void MouseRptParser::OnMiddleButtonDown(MOUSEINFO *mi)
{
  sending_mode = MANUAL_SENDING;
  tx_and_sidetone_key(1);
};
#endif // FEATURE_USB_MOUSE
//---------------------------------------------------------------------

#ifdef FEATURE_CAPACITIVE_PADDLE_PINS
uint8_t read_capacitive_pin(int pinToMeasure)
{

  /*

  This code is from http://playground.arduino.cc/Code/CapacitiveSensor

  Original code by Mario Becker, Fraunhofer IGD, 2007 http://www.igd.fhg.de/igd-a4

  Updated by: Alan Chatham http://unojoy.tumblr.com

  Updated by Paul Stoffregen: Replaced '328 specific code with portOutputRegister, etc for compatibility with Arduino Mega, Teensy, Sanguino and other boards

  Gratuitous optimization to improve sensitivity by Casey Rodarmor.

  */

  // Variables used to translate from Arduino to AVR pin naming

  volatile uint8_t *port;
  volatile uint8_t *ddr;
  volatile uint8_t *pin;

  // Here we translate the input pin number from
  //  Arduino pin number to the AVR PORT, PIN, DDR,
  //  and which bit of those registers we care about.

  byte bitmask;
  port = portOutputRegister(digitalPinToPort(pinToMeasure));
  ddr = portModeRegister(digitalPinToPort(pinToMeasure));
  bitmask = digitalPinToBitMask(pinToMeasure);
  pin = portInputRegister(digitalPinToPort(pinToMeasure));
  // Discharge the pin first by setting it low and output
  *port &= ~(bitmask);
  *ddr |= bitmask;
  delay(1);
  // Prevent the timer IRQ from disturbing our measurement
  noInterrupts();
  // Make the pin an input with the internal pull-up on
  *ddr &= ~(bitmask);
  *port |= bitmask;

  // Now see how long the pin to get pulled up. This manual unrolling of the loop
  // decreases the number of hardware cycles between each read of the pin,
  // thus increasing sensitivity.
  uint8_t cycles = 17;
  /*     if (*pin & bitmask) { cycles =  0;}
  else if (*pin & bitmask) { cycles =  1;}
  else if (*pin & bitmask) { cycles =  2;}
  else if (*pin & bitmask) { cycles =  3;}
  else if (*pin & bitmask) { cycles =  4;}
  else if (*pin & bitmask) { cycles =  5;}
  else if (*pin & bitmask) { cycles =  6;}
  else if (*pin & bitmask) { cycles =  7;}
  else if (*pin & bitmask) { cycles =  8;}
  else if (*pin & bitmask) { cycles =  9;}
  else if (*pin & bitmask) { cycles = 10;}
  else if (*pin & bitmask) { cycles = 11;}
  else if (*pin & bitmask) { cycles = 12;}
  else if (*pin & bitmask) { cycles = 13;}
  else if (*pin & bitmask) { cycles = 14;}
  else if (*pin & bitmask) { cycles = 15;}
  else if (*pin & bitmask) { cycles = 16;}*/

  if (*pin & bitmask)
  {
    cycles = 0;
  }
  else
  {
    if (*pin & bitmask)
    {
      cycles = 1;
    }
    else
    {
      if (*pin & bitmask)
      {
        cycles = 2;
      }
      else
      {
        if (*pin & bitmask)
        {
          cycles = 3;
        }
        else
        {
          if (*pin & bitmask)
          {
            cycles = 4;
          }
          else
          {
            if (*pin & bitmask)
            {
              cycles = 5;
            }
            else
            {
              if (*pin & bitmask)
              {
                cycles = 6;
              }
              else
              {
                if (*pin & bitmask)
                {
                  cycles = 7;
                }
                else
                {
                  if (*pin & bitmask)
                  {
                    cycles = 8;
                  }
                  else
                  {
                    if (*pin & bitmask)
                    {
                      cycles = 9;
                    }
                    else
                    {
                      if (*pin & bitmask)
                      {
                        cycles = 10;
                      }
                      else
                      {
                        if (*pin & bitmask)
                        {
                          cycles = 11;
                        }
                        else
                        {
                          if (*pin & bitmask)
                          {
                            cycles = 12;
                          }
                          else
                          {
                            if (*pin & bitmask)
                            {
                              cycles = 13;
                            }
                            else
                            {
                              if (*pin & bitmask)
                              {
                                cycles = 14;
                              }
                              else
                              {
                                if (*pin & bitmask)
                                {
                                  cycles = 15;
                                }
                                else
                                {
                                  if (*pin & bitmask)
                                  {
                                    cycles = 16;
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // End of timing-critical section
  interrupts();

  // Discharge the pin again by setting it low and output
  //  It's important to leave the pins low if you want to
  //  be able to touch more than 1 sensor at a time - if
  //  the sensor is left pulled high, when you touch
  //  two sensors, your body will transfer the charge between
  //  sensors.
  *port &= ~(bitmask);
  *ddr |= bitmask;

#ifdef DEBUG_CAPACITIVE_PADDLE
  static unsigned long last_cap_paddle_debug = 0;
  if ((millis() - last_cap_paddle_debug) > 250)
  {
    debug_serial_port->flush();
    debug_serial_port->print("read_capacitive_pin: pin:");
    debug_serial_port->print(pinToMeasure);
    debug_serial_port->print(" cyc:");
    debug_serial_port->println(cycles);
    last_cap_paddle_debug = millis();
  }
#endif // DEBUG_CAPACITIVE_PADDLE

  return cycles;
}

#endif // FEATURE_CAPACITIVE_PADDLE_PINS

//---------------------------------------------------------------------
#ifdef FEATURE_LED_RING
void update_led_ring()
{

  static int last_leds = 0;
  int leds = 0;

  leds = map(configuration.wpm, led_ring_low_limit, led_ring_high_limit, 0, 15);
  if (leds < 0)
  {
    leds = 0;
  }
  if (leds > 15)
  {
    leds = 15;
  }

  if (leds != last_leds)
  {
    digitalWrite(led_ring_le, LOW);

    digitalWrite(led_ring_sdi, LOW);
    digitalWrite(led_ring_clk, HIGH);
    digitalWrite(led_ring_clk, LOW);

    for (int x = 15; x > 0; x--)
    {
      if (x <= leds)
      {
        digitalWrite(led_ring_sdi, HIGH);
      }
      else
      {
        digitalWrite(led_ring_sdi, LOW);
      }
      digitalWrite(led_ring_clk, HIGH);
      digitalWrite(led_ring_clk, LOW);
    }

    // shiftOut(led_ring_sdi,led_ring_clk,MSBFIRST,(sequence[y][x] >> 8));    //High byte first
    // shiftOut(led_ring_sdi,led_ring_clk,MSBFIRST,sequence[y][x]);           //Low byte second
    digitalWrite(led_ring_le, HIGH);

    last_leds = leds;
    digitalWrite(led_ring_sdi, LOW);
  }
}
#endif // FEATURE_LED_RING
//---------------------------------------------------------------------

#ifdef FEATURE_ALPHABET_SEND_PRACTICE
void command_alphabet_send_practice()
{

  // contributed by Ryan, KC2ZWM

  int cw_char;
  char letter = 'A';

  do
  {
    cw_char = get_cw_input_from_user(0);
    if (letter == (char)(convert_cw_number_to_ascii(cw_char)))
    {
      if (correct_answer_led)
      {
        digitalWrite(correct_answer_led, HIGH);
      }
      if (wrong_answer_led)
      {
        digitalWrite(wrong_answer_led, LOW);
      }
      beep();

      // send_dit();
      if (letter < 'Z')
        letter++;
      else
        letter = 'A';
    }
    else if (cw_char != 9)
    {
      if (wrong_answer_led)
      {
        digitalWrite(wrong_answer_led, HIGH);
      }
      if (correct_answer_led)
      {
        digitalWrite(correct_answer_led, LOW);
      }
      boop();
      boop();
      // send_dah();
    }
  } while (cw_char != 9);

  if (correct_answer_led)
  {
    digitalWrite(correct_answer_led, LOW);
  }
  if (wrong_answer_led)
  {
    digitalWrite(wrong_answer_led, LOW);
  }
}
#endif // FEATURE_ALPHABET_SEND_PRACTICE

//---------------------------------------------------------------------
#ifdef FEATURE_PTT_INTERLOCK
void service_ptt_interlock()
{

  static unsigned long last_ptt_interlock_check = 0;

  if ((millis() - last_ptt_interlock_check) > ptt_interlock_check_every_ms)
  {
    if (digitalRead(ptt_interlock) == ptt_interlock_active_state)
    {
      if (!ptt_interlock_active)
      {
        ptt_interlock_active = 1;
#ifdef FEATURE_DISPLAY
        if (LCD_COLUMNS < 9)
        {
          lcd_center_print_timed("PTT Lock", 0, 2000);
        }
        else
        {
          lcd_center_print_timed("PTT Interlock", 0, 2000);
        }
#endif // FEATURE_DISPLAY
      }
    }
    else
    {
      if (ptt_interlock_active)
      {
        ptt_interlock_active = 0;
      }
    }
    last_ptt_interlock_check = millis();
  }
}
#endif // FEATURE_PTT_INTERLOCK

//---------------------------------------------------------------------

void initialize_ethernet_variables()
{

#if defined(FEATURE_ETHERNET)
  for (int x = 0; x < 4; x++)
  {
    configuration.ip[x] = default_ip[x];
    configuration.gateway[x] = default_gateway[x];
    configuration.subnet[x] = default_subnet[x];
    for (int y = 0; y < FEATURE_INTERNET_LINK_MAX_LINKS; y++)
    {
      configuration.link_send_ip[x][y] = 0;
      configuration.link_send_enabled[y] = 0;
      configuration.link_send_udp_port[y] = FEATURE_INTERNET_LINK_DEFAULT_RCV_UDP_PORT;
    }
  }
  configuration.link_receive_udp_port = FEATURE_INTERNET_LINK_DEFAULT_RCV_UDP_PORT;
  configuration.link_receive_enabled = 0;
#endif // FEATURE_ETHERNET
}

//-------------------------------------------------------------------------------------------------------
void initialize_ethernet()
{

#if defined(FEATURE_ETHERNET)
  Ethernet.begin(mac, configuration.ip, configuration.dns_server, configuration.gateway, configuration.subnet);
#endif
}

//-------------------------------------------------------------------------------------------------------

void initialize_udp()
{

#if defined(FEATURE_UDP)
  int udpbegin_result = Udp.begin(udp_listener_port);

#if defined(DEBUG_UDP)
  if (!udpbegin_result)
  {
    debug_serial_port->println("initialize_udp: Udp.begin error");
  }
#endif
#endif // FEATURE_UDP
}

//-------------------------------------------------------------------------------------------------------

void initialize_web_server()
{
#if defined(FEATURE_WEB_SERVER)

  server.begin();

#ifdef DEBUG_WEB_SERVER
  debug_serial_port->print(F("initialize_web_server: server is at "));
  debug_serial_port->println(Ethernet.localIP());
#endif

#endif // FEATURE_WEB_SERVER
}

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_ETHERNET)
void check_for_network_restart()
{

  if (restart_networking)
  {
    initialize_web_server();
    restart_networking = 0;
  }
}
#endif // FEATURE_ETHERNET

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void service_web_server()
{

  if ((web_control_tx_key_time > 0) && ((millis() - web_control_tx_key_time) > (WEB_SERVER_CONTROL_TX_KEY_TIME_LIMIT_SECS * 1000)))
  {
    tx_and_sidetone_key(0);
    web_control_tx_key_time = 0;
  }

  // Create a client connection
  EthernetClient client = server.available();
  if (client)
  {

    valid_request = 0;

    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();

        // read char by char HTTP request
        if (web_server_incoming_string.length() < MAX_WEB_REQUEST)
        {
          // store characters to string
          web_server_incoming_string += c;
#if defined(DEBUG_WEB_SERVER_READS)
          debug_serial_port->print("service_web_server: read: ");
          debug_serial_port->print(c);
#endif // DEBUG_WEB_SERVER_READS
        }
        else
        {
          // web_server_incoming_string = "";
        }

        // has HTTP request ended?
        if (c == '\n')
        {

#if defined(DEBUG_WEB_SERVER_READS)
          debug_serial_port->println(web_server_incoming_string); // print to serial monitor for debuging
#endif                                                            // DEBUG_WEB_SERVER_READS

          if (web_server_incoming_string.startsWith("GET / "))
          {
            valid_request = 1;
            web_print_page_main_menu(client);
          }

          if (web_server_incoming_string.startsWith("GET /About"))
          {
            valid_request = 1;
            web_print_page_about(client);
          }

          if (web_server_incoming_string.startsWith("GET /KeyerSettings"))
          {
            valid_request = 1;
            // are there form results being posted?
            if (web_server_incoming_string.indexOf("?") > 0)
            {
              web_print_page_keyer_settings_process(client);
            }
            else
            {
              web_print_page_keyer_settings(client);
            }
          }

          if (web_server_incoming_string.startsWith("GET /NetworkSettings"))
          {
            valid_request = 1;
            // are there form results being posted?
            if (web_server_incoming_string.indexOf("?ip0=") > 0)
            {
              web_print_page_network_settings_process(client);
            }
            else
            {
              web_print_page_network_settings(client);
            }
          }

#if defined(FEATURE_INTERNET_LINK)
          if (web_server_incoming_string.startsWith("GET /LinkSettings"))
          {
            valid_request = 1;
            // are there form results being posted?
            if (web_server_incoming_string.indexOf("?ip") > 0)
            {
              web_print_page_link_settings_process(client);
            }
            else
            {
              web_print_page_link_settings(client);
            }
          }
#endif // FEATURE_INTERNET_LINK
          if (web_server_incoming_string.startsWith("GET /ctrl"))
          {
            valid_request = 1;
            web_print_page_control(client);
          }

#if defined(FEATURE_MEMORIES)
          if (web_server_incoming_string.startsWith("GET /mem"))
          {
            valid_request = 1;
            // are there form results being posted?
            // if (web_server_incoming_string.indexOf("?") > 0){
            //   web_print_page_memories_process(client);
            // } else {
            web_print_page_memories(client);
            // }
          }
#endif // FEATURE_MEMORIES

          if (!valid_request)
          {
            web_print_page_404(client);
          }

          delay(1);
          client.stop();
          web_server_incoming_string = "";
        }
      }
    }
  }
}
#endif // FEATURE_WEB_SERVER
//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)
void web_print_200OK(EthernetClient client)
{

  web_client_print(client, F("HTTP/1.1 200 OK\nContent-Type: text/html\n\n"));
}
#endif // FEATURE_WEB_SERVER
//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)
void web_print_header(EthernetClient client)
{

  web_print_200OK(client);
  web_client_println(client, F("<HTML><HEAD><meta name='apple-mobile-web-app-capable' content='yes' /><meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />"));
}
#endif // FEATURE_WEB_SERVER
//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)
void web_print_style_sheet(EthernetClient client)
{

  web_client_print(client, F("<style>body{margin:60px 0px; padding:0px;text-align:center;font-family:\"Trebuchet MS\", Arial, Helvetica, sans-serif;}h1{text-align: center;font-family:Arial, \"Trebuchet MS\", Helvetica,"));
  web_client_print(client, F("sans-serif;}h2{text-align: center;font-family:\"Trebuchet MS\", Arial, Helvetica, sans-serif;}"));

  // internal hyperlinks
  web_client_print(client, F("a.internal{text-decoration:none;width:75px;height:50px;border-color:black;border-top:2px solid;border-bottom:2px solid;border-right:2px solid;border-left:2px solid;border-radius:10px 10px 10px;"));
  web_client_print(client, F("-o-border-radius:10px 10px 10px;-webkit-border-radius:10px 10px 10px;font-family:\"Trebuchet MS\",Arial, Helvetica, sans-serif;"));
  web_client_print(client, F("-moz-border-radius:10px 10px 10px;background-color:#293F5E;padding:8px;text-align:center;}"));
  web_client_print(client, F("a.internal:link {color:white;} a.internal:visited {color:white;}"));
  web_client_print(client, F(" a.internal:hover {color:white;} a.internal:active {color:white;}"));

  // control screen buttons
  web_client_print(client, F("a.ctrl{text-decoration:none;width:75px;height:50px;border-color:black;border-top:2px solid;border-bottom:2px solid;border-right:2px solid;border-left:2px solid;border-radius:10px 10px 10px;"));
  web_client_print(client, F("-o-border-radius:10px 10px 10px;-webkit-border-radius:10px 10px 10px;font-family:\"Trebuchet MS\",Arial, Helvetica, sans-serif;"));
  web_client_print(client, F("-moz-border-radius:10px 10px 10px;background-color:#293F5E;padding:15px;text-align:center;}"));
  web_client_print(client, F("a.ctrl:link {color:white;} a.ctrl:visited {color:white;}"));
  web_client_print(client, F(" a.ctrl:hover {color:white;} a.ctrl:active {color:white;}"));

  // external hyperlinks
  web_client_print(client, F("a.external{"));
  web_client_print(client, F("font-family:\"Trebuchet MS\",Arial, Helvetica, sans-serif;"));
  web_client_print(client, F("text-align:center;}"));
  web_client_print(client, F("a.external:link {color:blue;} a.external:visited {color:purple;}"));
  web_client_println(client, F(" a.external:hover {color:red;} a.external:active {color:green;}"));

  // ip address text blocks
  web_client_println(client, F(".addr {width: 30px; text-align:center }"));

  // ip port text blocks
  web_client_println(client, F(".ipprt {width: 45px; text-align:center }"));

  web_client_println(client, F(".container {display: flex;}"));

  /*for demo purposes only */
  // web_client_println(client,F(".column {flex: 1; background: #f2f2f2; border: 1px solid #e6e6e6; box-sizing: border-box;}"));
  // web_client_println(client,F(".column-1 {order: 1;} .column-2 {order: 2;} .column-3 {order: 3;} .column-4 {order: 4;} .column-5 {order: 5;}"));

  web_client_println(client, F("</style>"));
}
#endif // FEATURE_WEB_SERVER
//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)
void web_print_home_link(EthernetClient client)
{

  web_client_println(client, F("<br><a href=\"\x2F\" class=\"internal\">Home</a><br />"));
}
#endif // FEATURE_WEB_SERVER
//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)
void web_print_footer(EthernetClient client)
{

  web_client_println(client, F("<br></BODY></HTML>"));
}
#endif // FEATURE_WEB_SERVER
//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)
void web_print_title(EthernetClient client)
{

  web_client_println(client, F("<TITLE>K3NG CW Keyer</TITLE></HEAD><BODY>"));
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)

void web_print_page_network_settings(EthernetClient client)
{

  web_print_header(client);

  web_print_style_sheet(client);

  web_print_title(client);

  web_client_println(client, F("<H1>Network Settings</H1><hr><br>"));

  // input form
  web_client_print(client, F("<br><br><form>IP: <input type=\"text\" name=\"ip0\" class=\"addr\" value=\""));
  web_client_print(client, configuration.ip[0]);
  web_client_print(client, F("\">.<input type=\"text\" name=\"ip1\" class=\"addr\" value=\""));
  web_client_print(client, configuration.ip[1]);
  web_client_print(client, F("\">.<input type=\"text\" name=\"ip2\" class=\"addr\" value=\""));
  web_client_print(client, configuration.ip[2]);
  web_client_print(client, F("\">.<input type=\"text\" name=\"ip3\" class=\"addr\" value=\""));
  web_client_print(client, configuration.ip[3]);
  web_client_println(client, "\">");

  web_client_print(client, F("<br><br>Gateway: <input type=\"text\" name=\"gw0\" class=\"addr\" value=\""));
  web_client_print(client, configuration.gateway[0]);
  web_client_print(client, F("\">.<input type=\"text\" name=\"gw1\" class=\"addr\" value=\""));
  web_client_print(client, configuration.gateway[1]);
  web_client_print(client, F("\">.<input type=\"text\" name=\"gw2\" class=\"addr\" value=\""));
  web_client_print(client, configuration.gateway[2]);
  web_client_print(client, F("\">.<input type=\"text\" name=\"gw3\" class=\"addr\" value=\""));
  web_client_print(client, configuration.gateway[3]);
  web_client_println(client, "\">");

  web_client_print(client, F("<br><br>Subnet Mask: <input type=\"text\" name=\"sn0\" class=\"addr\" value=\""));
  web_client_print(client, configuration.subnet[0]);
  web_client_print(client, F("\">.<input type=\"text\" name=\"sn1\" class=\"addr\" value=\""));
  web_client_print(client, configuration.subnet[1]);
  web_client_print(client, F("\">.<input type=\"text\" name=\"sn2\" class=\"addr\" value=\""));
  web_client_print(client, configuration.subnet[2]);
  web_client_print(client, F("\">.<input type=\"text\" name=\"sn3\" class=\"addr\" value=\""));
  web_client_print(client, configuration.subnet[3]);
  web_client_println(client, "\"><br><br><input type=\"submit\" value=\"Save\"></form>");

  web_print_home_link(client);

  web_print_footer(client);
}

#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER) && defined(FEATURE_INTERNET_LINK)

void web_print_page_link_settings(EthernetClient client)
{

  web_print_header(client);

  web_print_style_sheet(client);

  web_print_title(client);

  web_client_println(client, F("<H1>Link Settings</H1><hr><br><H2>Link Send Settings</H2><form>"));

  // input form
  for (int x = 0; x < FEATURE_INTERNET_LINK_MAX_LINKS; x++)
  {
    web_client_print(client, "<br><br>Link ");
    web_client_print(client, x + 1);
    web_client_print(client, ": IP: <input type=\"text\" name=\"ip");
    web_client_print(client, x);
    web_client_print(client, "0\" class=\"addr\" value=\"");
    web_client_print(client, configuration.link_send_ip[0][x]);
    web_client_print(client, "\"");
    web_client_print(client, ">.<input type=\"text\" name=\"ip");
    web_client_print(client, x);
    web_client_print(client, "1\" class=\"addr\" value=\"");
    web_client_print(client, configuration.link_send_ip[1][x]);
    web_client_print(client, "\"");
    web_client_print(client, ">.<input type=\"text\" name=\"ip");
    web_client_print(client, x);
    web_client_print(client, "2\" class=\"addr\" value=\"");
    web_client_print(client, configuration.link_send_ip[2][x]);
    web_client_print(client, "\"");
    web_client_print(client, ">.<input type=\"text\" name=\"ip");
    web_client_print(client, x);
    web_client_print(client, "3\" class=\"addr\" value=\"");
    web_client_print(client, configuration.link_send_ip[3][x]);
    web_client_println(client, "\">");

    web_client_print(client, " UDP Port: <input type=\"text\" name=\"sp");
    web_client_print(client, x);
    web_client_print(client, "\" class=\"ipprt\" value=\"");
    web_client_print(client, configuration.link_send_udp_port[x]);
    web_client_print(client, "\">  ");

    // link active/inactive radio buttons
    web_client_print(client, "<input type=\"radio\" name=\"act");
    web_client_print(client, x);
    web_client_print(client, "\" value=\"1\" ");
    if (configuration.link_send_enabled[x])
    {
      web_client_print(client, "checked");
    }
    web_client_print(client, ">Enabled <input type=\"radio\" name=\"act");
    web_client_print(client, x);
    web_client_print(client, "\" value=\"0\" ");
    if (!(configuration.link_send_enabled[x]))
    {
      web_client_print(client, "checked");
    }
    web_client_print(client, ">Disabled<br>");
  }

  web_client_print(client, F("<br><br><H2>Link Receive Settings</H2><br><br>UDP Receive Port: <input type=\"text\" name=\"ud\" class=\"ipprt\" value=\""));
  web_client_print(client, configuration.link_receive_udp_port);
  web_client_println(client, "\">");

  web_client_print(client, "<input type=\"radio\" name=\"lr");
  web_client_print(client, "\" value=\"1\" ");
  if (configuration.link_receive_enabled)
  {
    web_client_print(client, "checked");
  }
  web_client_print(client, ">Enabled <input type=\"radio\" name=\"lr");
  web_client_print(client, "\" value=\"0\" ");
  if (!(configuration.link_receive_enabled))
  {
    web_client_print(client, "checked");
  }
  web_client_print(client, ">Disabled<br>");

  web_client_println(client, "<br><br><input type=\"submit\" value=\"Save\"></form>");

  web_print_home_link(client);

  web_print_footer(client);
}

#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)

void web_print_page_404(EthernetClient client)
{

  web_client_println(client, F("HTTP/1.1 404 NOT FOUND"));
  web_client_println(client, F("Content-Type: text/html\n"));
  web_client_println(client, F("<HTML><HEAD></HEAD><BODY>Sorry, dude.  Page not found."));
  web_print_home_link(client);
  web_print_footer(client);
}

#endif // FEATURE_WEB_SERVER
//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)

void web_print_page_about(EthernetClient client)
{

  web_print_header(client);

  web_client_println(client, F("<meta http-equiv=\"refresh\" content=\"5\"/>"));

  web_print_style_sheet(client);

  web_print_title(client);

  web_client_println(client, F("<H1>About</H1><hr><br>"));
  web_client_println(client, CODE_VERSION);
  web_client_println(client, "<br>");

#if !defined(HARDWARE_GENERIC_STM32F103C) && !defined(ARDUINO_RASPBERRY_PI_PICO_W) && !defined(ARDUINO_RASPBERRY_PI_PICO)
  void *HP = malloc(4);
  if (HP)
  {
    free(HP);
  }
  unsigned long free = (unsigned long)SP - (unsigned long)HP;

  // web_client_print(client,"Heap = 0x");
  // web_client_println(client,(unsigned long)HP,HEX);
  // web_client_println(client,"<br />");
  // web_client_print(client,"Stack = 0x");
  // web_client_println(client,(unsigned long)SP,HEX);
  // web_client_println(client,"<br />");

  web_client_print(client, free);
  web_client_println(client, F(" bytes free<br><br>"));
#endif

  unsigned long seconds = (millis() / 1000L) + ((pow(2, 32) / 1000L) * millis_rollover);

  int days = seconds / 86400L;
  seconds = seconds - (long(days) * 86400L);

  int hours = seconds / 3600L;
  seconds = seconds - (long(hours) * 3600L);

  int minutes = seconds / 60L;
  seconds = seconds - (minutes * 60);

  web_client_print(client, days);
  web_client_print(client, ":");
  if (hours < 10)
  {
    web_client_print(client, "0");
  }
  web_client_print(client, hours);
  web_client_print(client, ":");
  if (minutes < 10)
  {
    web_client_print(client, "0");
  }
  web_client_print(client, minutes);
  web_client_print(client, ":");
  if (seconds < 10)
  {
    web_client_print(client, "0");
  }
  web_client_print(client, seconds);
  web_client_println(client, F(" dd:hh:mm:ss uptime<br>"));

  web_client_println(client, F("<br><br><br>Anthony Good, K3NG<br>anthony.good@gmail.com<br><a href=\"http://blog.radioartisan.com/\"\" class=\"external\">Radio Artisan</a><br><br>"));

  web_print_home_link(client);

  web_print_footer(client);
}

#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)
void parse_get(String str)
{

  String workstring = "";
  String parameter = "";
  String value = "";

  for (int x = 0; x < MAX_PARSE_RESULTS; x++)
  {
    parse_get_results[x].parameter = "";
    parse_get_results[x].value_string = "";
    parse_get_results[x].value_long = 0;
  }
  parse_get_results_index = 0;

#if defined(DEBUG_WEB_PARSE_GET)
  debug_serial_port->print("parse_get: raw workstring: ");
  Serial.println(str);
#endif

  workstring = str.substring(str.indexOf("?") + 1);

#if defined(DEBUG_WEB_PARSE_GET)
  debug_serial_port->print("parse_get: workstring: ");
  Serial.println(workstring);
#endif

  while (workstring.indexOf("=") > 0)
  {
    parameter = workstring.substring(0, workstring.indexOf("="));
    if (workstring.indexOf("&") > 0)
    {
      value = workstring.substring(workstring.indexOf("=") + 1, workstring.indexOf("&"));
      workstring = workstring.substring(workstring.indexOf("&") + 1);
    }
    else
    {
      value = workstring.substring(workstring.indexOf("=") + 1, workstring.indexOf(" "));
      // value = workstring.substring(workstring.indexOf("=")+1);
      workstring = "";
    }
#if defined(DEBUG_WEB_PARSE_GET)
    debug_serial_port->print("parse_get: parameter: ");
    debug_serial_port->print(parameter);
    debug_serial_port->print(" value: ");
    debug_serial_port->println(value);
#endif // DEBUG_WEB_PARSE_GET

    if (parse_get_results_index < MAX_PARSE_RESULTS)
    {
      parse_get_results[parse_get_results_index].parameter = parameter;
      parse_get_results[parse_get_results_index].value_string = value;
      parse_get_results[parse_get_results_index].value_long = value.toInt();

      // Serial.print(parse_get_results_index);
      // Serial.print(":");
      // Serial.print(parse_get_results[parse_get_results_index].parameter);
      // Serial.print(":");
      // Serial.print(parse_get_results[parse_get_results_index].value_string);
      // Serial.print(":");
      // Serial.print(parse_get_results[parse_get_results_index].value_long);
      // Serial.println("$");

      parse_get_results_index++;
    }
  }
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)

void web_print_page_main_menu(EthernetClient client)
{

  web_print_header(client);

  web_print_style_sheet(client);

  web_print_title(client);

  web_client_println(client, F("<H1>K3NG CW Keyer</H1><hr><br><br><a href=\"ctrl\"\" class=\"internal\">Control</a><br><br>"));
#if defined(FEATURE_MEMORIES)
  web_client_println(client, F("<a href=\"mem\"\" class=\"internal\">Memories</a><br><br>"));
#endif // FEATURE_MEMORIES
  web_client_println(client, F("<a href=\"KeyerSettings\"\" class=\"internal\">Keyer Settings</a><br><br>"));
#if defined(FEATURE_INTERNET_LINK)
  web_client_println(client, F("<a href=\"LinkSettings\"\" class=\"internal\">Link Settings</a><br><br>"));
#endif // FEATURE_INTERNET_LINK
  web_client_println(client, F("<a href=\"NetworkSettings\"\" class=\"internal\">Network Settings</a><br><br><a href=\"About\"\" class=\"internal\">About</a><br>"));

  web_print_footer(client);
}

#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)

void web_print_control_radio(EthernetClient client, const char *name, int value, uint8_t checked, const char *caption)
{

  web_client_print(client, F("<label><input type=\"radio\" name=\""));
  web_client_print(client, name);
  web_client_print(client, F("\" value=\""));
  web_client_print(client, value);
  web_client_print(client, "\"");
  if (checked)
  {
    web_client_print(client, F(" checked"));
  }
  web_client_print(client, ">");
  web_client_print(client, caption);
  web_client_print(client, F("</label>"));
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)

void web_print_control_checkbox(EthernetClient client, const char *name, uint8_t checked, const char *caption)
{

  web_client_print(client, F("<label><input type=\"checkbox\" id=\"cbox"));
  web_client_print(client, name);
  web_client_print(client, F("\" value=\""));
  web_client_print(client, name);
  web_client_print(client, F("\""));
  if (checked)
  {
    web_client_print(client, F(" checked"));
  }
  web_client_print(client, F(">"));
  web_client_print(client, caption);
  web_client_print(client, F("</label>"));
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)

void web_print_control_textbox(EthernetClient client, const char *name, const char *textbox_class, int textbox_value, const char *front_caption, const char *back_caption)
{

  web_client_print(client, F("<label>"));
  web_client_print(client, front_caption);
  web_client_print(client, F("<input type=\"text\" name=\""));
  web_client_print(client, name);
  web_client_print(client, F("\" class=\""));
  web_client_print(client, textbox_class);
  web_client_print(client, F("\" value=\""));
  web_client_print(client, textbox_value);
  web_client_print(client, F("\">"));
  web_client_print(client, back_caption);
  web_client_print(client, F("</label>"));
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)

void web_print_control_textbox(EthernetClient client, const char *name, const char *textbox_class, float textbox_value, const char *front_caption, const char *back_caption)
{

  web_client_print(client, F("<label>"));
  web_client_print(client, front_caption);
  web_client_print(client, F("<input type=\"text\" name=\""));
  web_client_print(client, name);
  web_client_print(client, F("\" class=\""));
  web_client_print(client, textbox_class);
  web_client_print(client, F("\" value=\""));
  web_client_print(client, textbox_value);
  web_client_print(client, F("\">"));
  web_client_print(client, back_caption);
  web_client_print(client, F("</label>"));
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)

void web_print_page_keyer_settings(EthernetClient client)
{

  uint8_t pin_read = 0;

  web_print_header(client);

  web_print_style_sheet(client);

  web_print_title(client);

  web_client_println(client, F("<H1>Keyer Settings</H1><hr><br><form>"));

  web_print_control_radio(client, "md", IAMBIC_A, (configuration.keyer_mode == IAMBIC_A) ? 1 : 0, "Iambic A ");
  web_print_control_radio(client, "md", IAMBIC_B, (configuration.keyer_mode == IAMBIC_B) ? 1 : 0, "Iambic B ");
  web_print_control_radio(client, "md", BUG, (configuration.keyer_mode == BUG) ? 1 : 0, "Bug ");
  web_print_control_radio(client, "md", STRAIGHT, (configuration.keyer_mode == STRAIGHT) ? 1 : 0, "Straight Key");
#ifndef OPTION_NO_ULTIMATIC
  web_print_control_radio(client, "md", ULTIMATIC, (configuration.keyer_mode == ULTIMATIC) ? 1 : 0, "Ultimatic");
#endif
  web_print_control_radio(client, "md", SINGLE_PADDLE, (configuration.keyer_mode == SINGLE_PADDLE) ? 1 : 0, "Single Paddle");
  web_client_println(client, "<br>");

#ifdef FEATURE_CMOS_SUPER_KEYER_IAMBIC_B_TIMING
  web_print_control_checkbox(client, "cs", (configuration.cmos_super_keyer_iambic_b_timing_on) ? 1 : 0, " CMOS Superkeyer Iambic B Timing ");
  web_print_control_textbox(client, "cp", "addr", configuration.cmos_super_keyer_iambic_b_timing_percent, "", "%");
  web_client_println(client, "<br>");
#endif

  // web_print_control_checkbox(client,"di",(!configuration.dit_buffer_off)?1:0," Dit Buffer ");  // couldn't get checkboxes working correctly 2016-12-11

  web_client_print(client, "Dit Buffer");
  web_print_control_radio(client, "di", 0, (configuration.dit_buffer_off) ? 0 : 1, "On ");
  web_print_control_radio(client, "di", 1, (configuration.dit_buffer_off) ? 1 : 0, "Off   ");
  web_client_println(client, "<br>");

  // web_print_control_checkbox(client,"da",(!configuration.dah_buffer_off)?1:0," Dah Buffer<br>");

  web_client_print(client, "Dah Buffer");
  web_print_control_radio(client, "da", 0, (configuration.dah_buffer_off) ? 0 : 1, "On ");
  web_print_control_radio(client, "da", 1, (configuration.dah_buffer_off) ? 1 : 0, "Off");
  web_client_println(client, "<br>");

  web_print_control_radio(client, "sm", SPEED_NORMAL, (speed_mode == SPEED_NORMAL) ? 1 : 0, "Normal Speed Mode ");

  web_print_control_textbox(client, "wp", "addr", (int)configuration.wpm, "", " WPM ");

#if defined(FEATURE_FARNSWORTH)
  web_print_control_textbox(client, "fw", "addr", (int)configuration.wpm_farnsworth, "", " Farnsworth WPM");
#endif // FEATURE_FARNSWORTH

  web_client_println(client, "<br>");

  web_print_control_radio(client, "sm", SPEED_QRSS, (speed_mode == SPEED_QRSS) ? 1 : 0, "QRSS Mode   Dit Length ");
  web_client_print(client, F("<label><input type=\"text\" name=\"qd\" class=\"addr\" value=\""));
  web_client_println(client, qrss_dit_length);
  web_client_print(client, F("\"> s</label>"));
  web_client_println(client, F("<br>"));

  web_client_print(client, F("Sidetone: "));
  web_print_control_radio(client, "st", SIDETONE_ON, (configuration.sidetone_mode == SIDETONE_ON) ? 1 : 0, "On ");
  web_print_control_radio(client, "st", SIDETONE_OFF, (configuration.sidetone_mode == SIDETONE_OFF) ? 1 : 0, "Off ");
  web_print_control_radio(client, "st", SIDETONE_PADDLE_ONLY, (configuration.sidetone_mode == SIDETONE_PADDLE_ONLY) ? 1 : 0, "Paddle Only ");
  web_print_control_textbox(client, "hz", "addr", (int)configuration.hz_sidetone, "", " hz");
  web_client_println(client, F("<br>"));

  web_print_control_textbox(client, "dd", "addr", float(configuration.dah_to_dit_ratio / 100.0), "", " Dah/Dit Ratio");
  web_client_println(client, F("<br>"));

  web_print_control_textbox(client, "wt", "addr", (int)configuration.weighting, "Weighting ", "");
  web_client_println(client, F("<br>"));

#ifdef FEATURE_COMMAND_LINE_INTERFACE
  web_print_control_textbox(client, "sn", "addr", (int)serial_number, "Serial # ", "");
  web_client_println(client, F("<br>"));
  ;
#endif

#if defined(FEATURE_POTENTIOMETER)
  // web_print_control_textbox(client,"po","addr",(int)pot_value_wpm(),"Potentiometer "," WPM ");
  // web_print_control_checkbox(client,"pa",(configuration.pot_activated)?1:0," Active");
  web_client_print(client, "Potentiometer ");
  web_print_control_radio(client, "pa", 1, (configuration.pot_activated) ? 1 : 0, "Active ");
  web_print_control_radio(client, "pa", 0, (configuration.pot_activated) ? 0 : 1, "Inactive");
  web_client_println(client, F("<br>"));
#endif

#if defined(FEATURE_AUTOSPACE)
  // web_print_control_checkbox(client,"as",(configuration.autospace_active)?1:0," Autospace<br>");
  web_client_print(client, "Autospace");
  web_print_control_radio(client, "as", 1, (configuration.autospace_active) ? 1 : 0, "On ");
  web_print_control_radio(client, "as", 0, (configuration.autospace_active) ? 0 : 1, "Off");
  web_client_println(client, "<br>");
#endif // FEATURE_AUTOSPACE

  web_print_control_textbox(client, "ws", "addr", (int)configuration.length_wordspace, "Wordspace ", "");
  web_client_println(client, F("<br>"));

  web_print_control_textbox(client, "tx", "addr", (int)configuration.current_tx, "TX ", "");
  web_client_println(client, F("<br>"));

#if defined(FEATURE_QLF)
  // web_print_control_checkbox(client,"ql",(qlf_active)?1:0," QLF<br>");

  web_client_print(client, "QLF");
  web_print_control_radio(client, "ql", 1, (qlf_active) ? 1 : 0, "On ");
  web_print_control_radio(client, "ql", 0, (qlf_active) ? 0 : 1, "Off");
  web_client_println(client, "<br>");

#endif // FEATURE_QLF

  web_client_println(client, F("<br><br><input type=\"submit\" value=\"Save\"></form>"));

  web_print_home_link(client);

  web_print_footer(client);
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)

void web_print_page_keyer_settings_process(EthernetClient client)
{

  uint8_t invalid_data = 0;

  unsigned int ud = 0;

  uint8_t temp_keyer_mode = 0;
  uint8_t temp_dit_buffer_off = 0;
  uint8_t temp_dah_buffer_off = 0;
  uint8_t temp_speed_mode = 0;
  unsigned int temp_wpm = 0;
  unsigned int temp_qrss_dit_length = 0;
  uint8_t temp_sidetone_mode = 0;
  unsigned int temp_sidetone_hz = 0;
  String temp_string_dit_dah_ratio;
  uint8_t temp_weight = 0;
  unsigned int temp_serial = 0;
  uint8_t temp_wordspace = 0;
  uint8_t temp_tx = 0;

#if defined(FEATURE_QLF)
  uint8_t temp_qlf = 0;
#endif // FEATURE_QLF

#if defined(FEATURE_POTENTIOMETER)
  uint8_t temp_pot_activated = 0;
#endif // FEATURE_POTENTIOMETER

#if defined(FEATURE_AUTOSPACE)
  uint8_t temp_autospace_active = 0;
#endif // FEATURE_AUTOSPACE

#if defined(FEATURE_FARNSWORTH)
  unsigned int temp_farnsworth = 0;
#endif // FEATURE_FARNSWORTH

  parse_get(web_server_incoming_string);

  if (parse_get_results_index)
  {

    for (int x = 0; x < parse_get_results_index; x++)
    {
      if (parse_get_results[x].parameter == "md")
      {
        temp_keyer_mode = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "di")
      {
        temp_dit_buffer_off = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "da")
      {
        temp_dah_buffer_off = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "sm")
      {
        temp_speed_mode = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "wp")
      {
        temp_wpm = parse_get_results[x].value_long;
      }
#if defined(FEATURE_FARNSWORTH)
      if (parse_get_results[x].parameter == "fw")
      {
        temp_farnsworth = parse_get_results[x].value_long;
      }
#endif // FEATURE_FARNSWORTH
      if (parse_get_results[x].parameter == "qd")
      {
        temp_qrss_dit_length = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "st")
      {
        temp_sidetone_mode = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "hz")
      {
        temp_sidetone_hz = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "dd")
      {
        temp_string_dit_dah_ratio = parse_get_results[x].value_string;
      }
      if (parse_get_results[x].parameter == "wt")
      {
        temp_weight = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "sn")
      {
        temp_serial = parse_get_results[x].value_long;
      }
// po - nothing to do for potentiometer value
#if defined(FEATURE_POTENTIOMETER)
      if (parse_get_results[x].parameter == "pa")
      {
        temp_pot_activated = parse_get_results[x].value_long;
      }
#endif // FEATURE_POTENTIOMETER
#if defined(FEATURE_AUTOSPACE)
      if (parse_get_results[x].parameter == "as")
      {
        temp_autospace_active = parse_get_results[x].value_long;
      }
#endif // FEATURE_AUTOSPACE
      if (parse_get_results[x].parameter == "ws")
      {
        temp_wordspace = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "tx")
      {
        temp_tx = parse_get_results[x].value_long;
      }
#if defined(FEATURE_QLF)
      if (parse_get_results[x].parameter == "ql")
      {
        temp_qlf = parse_get_results[x].value_long;
      }
#endif // FEATURE_QLF
    }

    // data validation

    // TODO !  data validation

    if (invalid_data)
    {

      web_print_header(client);
      web_print_meta_refresh(client, configuration.ip[0], configuration.ip[1], configuration.ip[2], configuration.ip[3], 2);
      web_client_println(client, F("\/KeyerSettings'\" />"));
      web_print_style_sheet(client);
      web_print_title(client);
      web_client_println(client, F("<br>Bad data!<br>"));
      web_print_home_link(client);
      web_print_footer(client);
    }
    else
    {

      // assign to variables

      configuration.keyer_mode = temp_keyer_mode;
      configuration.dit_buffer_off = temp_dit_buffer_off;
      configuration.dah_buffer_off = temp_dah_buffer_off;
      speed_mode = temp_speed_mode;
      configuration.wpm = temp_wpm;
      qrss_dit_length = temp_qrss_dit_length;
      configuration.sidetone_mode = temp_sidetone_mode;
      configuration.hz_sidetone = temp_sidetone_hz;
      temp_string_dit_dah_ratio.replace(".", "");
      configuration.dah_to_dit_ratio = temp_string_dit_dah_ratio.toInt();
      configuration.weighting = temp_weight;
#if defined(FEATURE_COMMAND_MODE)
      serial_number = temp_serial;
#endif
      configuration.length_wordspace = temp_wordspace;
      configuration.current_tx = temp_tx;
#if defined(FEATURE_QLF)
      qlf_active = temp_qlf;
#endif // FEATURE_QLF
#if defined(FEATURE_POTENTIOMETER)
      configuration.pot_activated = temp_pot_activated;
#endif // FEATURE_POTENTIOMETER
#if defined(FEATURE_AUTOSPACE)
      configuration.autospace_active = temp_autospace_active;
#endif // FEATURE_AUTOSPACE
#if defined(FEATURE_FARNSWORTH)
      configuration.wpm_farnsworth = temp_farnsworth;
#endif // FEATURE_FARNSWORTH

      web_print_header(client);
      web_print_meta_refresh(client, configuration.ip[0], configuration.ip[1], configuration.ip[2], configuration.ip[3], 2);
      web_client_println(client, F("\/KeyerSettings'\" />"));
      web_print_style_sheet(client);
      web_print_title(client);
      web_client_println(client, F("<br>Configuration saved<br><br>"));
      web_print_home_link(client);
      web_print_footer(client);
      config_dirty = 1;
    }
  }
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER) && defined(FEATURE_MEMORIES)

void web_print_page_memories(EthernetClient client)
{

  int memory_number_to_send = 0;
  int last_memory_location;

#if defined(OPTION_PROSIGN_SUPPORT)
  byte eeprom_temp = 0;
  static char *prosign_temp = 0;
#endif

  web_print_header(client);

  web_print_style_sheet(client);

  web_print_title(client);

  web_client_println(client, F("<H1>Memories</H1><hr><br>"));

  web_client_print(client, F("<br><br>"));

  // if (web_server_incoming_string.length() > 14){web_server_incoming_string.remove(14);}

  if ((web_server_incoming_string.indexOf("?m") > 0) && (web_server_incoming_string.length() > (web_server_incoming_string.indexOf("?m") + 2)))
  {
    memory_number_to_send = ((web_server_incoming_string.charAt(web_server_incoming_string.indexOf("?m") + 2) - 48) * 10) + (web_server_incoming_string.charAt(web_server_incoming_string.indexOf("?m") + 3) - 48);

    // web_client_print(client,web_server_incoming_string);
    // web_client_print(client,F("<br><br>"));

    // web_client_print(client,F("mem number: "));
    // web_client_print(client,memory_number_to_send);
    // web_client_print(client,F("<br><br>"));

    // web_client_print(client,web_server_incoming_string.charAt(web_server_incoming_string.indexOf("?m")+1));
    // web_client_print(client,web_server_incoming_string.charAt(web_server_incoming_string.indexOf("?m")+2));
    // web_client_print(client,F("<br><br>"));

    add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
    add_to_send_buffer(memory_number_to_send - 1);
  }

  for (int i = 0; i < number_of_memories; i++)
  {
    web_client_print(client, F("<a href=\"/mem?m"));
    if (i < 9)
    {
      web_client_print(client, "0");
    }
    web_client_print(client, i + 1);
    web_client_print(client, "\" class=\"ctrl\">");
    web_client_print(client, i + 1);

    last_memory_location = memory_end(i) + 1;

    if (EEPROM.read(memory_start(i)) == 255)
    {
      // web_client_print(client,F("{empty}"));
      web_client_print(client, F("       "));
    }
    else
    {
      web_client_print(client, ") ");
      for (int y = (memory_start(i)); (y < last_memory_location); y++)
      {
        if (EEPROM.read(y) < 255)
        {
#if defined(OPTION_PROSIGN_SUPPORT)
          eeprom_temp = EEPROM.read(y);
          if ((eeprom_temp > PROSIGN_START) && (eeprom_temp < PROSIGN_END))
          {
            prosign_temp = convert_prosign(eeprom_temp);
            web_client_write(client, prosign_temp[0]);
            web_client_write(client, prosign_temp[1]);
            if (strlen(prosign_temp) == 3)
              web_client_write(client, prosign_temp[2]);
          }
          else
          {
            web_client_write(client, eeprom_temp);
          }
#else
          web_client_write(client, EEPROM.read(y));
#endif // OPTION_PROSIGN_SUPPORT
        }
        else
        {
          y = last_memory_location;
        }
      }
    }

    web_client_print(client, "</a>");
    // web_client_print(client,"<br><br><br>");
    if (number_of_memories > 4)
    {
      if (((i + 1) % 4) == 0)
      {
        web_client_print(client, "<br><br><br>");
      }
    }
  }

  web_client_print(client, F("<br>"));

  web_print_home_link(client);

  web_print_footer(client);
}
#endif // FEATURE_WEB_SERVER && FEATURE_MEMORIES

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)

void web_print_page_control(EthernetClient client)
{

  /*

    /ctrl - regular page

    /ctrlnd - no display

    /ctrlnd?st<TEXTTOSEND>/

    http://192.168.1.178/ctrlnd?sttest/


  */

  uint8_t pin_read = 0;

#if defined(FEATURE_MEMORIES)
  uint8_t memory_number_to_send = 0;
#endif // FEATURE_MEMORIES

  int search_string_start_position = 0;

  String url_sub_string;

  if ((web_server_incoming_string.indexOf("ctrl?") > 0) || (web_server_incoming_string.indexOf("ctrlnd?") > 0))
  {
    url_sub_string = web_server_incoming_string;
    if (url_sub_string.length() > 14)
    {
      url_sub_string.remove(14);
    }
    if (url_sub_string.indexOf("?ky") > 0)
    {
      sending_mode = AUTOMATIC_SENDING;
      web_control_tx_key_time = millis();
      tx_and_sidetone_key(1);
    }
    if (url_sub_string.indexOf("?uk") > 0)
    {
      sending_mode = AUTOMATIC_SENDING;
      tx_and_sidetone_key(0);
      web_control_tx_key_time = 0;
    }
    if (url_sub_string.indexOf("?wn") > 0)
    {
      speed_change(-2);
    }
    if (url_sub_string.indexOf("?wp") > 0)
    {
      speed_change(2);
    }
#if defined(FEATURE_MEMORIES)
    if ((web_server_incoming_string.indexOf("?m") > 0) & (web_server_incoming_string.length() > (web_server_incoming_string.indexOf("?m") + 2)))
    {
      memory_number_to_send = ((web_server_incoming_string.charAt(web_server_incoming_string.indexOf("m") + 1) - 48) * 10) + (web_server_incoming_string.charAt(web_server_incoming_string.indexOf("m") + 2) - 48);
      add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
      add_to_send_buffer(memory_number_to_send - 1);
    }
#endif // FEATURE_MEMORIES
    if (url_sub_string.indexOf("?st") > 0)
    {
      for (int x = (web_server_incoming_string.indexOf("st") + 2); x < web_server_incoming_string.length(); x++)
      {
        if (web_server_incoming_string.charAt(x) == '/')
        {
          x = web_server_incoming_string.length();
        }
        else
        {
          if (web_server_incoming_string.charAt(x) == '%')
          { // do we have a http hex code?
            add_to_send_buffer((((uint8_t)web_server_incoming_string.charAt(x + 1) - 48) << 4) + ((uint8_t)web_server_incoming_string.charAt(x + 2) - 48));
            x = x + 2;
          }
          else
          {
            add_to_send_buffer(uppercase(web_server_incoming_string.charAt(x)));
          }
        }
      }
    }
  }

  if (web_server_incoming_string.indexOf("nd") > 0)
  { // no display option

    web_print_200OK(client);
  }
  else
  {

    web_print_header(client);

    web_print_style_sheet(client);

    web_print_title(client);

    web_client_println(client, F("<H1>Control</H1><hr><br>"));

    // web_client_print(client,"web_server_incoming_string: ");
    // web_client_print(client,web_server_incoming_string);
    // web_client_print(client,"url_sub_string: ");
    // web_client_print(client,url_sub_string);
    // web_client_println(client,F("<br><br>"));

#if defined(FEATURE_MEMORIES)
    web_client_print(client, F("<br><br>"));
    for (int i = 0; i < number_of_memories; i++)
    {
      web_client_print(client, F("<a href=\"/ctrl?m"));
      if (i < 9)
      {
        web_client_print(client, "0");
      }
      web_client_print(client, i + 1);
      web_client_print(client, "\" class=\"ctrl\">");
      web_client_print(client, i + 1);
      web_client_print(client, "</a>");
      if (number_of_memories > 4)
      {
        if (((i + 1) % 4) == 0)
        {
          web_client_print(client, "<br><br><br>");
        }
      }
    }

    web_client_print(client, F("<br>"));

#endif // FEATURE_MEMORIES

    web_client_println(client, F("<br><a href=\"/ctrl?wn\" class=\"ctrl\">WPM -2</a><a href=\"/ctrl?wp\" class=\"ctrl\">WPM +2</a><br><br><br>"));

    web_client_println(client, F("<br><a href=\"/ctrl?ky\" class=\"ctrl\">Key</a><a href=\"/ctrl?uk\" class=\"ctrl\">Unkey</a><br><br><br>"));

    web_print_home_link(client);

    web_print_footer(client);
  }
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_println(EthernetClient client, const __FlashStringHelper *str)
{

  web_client_print(client, str);
  client.println();
}

#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_print(EthernetClient client, const __FlashStringHelper *str)
{

  char c;
  if (!str)
    return;
  char charstring[255] = "";
  int charstringindex = 0;

  /* since str is a const we can't increment it, so do this instead */
  char *p = (char *)str;

  /* keep going until we find the null */
  while ((c = pgm_read_byte(p++)))
  {
    if (charstringindex < 254)
    {
      charstring[charstringindex] = c;
      charstringindex++;
    }
  }
  charstring[charstringindex] = 0;
  client.print(charstring);
}

#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_print(EthernetClient client, String str)
{

  client.print(str);
}

#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_print(EthernetClient client, const char *str)
{

  client.print(str);
}

#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_println(EthernetClient client, const char *str)
{

  client.println(str);
}

#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_print(EthernetClient client, int i)
{

  client.print(i);
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_print(EthernetClient client, float f)
{

  client.print(f);
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_print(EthernetClient client, unsigned long i)
{

  client.print(i);
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_print(EthernetClient client, unsigned int i)
{

  client.print(i);
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_println(EthernetClient client, unsigned long i)
{

  client.println(i);
}
#endif // FEATURE_WEB_SERVER
//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_println(EthernetClient client, unsigned long i, int something)
{

  client.println(i, something);
}
#endif // FEATURE_WEB_SERVER
//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_WEB_SERVER)
void web_client_write(EthernetClient client, uint8_t i)
{

  client.write(i);
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)

void web_print_page_link_settings_process(EthernetClient client)
{

  uint8_t parsed_link_ip[4][FEATURE_INTERNET_LINK_MAX_LINKS];
  uint8_t parsed_link_enabled[FEATURE_INTERNET_LINK_MAX_LINKS];
  int parsed_link_send_udp_port[FEATURE_INTERNET_LINK_MAX_LINKS];
  int parsed_link_receive_udp_port = 0;
  uint8_t parsed_link_receive_enabled = 0;

  uint8_t invalid_data = 0;

  unsigned int ud = 0;

  parse_get(web_server_incoming_string);
  if (parse_get_results_index)
  {

    for (int x = 0; x < parse_get_results_index; x++)
    { // TODO - rewrite this to scale...
      if (parse_get_results[x].parameter == "ip00")
      {
        parsed_link_ip[0][0] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip01")
      {
        parsed_link_ip[1][0] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip02")
      {
        parsed_link_ip[2][0] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip03")
      {
        parsed_link_ip[3][0] = parse_get_results[x].value_long;
      }

      if (parse_get_results[x].parameter == "ip10")
      {
        parsed_link_ip[0][1] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip11")
      {
        parsed_link_ip[1][1] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip12")
      {
        parsed_link_ip[2][1] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip13")
      {
        parsed_link_ip[3][1] = parse_get_results[x].value_long;
      }

      if (parse_get_results[x].parameter == "ip20")
      {
        parsed_link_ip[0][2] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip21")
      {
        parsed_link_ip[1][2] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip22")
      {
        parsed_link_ip[2][2] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip23")
      {
        parsed_link_ip[3][2] = parse_get_results[x].value_long;
      }

      if (parse_get_results[x].parameter == "act0")
      {
        parsed_link_enabled[0] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "act1")
      {
        parsed_link_enabled[1] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "act2")
      {
        parsed_link_enabled[2] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "act3")
      {
        parsed_link_enabled[3] = parse_get_results[x].value_long;
      }

      if (parse_get_results[x].parameter == "sp0")
      {
        parsed_link_send_udp_port[0] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "sp1")
      {
        parsed_link_send_udp_port[1] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "sp2")
      {
        parsed_link_send_udp_port[2] = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "sp3")
      {
        parsed_link_send_udp_port[3] = parse_get_results[x].value_long;
      }

      if (parse_get_results[x].parameter == "ud")
      {
        parsed_link_receive_udp_port = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "lr")
      {
        parsed_link_receive_enabled = parse_get_results[x].value_long;
      }
    }

    // data validation

    for (int x = 0; x < FEATURE_INTERNET_LINK_MAX_LINKS; x++)
    {
      if (parsed_link_enabled[x])
      {
        for (int y = 0; y < 4; y++)
        {
          if ((parsed_link_ip[y][x] < 0) || (parsed_link_ip[y][x] > 255))
          {
            invalid_data = 1;
          }
        }
        if ((parsed_link_ip[3][x] == 0) || (parsed_link_ip[3][x] == 255) || (parsed_link_ip[0][x] == 0) || (parsed_link_ip[0][x] == 255))
        {
          invalid_data = 1;
        }
        if ((parsed_link_send_udp_port[x] < 1) || (parsed_link_send_udp_port[x] > 65535))
        {
          invalid_data = 1;
        }
      }
    }

    if (invalid_data)
    {

      web_print_header(client);
      web_print_meta_refresh(client, configuration.ip[0], configuration.ip[1], configuration.ip[2], configuration.ip[3], 2);
      web_client_println(client, F("\/LinkSettings'\" />"));
      web_print_style_sheet(client);
      web_print_title(client);
      web_client_println(client, F("<br>Bad data!<br>"));
      web_print_home_link(client);
      web_print_footer(client);
    }
    else
    {

      for (int x = 0; x < FEATURE_INTERNET_LINK_MAX_LINKS; x++)
      {

        configuration.link_send_ip[0][x] = parsed_link_ip[0][x];
        configuration.link_send_ip[1][x] = parsed_link_ip[1][x];
        configuration.link_send_ip[2][x] = parsed_link_ip[2][x];
        configuration.link_send_ip[3][x] = parsed_link_ip[3][x];

        configuration.link_send_udp_port[x] = parsed_link_send_udp_port[x];

        configuration.link_send_enabled[x] = parsed_link_enabled[x];
      }

      configuration.link_receive_udp_port = parsed_link_receive_udp_port;
      configuration.link_receive_enabled = parsed_link_receive_enabled;

      web_print_header(client);
      web_print_meta_refresh(client, configuration.ip[0], configuration.ip[1], configuration.ip[2], configuration.ip[3], 5);
      web_client_println(client, F("\/LinkSettings'\" />"));
      web_print_style_sheet(client);
      web_print_title(client);
      web_client_println(client, F("<br>Configuration saved<br><br>"));
      web_print_home_link(client);
      web_print_footer(client);
      config_dirty = 1;
    }
  }
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)

void web_print_page_network_settings_process(EthernetClient client)
{

  uint8_t ip0 = 0;
  uint8_t ip1 = 0;
  uint8_t ip2 = 0;
  uint8_t ip3 = 0;
  uint8_t gw0 = 0;
  uint8_t gw1 = 0;
  uint8_t gw2 = 0;
  uint8_t gw3 = 0;
  uint8_t sn0 = 0;
  uint8_t sn1 = 0;
  uint8_t sn2 = 0;
  uint8_t sn3 = 0;

  uint8_t invalid_data = 0;

  unsigned int ud = 0;

  parse_get(web_server_incoming_string);
  if (parse_get_results_index)
  {

    for (int x = 0; x < parse_get_results_index; x++)
    {
      if (parse_get_results[x].parameter == "ip0")
      {
        ip0 = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip1")
      {
        ip1 = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip2")
      {
        ip2 = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "ip3")
      {
        ip3 = parse_get_results[x].value_long;
      }

      if (parse_get_results[x].parameter == "gw0")
      {
        gw0 = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "gw1")
      {
        gw1 = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "gw2")
      {
        gw2 = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "gw3")
      {
        gw3 = parse_get_results[x].value_long;
      }

      if (parse_get_results[x].parameter == "sn0")
      {
        sn0 = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "sn1")
      {
        sn1 = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "sn2")
      {
        sn2 = parse_get_results[x].value_long;
      }
      if (parse_get_results[x].parameter == "sn3")
      {
        sn3 = parse_get_results[x].value_long;
      }
    }

    // invalid_data = 1;

    // data validation

    if ((ip0 == 0) || (ip3 == 255) || (ip3 == 0))
    {
      invalid_data = 1;
    }
    if (((ip0 & sn0) != (gw0 & sn0)) || ((ip1 & sn1) != (gw1 & sn1)) || ((ip2 & sn2) != (gw2 & sn2)) || ((ip3 & sn3) != (gw3 & sn3)))
    {
      invalid_data = 1;
    }
    if ((sn0 == 0) || (sn1 > sn0) || (sn2 > sn1) || (sn3 > sn2) || (sn3 > 252))
    {
      invalid_data = 1;
    }

    if (invalid_data)
    {

      web_print_header(client);
      web_print_style_sheet(client);
      web_print_title(client);
      web_client_println(client, F("<br>Bad data!<br>"));
      web_print_home_link(client);
      web_print_footer(client);
    }
    else
    {

      configuration.ip[0] = ip0;
      configuration.ip[1] = ip1;
      configuration.ip[2] = ip2;
      configuration.ip[3] = ip3;

      configuration.gateway[0] = gw0;
      configuration.gateway[1] = gw1;
      configuration.gateway[2] = gw2;
      configuration.gateway[3] = gw3;

      configuration.subnet[0] = sn0;
      configuration.subnet[1] = sn1;
      configuration.subnet[2] = sn2;
      configuration.subnet[3] = sn3;

      web_print_header(client);
      web_print_meta_refresh(client, ip0, ip1, ip2, ip3, 5);
      web_client_println(client, F("'\" />"));
      web_print_style_sheet(client);
      web_print_title(client);
      web_client_println(client, F("<br>Configuration saved<br>Restarting networking<br><br>You will be redirected to new address in 5 seconds...<br>"));
      web_print_home_link(client);
      web_print_footer(client);
      restart_networking = 1;
      config_dirty = 1;
    }
  }
}
#endif // FEATURE_WEB_SERVER

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_WEB_SERVER)

void web_print_meta_refresh(EthernetClient client, uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t refresh_time)
{

  web_client_print(client, F("<meta http-equiv=\"refresh\" content=\""));
  web_client_print(client, refresh_time);
  web_client_print(client, F("; URL='http://"));
  web_client_print(client, ip0);
  web_client_print(client, ".");
  web_client_print(client, ip1);
  web_client_print(client, ".");
  web_client_print(client, ip2);
  web_client_print(client, ".");
  web_client_print(client, ip3);
}
#endif // FEATURE_WEB_SERVER
//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_INTERNET_LINK)
void link_key(uint8_t link_key_state)
{

  static unsigned long last_link_key_action_time = 0;
  static uint8_t current_link_key_state = 0;
  static uint8_t buffered_key_down = 0;
  uint8_t bytes_to_send[8];
  uint8_t bytes_to_send_counter = 0;

  if (link_key_state != current_link_key_state)
  {

    if (((millis() - last_link_key_action_time) < FEATURE_INTERNET_LINK_BUFFER_TIME_MS) && (last_link_key_action_time != 0))
    {
      if (link_key_state)
      {
#if defined(DEBUG_INTERNET_LINKING_SEND)
        debug_serial_port->print("link_key: D");
#endif // DEBUG_INTERNET_LINKING_SEND
        bytes_to_send[0] = 'D';
        add_to_udp_send_buffer(bytes_to_send, 1);
      }
      else
      {
        if (buffered_key_down)
        {
#if defined(DEBUG_INTERNET_LINKING_SEND)
          debug_serial_port->print("link_key: V");
#endif // DEBUG_INTERNET_LINKING_SEND
          bytes_to_send[0] = 'V';
          add_to_udp_send_buffer(bytes_to_send, 1);
          buffered_key_down = 0;
        }
        else
        {
#if defined(DEBUG_INTERNET_LINKING_SEND)
          debug_serial_port->print("link_key: U");
#endif // DEBUG_INTERNET_LINKING_SEND
          bytes_to_send[0] = 'U';
          add_to_udp_send_buffer(bytes_to_send, 1);
        }
      }
#if defined(DEBUG_INTERNET_LINKING_SEND)
      debug_serial_port->print(millis() - last_link_key_action_time);
#endif // DEBUG_INTERNET_LINKING_SEND
      unsigned int number_to_send = millis() - last_link_key_action_time;
      if ((number_to_send / 10000) > 0)
      {
        bytes_to_send[0] = (number_to_send / 10000) + 48;
        number_to_send = number_to_send % 10000;
        bytes_to_send_counter++;
      }
      if ((number_to_send / 1000) > 0)
      {
        bytes_to_send[bytes_to_send_counter] = (number_to_send / 1000) + 48;
        number_to_send = number_to_send % 1000;
        bytes_to_send_counter++;
      }
      if ((number_to_send / 100) > 0)
      {
        bytes_to_send[bytes_to_send_counter] = (number_to_send / 100) + 48;
        number_to_send = number_to_send % 100;
        bytes_to_send_counter++;
      }
      if ((number_to_send / 10) > 0)
      {
        bytes_to_send[bytes_to_send_counter] = (number_to_send / 10) + 48;
        number_to_send = number_to_send % 10;
        bytes_to_send_counter++;
      }
      bytes_to_send[bytes_to_send_counter] = number_to_send + 48;
      bytes_to_send_counter++;
      add_to_udp_send_buffer(bytes_to_send, bytes_to_send_counter);
    }
    else
    {
      buffered_key_down = 1;
    }
#if defined(DEBUG_INTERNET_LINKING_SEND)
    debug_serial_port->println("");
#endif // DEBUG_INTERNET_LINKING_SEND
    current_link_key_state = link_key_state;
    last_link_key_action_time = millis();
  }
}

#endif // FEATURE_INTERNET_LINK

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_INTERNET_LINK)
void add_to_udp_send_buffer(uint8_t bytes_to_send[8], uint8_t number_of_bytes)
{

  for (int x = 0; x < number_of_bytes; x++)
  {
    if (udp_send_buffer_bytes < FEATURE_UDP_SEND_BUFFER_SIZE)
    {
      udp_send_buffer[udp_send_buffer_bytes] = bytes_to_send[x];
      udp_send_buffer_bytes++;
    }
  }
}

#endif // FEATURE_INTERNET_LINK
//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_INTERNET_LINK)
void service_udp_send_buffer()
{

  static uint8_t link_send_buffer[FEATURE_INTERNET_LINK_MAX_LINKS][FEATURE_UDP_SEND_BUFFER_SIZE];
  static uint8_t link_send_buffer_bytes[FEATURE_INTERNET_LINK_MAX_LINKS];
  static uint8_t link_send_buffer_bytes_initialized = 0;

  if (!link_send_buffer_bytes_initialized)
  {
    for (int x = 0; x < FEATURE_INTERNET_LINK_MAX_LINKS; x++)
    {
      link_send_buffer_bytes[x] = 0;
    }
    link_send_buffer_bytes_initialized = 1;
  }

  // load up the bytes sitting in the udp_send_buffer into the individual link buffers
  if (udp_send_buffer_bytes)
  {
    for (int y = 0; y < FEATURE_INTERNET_LINK_MAX_LINKS; y++)
    { // enumerate the individual links
      for (int x = 0; x < udp_send_buffer_bytes; x++)
      { // loop through the bytes in the udp_send_buffer
        if (configuration.link_send_enabled[y])
        {
          if (link_send_buffer_bytes[y] < FEATURE_UDP_SEND_BUFFER_SIZE)
          {
            link_send_buffer[y][link_send_buffer_bytes[y]] = udp_send_buffer[x];
            link_send_buffer_bytes[y]++;
          }
          else
          {
#if defined(DEBUG_UDP)
            debug_serial_port->println("service_udp_send_buffer: link_send_buffer_overflow");
#endif
          }
        }
      }
    }
    udp_send_buffer_bytes = 0;
    return;
  }

  // send out a packet for the first link that has packets in the buffer (don't do them all at once so we don't hog up the CPU)
  for (int y = 0; y < FEATURE_INTERNET_LINK_MAX_LINKS; y++)
  {
    if ((configuration.link_send_enabled[y]) && (link_send_buffer_bytes[y]))
    {
      IPAddress ip(configuration.link_send_ip[0][y], configuration.link_send_ip[1][y], configuration.link_send_ip[2][y], configuration.link_send_ip[3][y]);
#if defined(DEBUG_UDP)
      debug_serial_port->print(F("service_udp_send_buffer: beginPacket "));
      debug_serial_port->print(configuration.link_send_ip[0][y]);
      debug_serial_port->print(F("."));
      debug_serial_port->print(configuration.link_send_ip[1][y]);
      debug_serial_port->print(F("."));
      debug_serial_port->print(configuration.link_send_ip[2][y]);
      debug_serial_port->print(F("."));
      debug_serial_port->print(configuration.link_send_ip[3][y]);
      debug_serial_port->print(F(":"));
      debug_serial_port->println(configuration.link_send_udp_port[y]);
#endif

      Udp.beginPacket(ip, configuration.link_send_udp_port[y]);

      for (int x = 0; x < link_send_buffer_bytes[y]; x++)
      {
        udp_write(link_send_buffer[y][x]);
      }
#if defined(DEBUG_UDP)
      debug_serial_port->print("\n\rservice_udp_send_buffer: endPacket ");
      unsigned long beginPacket_start = millis();
#endif
      int endpacket_result = Udp.endPacket();
#if defined(DEBUG_UDP)
      unsigned long beginPacket_end = millis();
      if (!endpacket_result)
      {
        debug_serial_port->print("error");
      }
      else
      {
        debug_serial_port->print("OK");
      }
      debug_serial_port->print(" time:");
      debug_serial_port->print(beginPacket_end - beginPacket_start);
      debug_serial_port->println(" mS");
#endif
      link_send_buffer_bytes[y] = 0;
      y = FEATURE_INTERNET_LINK_MAX_LINKS; // exit after we've process one buffer with bytes
    }
  }
}

#endif // FEATURE_INTERNET_LINK

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_UDP)
void udp_write(uint8_t byte_to_write)
{

  Udp.write(byte_to_write);

#if defined(DEBUG_UDP_WRITE)

  static char ascii_sent[17] = "";

  debug_serial_port->print(" ");
  if (byte_to_write < 16)
  {
    debug_serial_port->print("0");
  }
  debug_serial_port->print(byte_to_write, HEX);
  debug_serial_port->print(" ");
  debug_serial_port->write(byte_to_write);
#endif // DEBUG_UDP_WRITE
}
#endif // FEATURE_UDP

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_UDP)
void service_udp_receive()
{

  char udp_char_receive_packet_buffer[FEATURE_UDP_RECEIVE_BUFFER_SIZE];

  if (configuration.link_receive_enabled)
  {
    int packet_size = Udp.parsePacket();
    if (packet_size)
    {

      Udp.read(udp_char_receive_packet_buffer, FEATURE_UDP_RECEIVE_BUFFER_SIZE);

#if defined(DEBUG_UDP_PACKET_RECEIVE)
      debug_serial_port->print(F("service_udp_receive: received packet: size "));
      debug_serial_port->print(packet_size);
      debug_serial_port->print(" from ");
      IPAddress remote = Udp.remoteIP();
      for (int i = 0; i < 4; i++)
      {
        debug_serial_port->print(remote[i], DEC);
        if (i < 3)
        {
          debug_serial_port->print(".");
        }
      }
      debug_serial_port->print(":");
      debug_serial_port->print(Udp.remotePort());
      debug_serial_port->print(" contents: ");
      for (int x = 0; x < packet_size; x++)
      {
        debug_serial_port->print(udp_char_receive_packet_buffer[x]);
      }
      debug_serial_port->println("$");
#endif // DEBUG_UDP

      if (packet_size > FEATURE_UDP_RECEIVE_BUFFER_SIZE)
      {
        packet_size = FEATURE_UDP_RECEIVE_BUFFER_SIZE;
      }

      for (int x = 0; x < packet_size; x++)
      {
        if (udp_receive_packet_buffer_bytes < FEATURE_UDP_RECEIVE_BUFFER_SIZE)
        {
          udp_receive_packet_buffer[udp_receive_packet_buffer_bytes] = udp_char_receive_packet_buffer[x];
          udp_receive_packet_buffer_bytes++;
        }
      }
    }
  }
}

#endif // FEATURE_UDP

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_UDP)
uint8_t get_udp_receive_buffer_byte()
{

  if (udp_receive_packet_buffer_bytes)
  {

    uint8_t byte_to_return = udp_receive_packet_buffer[0];

    udp_receive_packet_buffer_bytes--;

    if (udp_receive_packet_buffer_bytes)
    {
      for (int x = 0; x < udp_receive_packet_buffer_bytes; x++)
      {
        udp_receive_packet_buffer[x] = udp_receive_packet_buffer[x + 1];
      }
    }

#if defined(DEBUG_UDP_PACKET_RECEIVE)
    debug_serial_port->print(F("get_udp_receive_buffer_byte: returning: "));
    debug_serial_port->write(byte_to_return);
    debug_serial_port->print(F(" udp_receive_packet_buffer_bytes: "));
    debug_serial_port->println(udp_receive_packet_buffer_bytes);
#endif // DEBUG_UDP_PACKET_RECEIVE

    return byte_to_return;
  }
  else
  {
    return 0;
  }
}
#endif // FEATURE_UDP
//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_UDP)
uint8_t get_udp_receive_buffer_link_command(uint8_t *command, unsigned int *parameter)
{

  // this extracts received link commands from the udp_receive_packet_buffer

  uint8_t incoming_byte = 0;
  uint8_t return_value = 0;
  static uint8_t static_return_value = 0;
  static uint8_t command_value = 0;
  static uint8_t hit_vdu_command = 0;
  static unsigned int parameter_value = 0;
  static uint8_t digits = 0;

  static unsigned long last_byte_receive_time = 0;

  if (((millis() - last_byte_receive_time) > 500) && (hit_vdu_command))
  {
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
    if (static_return_value)
    {
      debug_serial_port->println(F("get_udp_receive_buffer_link_command: expired buffer"));
    }
#endif // DEBUG_INTERNET_LINKING_RECEIVE
    parameter_value = 0;
    hit_vdu_command = 0;
    digits = 0;
    // command_value = 0;
    static_return_value = 0;
  }

  if (udp_receive_packet_buffer_bytes)
  {

    for (int x = 0; ((x < udp_receive_packet_buffer_bytes) && (static_return_value == 0)); x++)
    {
      incoming_byte = get_udp_receive_buffer_byte();
      last_byte_receive_time = millis();
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
      // debug_serial_port->print(F("get_udp_receive_buffer_link_command: incoming_byte: "));
      // debug_serial_port->write(incoming_byte);
      // debug_serial_port->print(F(" hit_vdu_command: "));
      // debug_serial_port->println(hit_vdu_command);
#endif // DEBUG_INTERNET_LINKING_RECEIVE
      if (!hit_vdu_command)
      {
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
        // debug_serial_port->println(F("get_udp_receive_buffer_link_command: looking for V D U"));
#endif // DEBUG_INTERNET_LINKING_RECEIVE
        if ((incoming_byte == 'V') || (incoming_byte == 'D') || (incoming_byte == 'U'))
        {
          command_value = incoming_byte;
          hit_vdu_command = 1;
          parameter_value = 0;
          digits = 0;
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
          // debug_serial_port->println(F("get_udp_receive_buffer_link_command: hit_vdu_command"));
#endif // DEBUG_INTERNET_LINKING_RECEIVE
        }
      }
      else
      { // we've hit a V, D, or U command
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
        // debug_serial_port->println(F("get_udp_receive_buffer_link_command: looking for a number"));
#endif // DEBUG_INTERNET_LINKING_RECEIVE
        if ((incoming_byte > 47) && (incoming_byte < 58))
        {
          parameter_value = (parameter_value * 10) + (incoming_byte - 48);
          digits++;
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
          // debug_serial_port->print(F("get_udp_receive_buffer_link_command: parameter_value: "));
          // debug_serial_port->print(parameter_value);
          // debug_serial_port->print(F(" digits: "));
          // debug_serial_port->println(digits);
#endif // DEBUG_INTERNET_LINKING_RECEIVE
       // peek at next byte to see if we're at the end
          service_udp_receive();
          if (((udp_receive_packet_buffer_bytes > 0) && ((udp_receive_packet_buffer[0] == 'V') || (udp_receive_packet_buffer[0] == 'D') || (udp_receive_packet_buffer[0] == 'U'))) ||
              (udp_receive_packet_buffer_bytes == 0) || (digits > 4))
          {
            static_return_value = 1;
          }
        }
        else
        { // something bogus came in - reset everything
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
          debug_serial_port->print(F("get_udp_receive_buffer_link_command: reset digits:"));
          debug_serial_port->print(digits);
          debug_serial_port->print(F(" incoming_byte:"));
          debug_serial_port->write(incoming_byte);
          debug_serial_port->println();
#endif // DEBUG_INTERNET_LINKING_RECEIVE
       // parameter_value = 0;
       // digits = 0;
       // command_value = 0;
          hit_vdu_command = 0;
        }
      }
    }
  }

#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
  if (static_return_value)
  {
    debug_serial_port->print(F("get_udp_receive_buffer_link_command: exiting: cmd: "));
    debug_serial_port->write(command_value);
    debug_serial_port->print(F(" parameter: "));
    debug_serial_port->println(parameter_value);
  }
#endif // DEBUG_INTERNET_LINKING_RECEIVE

  if (static_return_value)
  {
    *command = command_value;
    *parameter = parameter_value;
    // parameter_value = 0;
    // digits = 0;
    // command_value = 0;
    static_return_value = 0;
    hit_vdu_command = 0;
    return_value = 1;
  }

  return return_value;
}
#endif // FEATURE_UDP
//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_UDP)
void service_internet_link_udp_receive_buffer()
{

  //  Vxxxxx = key down immediately, stay keyed down for xxxxx mS, then key up
  //  Dxxxxx = key down xxxxx mS after last command
  //  Uxxxxx = key up xxxxx mS after last command

#define LINK_NO_COMMAND 0
#define LINK_V_COMMAND_IN_PROGRESS 1
#define LINK_U_COMMAND_BUFFERED 2
#define LINK_D_COMMAND_BUFFERED 3

  uint8_t incoming_link_command = 0;
  unsigned int incoming_link_command_parameter = 0;

  static uint8_t current_link_control_state = LINK_NO_COMMAND;
  static unsigned long v_command_key_down_expire_time = 0;
  static unsigned long last_command_completion_time = 0;
  static unsigned long buffered_command_execution_time = 0;
  static unsigned long key_down_time = 0;

  if ((key_down_time > 0) && ((millis() - key_down_time) > (FEATURE_INTERNET_LINK_KEY_DOWN_TIMEOUT_SECS * 1000)))
  {
    tx_and_sidetone_key(0);
    key_down_time = 0;
  }

  switch (current_link_control_state)
  {
  case LINK_NO_COMMAND:
    // is there a command in the buffer, if so read it and execute
    if (get_udp_receive_buffer_link_command(&incoming_link_command, &incoming_link_command_parameter))
    {
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
      debug_serial_port->print(F("service_internet_link_udp_receive_buffer: incoming_link_command: "));
      debug_serial_port->write(incoming_link_command);
      debug_serial_port->print(F(" incoming_link_command_parameter: "));
      debug_serial_port->println(incoming_link_command_parameter);
#endif // DEBUG_INTERNET_LINKING_RECEIVE
      if (incoming_link_command == 'V')
      { // key down immediately for incoming_link_parameter mS
        tx_and_sidetone_key(1);
        key_down_time = millis();
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
        debug_serial_port->println(F("service_internet_link_udp_receive_buffer: LINK_V_COMMAND_IN_PROGRESS tx_and_sidetone_key: 1"));
#endif // DEBUG_INTERNET_LINKING_RECEIVE
        v_command_key_down_expire_time = millis() + incoming_link_command_parameter;
        current_link_control_state = LINK_V_COMMAND_IN_PROGRESS;
      }
      if (incoming_link_command == 'U')
      {
        current_link_control_state = LINK_U_COMMAND_BUFFERED;
        buffered_command_execution_time = last_command_completion_time + incoming_link_command_parameter;
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
        debug_serial_port->println(F("service_internet_link_udp_receive_buffer: LINK_U_COMMAND_BUFFERED"));
#endif // DEBUG_INTERNET_LINKING_RECEIVE
      }
      if (incoming_link_command == 'D')
      {
        current_link_control_state = LINK_D_COMMAND_BUFFERED;
        buffered_command_execution_time = last_command_completion_time + incoming_link_command_parameter;
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
        debug_serial_port->println(F("service_internet_link_udp_receive_buffer: LINK_D_COMMAND_BUFFERED"));
#endif // DEBUG_INTERNET_LINKING_RECEIVE
      }
    }
    break;
  case LINK_U_COMMAND_BUFFERED: // key up after last command time has passed
    if (millis() >= buffered_command_execution_time)
    {
      tx_and_sidetone_key(0);
      key_down_time = 0;
      last_command_completion_time = millis();
      current_link_control_state = LINK_NO_COMMAND;
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
      debug_serial_port->println(F("service_internet_link_udp_receive_buffer: LINK_U_COMMAND_BUFFERED->LINK_NO_COMMAND tx_and_sidetone_key: 0"));
#endif // DEBUG_INTERNET_LINKING_RECEIVE
    }
    break;
  case LINK_D_COMMAND_BUFFERED: // key down after last command time has passed
    if (millis() >= buffered_command_execution_time)
    {
      tx_and_sidetone_key(1);
      key_down_time = millis();
      last_command_completion_time = millis();
      current_link_control_state = LINK_NO_COMMAND;
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
      debug_serial_port->println(F("service_internet_link_udp_receive_buffer: LINK_D_COMMAND_BUFFERED->LINK_NO_COMMAND tx_and_sidetone_key: 1"));
#endif // DEBUG_INTERNET_LINKING_RECEIVE
    }
    break;
  case LINK_V_COMMAND_IN_PROGRESS: // we're in key down, check if it time to key up and complete
    if (millis() >= v_command_key_down_expire_time)
    {
      tx_and_sidetone_key(0);
      key_down_time = 0;
      v_command_key_down_expire_time = 0;
      last_command_completion_time = millis();
      current_link_control_state = LINK_NO_COMMAND;
#if defined(DEBUG_INTERNET_LINKING_RECEIVE)
      debug_serial_port->println(F("service_internet_link_udp_receive_buffer: LINK_V_COMMAND_IN_PROGRESS->LINK_NO_COMMAND tx_and_sidetone_key: 0"));
#endif // DEBUG_INTERNET_LINKING_RECEIVE
    }
    break;

  } // switch(current_link_control_state)
}

#endif // FEATURE_UDP

//-------------------------------------------------------------------------------------------------------

void service_millis_rollover()
{

  static unsigned long last_millis = 0;

  if (millis() < last_millis)
  {
    millis_rollover++;
  }
  last_millis = millis();
}
//-------------------------------------------------------------------------------------------------------
#ifdef OPTION_NON_ENGLISH_EXTENSIONS
byte convert_unicode_to_send_char_code(byte first_byte, byte second_byte)
{

  if (first_byte == 195)
  {
    switch (second_byte)
    {
    case 133:
      return 197; // Å AA_capital (OZ, LA, SM)
    case 134:
      return 198; // Æ (OZ, LA)
    case 152:
      return 216; // Ø (OZ, LA)
    case 128:
      return 192; // À - A accent
    case 132:
      return 196; // Ä - A_umlaut (D, SM, OH, ...)
    case 145:
      return 209; // Ñ - (EA)
    case 150:
      return 214; // Ö – O_umlaut  (D, SM, OH, ...)
    case 146:
      return 211; // Ò - O accent
    case 156:
      return 220; // Ü - U_umlaut     (D, ...)
    case 135:
      return 199; // Ç
    case 144:
      return 208; // Ð
    case 136:
      return 200; // È
    case 137:
      return 201; // É
    }

    if (first_byte == 197)
    {
      switch (second_byte)
      {
      case 189:
        return 142; // Ž
      }
    }
  }

  return (0);
}
#endif

//-------------------------------------------------------------------------------------------------------

void initialize_sd_card()
{

#if defined(FEATURE_SD_CARD_SUPPORT)
  if (!SD.begin(sd_card_spi_ss_line))
  {
#if defined(DEBUG_SD_CARD)
    debug_serial_port->println(F("initialize_sd_card: initialization failed"));
#endif
    return;
  }

  sd_card_state = SD_CARD_AVAILABLE;

  // This causes a problem with directory listing...
  // if (!SD.exists("/keyer")){
  //  SD.mkdir("/keyer");
  //   #if defined(DEBUG_SD_CARD)
  //     debug_serial_port->println(F("initialize_sd_card: created /keyer"));
  //   #endif
  // }

  if (SD.exists("/keyer/beacon.txt"))
  {
    sd_card_state = SD_CARD_AVAILABLE_BEACON_FILE_FOUND;
  }

#if defined(DEBUG_SD_CARD)
  debug_serial_port->println(F("initialize_sd_card: initialization done"));
#endif

#endif // FEATURE_SD_CARD_SUPPORT
}

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_SD_CARD_SUPPORT)
void service_sd_card()
{

  static unsigned long last_sd_log_file_save = 0;

  if (sd_card_state == SD_CARD_AVAILABLE_BEACON_FILE_RUNNING)
  {
    if (dit_buffer || dah_buffer)
    {
      sd_card_state = SD_CARD_AVAILABLE;
      sdfile.close();
    }
    else
    {
      if (send_buffer_bytes == 0)
      {
        if (sdfile.available())
        {
          add_to_send_buffer(uppercase(sdfile.read()));
        }
        else
        {
          sdfile.seek(0);
        }
      }
    }
  }

  if (sd_card_state == SD_CARD_AVAILABLE_BEACON_FILE_FOUND)
  {
    sdfile = SD.open("/keyer/beacon.txt");
    if (sdfile)
    {
      sd_card_state = SD_CARD_AVAILABLE_BEACON_FILE_RUNNING;
    }
    else
    {
      sd_card_state = SD_CARD_ERROR_;
    }
  }

  if ((sd_card_log_state == SD_CARD_LOG_OPEN) && ((millis() - last_sd_log_file_save) > 60000))
  {
    sdlogfile.flush();
    last_sd_log_file_save = millis();
  }
}
#endif // FEATURE_SD_CARD_SUPPORT
//-------------------------------------------------------------------------------------------------------
byte is_visible_character(byte char_in)
{

  if ((char_in > 31) || (char_in == 9) || (char_in == 10) || (char_in == 13))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

//-------------------------------------------------------------------------------------------------------
#if defined(FEATURE_SD_CARD_SUPPORT)
void sd_card_log(String string_to_log, byte byte_to_log)
{

  char logchar[10];

  if (sd_card_log_state == SD_CARD_LOG_OPEN)
  {
    if (string_to_log.length() > 0)
    {
      string_to_log.toCharArray(logchar, 9);
      sdlogfile.print(logchar);
    }
    else
    {
      if (is_visible_character(byte_to_log))
      {
        sdlogfile.write(byte_to_log);
      }
    }
  }

  if ((sd_card_log_state == SD_CARD_LOG_NOT_OPEN) && (sd_card_state == SD_CARD_AVAILABLE))
  {
    sdlogfile = SD.open("/keyer/keyer.log", FILE_WRITE);
    if (!sdlogfile)
    {
      sd_card_log_state = SD_CARD_LOG_ERROR;
    }
    else
    {
      sd_card_log_state = SD_CARD_LOG_OPEN;
      sdlogfile.println("\r\nstart of log ");
      if (configuration.cli_mode == CLI_MILL_MODE_PADDLE_SEND)
      {
        sdlogfile.print("TX:");
        sdlogfile.flush();
      }
      else
      {
        sdlogfile.print("RX:");
        sdlogfile.flush();
      }
    }
  }
}
#endif // FEATURE_SD_CARD_SUPPORT

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_SO2R_BASE
void so2r_set_tx()
{
  if (so2r_tx == SO2R_TX_1)
  {
    if (so2r_tx_1)
    {
      digitalWrite(so2r_tx_1, HIGH);
    }

    if (so2r_tx_2)
    {
      digitalWrite(so2r_tx_2, LOW);
    }

    current_tx_ptt_line = ptt_tx_1;
    current_tx_key_line = tx_key_line_1;
  }
  else
  {
    if (so2r_tx_1)
    {
      digitalWrite(so2r_tx_1, LOW);
    }

    if (so2r_tx_2)
    {
      digitalWrite(so2r_tx_2, HIGH);
    }

    if (ptt_tx_2)
    {
      current_tx_ptt_line = ptt_tx_2;
    }
    else
    {
      current_tx_ptt_line = ptt_tx_1;
    }
    current_tx_key_line = tx_key_line_2;
  }
}

//-------------------------------------------------------------------------------------------------------

void so2r_set_rx()
{
  uint8_t rx;
  if (so2r_latch && (so2r_ptt || (ptt_line_activated && (sending_mode == AUTOMATIC_SENDING))))
  {
    if (so2r_tx == 1)
    {
      rx = 2;
    }
    else
    {
      rx = 1;
    }
  }
  else
  {
    rx = so2r_rx;
  }

  switch (rx)
  {
  case SO2R_RX_1:
    // Receive on radio 1 only
    if (so2r_rx_1)
    {
      digitalWrite(so2r_rx_1, HIGH);
    }

    if (so2r_rx_1s)
    {
      digitalWrite(so2r_rx_1s, HIGH);
    }

    if (so2r_rx_2)
    {
      digitalWrite(so2r_rx_2, LOW);
    }

    if (so2r_rx_2s)
    {
      digitalWrite(so2r_rx_2s, LOW);
    }

    if (so2r_rx_s)
    {
      digitalWrite(so2r_rx_s, LOW);
    }
    break;

  case SO2R_RX_2:
    // Receive on radio 2 only
    if (so2r_rx_1)
    {
      digitalWrite(so2r_rx_1, LOW);
    }

    if (so2r_rx_1s)
    {
      digitalWrite(so2r_rx_1s, LOW);
    }

    if (so2r_rx_2)
    {
      digitalWrite(so2r_rx_2, HIGH);
    }

    if (so2r_rx_2s)
    {
      digitalWrite(so2r_rx_2s, HIGH);
    }

    if (so2r_rx_s)
    {
      digitalWrite(so2r_rx_s, LOW);
    }
    break;

  case SO2R_RX_S:
  case SO2R_RX_R:
    // Receive on radio 1 and 2 (stereo)
    if (so2r_rx_1)
    {
      digitalWrite(so2r_rx_1, LOW);
    }

    if (so2r_rx_1s)
    {
      digitalWrite(so2r_rx_1s, HIGH);
    }

    if (so2r_rx_2)
    {
      digitalWrite(so2r_rx_2, LOW);
    }

    if (so2r_rx_2s)
    {
      digitalWrite(so2r_rx_2s, HIGH);
    }

    if (so2r_rx_s)
    {
      digitalWrite(so2r_rx_s, HIGH);
    }
    break;
  }
}

//-------------------------------------------------------------------------------------------------------

void so2r_command()
{
  if ((incoming_serial_byte & 0xf0) == 0x90)
  {
    // 0 is RX 1
    // 1 is RX 2
    // 2 is RX 1 and RX2 stereo
    // 3 is RX 1 and RX2 stereo (reverse if possible but this box doesn't have that capability)
    so2r_rx = (incoming_serial_byte & 3) + 1;
    so2r_set_rx();

    byte tx = SO2R_TX_1;
    if (incoming_serial_byte & 4)
    {
      tx = SO2R_TX_2;
    }

    // Don't switch transmitter while transmitting.
    if (tx == so2r_tx)
    {
      so2r_pending_tx = 0;
    }
    else
    {
      if (ptt_line_activated)
      {
        so2r_pending_tx = tx;
#ifdef FEATURE_WINKEY_EMULATION
        if (winkey_sending && winkey_host_open)
        {
          // Fake a paddle interrupt to stop computer sending
          winkey_port_write(0xc2 | winkey_sending | winkey_xoff, 0); // 0xc2 - BREAKIN bit set high
          winkey_interrupted = 1;
        }
#endif
      }
      else
      {
        so2r_tx = tx;
        so2r_set_tx();
      }
    }
    return;
  }

#ifdef FEATURE_SO2R_ANTENNA
  if ((incoming_serial_byte & 0xf0) == 0xa0)
  {
    so2r_antenna_1 = incoming_serial_byte & 0x0f;
    // TBD:  Provide antenna information outputs
    return;
  }

  if ((incoming_serial_byte & 0xf0) == 0xb0)
  {
    so2r_antenna_2 = incoming_serial_byte & 0x0f;
    // TBD:  Provide antenna information outputs
    return;
  }
#endif // FEATURE_SO2R_ANTENNA

  switch (incoming_serial_byte)
  {
  case 0x80: // SO2R Close
#ifdef FEATURE_SO2R_SWITCHES
    so2r_open = 0;
    so2r_debounce = 0;
    so2r_latch = 0;
#endif
    break;

  case 0x81: // SO2R Open
#ifdef FEATURE_SO2R_SWITCHES
    so2r_open = 1;
#endif
    break;

  case 0x82: // PTT Off
    so2r_ptt = 0;
    break;

  case 0x83: // PTT On
    so2r_ptt = 1;
    ptt_key();
    break;

  case 0x84: // Latch Off
    so2r_latch = 0;
    so2r_set_rx();
    break;

  case 0x85: // Latch On
    so2r_latch = 1;
    so2r_set_rx();
    break;
  }
}

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_SO2R_SWITCHES
void so2r_switches()
{
  if (so2r_open)
  {
    return;
  }

  if (so2r_debounce)
  {
    if ((so2r_debounce_time - millis()) < 20)
    {
      return;
    }
    so2r_debounce = 0;
  }

  if (so2r_tx_switch)
  {
    uint8_t tx = 1;
    if (digitalRead(so2r_tx_switch) != LOW)
    {
      tx = 2;
    }
    if (tx != so2r_tx)
    {
      if (ptt_line_activated)
      {
#ifdef FEATURE_WINKEY_EMULATION
        if (winkey_sending && winkey_host_open)
        {
          // Fake a paddle interrupt to stop computer sending
          winkey_port_write(0xc2 | winkey_sending | winkey_xoff, 0); // 0xc2 - BREAKIN bit set high
          winkey_interrupted = 1;
        }
#endif
      }
      else
      {
        so2r_tx = tx;
        so2r_set_tx();
        so2r_debounce_time = millis();
        so2r_debounce = 1;
      }
    }

    if (so2r_rx1_switch)
    {
      uint8_t rx = 1; // RX 1
      if (digitalRead(so2r_rx1_switch) != LOW)
      {
        if (so2r_rx2_switch && (digitalRead(so2r_rx2_switch) != LOW))
        {
          rx = 3; // Stereo
        }
        else
        {
          rx = 2; // RX 2
        }
      }

      if (rx != so2r_rx)
      {
        so2r_rx = rx;
        so2r_set_rx();
        so2r_debounce_time = millis();
        so2r_debounce = 1;
      }
    }
  }
}
#endif // FEATURE_SO2R_SWITCHES
#endif // FEATURE_SO2R_BASE

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_MIDI

void midi_setup()
{
  // set callback for commands
  usbMIDI.setHandleControlChange(myControlChange);
}

void midi_key_tx(int state)
{
  if (state)
  {
    usbMIDI.sendNoteOn(OPTION_MIDI_BASE_NOTE + 1, 99, OPTION_MIDI_KEYER_CHANNEL);
  }
  else
  {
    usbMIDI.sendNoteOff(OPTION_MIDI_BASE_NOTE + 1, 0, OPTION_MIDI_KEYER_CHANNEL);
  }
}

void midi_key_ptt(int state)
{
  if (state)
  {
    usbMIDI.sendNoteOn(OPTION_MIDI_BASE_NOTE + 0, 99, OPTION_MIDI_KEYER_CHANNEL);
  }
  else
  {
    usbMIDI.sendNoteOff(OPTION_MIDI_BASE_NOTE + 0, 0, OPTION_MIDI_KEYER_CHANNEL);
  }
}

// MIDI callback
void myControlChange(byte channel, byte control, byte value)
{
  // debug
  // usbMIDI.sendNoteOn(OPTION_MIDI_BASE_NOTE+1, 99, OPTION_MIDI_KEYER_CHANNEL);
  // delay(100);
  // usbMIDI.sendNoteOff(OPTION_MIDI_BASE_NOTE+1, 0, OPTION_MIDI_KEYER_CHANNEL);
  // end debug

  int ok = 1;

  if (channel != OPTION_MIDI_INPUT_CHANNEL)
  {
    // error, unexpected channel
    sendMidiResponseOk(0);
    return;
  }

  switch (control)
  {
  case OPTION_MIDI_IS_KEYER_CONTROL:
    // no switching in this Sketch
    sendMidiResponseOk(0);
    //      if (value > 0) {
    //        setupIambic(1);
    //      } else {
    //        setupIambic(0);
    //      }
    break;
  case OPTION_MIDI_IAMBIC_CONTROL:
    if (value > 0)
    {
      setupIambicMode(1);
    }
    else
    {
      setupIambicMode(0);
    }
    break;
  case OPTION_MIDI_WPM_CONTROL:
    speed_set(value);
#ifdef FEATURE_WINKEY_EMULATION
    winkey_port_write(((value - pot_wpm_low_value) | 128), 0);
#endif
    break;
  case OPTION_MIDI_REVERSE_CONTROL:
    if (value > 0)
    {
      configuration.paddle_mode = PADDLE_REVERSE;
    }
    else
    {
      configuration.paddle_mode = PADDLE_NORMAL;
    }
    break;
  case OPTION_MIDI_GET_KEYER_STATE_CONTROL:
    sendKeyerStateResponse();
    break;
  default:
    // unknown command
    sendMidiResponseOk(0);
  }
  if (ok == 1)
  {
    sendMidiResponseOk(1);
  }
  else
  {
    sendMidiResponseOk(0);
  }
}

void setupIambicMode(int modeB)
{
  if (modeB == 1)
  {
    configuration.keyer_mode = IAMBIC_B;
  }
  else
  {
    configuration.keyer_mode = IAMBIC_A;
  }
  config_dirty = 1;
}

void sendMidiResponseOk(int ok)
{
  if (ok == 1)
  {
    // ok
    usbMIDI.sendControlChange(OPTION_MIDI_RESPONSE_OK, 99, OPTION_MIDI_RESPONSE_CHANNEL);
  }
  else
  {
    // error
    usbMIDI.sendControlChange(OPTION_MIDI_RESPONSE_FAIL, 0, OPTION_MIDI_RESPONSE_CHANNEL);
  }
}

void sendKeyerStateResponse()
{
  usbMIDI.sendControlChange(OPTION_MIDI_RESPONSE_IS_KEYER, 2, OPTION_MIDI_RESPONSE_CHANNEL);
  usbMIDI.sendControlChange(OPTION_MIDI_RESPONSE_WPM, configuration.wpm, OPTION_MIDI_RESPONSE_CHANNEL);
  usbMIDI.sendControlChange(OPTION_MIDI_RESPONSE_REVERSE, (configuration.paddle_mode == PADDLE_REVERSE ? 1 : 0), OPTION_MIDI_RESPONSE_CHANNEL);
  if (configuration.keyer_mode == IAMBIC_B)
  {
    usbMIDI.sendControlChange(OPTION_MIDI_RESPONSE_IAMBIC, 1, OPTION_MIDI_RESPONSE_CHANNEL);
  }
  else if (configuration.keyer_mode == IAMBIC_A)
  {
    usbMIDI.sendControlChange(OPTION_MIDI_RESPONSE_IAMBIC, 0, OPTION_MIDI_RESPONSE_CHANNEL);
  }
}

void midi_send_wpm_response()
{
  usbMIDI.sendControlChange(OPTION_MIDI_RESPONSE_WPM, configuration.wpm, OPTION_MIDI_RESPONSE_CHANNEL);
}

#endif // FEATURE_MIDI
//-------------------------------------------------------------------------------------------------------

// DL2DBG contributed code (adapted into code by Goody K3NG)
// Based on https://forum.arduino.cc/index.php?topic=446209.15

// #if defined(FEATURE_SINEWAVE_SIDETONE)

// void compute_sinetone(int hz, int volume){ //dl2dbg

//   omega = 2*pi*hz ;
//   c1 = (8.0 - 2.0*pow(omega*T/1000000.0,2))/(4.0+pow(omega*T/1000000.0,2));
//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_1)
//     Timer1.detachInterrupt();
//   #endif
//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_3)
//     Timer3.detachInterrupt();
//   #endif
//   a[0]= 0.0;
//   a[1]= volume*sin(omega*T/1000000.0);
//   a[2]= 0.0;
//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_1)
//     Timer1.attachInterrupt(sinewave_interrupt_compute);
//   #endif
//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_3)
//     Timer3.attachInterrupt(sinewave_interrupt_compute);
//   #endif

// }
// #endif //FEATURE_SINEWAVE_SIDETONE

//-------------------------------------------------------------------------------------------------------

// #if defined(FEATURE_SINEWAVE_SIDETONE)

// void sinewave_interrupt_compute(){ //dl2dbg

//   a[2] = c1 * a[1] - a[0];
//   a[0] = a[1] ;
//   a[1] = a[2] ;
//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_1)
//     Timer1.setPwmDuty(sidetone_line, map( a[2],-512, 512, 0, 1000));
//   #endif
//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_3)
//     Timer3.setPwmDuty(sidetone_line, map( a[2],-512, 512, 0, 1000));
//   #endif

// }
// #endif //FEATURE_SINEWAVE_SIDETONE

//-------------------------------------------------------------------------------------------------------
// #if defined(FEATURE_SINEWAVE_SIDETONE)

// void initialize_tonsin(){ //dl2dbg

//   //configuration.sidetone_volume = sidetone_volume_low_limit + ((sidetone_volume_high_limit - sidetone_volume_low_limit) / 2);
//   compute_sinetone(configuration.hz_sidetone,configuration.sidetone_volume);
//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_1)
//     Timer1.initialize(T);  // set sample time for discrete tone signal
//     Timer1.pwm(sidetone_line, 0, T);
//     Timer1.attachInterrupt(sinewave_interrupt_compute);
//     Timer1.stop();
//   #endif
//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_3)
//     Timer3.initialize(T);  // set sample time for discrete tone signal
//     Timer3.pwm(sidetone_line, 0, T);
//     Timer3.attachInterrupt(sinewave_interrupt_compute);
//     Timer3.stop();
//   #endif

// }
// #endif //FEATURE_SINEWAVE_SIDETONE
//-------------------------------------------------------------------------------------------------------

// #if defined(FEATURE_SINEWAVE_SIDETONE)
// void sinetone(uint8_t pin_dummy_variable, unsigned short freq){  //dl2dbg

//   static int last_freq;
//   static int last_volume;

//   if ((freq != last_freq) || (configuration.sidetone_volume != last_volume)){
//     compute_sinetone(freq,configuration.sidetone_volume);
//     last_freq = freq;
//     last_volume = configuration.sidetone_volume;
//   }

//   //delay (2); compute_sinetone(freq,sidetone_volume/4);
//   //delay (2); compute_sinetone(freq,sidetone_volume/2);
//   //compute_sinetone(freq,configuration.sidetone_volume);

//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_1)
//     Timer1.restart();
//   #endif
//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_3)
//     Timer3.restart();
//   #endif

// }
// #endif //FEATURE_SINEWAVE_SIDETONE

//-------------------------------------------------------------------------------------------------------

// #if defined(FEATURE_SINEWAVE_SIDETONE)
// void nosineTone(uint8_t pin_dummy_variable){    // disable tone on specified pin, if any    dl2dbg

//   //delay (2); compute_sinetone(freq,sidetone_volume/2);
//   //delay (2); compute_sinetone(freq,sidetone_volume/4);
//   // compute_sinetone(configuration.hz_sidetone,0);

//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_1)
//     Timer1.stop();
//   #endif
//   #if defined(FEATURE_SINEWAVE_SIDETONE_USING_TIMER_3)
//     Timer3.stop();
//   #endif

//   //digitalWrite(sidetone_line,LOW);

// }

// #endif //FEATURE_SINEWAVE_SIDETONE

//-------------------------------------------------------------------------------------------------------

void debug_blink()
{
#if defined(DEBUG_STARTUP_BLINKS)
  digitalWrite(PIN_LED, HIGH);
  delay(250);
  digitalWrite(PIN_LED, LOW);
  delay(1000);
#endif // DEBUG_STARTUP
}

//-------------------------------------------------------------------------------------------------------

//
//
// Congratulations.  You've gotten to the end.  But this is just the beginning.
//
//

/*



  #ifdef FEATURE_CLOCK
    int temp_year = 0;
    byte temp_month = 0;
    byte temp_day = 0;
    byte temp_minute = 0;
    byte temp_hour = 0;
    byte negative_flag = 0;
  #endif // FEATURE_CLOCK



    #ifdef FEATURE_CLOCK
    case 'C':         // show clock
      update_time();
      sprintf(return_string, "%s", timezone_modified_clock_string());


      break;
    case 'O':         // set clock UTC time
      temp_year = ((input_buffer[2] - 48) * 1000) + ((input_buffer[3] - 48) * 100) + ((input_buffer[4] - 48) * 10) + (input_buffer[5] - 48);
      temp_month = ((input_buffer[6] - 48) * 10) + (input_buffer[7] - 48);
      temp_day = ((input_buffer[8] - 48) * 10) + (input_buffer[9] - 48);
      temp_hour = ((input_buffer[10] - 48) * 10) + (input_buffer[11] - 48);
      temp_minute = ((input_buffer[12] - 48) * 10) + (input_buffer[13] - 48);
      if ((temp_year > 2013) && (temp_year < 2070) &&
          (temp_month > 0) && (temp_month < 13) &&
          (temp_day > 0) && (temp_day < 32) &&
          (temp_hour >= 0) && (temp_hour < 24) &&
          (temp_minute >= 0) && (temp_minute < 60) &&
          (input_buffer_index == 14)) {

        clock_year_set = temp_year;
        clock_month_set = temp_month;
        clock_day_set = temp_day;
        clock_hour_set = temp_hour;
        clock_min_set = temp_minute;
        clock_sec_set = 0;
        millis_at_last_calibration = millis();

        #if defined(FEATURE_RTC_DS1307)
        rtc.adjust(DateTime(temp_year, temp_month, temp_day, temp_hour, temp_minute, 0));
        #endif // defined(FEATURE_RTC_DS1307)
        #if defined(FEATURE_RTC_PCF8583)
        rtc.year = temp_year;
        rtc.month = temp_month;
        rtc.day = temp_day;
        rtc.hour  = temp_hour;
        rtc.minute = temp_minute;
        rtc.second = 0;
        rtc.set_time();
        #endif // defined(FEATURE_RTC_PCF8583)

        #if (!defined(FEATURE_RTC_DS1307) && !defined(FEATURE_RTC_PCF8583))
        strcpy(return_string, "Clock set to ");
        update_time();
        strcat(return_string, timezone_modified_clock_string());
        #else
        strcpy(return_string, "Internal clock and RTC set to ");
        update_time();
        strcat(return_string, timezone_modified_clock_string());
        #endif
      } else {
        strcpy(return_string, "Error. Usage: \\OYYYYMMDDHHmm");
      }
      break;

    case 'V': //  \Vx[xxx][.][xxxx]   Set time zone offset
      negative_flag = 0;
      place_multiplier = 1;
      for (int x = input_buffer_index - 1; x > 1; x--) {
        if (char(input_buffer[x]) == '-') {
          negative_flag = 1;
        } else {
          if (char(input_buffer[x]) != '.') {
            tempfloat += (input_buffer[x] - 48) * place_multiplier;
            place_multiplier = place_multiplier * 10;
          } else {
            decimalplace = x;
          }
        }
      }
      if (decimalplace) {
        tempfloat = tempfloat / pow(10, (input_buffer_index - decimalplace - 1));
      }
      if (negative_flag){tempfloat = tempfloat * -1.0;}
      if ((tempfloat >= -24.0) && (tempfloat <= 24.0)) {
        configuration.clock_timezone_offset = tempfloat;
        configuration_dirty = 1;
        strcpy(return_string, "Timezone offset set to ");
        dtostrf(tempfloat, 0, 2, temp_string);
        strcat(return_string, temp_string);
      } else {
        strcpy(return_string, "Error.");
      }
      break;
    #endif // FEATURE_CLOCK

// --------------------------------------------------------------

#ifdef FEATURE_CLOCK
char * timezone_modified_clock_string(){

  static char return_string[32] = "";
  char temp_string[16] = "";


  dtostrf(local_clock_years, 0, 0, temp_string);
  strcpy(return_string, temp_string);
  strcat(return_string, "-");
  if (local_clock_months < 10) {
    strcat(return_string, "0");
  }
  dtostrf(local_clock_months, 0, 0, temp_string);
  strcat(return_string, temp_string);
  strcat(return_string, "-");
  if (local_clock_days < 10) {
    strcat(return_string, "0");
  }
  dtostrf(local_clock_days, 0, 0, temp_string);
  strcat(return_string, temp_string);
  strcat(return_string, " ");

  if (local_clock_hours < 10) {
    strcat(return_string, "0");
  }
  dtostrf(local_clock_hours, 0, 0, temp_string);
  strcat(return_string, temp_string);
  strcat(return_string, ":");
  if (local_clock_minutes < 10) {
    strcat(return_string, "0");
  }
  dtostrf(local_clock_minutes, 0, 0, temp_string);
  strcat(return_string, temp_string);
  strcat(return_string, ":");
  if (local_clock_seconds < 10) {
    strcat(return_string, "0");
  }
  dtostrf(local_clock_seconds, 0, 0, temp_string);
  strcat(return_string, temp_string);
  if (configuration.clock_timezone_offset == 0){
    strcat(return_string,"Z");
  }
  return return_string;

} // clock_string
#endif // FEATURE_CLOCK

// --------------------------------------------------------------

#ifdef FEATURE_CLOCK
char * zulu_clock_string(){

  static char return_string[32] = "";
  char temp_string[16] = "";


  dtostrf(clock_years, 0, 0, temp_string);
  strcpy(return_string, temp_string);
  strcat(return_string, "-");
  if (clock_months < 10) {
    strcat(return_string, "0");
  }
  dtostrf(clock_months, 0, 0, temp_string);
  strcat(return_string, temp_string);
  strcat(return_string, "-");
  if (clock_days < 10) {
    strcat(return_string, "0");
  }
  dtostrf(clock_days, 0, 0, temp_string);
  strcat(return_string, temp_string);
  strcat(return_string, " ");

  if (clock_hours < 10) {
    strcat(return_string, "0");
  }
  dtostrf(clock_hours, 0, 0, temp_string);
  strcat(return_string, temp_string);
  strcat(return_string, ":");
  if (clock_minutes < 10) {
    strcat(return_string, "0");
  }
  dtostrf(clock_minutes, 0, 0, temp_string);
  strcat(return_string, temp_string);
  strcat(return_string, ":");
  if (clock_seconds < 10) {
    strcat(return_string, "0");
  }
  dtostrf(clock_seconds, 0, 0, temp_string);
  strcat(return_string, temp_string);
  strcat(return_string,"Z");
  return return_string;

} // zulu_clock_string
#endif // FEATURE_CLOCK

// --------------------------------------------------------------

#ifdef FEATURE_CLOCK
void update_time(){
  unsigned long runtime = millis() - millis_at_last_calibration;

  // calculate UTC

  unsigned long time = (3600L * clock_hour_set) + (60L * clock_min_set) + clock_sec_set + ((runtime + (runtime * INTERNAL_CLOCK_CORRECTION)) / 1000.0);

  clock_years = clock_year_set;
  clock_months = clock_month_set;
  clock_days = time / 86400L;
  time -= clock_days * 86400L;
  clock_days += clock_day_set;
  clock_hours = time / 3600L;

  switch (clock_months) {

    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      if (clock_days > 31) {
        clock_days = 1; clock_months++;
      }
      break;

    case 2:
      if ((float(clock_years) / 4.0) == 0.0) {  // do we have a leap year?
        if (clock_days > 29) {
          clock_days = 1; clock_months++;
        }
      } else {
        if (clock_days > 28) {
          clock_days = 1; clock_months++;
        }
      }
      break;

    case 4:
    case 6:
    case 9:
    case 11:
      if (clock_days > 30) {
        clock_days = 1; clock_months++;
      }
      break;
  } // switch

  if (clock_months > 12) {
    clock_months = 1; clock_years++;
  }

  time -= clock_hours * 3600L;
  clock_minutes  = time / 60L;
  time -= clock_minutes * 60L;
  clock_seconds = time;


  // calculate local time

  long local_time = (configuration.clock_timezone_offset * 60L * 60L) + (3600L * clock_hour_set) + (60L * clock_min_set) + clock_sec_set + ((runtime + (runtime * INTERNAL_CLOCK_CORRECTION)) / 1000.0);

  local_clock_years = clock_year_set;
  local_clock_months = clock_month_set;
  local_clock_days = clock_day_set;

  if (local_time < 0){
    local_time = local_time + (24L * 60L * 60L) - 1;
    local_clock_days--;
    if (local_clock_days < 1){
      local_clock_months--;
      switch (local_clock_months) {
        case 0:
          local_clock_months = 12;
          local_clock_days = 31;
          local_clock_years--;
          break;
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
          local_clock_days = 31;
          break;
        case 2: //February
          if ((float(local_clock_years) / 4.0) == 0.0) {  // do we have a leap year?
            local_clock_days = 29;
          } else {
            local_clock_days = 28;
          }
          break;
        case 4:
        case 6:
        case 9:
        case 11:
          local_clock_days = 30;
          break;
      } // switch
    }
    local_clock_hours = local_time / 3600L;
    local_time -= local_clock_hours * 3600L;
    local_clock_minutes  = local_time / 60L;
    local_time -= local_clock_minutes * 60L;
    local_clock_seconds = local_time;

  } else {  //(local_time < 0)

    local_clock_days = local_time / 86400L;
    local_time -= local_clock_days * 86400L;
    local_clock_days += clock_day_set;
    local_clock_hours = local_time / 3600L;

    switch (local_clock_months) {

      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
      case 12:
        if (local_clock_days > 31) {
          local_clock_days = 1;
          local_clock_months++;
        }
        break;

      case 2:
        if ((float(local_clock_years) / 4.0) == 0.0) {  // do we have a leap year?
          if (local_clock_days > 29) {
            local_clock_days = 1;
            local_clock_months++;
          }
        } else {
          if (local_clock_days > 28) {
            local_clock_days = 1;
            local_clock_months++;
          }
        }
        break;

      case 4:
      case 6:
      case 9:
      case 11:
        if (local_clock_days > 30) {
          local_clock_days = 1;
          local_clock_months++;
        }
        break;
    } // switch

    if (local_clock_months > 12) {
      local_clock_months = 1;
      local_clock_years++;
    }

    local_time -= local_clock_hours * 3600L;
    local_clock_minutes  = local_time / 60L;
    local_time -= local_clock_minutes * 60L;
    local_clock_seconds = local_time;


  }  //(local_time < 0)


} // update_time
#endif // FEATURE_CLOCK
// --------------------------------------------------------------
*/
