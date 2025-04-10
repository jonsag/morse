
#ifdef FEATURE_WINKEY_EMULATION
void winkey_ptt_times_parm1_command(byte incoming_serial_byte)
{
#if !defined(DEBUG_WINKEY_DISABLE_LEAD_IN_TIME_SETTING)
    configuration.ptt_lead_time[configuration.current_tx - 1] = (incoming_serial_byte * 10);
#else
    configuration.ptt_lead_time[configuration.current_tx - 1] = 0;
#endif
#ifdef DEBUG_WINKEY_PROTOCOL_USING_CW
    send_char('P', KEYER_NORMAL);
    send_char('1', KEYER_NORMAL);
#endif
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_port_write(byte byte_to_send, byte override_filter)
{

#ifdef DEBUG_WINKEY_PORT_WRITE
    if ((byte_to_send > 4) && (byte_to_send < 31))
    {
        boop();
        delay(500);
        boop();
        delay(500);
        boop();
        // return;
    }
#endif

    if (((byte_to_send > 4) && (byte_to_send < 31)) && (!override_filter))
    {
#ifdef DEBUG_WINKEY
        debug_serial_port->print("Winkey Port TX: FILTERED: ");
        if ((byte_to_send > 31) && (byte_to_send < 127))
        {
            debug_serial_port->write(byte_to_send);
        }
        else
        {
            debug_serial_port->print(".");
        }
        debug_serial_port->print(" [");
        debug_serial_port->print(byte_to_send);
        debug_serial_port->print("] [0x");
        debug_serial_port->print(byte_to_send, HEX);
        debug_serial_port->println("]");
#endif
        return;
    }

    primary_serial_port->write(byte_to_send);
#ifdef DEBUG_WINKEY
    debug_serial_port->print("Winkey Port TX: ");
    if ((byte_to_send > 31) && (byte_to_send < 127))
    {
        debug_serial_port->write(byte_to_send);
    }
    else
    {
        debug_serial_port->print(".");
    }
    debug_serial_port->print(" [");
    debug_serial_port->print(byte_to_send);
    debug_serial_port->print("] [0x");
    debug_serial_port->print(byte_to_send, HEX);
    debug_serial_port->println("]");
#endif
}

#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_setmode_command(byte incoming_serial_byte)
{

    config_dirty = 1;

    if (incoming_serial_byte & 4)
    { // serial echo enable
#ifdef DEBUG_WINKEY_PROTOCOL_USING_CW
        send_char('S', KEYER_NORMAL);
#endif
        winkey_serial_echo = 1;
    }
    else
    {
        winkey_serial_echo = 0;
    }
    if (incoming_serial_byte & 8)
    { // paddle_swap
        configuration.paddle_mode = PADDLE_REVERSE;
    }
    else
    {
        configuration.paddle_mode = PADDLE_NORMAL;
    }
    switch (incoming_serial_byte & 48)
    {
    case 0:
        configuration.keyer_mode = IAMBIC_B;
#ifdef DEBUG_WINKEY_PROTOCOL_USING_CW
        send_char('B', KEYER_NORMAL);
#endif
        break;
    case 16:
        configuration.keyer_mode = IAMBIC_A;
#ifdef DEBUG_WINKEY_PROTOCOL_USING_CW
        send_char('A', KEYER_NORMAL);
#endif
        break;
    case 32:
        configuration.keyer_mode = ULTIMATIC;
#ifdef DEBUG_WINKEY_PROTOCOL_USING_CW
        send_char('U', KEYER_NORMAL);
#endif
        break;
    case 48:
        configuration.keyer_mode = BUG;
#ifdef DEBUG_WINKEY_PROTOCOL_USING_CW
        send_char('G', KEYER_NORMAL);
#endif
        break;
    }
#ifdef FEATURE_DEAD_OP_WATCHDOG
    if ((incoming_serial_byte & 128) == 128)
    { // 1xxxxxxx = paddle watchdog (1 = disable)
        dead_op_watchdog_active = 0;
    }
    else
    {
        dead_op_watchdog_active = 1;
    }
#endif
#ifdef FEATURE_AUTOSPACE
    if ((incoming_serial_byte & 2) == 2)
    { // xxxxxx1x = autospace
        configuration.autospace_active = 1;
#ifdef DEBUG_WINKEY_PROTOCOL_USING_CW
        send_char('T', KEYER_NORMAL);
#endif
    }
    else
    {
        configuration.autospace_active = 0;
    }
#endif
    if ((incoming_serial_byte & 1) == 1)
    { // xxxxxxx1 = contest wordspace
        configuration.length_wordspace = 6;
    }
    else
    {
        configuration.length_wordspace = 7;
    }

    if ((incoming_serial_byte & 64) == 64)
    { // x1xxxxxx = paddle echo
        winkey_paddle_echo_activated = 1;
    }
    else
    {
        winkey_paddle_echo_activated = 0;
    }
}

#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_unbuffered_speed_command(byte incoming_serial_byte)
{

    if (incoming_serial_byte == 0)
    {
#ifdef FEATURE_POTENTIOMETER
        configuration.pot_activated = 1;
#endif
    }
    else
    {
        configuration.wpm = incoming_serial_byte;
        winkey_speed_state = WINKEY_UNBUFFERED_SPEED;
        winkey_last_unbuffered_speed_wpm = configuration.wpm;
// calculate_element_length();
#ifdef OPTION_WINKEY_STRICT_EEPROM_WRITES_MAY_WEAR_OUT_EEPROM
        config_dirty = 1;
#endif

#ifdef FEATURE_LED_RING
        update_led_ring();
#endif // FEATURE_LED_RING
    }
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------
#ifdef FEATURE_WINKEY_EMULATION
void winkey_farnsworth_command(byte incoming_serial_byte)
{

#ifdef FEATURE_FARNSWORTH
    if ((incoming_serial_byte > 9) && (incoming_serial_byte < 100))
    {
        configuration.wpm_farnsworth = incoming_serial_byte;
    }
#else
    (void)incoming_serial_byte; // to get rid of compiler warning about unused variable
#endif // FEATURE_FFARNSWORTH
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_sidetone_freq_command(byte incoming_serial_byte)
{

#ifdef OPTION_WINKEY_2_SUPPORT
    if (incoming_serial_byte & 128)
    {
        if (configuration.sidetone_mode == SIDETONE_ON)
        {
            configuration.sidetone_mode = SIDETONE_PADDLE_ONLY;
        }
        wk2_paddle_only_sidetone = 1;
    }
    else
    {
        if (configuration.sidetone_mode == SIDETONE_PADDLE_ONLY)
        {
            configuration.sidetone_mode = SIDETONE_ON;
        }
        wk2_paddle_only_sidetone = 0;
    }
#endif

    switch (incoming_serial_byte & 15)
    {
    case 1:
        configuration.hz_sidetone = WINKEY_SIDETONE_1;
        break;
    case 2:
        configuration.hz_sidetone = WINKEY_SIDETONE_2;
        break;
    case 3:
        configuration.hz_sidetone = WINKEY_SIDETONE_3;
        break;
    case 4:
        configuration.hz_sidetone = WINKEY_SIDETONE_4;
        break;
    case 5:
        configuration.hz_sidetone = WINKEY_SIDETONE_5;
        break;
    case 6:
        configuration.hz_sidetone = WINKEY_SIDETONE_6;
        break;
    case 7:
        configuration.hz_sidetone = WINKEY_SIDETONE_7;
        break;
    case 8:
        configuration.hz_sidetone = WINKEY_SIDETONE_8;
        break;
    case 9:
        configuration.hz_sidetone = WINKEY_SIDETONE_9;
        break;
    case 10:
        configuration.hz_sidetone = WINKEY_SIDETONE_10;
        break;
    }
#ifdef OPTION_WINKEY_STRICT_EEPROM_WRITES_MAY_WEAR_OUT_EEPROM
    config_dirty = 1;
#endif
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_weighting_command(byte incoming_serial_byte)
{

    if ((incoming_serial_byte > 9) && (incoming_serial_byte < 91))
    {
        configuration.weighting = incoming_serial_byte;
    }
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_ptt_times_parm2_command(byte incoming_serial_byte)
{

    configuration.ptt_tail_time[configuration.current_tx - 1] = (3 * int(1200 / configuration.wpm)) + (incoming_serial_byte * 10);
    winkey_session_ptt_tail = incoming_serial_byte;
#ifdef DEBUG_WINKEY_PROTOCOL_USING_CW
    send_char('P', KEYER_NORMAL);
    send_char('2', KEYER_NORMAL);
#endif
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_set_pot_parm1_command(byte incoming_serial_byte)
{

    pot_wpm_low_value = incoming_serial_byte;
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_set_pot_parm2_command(byte incoming_serial_byte)
{

#ifdef FEATURE_POTENTIOMETER
    pot_wpm_high_value = (pot_wpm_low_value + incoming_serial_byte);
#else
    (void)incoming_serial_byte; // to get rid of compiler warning about unused variable
#endif
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_first_extension_command(byte incoming_serial_byte)
{

    first_extension_time = incoming_serial_byte;
#ifdef DEBUG_WINKEY_PROTOCOL_USING_CW
    send_char('X', KEYER_NORMAL);
#endif
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_keying_compensation_command(byte incoming_serial_byte)
{

    configuration.keying_compensation = incoming_serial_byte;
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_dah_to_dit_ratio_command(byte incoming_serial_byte)
{

    if ((incoming_serial_byte > 32) && (incoming_serial_byte < 67))
    {
        configuration.dah_to_dit_ratio = (300 * (float(incoming_serial_byte) / 50));
#ifdef OPTION_WINKEY_STRICT_EEPROM_WRITES_MAY_WEAR_OUT_EEPROM
        config_dirty = 1;
#endif
    }
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_set_pinconfig_command(byte incoming_serial_byte)
{

    if (incoming_serial_byte & 1)
    {
        configuration.ptt_buffer_hold_active = 1;
        winkey_pinconfig_ptt_bit = 1;
    }
    else
    {
        configuration.ptt_buffer_hold_active = 0;
#ifdef OPTION_WINKEY_2_SUPPORT
        winkey_pinconfig_ptt_bit = 0;
#endif
    }

    if (incoming_serial_byte & 2)
    {
#ifdef OPTION_WINKEY_2_SUPPORT
        if (wk2_paddle_only_sidetone)
        {
            configuration.sidetone_mode = SIDETONE_PADDLE_ONLY;
        }
        else
        {
#endif
            configuration.sidetone_mode = SIDETONE_ON;
#ifdef OPTION_WINKEY_2_SUPPORT
        }
#endif
    }
    else
    {
        configuration.sidetone_mode = SIDETONE_OFF;
    }

#ifndef OPTION_NO_ULTIMATIC
    switch (incoming_serial_byte & 192)
    {
    case 0:
        ultimatic_mode = ULTIMATIC_NORMAL;
        break;
    case 64:
        ultimatic_mode = ULTIMATIC_DAH_PRIORITY;
        break;
    case 128:
        ultimatic_mode = ULTIMATIC_DIT_PRIORITY;
        break;
    }
#endif

    switch (incoming_serial_byte & 48)
    {
    case 0:
        ptt_hang_time_wordspace_units = WINKEY_HANG_TIME_1_0;
        break;
    case 16:
        ptt_hang_time_wordspace_units = WINKEY_HANG_TIME_1_33;
        break;
    case 32:
        ptt_hang_time_wordspace_units = WINKEY_HANG_TIME_1_66;
        break;
    case 48:
        ptt_hang_time_wordspace_units = WINKEY_HANG_TIME_2_0;
        break;
    }
#ifndef FEATURE_SO2R_BASE
    switch (incoming_serial_byte & 12)
    {
    case 0:
        key_tx = 0;
#ifdef OPTION_WINKEY_2_SUPPORT
        wk2_both_tx_activated = 0;
#endif
        break;
    case 4:
        key_tx = 1;
        configuration.current_ptt_line = ptt_tx_1;
        current_tx_key_line = tx_key_line_1;
        configuration.current_tx = 1;
#ifdef OPTION_WINKEY_2_SUPPORT
        wk2_both_tx_activated = 0;
#endif
        break;
    case 8:
        key_tx = 1;
        if (ptt_tx_2)
        {
            configuration.current_ptt_line = ptt_tx_2;
        }
        else
        {
            configuration.current_ptt_line = ptt_tx_1;
        }
        if (tx_key_line_2)
        {
            current_tx_key_line = tx_key_line_2;
        }
        else
        {
            current_tx_key_line = tx_key_line_1;
        }
#ifdef OPTION_WINKEY_2_SUPPORT
        wk2_both_tx_activated = 0;
#endif
        break;
    case 12:
        key_tx = 1;
        configuration.current_ptt_line = ptt_tx_1;
        current_tx_key_line = tx_key_line_1;
        configuration.current_tx = 1;
#ifdef OPTION_WINKEY_2_SUPPORT
        wk2_both_tx_activated = 1;
#endif
        break;
    }

    config_dirty = 1;
#endif // FEATURE_SO2R_BASE
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_set_pot_parm3_command(byte incoming_serial_byte)
{

#ifdef FEATURE_POTENTIOMETER
#ifdef OPTION_WINKEY_2_SUPPORT
    pot_full_scale_reading = 1022;
#else  // OPTION_WINKEY_2_SUPPORT
    if (incoming_serial_byte == 255)
    {
        pot_full_scale_reading = 1031;
    }
    else
    {
        if (incoming_serial_byte == 127)
        {
            pot_full_scale_reading = 515;
        }
    }
#endif // OPTION_WINKEY_2_SUPPORT
    configuration.pot_activated = 1;
#else
    (void)incoming_serial_byte; // to get rid of compiler warning about unused variable
#endif
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_load_settings_command(byte winkey_status, byte incoming_serial_byte)
{

    switch (winkey_status)
    {
    case WINKEY_LOAD_SETTINGS_PARM_1_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_1_COMMAND");
#endif // DEBUG_WINKEY
        winkey_setmode_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_2_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_2_COMMAND");
#endif // DEBUG_WINKEY
        winkey_unbuffered_speed_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_3_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_3_COMMAND");
#endif // DEBUG_WINKEY
        winkey_sidetone_freq_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_4_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_4_COMMAND");
#endif // DEBUG_WINKEY
        winkey_weighting_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_5_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_5_COMMAND");
#endif // DEBUG_WINKEY
        winkey_ptt_times_parm1_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_6_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_6_COMMAND");
#endif // DEBUG_WINKEY
        winkey_ptt_times_parm2_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_7_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_7_COMMAND");
#endif // DEBUG_WINKEY
        winkey_set_pot_parm1_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_8_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_8_COMMAND");
#endif // DEBUG_WINKEY
        winkey_set_pot_parm2_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_9_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_9_COMMAND");
#endif // DEBUG_WINKEY
        winkey_first_extension_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_10_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_10_COMMAND");
#endif // DEBUG_WINKEY
        winkey_keying_compensation_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_11_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_11_COMMAND");
#endif // DEBUG_WINKEY
        winkey_farnsworth_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_12_COMMAND: // paddle switchpoint - don't need to support
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_12_COMMAND");
#endif // DEBUG_WINKEY
        break;
    case WINKEY_LOAD_SETTINGS_PARM_13_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_13_COMMAND");
#endif // DEBUG_WINKEY
        winkey_dah_to_dit_ratio_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_14_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_14_COMMAND");
#endif // DEBUG_WINKEY
        winkey_set_pinconfig_command(incoming_serial_byte);
        break;
    case WINKEY_LOAD_SETTINGS_PARM_15_COMMAND:
#ifdef DEBUG_WINKEY
        debug_serial_port->println("winkey_load_settings_command: WINKEY_LOAD_SETTINGS_PARM_15_COMMAND");
#endif // DEBUG_WINKEY
        winkey_set_pot_parm3_command(incoming_serial_byte);
        break;
    }
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void winkey_admin_get_values_command()
{

    byte byte_to_send;

    // 1 - mode register
    byte_to_send = 0;
    if (configuration.length_wordspace != default_length_wordspace)
    {
        byte_to_send = byte_to_send | 1;
    }
#ifdef FEATURE_AUTOSPACE
    if (configuration.autospace_active)
    {
        byte_to_send = byte_to_send | 2;
    }
#endif
    if (winkey_serial_echo)
    {
        byte_to_send = byte_to_send | 4;
    }
    if (configuration.paddle_mode == PADDLE_REVERSE)
    {
        byte_to_send = byte_to_send | 8;
    }
    switch (configuration.keyer_mode)
    {
    case IAMBIC_A:
        byte_to_send = byte_to_send | 16;
        break;
    case ULTIMATIC:
        byte_to_send = byte_to_send | 32;
        break;
    case BUG:
        byte_to_send = byte_to_send | 48;
        break;
    }
    if (winkey_paddle_echo_activated)
    {
        byte_to_send = byte_to_send | 64;
    }
#ifdef FEATURE_DEAD_OP_WATCHDOG
    if (!dead_op_watchdog_active)
    {
        byte_to_send = byte_to_send | 128;
    }
#endif // FEATURE_DEAD_OP_WATCHDOG
    winkey_port_write(byte_to_send, 1);

    // 2 - speed
    if (configuration.wpm > 99)
    {
        winkey_port_write(99, 1);
    }
    else
    {
        byte_to_send = configuration.wpm;
        winkey_port_write(byte_to_send, 1);
    }

    // 3 - sidetone
    switch (configuration.hz_sidetone)
    {
    case WINKEY_SIDETONE_1:
        winkey_port_write(1, 1);
        break;
    case WINKEY_SIDETONE_2:
        winkey_port_write(2, 1);
        break;
    case WINKEY_SIDETONE_3:
        winkey_port_write(3, 1);
        break;
    case WINKEY_SIDETONE_4:
        winkey_port_write(4, 1);
        break;
    case WINKEY_SIDETONE_5:
        winkey_port_write(5, 1);
        break;
    case WINKEY_SIDETONE_6:
        winkey_port_write(6, 1);
        break;
    case WINKEY_SIDETONE_7:
        winkey_port_write(7, 1);
        break;
    case WINKEY_SIDETONE_8:
        winkey_port_write(8, 1);
        break;
    case WINKEY_SIDETONE_9:
        winkey_port_write(9, 1);
        break;
    case WINKEY_SIDETONE_10:
        winkey_port_write(10, 1);
        break;
    default:
        winkey_port_write(5, 1);
        break;
    }

    // 4 - weight
    winkey_port_write(configuration.weighting, 1);

    // 5 - ptt lead
    if (configuration.ptt_lead_time[configuration.current_tx - 1] < 256)
    {
        winkey_port_write(configuration.ptt_lead_time[configuration.current_tx - 1] / 10, 1);
    }
    else
    {
        winkey_port_write(255, 1);
    }

    // 6 - ptt tail
    // if (configuration.ptt_tail_time[configuration.current_tx-1] < 256){
    // winkey_port_write((configuration.ptt_tail_time[configuration.current_tx-1] - (3*int(1200/configuration.wpm)))/10,1);
    winkey_port_write(winkey_session_ptt_tail, 1);
// } else {
//   winkey_port_write(winkey_port_write(255,1);
// }

// 7 - pot min wpm
#ifdef FEATURE_POTENTIOMETER
    winkey_port_write(pot_wpm_low_value, 1);
#else
    winkey_port_write(15, 1);
#endif

// 8 - pot wpm range
#ifdef FEATURE_POTENTIOMETER
    winkey_port_write(pot_wpm_high_value - pot_wpm_low_value, 1);
#else
    winkey_port_write(20, 1);
#endif

    // 9 - 1st extension
    winkey_port_write(first_extension_time, 1);

    // 10 - compensation
    winkey_port_write(configuration.keying_compensation, 1);

// 11 - farnsworth wpm
#ifdef FEATURE_FARNSWORTH
    winkey_port_write(configuration.wpm_farnsworth, 1);
#else
    winkey_port_write(zero, 1);
#endif

    // 12 - paddle setpoint
    winkey_port_write(50, 1); // default value

    // 13 - dah to dit ratio
    winkey_port_write(50, 1); // TODO -backwards calculate

// 14 - pin config
#ifdef OPTION_WINKEY_2_SUPPORT
    byte_to_send = 0;
    if (configuration.current_ptt_line != 0)
    {
        byte_to_send = byte_to_send | 1;
    }
    if ((configuration.sidetone_mode == SIDETONE_ON) || (configuration.sidetone_mode == SIDETONE_PADDLE_ONLY))
    {
        byte_to_send = byte_to_send | 2;
    }
    if (current_tx_key_line == tx_key_line_1)
    {
        byte_to_send = byte_to_send | 4;
    }
    if (current_tx_key_line == tx_key_line_2)
    {
        byte_to_send = byte_to_send | 8;
    }
#ifndef FEATURE_SO2R_BASE
    if (wk2_both_tx_activated)
    {
        byte_to_send = byte_to_send | 12;
    }
#endif

#ifndef OPTION_NO_ULTIMATIC
    if (ultimatic_mode == ULTIMATIC_DIT_PRIORITY)
    {
        byte_to_send = byte_to_send | 128;
    }
    if (ultimatic_mode == ULTIMATIC_DAH_PRIORITY)
    {
        byte_to_send = byte_to_send | 64;
    }
#endif

    if (ptt_hang_time_wordspace_units == 1.33)
    {
        byte_to_send = byte_to_send | 16;
    }
    if (ptt_hang_time_wordspace_units == 1.66)
    {
        byte_to_send = byte_to_send | 32;
    }
    if (ptt_hang_time_wordspace_units == 2.0)
    {
        byte_to_send = byte_to_send | 48;
    }
    winkey_port_write(byte_to_send, 1);
#else
    winkey_port_write(5, 1); // default value
#endif

// 15 - pot range
#ifdef OPTION_WINKEY_2_SUPPORT
    winkey_port_write(zero, 1);
#else
    winkey_port_write(0xFF, 1);
#endif
}
#endif

/*

Chapter One

It was late on a rainy Sunday evening.  Static crashes on the direct conversion receiver signaled a distant thunderstorm, due to arrive in an hour or so.  Colin knew he would have to disconnect the little microcontroller circuit from the receiver and all the station antennas soon, but it was getting late and he had to get his sleep for work the next day.

The contraption was a tangled mess on his desk, something only a radio amateur or mad scientist could appreciate.  Alligator clips connected the I and Q audio from the simple receiver to the microcontroller.  Colin had been learning about fast Fourier analysis.  This was his first attempt at actually running the code in an effort to decode RTTY signals.   The microcontroller probably lacked the horsepower to do it, and Colin knew expecting any sort of performance from his creation was a long shot.

Colin tuned to some RTTY signals but couldn't copy anything, despite carefully and slowly tuning the receiver in hopes of hitting that sweet spot where perhaps the microcontroller would blurt out some intelligence, some discernable word or text.  Just one recognizable snippet would give him the feeling of accomplishment or even victory, even if his design never proved to be usable in his nightly hobby.

The static crashes grew stronger and more frequent.  Colin had resigned himself to the fact that success would not be achieved this evening.  The approaching storm along with his growing fatigue convinced him to shut things down and head upstairs to bed.  Just then a burst of noise, different from the thunderstorm static crashes, but a type that you normally hear on 80 meters each night blurted out.  The microcontroller sent out its serial port a string of random characters, in a vain attempt to decode the sounds:

GEHZCVFNOVTZBEBA

Colin went about the process of disconnecting the power to everything and disconnecting antennas and went to bed.

The next evening after supper with his family, Colin went to his basement radio room again, determined to work again on his project but perhaps less eager than before due to the increasing futility of his efforts.  The microcontroller sat connected to the receiver, and the controller to the computer.  He listened to the receiver in the background while responding to emails.  There was a QSO in progress, an old man talking about his dog itching a lot.  The two old men in the conversation droned on forever, with Colin chuckling to himself, but too caught up in his email to reach over and tune the rig to another frequency in hopes of finding a more interesting conversation.

A burst of noise came through the rig again, much like the night before, though much stronger.  The simple receiver lacked automatic gain control and the strong signal produced a rather loud, annoying noise emanating from the rig, prompting Colin to reach over and turn the volume down.  Colin noticed on his serial terminal program another random string of characters which the microcontroller dutifully decoded:

GEHZCVFNOVTZBEBA

The string looked familiar to Colin.  He copied and pasted the string into search on his computer.  The search produced one hit, the terminal program log from the previous evening.  Colin opened the file and saw the matching string, 16 characters.  "What are the chances of that happening?", he thought.  He looked through his code again, looking for some sort of mistake, pattern in the code algorithm, or some plausible explanation. The receiver belched again:

GEHZCVFNOVTZBEBA

At this point Colin had no plausible explanation why the same random string of characters would be decoded last night and this evening, from mere noise bursts.  Frustrated, he decided to post a message on an Internet group describing the strange behavior and the random characters, and then walked away from his radios to watch TV with the family.  After almost an hour of watching mindless sitcoms, it was time for the kids to go to bed.  After they were tucked into bed, Colin came back to his desk to catch up on email.

The receiver, still powered up with the random noise of the universe coming out of the speaker at a low level, and the connected microcontroller circuit sat idle, waiting for some signal to decode.  An AM roundtable comes up on frequency and he listens awhile, while he continues to web surf, looking for something to occupy his mind.  A static crash comes through the speaks and the microcontroller terminal comes alive again, spewing characters:

COLINMEETME@40-10-45.5&75-10-52.6@SAT1200Z

"Wow" Colin exclaims, almost involuntarily.  He pauses for a moment, hoping his wife in the next room hasn't heard him.  She doesn't respond, continuing to watch TV.  "That's my name....coordinates, and a day and time.  That can't be a coincidence.  What in the world have I stumbled upon?" he thinks.  Nervously he brings up Google Earth and enters  the coordinates.  It's a coffee shop, about an hour and twenty minutes south. " Whoever sent this wants to meet me?"

Chapter Two

Colin barely slept the rest of the nights that week thinking about the message.  He stays out of his radio room which is very unlike him.  His wife is out of town this weekend, and Colin rationalizes that there's no excuse to not go to the coffee shop.  Early Saturday morning he quickly gets up, and nervously gets dressed.  He worries if he's given himself enough time to get there.  It's near the city and the surrounding suburban area where the coffee shop is located is notorious for bad traffic.  He decides to take a toll road and exit where he can take back roads to avoid the main thoroughfares.

He arrived at the coffee with a few minutes to spare, takes an out of the way parking spot towards the rear of the restaurant, backing in so he can see anyone pulling in or out, and the side entrance of the coffee shop.  He sits in the vehicle and surveys the parking lot.   Opening the glove compartment he pulls out a pistol in a holster.  Although licensed for carrying a sidearm, Colin rarely, if ever actual wore it in public.  He strapped it on to his belt and double-checked that his jacket concealed it.  His hands shook nervously, but he reassured himself he was somewhat prepared in case the proverbial "men in black" attempted to swoop down and throw him into a black van and drive off.

Looking up, Colin sees an old man in the parking lot looking his way.  They make eye contact.  Colin looks away but it's clear the old man is has somehow identified him.  Colin sighs.  "Perhaps he saw all the antennas on my vehicle, or my callsign plate."  He gets out of the vehicle, locks it, and walks over to the old man.

"Hello" he says in a somewhat frail voice.  "You Colin?"

"Yes" replies Colin, nervously.

The old man nods and his face lightens up.  "Come inside, let's talk."

They go inside and get in line.  The old man orders a coffee, and Colin, never acquiring a taste for coffee, get a hot chocolate.  They grab a table towards the back, away from everyone else.  The old man looks around to make sure they're out of earshot of others.

The old man leans inward, "So you copied my transmission the other day?"

"Yes."  Colin tells him the story of how he came upon the transmission.

"Well, congratulations.  You've stumbled upon something I think you're going to be very happy about.  You're in amateur radio?"  Colin nods.  "You've come upon a secret society.  We've been around a long time, since World War II.  Some of us are hams, others aren't.  We're everywhere.  You've heard us anytime you've turned on a radio, you just didn't know it.  We're the people you don't normally find on the air....the academics, scientists, progressives, politicians, famous people...activists...introverts...geniuses...people close to world leaders.  We communicate via encrypted messaging.  Those noise bursts you heard were transmissions from me.  Some of our communications are noise bursts.  Sometime we communicate with pure noise, indistinguishable from the normal noise you hear on your receiver everyday.  We hide out in the open."

"But how do you do this?"  Colin's technical curiosity emerges.  "How do you communicate with noise?"

The old man takes a sip from his coffee.  "We use a pseudo-random bit stream and quadrature modulate a digital signal taken from a special alphabet, somewhat like ASCII.  It's amazingly simple but nearly impossible to break without the bit stream.  You were just lucky to receive it.  Apparently the buggy code in your microcontroller digital signal processing generates part of the pseudo random stream under the right conditions.  Everyone thinks 80 meters is noisy.  It's really not, there's just a lot of us talking on it.  You ever turn on your radio and it's S9 noise everywhere?"

"Yes" replies Colin.

"Sometimes that's us.  We sometimes modulate wideband noise when we have a particularly large message to send out, something important.  The technology is really interesting.  It pushes the limits of Shannon's Equation." he pauses.  "You ever hear of long delay echos?"

"I've never experienced one, but I've read about them and heard they're somewhat common." Colin says.

He smiles.  "That's us.  Sometime we communicate by receiving someone's signal on the air, we delay it, modulate the noise on it, and re-transmit it.  We do that for fun.  People seem to get a kick out of it."

"Why does this society exist?"  asks Colin.

"We serve a higher purpose." pointing above, he says.  "It came out of the Resistance in World War II and was originally intended to prevent atrocities like the Holocaust from happening again, but since then it's grown to encompass other things.  Many of us started off as radio amateurs and got bored with it.  We dropped out.  We're the radio guys you don't see at hamfests or on the Internet.  Those of us who are licensed amateurs usually lay low and don't get on the air, at least in a way you can hear us.  Amateur radio is to us as CB is to amateur radio.  Few of us fit in with them. Members communicate about important stuff, like scientific discoveries or secret information from governments that could save lives or change the world.  We've provided information that has ended wars, and started some.  Some say we provided the information that started the fall of the USSR.  We operate without borders or recognition of nationality.   I'm not sure how many of us there are, but it's perhaps in the thousands, worldwide."

Colin asks "Are you spies?"

"We're not spies, we're communicators." he replies.

"Does the government know of this network?"

"Perhaps, but not at a high level or in any official capacity that we know of.  We definitely have members close to people high up, advisers of sorts.  Undoubtedly there are members in intelligence agencies in various governments.  But they don't dare divulge knowledge of the network.  It's too valuable.  To them it's a tool, and they know they would be denied that tool, purged from the network, should they let others know of it.  But they are free to use the information they receive, as they see fit.  But they know they have a responsibility to use it for the greater good."

The old man clears his throat and takes another gulp of coffee.  "Communications is a weapon, more powerful than any weapon you can carry.  That phone," he said, pointing to my iPhone lying on the table, " is just as powerful as the weapon you have on your belt, just in a different way."

Colin tries to hide a puzzled look, wondering how the old man knew of his weapon.  Changing the subject, he asks "How do people get into this?"

"Membership is by invitation only.  We have 16 character identity strings.  You received mine.  An identity string is what you would call a callsign in amateur radio.  You're the first person I've ever heard of receiving the signal without knowledge of the code.  There's no process for someone like you to join.  But I'm getting old and I need to hand off my encryption stream to someone before I die, to keep it going.  You seem to be a nice enough guy, qualified to join, from what I have seen and heard about you."

"But.... this sounds like a network of rather smart and powerful people.  I'm just an ordinary guy who likes to play with radios and occasionally build something.  I'm not a scientist or someone powerful.  Is there some role I will have, something I need to do?" Colin asks.

"Some members just have fun with this, somewhat like a hobby.  They don't have roles, for now.  You will have a role, you just don't know what it is yet.  Do not seek out a role.  Do not try to make yourself important or identify some great thing to do.  Those who invent things to do, create crises, or give themselves power get purged from the network.  Your role will become known in due time and you will know it when you encounter it.  Trust me." 

He goes on, "You're going to receive more information.  It will explain the encryption algorithm.  You know how to program, so with a little bit of work you should be able to write the software for a transceiver that will work reliably.  I'll also give you an identity string.  It's derived from mine and you'll eventually be able to trace it back mathematically to previous identity strings and others in the hierarchy.  The more you communicate, your identity string will establish a trust relationship with other identity strings, other operators.  The more operators you gain trust with, you will get more of the algorithm and more of the bit stream.  With more of the algorithm and bit stream, the more signals you will be able to receive and you will be able to communicate with more people in the network hierarchy.  With perseverance and patience you'll get to know some high level members, perhaps even people you see on the news."

"I said before that there are thousands of operators.  The truth is I don't know how many operators there are.  No one does.  As more of the bit stream is revealed, more members appear.  For all we know there could be millions of members.  There could be extra-terrestrials in the network."  He chuckles.  "Some have theorized that some of the noise we receive from outer space could be actually intelligence encrypted in the noise, like we do.  We just don't have the information or computing power yet to decode it."

The smile leaves old man's face.  "You have to keep this a secret.  If you reveal this to the wrong people, the results would be disastrous.  Those who reveal the code of the noise are purged from the network, sometimes not seen again."
Before Colin could ask his next question, the old man got up, handed him a card with characters written in bold black marker:

8^fGwq9(:lLDPu6$

"Congratulations.  This is your identity string.  Memorize it.  Guard it with your life."  He offers his right hand and they shake hands.

Colin follows the old man out the door, wanting to ask more questions. "Where will I would get the information on the algorithm, how do I build a transceiver?"  he frantically asks.

"You have to listen to the noise."  he said as he walked to his car, got in, and drove off.

Colin drove home in somewhat of a daze, not sure what to make of all this.  Was the old man crazy, or was all this real?  Colin went about my business for a few days, thinking about the old man and wondering what would be next.  "Would I get something in the mail?  Perhaps an email?  Would he contact me again?"

A few days later while watching the local news, a story came on about the death of a prominent researcher.  Colin was shocked to see a grainy photo of the old man he had met at the coffee shop, the photo perhaps from the 60s as he looked younger, more Colin's age today.  Walter was his name.  He had worked at Bell Labs in New Jersey as a physicist and had made many discoveries in communications which were patented in the 60s and 70s.  Walter was a quiet man but was known for his community work.  He fled Germany with his family as a young boy prior to World War II breaking out.  His father was a poor potato farmer who later helped the allies in cryptography after he devised a code based on the patterns of eyes on potatoes.  His wife had passed before him several years earlier.  Walter died alone at his home, of unknown causes and his death was under investigation.  Investigators doubted there was foul play, but there was a rather odd paper he was writing with codes on it found next to him.  He was survived by two children and some grandchildren residing in Florida.  Colin thought perhaps he could contact his family, but he knew he couldn't risk revealing what he had heard from the man if what he said was true.  Colin sat dumbfounded, wondering if he had lost his one connection to the secret network.

Later that night Colin once again turned on his receiver to 80 meters.  The little circuit sat idle with alligator clips connecting the rig audio to it.  His original goal of copying a RTTY signal now seemed pointless and insignificant in the grand scheme of things with the new knowledge he had.  He wanted to write more code and figure out the algorithm, all of it.  But Colin had no idea what next step to take, no clue what the algorithm was that would grant him access to a whole new world.  He pulled the card out of his wallet with his identity string and stared at the seemingly random 16 characters.  It contained uppercase, lowercase, numbers, symbols, just about everything.  Perhaps it was a base 64 character set?  What secrets were in it?  His thoughts were a disorganized jumble, and feeling a headache coming on he stopped himself from thinking further about it.

He was no longer interested in listening to Morse code signals or voice conversations.  That was merely just meaningless noise, a distraction from what he was really looking for.  Every little pop and crackle on the receiver caught his attention.  Was it just random atmospheric noise leftover from the Big Bang or some noisy electrical appliance, or was there intelligence in each seemingly random sounds?  For hours he scanned through the band, hoping to catch the right signal in hopes that his little contraption might pick up some clue that would lead him to the next step, perhaps someone else in the network since his contact had passed away.  BZZZZZT bursts from the receiver and the microcontroller terminal screen came alive:

 8^fGwq9(:lLDPu6$ : KEEP LISTENING TO THE NOISE AND AWAIT FURTHER INFO.

*/

//-------------------------------------------------------------------------------------------------------

#if defined(FEATURE_SERIAL) && defined(FEATURE_WINKEY_EMULATION) && defined(OPTION_WINKEY_2_SUPPORT)

void winkey_eeprom_download()
{

    byte zero = 0;
    unsigned int x = 0;
    unsigned int bytes_sent = 0;

    winkey_port_write(0xA5, 1);        // 01 magic byte
    winkey_admin_get_values_command(); // 02-16

    winkey_port_write(byte(configuration.wpm), 1); // 17 cmdwpm
    bytes_sent = 17;

    // pad the rest with zeros
    for (x = 0; x < (256 - bytes_sent); x++)
    {
        winkey_port_write(zero, 1);
    }
}
#endif // defined(FEATURE_SERIAL) && defined(FEATURE_WINKEY_EMULATION) && defined(OPTION_WINKEY_2_SUPPORT)

//-------------------------------------------------------------------------------------------------------


void clear_send_buffer()
{
#ifdef FEATURE_WINKEY_EMULATION
  winkey_xoff = 0;
#endif
  send_buffer_bytes = 0;
}

byte service_tx_inhibit_and_pause()
{

  byte return_code = 0;
  static byte pause_sending_buffer_active = 0;

  if (tx_inhibit_pin)
  {
    if ((digitalRead(tx_inhibit_pin) == tx_inhibit_pin_active_state))
    {
      dit_buffer = 0;
      dah_buffer = 0;
      return_code = 1;
      if (send_buffer_bytes)
      {
        clear_send_buffer();
        send_buffer_status = SERIAL_SEND_BUFFER_NORMAL;
#ifdef FEATURE_MEMORIES
        play_memory_prempt = 1;
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
      ptt_unkey();
    }
  }

  if (tx_pause_pin)
  {
    if ((digitalRead(tx_pause_pin) == tx_pause_pin_active_state))
    {
      dit_buffer = 0;
      dah_buffer = 0;
      return_code = 1;
      if (!pause_sending_buffer_active)
      {
        pause_sending_buffer = 1;
        pause_sending_buffer_active = 1;
        delay(10);
      }
    }
    else
    {
      if (pause_sending_buffer_active)
      {
        pause_sending_buffer = 0;
        pause_sending_buffer_active = 0;
        delay(10);
      }
    }
  }

  return return_code;
}

//-------------------------------------------------------------------------------------------------------


void add_to_send_buffer(byte incoming_serial_byte)
{

  if (send_buffer_bytes < send_buffer_size)
  {
    if (incoming_serial_byte != 127)
    {
      send_buffer_bytes++;
      send_buffer_array[send_buffer_bytes - 1] = incoming_serial_byte;

#ifdef FEATURE_WINKEY_EMULATION
      if ((send_buffer_bytes > winkey_xoff_threshold) && winkey_host_open)
      {
        winkey_xoff = 1;
        winkey_port_write(0xc0 | winkey_sending | winkey_xoff, 0); // send XOFF status
      }
#endif
    }
    else
    { // we got a backspace
      if (send_buffer_bytes)
      {
        send_buffer_bytes--;
      }
    }

#if defined(DEBUG_SERVICE_SEND_BUFFER)
    debug_serial_port->print("add_to_send_buffer: ");
    debug_serial_port->write(incoming_serial_byte);
    debug_serial_port->print(" [");
    debug_serial_port->print(incoming_serial_byte);
    debug_serial_port->println("]");
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
  else
  {

#if defined(DEBUG_SERVICE_SEND_BUFFER)
    debug_serial_port->println("add_to_send_buffer: !send_buffer_bytes < send_buffer_size");
#endif
  }
}

//-------------------------------------------------------------------------------------------------------


void check_ptt_tail()
{
#ifdef DEBUG_LOOP
  debug_serial_port->println(F("loop: entering check_ptt_tail"));
#endif

#ifdef FEATURE_SO2R_BASE
  if (so2r_ptt)
  {
    return;
  }
#endif

  // static byte manual_ptt_invoke_ptt_input_pin = 0;

  if (ptt_input_pin)
  {
    if ((digitalRead(ptt_input_pin) == ptt_input_pin_active_state))
    {
      if (!manual_ptt_invoke)
      {
        manual_ptt_invoke = 1;
        manual_ptt_invoke_ptt_input_pin = 1;
        ptt_key();
        return;
      }
    }
    else
    {
      if ((manual_ptt_invoke) && (manual_ptt_invoke_ptt_input_pin))
      {
        manual_ptt_invoke = 0;
        manual_ptt_invoke_ptt_input_pin = 0;
        if (!key_state)
        {
          ptt_unkey();
        }
      }
    }
  }

#if !defined(FEATURE_WINKEY_EMULATION)
  if (key_state)
  {
    ptt_time = millis();
  }
  else
  {
    if ((ptt_line_activated) && (manual_ptt_invoke == 0))
    {
      // if ((millis() - ptt_time) > ptt_tail_time) {
      if (last_sending_mode == MANUAL_SENDING)
      {
#ifndef OPTION_INCLUDE_PTT_TAIL_FOR_MANUAL_SENDING

        // PTT Tail Time: N     PTT Hang Time: Y

        if ((millis() - ptt_time) >= ((configuration.length_wordspace * ptt_hang_time_wordspace_units) * float(1200 / configuration.wpm)))
        {
          ptt_unkey();
        }
#else // ndef OPTION_INCLUDE_PTT_TAIL_FOR_MANUAL_SENDING
#ifndef OPTION_EXCLUDE_PTT_HANG_TIME_FOR_MANUAL_SENDING

        // PTT Tail Time: Y     PTT Hang Time: Y

        if ((millis() - ptt_time) >= (((configuration.length_wordspace * ptt_hang_time_wordspace_units) * float(1200 / configuration.wpm)) + configuration.ptt_tail_time[configuration.current_tx - 1]))
        {
          ptt_unkey();
        }
#else  // OPTION_EXCLUDE_PTT_HANG_TIME_FOR_MANUAL_SENDING
        if ((millis() - ptt_time) >= configuration.ptt_tail_time[configuration.current_tx - 1])
        {

          // PTT Tail Time: Y    PTT Hang Time: N

          ptt_unkey();
        }
#endif // OPTION_EXCLUDE_PTT_HANG_TIME_FOR_MANUAL_SENDING
#endif // ndef OPTION_INCLUDE_PTT_TAIL_FOR_MANUAL_SENDING
      }
      else
      { // automatic sending
        if (((millis() - ptt_time) > configuration.ptt_tail_time[configuration.current_tx - 1]) && (!configuration.ptt_buffer_hold_active || ((!send_buffer_bytes) && configuration.ptt_buffer_hold_active) || (pause_sending_buffer)))
        {
          ptt_unkey();
        }
      }
    }
  }
#else // FEATURE_WINKEY_EMULATION

  if (key_state)
  {
    ptt_time = millis();
  }
  else
  {
    if ((ptt_line_activated) && (manual_ptt_invoke == 0))
    {
      // if ((millis() - ptt_time) > ptt_tail_time) {
      if (last_sending_mode == MANUAL_SENDING)
      {
#ifndef OPTION_INCLUDE_PTT_TAIL_FOR_MANUAL_SENDING

        // PTT Tail Time: N     PTT Hang Time: Y

        if ((millis() - ptt_time) >= ((configuration.length_wordspace * ptt_hang_time_wordspace_units) * float(1200 / configuration.wpm)))
        {
          ptt_unkey();
        }
#else // ndef OPTION_INCLUDE_PTT_TAIL_FOR_MANUAL_SENDING
#ifndef OPTION_EXCLUDE_PTT_HANG_TIME_FOR_MANUAL_SENDING

        // PTT Tail Time: Y     PTT Hang Time: Y

        if (winkey_host_open)
        {
          if ((millis() - ptt_time) >= (((configuration.length_wordspace * ptt_hang_time_wordspace_units) * float(1200 / configuration.wpm)) + (int(winkey_session_ptt_tail) * 10) + (3 * (1200 / configuration.wpm))))
          {
            ptt_unkey();
          }
        }
        else
        {
          if ((millis() - ptt_time) >= (((configuration.length_wordspace * ptt_hang_time_wordspace_units) * float(1200 / configuration.wpm)) + configuration.ptt_tail_time[configuration.current_tx - 1]))
          {
            ptt_unkey();
          }
        }

#else // OPTION_EXCLUDE_PTT_HANG_TIME_FOR_MANUAL_SENDING
        if (winkey_host_open)
        {
          if ((millis() - ptt_time) >= ((int(winkey_session_ptt_tail) * 10) + (3 * (1200 / configuration.wpm))))
          {

            // PTT Tail Time: Y    PTT Hang Time: N

            ptt_unkey();
          }
        }
        else
        {
          if ((millis() - ptt_time) >= configuration.ptt_tail_time[configuration.current_tx - 1])
          {

            // PTT Tail Time: Y    PTT Hang Time: N

            ptt_unkey();
          }
        }

#endif // OPTION_EXCLUDE_PTT_HANG_TIME_FOR_MANUAL_SENDING
#endif // ndef OPTION_INCLUDE_PTT_TAIL_FOR_MANUAL_SENDING
      }
      else
      { // automatic sending
        if (winkey_host_open)
        {
          if (((millis() - ptt_time) > ((int(winkey_session_ptt_tail) * 10) + (3 * (1200 / configuration.wpm)))) && (!configuration.ptt_buffer_hold_active || ((!send_buffer_bytes) && configuration.ptt_buffer_hold_active) || (pause_sending_buffer)))
          {
            ptt_unkey();
          }
        }
        else
        {
          if (((millis() - ptt_time) > configuration.ptt_tail_time[configuration.current_tx - 1]) && (!configuration.ptt_buffer_hold_active || ((!send_buffer_bytes) && configuration.ptt_buffer_hold_active) || (pause_sending_buffer)))
          {
            ptt_unkey();
          }
        }
      }
    }
  }
#endif // FEATURE_WINKEY_EMULATION
}

//-------------------------------------------------------------------------------------------------------

void tx_and_sidetone_key(int state)
{

#if defined(FEATURE_COMPETITION_COMPRESSION_DETECTION)

  byte i;

  if ((state == 0) && (key_state) && (compression_detection_key_up_time == 0) && (compression_detection_key_down_time == 0))
  {
    compression_detection_key_up_time = millis();
    // debug_serial_port->println("UP");
  }
  if ((state) && (key_state == 0) && (compression_detection_key_up_time > 0) && (compression_detection_key_down_time == 0))
  {
    compression_detection_key_down_time = millis();
    // debug_serial_port->println("DOWN");
  }

  unsigned long key_up_to_key_down_time = 0;

  if ((compression_detection_key_down_time != 0) && (compression_detection_key_up_time != 0))
  { // do we have a measurement waiting for us?
    key_up_to_key_down_time = compression_detection_key_down_time - compression_detection_key_up_time;
#if defined(DEBUG_FEATURE_COMPETITION_COMPRESSION_DETECTION)
    // debug_serial_port->print("service_competition_compression_detection: key_up_to_key_down_time:");
    // debug_serial_port->println(key_up_to_key_down_time);
#endif
    // is the time within the limits of what would be inter-character time?
    if ((key_up_to_key_down_time > ((1200 / configuration.wpm) * COMPETITION_COMPRESSION_DETECTION_TIME_INTERCHAR_LOWER_LIMIT)) && (key_up_to_key_down_time < ((1200 / configuration.wpm) * COMPETITION_COMPRESSION_DETECTION_TIME_INTERCHAR_UPPER_LIMIT)))
    {
      // add it to the array
      if (time_array_index < COMPETITION_COMPRESSION_DETECTION_ARRAY_SIZE)
      {

#if defined(DEBUG_FEATURE_COMPETITION_COMPRESSION_DETECTION)
        debug_serial_port->print("tx_and_sidetone_key: service_competition_compression_detection: array entry ");
        debug_serial_port->print(time_array_index);
        debug_serial_port->print(":");
        debug_serial_port->println(key_up_to_key_down_time);
#endif

        time_array[time_array_index] = key_up_to_key_down_time;
        time_array_index++;
      }
      else
      { // if time array is completely filled up, we do a first in, first out
        for (i = 0; i < (COMPETITION_COMPRESSION_DETECTION_ARRAY_SIZE - 1); i++)
        {
          time_array[i] = time_array[i + 1];
        }
        time_array[COMPETITION_COMPRESSION_DETECTION_ARRAY_SIZE - 1] = key_up_to_key_down_time;

#if defined(DEBUG_FEATURE_COMPETITION_COMPRESSION_DETECTION)
        debug_serial_port->print("tx_and_sidetone_key: service_competition_compression_detection: FIFO array entry ");
        debug_serial_port->print(time_array_index);
        debug_serial_port->print(":");
        debug_serial_port->println(key_up_to_key_down_time);
#endif
      }
    }
    else
    {
#if defined(DEBUG_FEATURE_COMPETITION_COMPRESSION_DETECTION)
      // debug_serial_port->print("tx_and_sidetone_key: service_competition_compression_detection: discarded entry: ");
      // debug_serial_port->println(key_up_to_key_down_time);
#endif
    }
    compression_detection_key_down_time = 0;
    compression_detection_key_up_time = 0;
  }

#endif // defined(FEATURE_COMPETITION_COMPRESSION_DETECTION)

#if !defined(FEATURE_PTT_INTERLOCK)
  if ((state) && (key_state == 0))
  {
    if (key_tx)
    {
      byte previous_ptt_line_activated = ptt_line_activated;
      ptt_key();
      if (current_tx_key_line)
      {
        digitalWrite(current_tx_key_line, tx_key_line_active_state);
      }
#if defined(OPTION_WINKEY_2_SUPPORT) && defined(FEATURE_WINKEY_EMULATION) && !defined(FEATURE_SO2R_BASE)
      if ((wk2_both_tx_activated) && (tx_key_line_2))
      {
        digitalWrite(tx_key_line_2, HIGH);
      }
#endif
      if ((first_extension_time) && (previous_ptt_line_activated == 0))
      {
        delay(first_extension_time);
      }
    }
    if ((configuration.sidetone_mode == SIDETONE_ON) || (keyer_machine_mode == KEYER_COMMAND_MODE) || ((configuration.sidetone_mode == SIDETONE_PADDLE_ONLY) && (sending_mode == MANUAL_SENDING)))
    {
#if !defined(OPTION_SIDETONE_DIGITAL_OUTPUT_NO_SQUARE_WAVE)
      tone(sidetone_line, configuration.hz_sidetone);
#else
      if (sidetone_line)
      {
        digitalWrite(sidetone_line, sidetone_line_active_state);
      }
#endif
    }
    key_state = 1;
  }
  else
  {
    if ((state == 0) && (key_state))
    {
      if (key_tx)
      {
        if (current_tx_key_line)
        {
          digitalWrite(current_tx_key_line, tx_key_line_inactive_state);
        }
#if defined(OPTION_WINKEY_2_SUPPORT) && defined(FEATURE_WINKEY_EMULATION) && !defined(FEATURE_SO2R_BASE)
        if ((wk2_both_tx_activated) && (tx_key_line_2))
        {
          digitalWrite(tx_key_line_2, LOW);
        }
#endif
        ptt_key();
      }
      if ((configuration.sidetone_mode == SIDETONE_ON) || (keyer_machine_mode == KEYER_COMMAND_MODE) || ((configuration.sidetone_mode == SIDETONE_PADDLE_ONLY) && (sending_mode == MANUAL_SENDING)))
      {
#if !defined(OPTION_SIDETONE_DIGITAL_OUTPUT_NO_SQUARE_WAVE)
        noTone(sidetone_line);
#else
        if (sidetone_line)
        {
          digitalWrite(sidetone_line, sidetone_line_inactive_state);
        }
#endif
      }
      key_state = 0;
    }
  }
#else // FEATURE_PTT_INTERLOCK
  if ((state) && (key_state == 0))
  {
    if (key_tx)
    {
      byte previous_ptt_line_activated = ptt_line_activated;
      if (!ptt_interlock_active)
      {
        ptt_key();
      }
      if (current_tx_key_line)
      {
        digitalWrite(current_tx_key_line, tx_key_line_active_state);
      }
#if defined(OPTION_WINKEY_2_SUPPORT) && defined(FEATURE_WINKEY_EMULATION) && !defined(FEATURE_SO2R_BASE)
      if ((wk2_both_tx_activated) && (tx_key_line_2))
      {
        digitalWrite(tx_key_line_2, HIGH);
      }
#endif
      if ((first_extension_time) && (previous_ptt_line_activated == 0))
      {
        delay(first_extension_time);
      }
    }
    if ((configuration.sidetone_mode == SIDETONE_ON) || (keyer_machine_mode == KEYER_COMMAND_MODE) || ((configuration.sidetone_mode == SIDETONE_PADDLE_ONLY) && (sending_mode == MANUAL_SENDING)))
    {
#if !defined(OPTION_SIDETONE_DIGITAL_OUTPUT_NO_SQUARE_WAVE)
      tone(sidetone_line, configuration.hz_sidetone);
#else
      if (sidetone_line)
      {
        digitalWrite(sidetone_line, sidetone_line_active_state);
      }
#endif
    }
    key_state = 1;
  }
  else
  {
    if ((state == 0) && (key_state))
    {
      if (key_tx)
      {
        if (current_tx_key_line)
        {
          digitalWrite(current_tx_key_line, tx_key_line_inactive_state);
        }
#if defined(OPTION_WINKEY_2_SUPPORT) && defined(FEATURE_WINKEY_EMULATION) && !defined(FEATURE_SO2R_BASE)
        if ((wk2_both_tx_activated) && (tx_key_line_2))
        {
          digitalWrite(tx_key_line_2, LOW);
        }
#endif
        if (!ptt_interlock_active)
        {
          ptt_key();
        }
      }
      if ((configuration.sidetone_mode == SIDETONE_ON) || (keyer_machine_mode == KEYER_COMMAND_MODE) || ((configuration.sidetone_mode == SIDETONE_PADDLE_ONLY) && (sending_mode == MANUAL_SENDING)))
      {
#if !defined(OPTION_SIDETONE_DIGITAL_OUTPUT_NO_SQUARE_WAVE)
        noTone(sidetone_line);
#else
        if (sidetone_line)
        {
          digitalWrite(sidetone_line, sidetone_line_inactive_state);
        }
#endif
      }
      key_state = 0;
    }
  }

#endif // FEATURE_PTT_INTERLOCK

#if defined(FEATURE_INTERNET_LINK)
  link_key(state);
#endif

  check_ptt_tail();
}

//-------------------------------------------------------------------------------------------------------

#ifdef FEATURE_WINKEY_EMULATION
void service_winkey(byte action)
{

  static byte winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
  static int winkey_parmcount = 0;
  static unsigned long winkey_last_activity;
  byte status_byte_to_send;
  static byte winkey_paddle_echo_space_sent = 1;

#ifdef OPTION_WINKEY_DISCARD_BYTES_AT_STARTUP
  static byte winkey_discard_bytes_init_done = 0;
  if (!winkey_discard_bytes_init_done)
  {
    if (primary_serial_port->available())
    {
      for (int z = winkey_discard_bytes_startup; z > 0; z--)
      {
        while (primary_serial_port->available() == 0)
        {
        }
        primary_serial_port->read();
      }
      winkey_discard_bytes_init_done = 1;
    }
  }
#endif // OPTION_WINKEY_DISCARD_BYTES_AT_STARTUP

#ifdef DEBUG_WINKEY_SEND_ERRANT_BYTE
  static byte i_sent_it = 0;

  if ((millis() > 30000) && (!i_sent_it))
  {
    winkey_port_write(30, 1);
    i_sent_it = 1;
  }

#endif

#ifdef OPTION_WINKEY_IGNORE_FIRST_STATUS_REQUEST
  static byte ignored_first_status_request = 0;
#endif // OPTION_WINKEY_IGNORE_FIRST_STATUS_REQUEST

  if (action == WINKEY_HOUSEKEEPING)
  {
    if (winkey_last_unbuffered_speed_wpm == 0)
    {
      winkey_last_unbuffered_speed_wpm = configuration.wpm;
    }

    // Winkey interface emulation housekeeping items
    // check to see if we were sending stuff and the buffer is clear
    if (winkey_interrupted)
    { // if Winkey sending was interrupted by the paddle, look at PTT line rather than timing out to send 0xc0
      if (ptt_line_activated == 0)
      {
#ifdef DEBUG_WINKEY
        debug_serial_port->println("\r\nservice_winkey:sending unsolicited status byte due to paddle interrupt");
#endif // DEBUG_WINKEY
        winkey_sending = 0;
        clear_send_buffer();

#ifdef FEATURE_MEMORIES
        // clear_memory_button_buffer();
        play_memory_prempt = 1;
        repeat_memory = 255;
#endif

        winkey_interrupted = 0;
        // winkey_port_write(0xc2|winkey_sending|winkey_xoff);
        winkey_port_write(0xc6, 0); //<- this alone makes N1MM logger get borked (0xC2 = paddle interrupt)
        winkey_port_write(0xc0, 0); // so let's send a 0xC0 to keep N1MM logger happy
        winkey_buffer_counter = 0;
        winkey_buffer_pointer = 0;
      }
    }
    else
    { // if (winkey_interrupted)
      // if ((winkey_host_open) && (winkey_sending) && (send_buffer_bytes < 1) && ((millis() - winkey_last_activity) > winkey_c0_wait_time)) {
      if ((primary_serial_port->available() == 0) && (winkey_host_open) && (winkey_sending) && (send_buffer_bytes < 1) && ((millis() - winkey_last_activity) > winkey_c0_wait_time))
      {
#ifdef OPTION_WINKEY_SEND_WORDSPACE_AT_END_OF_BUFFER
        send_char(' ', KEYER_NORMAL);
#endif
// add_to_send_buffer(' ');    // this causes a 0x20 to get echoed back to host - doesn't seem to effect N1MM program
#ifdef DEBUG_WINKEY
        debug_serial_port->println("\r\nservice_winkey:sending unsolicited status byte");
#endif // DEBUG_WINKEY
        winkey_sending = 0;
        winkey_port_write(0xc0 | winkey_sending | winkey_xoff, 0); // tell the host we've sent everything
        winkey_buffer_counter = 0;
        winkey_buffer_pointer = 0;
      }
    } // if (winkey_interrupted)

    // failsafe check - if we've been in some command status for awhile waiting for something, clear things out
    if ((winkey_status != WINKEY_NO_COMMAND_IN_PROGRESS) && ((millis() - winkey_last_activity) > winkey_command_timeout_ms))
    {
#ifdef DEBUG_WINKEY
      debug_serial_port->print("\r\nservice_winkey:cmd tout!->WINKEY_NO_COMMAND_IN_PROGRESS cmd was:");
      debug_serial_port->println(winkey_status);
#endif // DEBUG_WINKEY
      winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      winkey_buffer_counter = 0;
      winkey_buffer_pointer = 0;
      winkey_port_write(0xc0 | winkey_sending | winkey_xoff, 0); // send a status byte back for giggles
    }

    if ((winkey_host_open) && (winkey_paddle_echo_buffer) && (winkey_paddle_echo_activated) && (millis() > winkey_paddle_echo_buffer_decode_time))
    {
#ifdef DEBUG_WINKEY
      debug_serial_port->println("\r\nservice_winkey:sending paddle echo char");
#endif // DEBUG_WINKEY
      winkey_port_write(byte(convert_cw_number_to_ascii(winkey_paddle_echo_buffer)), 0);
      winkey_paddle_echo_buffer = 0;
      winkey_paddle_echo_buffer_decode_time = millis() + (float(600 / configuration.wpm) * length_letterspace);
      winkey_paddle_echo_space_sent = 0;
    }

    if ((winkey_host_open) && (winkey_paddle_echo_buffer == 0) && (winkey_paddle_echo_activated) && (millis() > (winkey_paddle_echo_buffer_decode_time + (float(1200 / configuration.wpm) * (configuration.length_wordspace - length_letterspace)))) && (!winkey_paddle_echo_space_sent))
    {
#ifdef DEBUG_WINKEY
      debug_serial_port->println("\r\nservice_winkey:sending paddle echo space");
#endif // DEBUG_WINKEY
      winkey_port_write(' ', 0);
      winkey_paddle_echo_space_sent = 1;
    }
  } // if (action == WINKEY_HOUSEKEEPING)

  if (action == SERVICE_SERIAL_BYTE)
  {
#ifdef DEBUG_WINKEY
    debug_serial_port->print("Winkey Port RX: ");
    if ((incoming_serial_byte > 31) && (incoming_serial_byte < 127))
    {
      debug_serial_port->write(incoming_serial_byte);
    }
    else
    {
      debug_serial_port->print(".");
    }
    debug_serial_port->print(" [");
    debug_serial_port->print(incoming_serial_byte);
    debug_serial_port->print("]");
    debug_serial_port->print(" [0x");
    if (incoming_serial_byte < 16)
    {
      debug_serial_port->print("0");
    }
    debug_serial_port->print(incoming_serial_byte, HEX);
    debug_serial_port->println("]");
#endif // DEBUG_WINKEY

    winkey_last_activity = millis();

    if (winkey_status == WINKEY_NO_COMMAND_IN_PROGRESS)
    {
#if defined(FEATURE_SO2R_BASE)
      if (incoming_serial_byte >= 128)
      {
        so2r_command();
      }
#endif // FEATURE_SO2R_BASE

#if defined(OPTION_WINKEY_EXTENDED_COMMANDS_WITH_DOLLAR_SIGNS)

#if !defined(OPTION_WINKEY_IGNORE_LOWERCASE)
      if ((incoming_serial_byte > 31) && (incoming_serial_byte != 36))
      { // ascii 36 = '$'
#else
      if ((((incoming_serial_byte > 31) && (incoming_serial_byte < 97)) || (incoming_serial_byte == 124)) && (incoming_serial_byte != 36))
      { // 124 = ascii | = half dit
#endif

#else

#if !defined(OPTION_WINKEY_IGNORE_LOWERCASE)
      if (incoming_serial_byte > 31)
      {
#else
      if (((incoming_serial_byte > 31) && (incoming_serial_byte < 97)) || (incoming_serial_byte == 124))
      { // 124 = ascii | = half dit
#endif

#endif

#if !defined(OPTION_WINKEY_IGNORE_LOWERCASE)
        if ((incoming_serial_byte > 96) && (incoming_serial_byte < 123))
        {
          incoming_serial_byte = incoming_serial_byte - 32;
        }
#endif //! defined(OPTION_WINKEY_IGNORE_LOWERCASE)

        byte serial_buffer_position_to_overwrite;

        if (winkey_buffer_pointer > 0)
        {
          serial_buffer_position_to_overwrite = send_buffer_bytes - (winkey_buffer_counter - winkey_buffer_pointer) - 1;
          if ((send_buffer_bytes) && (serial_buffer_position_to_overwrite < send_buffer_bytes))
          {

#ifdef DEBUG_WINKEY
            debug_serial_port->print("service_winkey:serial_buffer_position_to_overwrite:");
            debug_serial_port->print(serial_buffer_position_to_overwrite);
            debug_serial_port->print(":");
            debug_serial_port->write(incoming_serial_byte);
            debug_serial_port->println();
#endif // DEBUG_WINKEY
            send_buffer_array[serial_buffer_position_to_overwrite] = incoming_serial_byte;
          }
          winkey_buffer_pointer++;
        }
        else
        {

          add_to_send_buffer(incoming_serial_byte);

#ifdef DEBUG_WINKEY
          debug_serial_port->print("service_winkey:add_to_send_buffer:");
          debug_serial_port->write(incoming_serial_byte);
          debug_serial_port->print(" send_buffer_bytes:");
          debug_serial_port->println(send_buffer_bytes);
#endif // DEBUG_WINKEY

#if defined(OPTION_WINKEY_INTERRUPTS_MEMORY_REPEAT) && defined(FEATURE_MEMORIES)
          play_memory_prempt = 1;
          repeat_memory = 255;
#endif
          winkey_buffer_counter++;
        }

        if (!winkey_sending)
        {
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:status byte:starting to send");
#endif // DEBUG_WINKEY
          winkey_sending = 0x04;
#if !defined(OPTION_WINKEY_UCXLOG_SUPRESS_C4_STATUS_BYTE)
          winkey_port_write(0xc4 | winkey_sending | winkey_xoff, 0); // tell the client we're starting to send
#endif                                                               // OPTION_WINKEY_UCXLOG_SUPRESS_C4_STATUS_BYTE
#ifdef FEATURE_MEMORIES
          repeat_memory = 255;
#endif
        }
      }
      else
      {

#ifdef OPTION_WINKEY_STRICT_HOST_OPEN
        if ((winkey_host_open) || (incoming_serial_byte == 0))
        {
#endif

          switch (incoming_serial_byte)
          {
          case 0x00:
            winkey_status = WINKEY_ADMIN_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:ADMIN_CMD");
#endif // DEBUG_WINKEY
            break;
          case 0x01:
            winkey_status = WINKEY_SIDETONE_FREQ_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_SIDETONE_FREQ_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x02: // speed command - unbuffered
            winkey_status = WINKEY_UNBUFFERED_SPEED_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_UNBUFFERED_SPEED_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x03: // weighting
            winkey_status = WINKEY_WEIGHTING_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_WEIGHTING_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x04: // PTT lead and tail time
            winkey_status = WINKEY_PTT_TIMES_PARM1_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_PTT_TIMES_PARM1_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x05: // speed pot set
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_SET_POT_PARM1_COMMAND");
#endif // DEBUG_WINKEY
            winkey_status = WINKEY_SET_POT_PARM1_COMMAND;
            break;
          case 0x06:
            winkey_status = WINKEY_PAUSE_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_PAUSE_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x07:
#ifdef FEATURE_POTENTIOMETER
            winkey_port_write(((pot_value_wpm() - pot_wpm_low_value) | 128), 0);
#else
          winkey_port_write((byte(configuration.wpm - pot_wpm_low_value) | 128), 0);
#endif
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:report pot");
#endif // DEBUG_WINKEY
            break;
          case 0x08: // backspace command
            if (send_buffer_bytes)
            {
              send_buffer_bytes--;
            }
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:backspace");
#endif // DEBUG_WINKEY
            break;
          case 0x09:
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_SET_PINCONFIG_COMMAND");
#endif // DEBUG_WINKEY
            winkey_status = WINKEY_SET_PINCONFIG_COMMAND;
            break;
          case 0x0a: // 0A - clear buffer - no parms
            // #ifdef DEBUG_WINKEY
            //   debug_serial_port->println("service_winkey:0A clear buff");
            // #endif //DEBUG_WINKEY
            clear_send_buffer();
            if (winkey_sending)
            {
              // clear_send_buffer();
              winkey_sending = 0;
              winkey_port_write(0xc0 | winkey_sending | winkey_xoff, 0);
            }
            pause_sending_buffer = 0;
            winkey_buffer_counter = 0;
            winkey_buffer_pointer = 0;
#if !defined(OPTION_DISABLE_SERIAL_PORT_CHECKING_WHILE_SENDING_CW)
            loop_element_lengths_breakout_flag = 0;
#endif
#ifdef FEATURE_MEMORIES
            repeat_memory = 255;
#endif
            sending_mode = AUTOMATIC_SENDING;
            manual_ptt_invoke = 0;
            tx_and_sidetone_key(0);
            winkey_speed_state = WINKEY_UNBUFFERED_SPEED;
            configuration.wpm = winkey_last_unbuffered_speed_wpm;
#ifdef DEBUG_WINKEY
            debug_serial_port->print("service_winkey:0A clearbuff send_buffer_bytes:");
            debug_serial_port->println(send_buffer_bytes);
#endif // DEBUG_WINKEY
            break;
          case 0x0b:
            winkey_status = WINKEY_KEY_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_KEY_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x0c:
            winkey_status = WINKEY_HSCW_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_HSCW_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x0d:
            winkey_status = WINKEY_FARNSWORTH_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_FARNSWORTH_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x0e:
            winkey_status = WINKEY_SETMODE_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_SETMODE_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x0f: // bulk load of defaults
            winkey_status = WINKEY_LOAD_SETTINGS_PARM_1_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_LOAD_SETTINGS_PARM_1_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x10:
            winkey_status = WINKEY_FIRST_EXTENSION_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_FIRST_EXTENSION_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x11:
            winkey_status = WINKEY_KEYING_COMPENSATION_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_KEYING_COMPENSATION_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x12:
            winkey_status = WINKEY_UNSUPPORTED_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:0x12unsupport");
#endif // DEBUG_WINKEY
            winkey_parmcount = 1;
            break;
          case 0x13: // NULL command
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:0x13null");
#endif // DEBUG_WINKEY
            break;
          case 0x14:
            winkey_status = WINKEY_SOFTWARE_PADDLE_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_SOFTWARE_PADDLE_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x15:                              // report status
#ifndef OPTION_WINKEY_IGNORE_FIRST_STATUS_REQUEST //--------------------
            status_byte_to_send = 0xc0 | winkey_sending | winkey_xoff;
            if (send_buffer_status == SERIAL_SEND_BUFFER_TIMED_COMMAND)
            {
              status_byte_to_send = status_byte_to_send | 16;
            }
            winkey_port_write(status_byte_to_send, 0);
#ifdef DEBUG_WINKEY
            debug_serial_port->print("service_winkey:0x15 rpt status:");
            debug_serial_port->println(status_byte_to_send);
#endif // DEBUG_WINKEY
#else  // OPTION_WINKEY_IGNORE_FIRST_STATUS_REQUEST ------------------------
          if (ignored_first_status_request)
          {
            status_byte_to_send = 0xc0 | winkey_sending | winkey_xoff;
            if (send_buffer_status == SERIAL_SEND_BUFFER_TIMED_COMMAND)
            {
              status_byte_to_send = status_byte_to_send | 16;
            }
            winkey_port_write(status_byte_to_send, 0);
#ifdef DEBUG_WINKEY
            debug_serial_port->print("service_winkey:0x15 rpt status:");
            debug_serial_port->println(status_byte_to_send);
#endif // DEBUG_WINKEY
          }
          else
          {
            ignored_first_status_request = 1;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:ignored first 0x15 status request");
#endif // DEBUG_WINKEY
          }
#endif // OPTION_WINKEY_IGNORE_FIRST_STATUS_REQUEST --------------------

            break;
          case 0x16: // Pointer operation
            winkey_status = WINKEY_POINTER_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_POINTER_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x17: // dit to dah ratio
            winkey_status = WINKEY_DAH_TO_DIT_RATIO_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_DAH_TO_DIT_RATIO_COMMAND");
#endif // DEBUG_WINKEY
            break;
          // start of buffered commands ------------------------------
          case 0x18: // buffer PTT on/off
            winkey_status = WINKEY_BUFFFERED_PTT_COMMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_BUFFFERED_PTT_COMMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x19:
            winkey_status = WINKEY_KEY_BUFFERED_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_KEY_BUFFERED_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x1a:
            winkey_status = WINKEY_WAIT_BUFFERED_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_WAIT_BUFFERED_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x1b:
            winkey_status = WINKEY_MERGE_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_MERGE_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x1c: // speed command - buffered
            winkey_status = WINKEY_BUFFERED_SPEED_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_BUFFERED_SPEED_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x1d:
            winkey_status = WINKEY_BUFFERED_HSCW_COMMAND;
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_BUFFERED_HSCW_COMMAND");
#endif // DEBUG_WINKEY
            break;
          case 0x1e: // cancel buffered speed command - buffered

#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_CANCEL_BUFFERED_SPEED_COMMAND");
#endif // DEBUG_WINKEY

            if (winkey_speed_state == WINKEY_BUFFERED_SPEED)
            {
              add_to_send_buffer(SERIAL_SEND_BUFFER_WPM_CHANGE);
              add_to_send_buffer(0);
              add_to_send_buffer((byte)winkey_last_unbuffered_speed_wpm);
              winkey_speed_state = WINKEY_UNBUFFERED_SPEED;
#ifdef DEBUG_WINKEY
              debug_serial_port->println("service_winkey:winkey_speed_state = WINKEY_UNBUFFERED_SPEED");
#endif // DEBUG_WINKEY
            }
            else
            {
#ifdef DEBUG_WINKEY
              debug_serial_port->println("service_winkey:WINKEY_CANCEL_BUFFERED_SPEED_COMMAND: no action");
#endif // DEBUG_WINKEY
            }
            winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
            break;
          case 0x1f: // buffered NOP - no need to do anything
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:1F NOP");
#endif // DEBUG_WINKEY
            break;

#ifdef OPTION_WINKEY_EXTENDED_COMMANDS_WITH_DOLLAR_SIGNS
          case 36:
            winkey_status = WINKEY_EXTENDED_COMMAND;
            beep();
#ifdef DEBUG_WINKEY
            debug_serial_port->println("service_winkey:WINKEY_EXTENDED_COMMAND");
#endif // DEBUG_WINKEY
            break;
#endif // OPTION_WINKEY_EXTENDED_COMMANDS_WITH_DOLLAR_SIGNS
          } // switch (incoming_serial_byte)

#ifdef OPTION_WINKEY_STRICT_HOST_OPEN
        } // if ((winkey_host_open) || (incoming_serial_byte == 0))
#endif
      }
    }
    else
    { // if (winkey_status == WINKEY_NO_COMMAND_IN_PROGRESS)

      if (winkey_status == WINKEY_UNSUPPORTED_COMMAND)
      {
        winkey_parmcount--;
#ifdef DEBUG_WINKEY
        debug_serial_port->print("service_winkey:WINKEY_UNSUPPORTED_COMMAND winkey_parmcount:");
        debug_serial_port->println(winkey_parmcount);
#endif // DEBUG_WINKEY
        if (winkey_parmcount == 0)
        {
          winkey_port_write(0xc0 | winkey_sending | winkey_xoff, 0);
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
#ifdef DEBUG_WINKEY
          debug_serial_port->print("service_winkey:WINKEY_UNSUPPORTED_COMMAND:WINKEY_NO_COMMAND_IN_PROGRESS");
          debug_serial_port->println(winkey_parmcount);
#endif // DEBUG_WINKEY
        }
      }

      // WINKEY_LOAD_SETTINGS_PARM_1_COMMAND IS 101
      if ((winkey_status > 100) && (winkey_status < 116))
      { // Load Settings Command - this has 15 parameters, so we handle it a bit differently
        winkey_load_settings_command(winkey_status, incoming_serial_byte);
        winkey_status++;
        if (winkey_status > 115)
        {
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:WINKEY_LOAD_SETTINGS_PARM_15->NOCMD");
#endif // DEBUG_WINKEY
        }
      }

#ifdef OPTION_WINKEY_EXTENDED_COMMANDS_WITH_DOLLAR_SIGNS
      if (winkey_status == WINKEY_EXTENDED_COMMAND)
      {
        // if (incoming_serial_byte != 36){
        // beep();
        //} else {
        boop();
        beep();
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
        //}
      }
#endif // OPTION_WINKEY_EXTENDED_COMMANDS_WITH_DOLLAR_SIGNS

      if (winkey_status == WINKEY_SET_PINCONFIG_COMMAND)
      {
        winkey_set_pinconfig_command(incoming_serial_byte);
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_MERGE_COMMAND)
      {
#ifdef FEATURE_MEMORIES
        repeat_memory = 255;
#endif
        add_to_send_buffer(SERIAL_SEND_BUFFER_PROSIGN);
        add_to_send_buffer(incoming_serial_byte);
        winkey_status = WINKEY_MERGE_PARM_2_COMMAND;
      }
      else
      {
        if (winkey_status == WINKEY_MERGE_PARM_2_COMMAND)
        {
          add_to_send_buffer(incoming_serial_byte);
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
        }
      }

      if (winkey_status == WINKEY_UNBUFFERED_SPEED_COMMAND)
      {
        winkey_unbuffered_speed_command(incoming_serial_byte);
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_FARNSWORTH_COMMAND)
      {
        winkey_farnsworth_command(incoming_serial_byte);
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_HSCW_COMMAND)
      {
        if (incoming_serial_byte == 0)
        {
#ifdef FEATURE_POTENTIOMETER
          configuration.pot_activated = 1;
#endif
        }
        else
        {
          configuration.wpm = ((incoming_serial_byte * 100) / 5);
          winkey_last_unbuffered_speed_wpm = configuration.wpm;
#ifdef OPTION_WINKEY_STRICT_EEPROM_WRITES_MAY_WEAR_OUT_EEPROM
          config_dirty = 1;
#endif
        }
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_BUFFERED_SPEED_COMMAND)
      {
#ifdef DEBUG_WINKEY
        debug_serial_port->print("service_winkey:BUFFERED_SPEED_CMD:send_buffer_bytes:");
        debug_serial_port->println(send_buffer_bytes);
#endif // DEBUG_WINKEY
        add_to_send_buffer(SERIAL_SEND_BUFFER_WPM_CHANGE);
        add_to_send_buffer(0);
        add_to_send_buffer(incoming_serial_byte);
#ifdef DEBUG_WINKEY
        debug_serial_port->print("service_winkey:BUFFERED_SPEED_CMD:send_buffer_bytes:");
        debug_serial_port->println(send_buffer_bytes);
#endif // DEBUG_WINKEY
#ifdef DEBUG_WINKEY
        debug_serial_port->println("service_winkey:WINKEY_BUFFERED_SPEED_COMMAND->NOCMD");
#endif // DEBUG_WINKEY
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_BUFFERED_HSCW_COMMAND)
      {
        if (incoming_serial_byte > 1)
        { // the HSCW command is overloaded; 0 = buffered TX 1, 1 = buffered TX 2, > 1 = HSCW WPM
          unsigned int send_buffer_wpm = ((incoming_serial_byte * 100) / 5);
          add_to_send_buffer(SERIAL_SEND_BUFFER_WPM_CHANGE);
          add_to_send_buffer(highByte(send_buffer_wpm));
          add_to_send_buffer(lowByte(send_buffer_wpm));
        }
        else
        {
          add_to_send_buffer(SERIAL_SEND_BUFFER_TX_CHANGE);
          add_to_send_buffer(incoming_serial_byte + 1);
        }
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_KEY_BUFFERED_COMMAND)
      {
        add_to_send_buffer(SERIAL_SEND_BUFFER_TIMED_KEY_DOWN);
        add_to_send_buffer(incoming_serial_byte);
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_WAIT_BUFFERED_COMMAND)
      {
        add_to_send_buffer(SERIAL_SEND_BUFFER_TIMED_WAIT);
        add_to_send_buffer(incoming_serial_byte);
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_BUFFFERED_PTT_COMMMAND)
      {
        if (incoming_serial_byte)
        {
          add_to_send_buffer(SERIAL_SEND_BUFFER_PTT_ON);
        }
        else
        {
          add_to_send_buffer(SERIAL_SEND_BUFFER_PTT_OFF);
        }
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_POINTER_01_COMMAND)
      { // move input pointer to new positon in overwrite mode
        winkey_buffer_pointer = incoming_serial_byte;
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
#ifdef DEBUG_WINKEY
        debug_serial_port->print("service_winkey:PTR1_CMD->NOCMD winkey_buffer_pointer:");
        debug_serial_port->print(winkey_buffer_pointer);
        debug_serial_port->print("send_buffer_bytes:");
        debug_serial_port->println(send_buffer_bytes);
#endif // DEBUG_WINKEY
      }

      if (winkey_status == WINKEY_POINTER_02_COMMAND)
      { // move input pointer to new position in append mode
        winkey_buffer_pointer = 0;
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
#ifdef DEBUG_WINKEY
        debug_serial_port->print("service_winkey:PTR2_CMD->NOCMD send_buffer_bytes:");
        debug_serial_port->print(send_buffer_bytes);
        debug_serial_port->print(" winkey_buffer_counter:");
        debug_serial_port->print(winkey_buffer_counter);
        debug_serial_port->print(" winkey_buffer_pointer:");
        debug_serial_port->println(winkey_buffer_pointer);
#endif // DEBUG_WINKEY
      }

      if (winkey_status == WINKEY_POINTER_03_COMMAND)
      { // add multiple nulls to buffer
#ifdef DEBUG_WINKEY
        debug_serial_port->print("service_winkey:PTR3_CMD send_buffer_bytes:");
        debug_serial_port->print(send_buffer_bytes);
        debug_serial_port->print(" winkey_buffer_counter:");
        debug_serial_port->print(winkey_buffer_counter);
        debug_serial_port->print(" winkey_buffer_pointer:");
        debug_serial_port->println(winkey_buffer_pointer);
#endif // DEBUG_WINKEY
        byte serial_buffer_position_to_overwrite;
        for (byte x = incoming_serial_byte; x > 0; x--)
        {
          if (winkey_buffer_pointer > 0)
          {
            serial_buffer_position_to_overwrite = send_buffer_bytes - (winkey_buffer_counter - winkey_buffer_pointer) - 1;
            if ((send_buffer_bytes) && (serial_buffer_position_to_overwrite < send_buffer_bytes))
            {
              send_buffer_array[serial_buffer_position_to_overwrite] = SERIAL_SEND_BUFFER_NULL;
            }
            winkey_buffer_pointer++;
          }
          else
          {
            add_to_send_buffer(SERIAL_SEND_BUFFER_NULL);
            winkey_buffer_counter++;
          }
        }
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
#ifdef DEBUG_WINKEY
        debug_serial_port->print("service_winkey:PTR3_CMD->NO_CMD send_buffer_bytes:");
        debug_serial_port->print(send_buffer_bytes);
        debug_serial_port->print(" winkey_buffer_counter:");
        debug_serial_port->print(winkey_buffer_counter);
        debug_serial_port->print(" winkey_buffer_pointer:");
        debug_serial_port->println(winkey_buffer_pointer);
#endif // DEBUG_WINKEY
      }

      if (winkey_status == WINKEY_POINTER_COMMAND)
      {
        switch (incoming_serial_byte)
        {
        case 0x00:
          winkey_buffer_counter = 0;
          winkey_buffer_pointer = 0;
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
#ifdef DEBUG_WINKEY
          debug_serial_port->print("service_winkey:PTR_CMD->NOCMD send_buffer_bytes:");
          debug_serial_port->print(send_buffer_bytes);
          debug_serial_port->print(" winkey_buffer_counter:");
          debug_serial_port->print(winkey_buffer_counter);
          debug_serial_port->print(" winkey_buffer_pointer:");
          debug_serial_port->println(winkey_buffer_pointer);
#endif // DEBUG_WINKEY
          break;
        case 0x01:
          winkey_status = WINKEY_POINTER_01_COMMAND;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:PTR1_CMD");
#endif // DEBUG_WINKEY
          break;
        case 0x02:
          winkey_status = WINKEY_POINTER_02_COMMAND; // move to new position in append mode
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:PTR2_CMD");
#endif // DEBUG_WINKEY
          break;
        case 0x03:
          winkey_status = WINKEY_POINTER_03_COMMAND;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:PTR3_CMD");
#endif // DEBUG_WINKEY
          break;
        default:
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:PTR_CMD->NOCMD");
#endif // DEBUG_WINKEY
          break;
        }
      }

#ifdef OPTION_WINKEY_2_SUPPORT
      if (winkey_status == WINKEY_SEND_MSG)
      {
        if ((incoming_serial_byte > 0) && (incoming_serial_byte < 7))
        {
          add_to_send_buffer(SERIAL_SEND_BUFFER_MEMORY_NUMBER);
          add_to_send_buffer(incoming_serial_byte - 1);
#ifdef FEATURE_MEMORIES
          repeat_memory = 255;
#endif
        }
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }
#endif // OPTION_WINKEY_2_SUPPORT

      if (winkey_status == WINKEY_ADMIN_COMMAND)
      {
        switch (incoming_serial_byte)
        {
        case 0x00:
          winkey_status = WINKEY_UNSUPPORTED_COMMAND;
          winkey_parmcount = 1;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:calib cmd UNSUPPORTED_COMMAND await 1 parm");
#endif           // DEBUG_WINKEY
          break; // calibrate command
        case 0x01:
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:WINKEY_ADMIN_COMMAND 0x01");
#endif                           // DEBUG_WINKEY
#if defined(__AVR__)             // #ifndef ARDUINO_SAM_DUE
          asm volatile("jmp 0"); /*wdt_enable(WDTO_30MS); while(1) {};*/
#else
          setup();
#endif             //__AVR__
          break;   // reset command
        case 0x02: // host open command - send version back to host
#ifdef OPTION_WINKEY_2_SUPPORT
          winkey_port_write(WINKEY_2_REPORT_VERSION_NUMBER, 1);
#else  // OPTION_WINKEY_2_SUPPORT
          winkey_port_write(WINKEY_1_REPORT_VERSION_NUMBER, 1);
#endif // OPTION_WINKEY_2_SUPPORT
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          manual_ptt_invoke = 0;
          winkey_host_open = 1;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD hostopen");
#endif // DEBUG_WINKEY
#if defined(OPTION_WINKEY_BLINK_PTT_ON_HOST_OPEN)
          blink_ptt_dits_and_dahs("..");
#else
          boop_beep();
#endif
          break;
        case 0x03: // host close command
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          manual_ptt_invoke = 0;
          winkey_host_open = 0;
#ifdef OPTION_WINKEY_SEND_VERSION_ON_HOST_CLOSE
#ifdef OPTION_WINKEY_2_SUPPORT
          winkey_port_write(WINKEY_2_REPORT_VERSION_NUMBER, 1);
#else  // OPTION_WINKEY_2_SUPPORT
          winkey_port_write(WINKEY_1_REPORT_VERSION_NUMBER, 1);
#endif // OPTION_WINKEY_2_SUPPORT
#endif // OPTION_WINKEY_SEND_VERSION_ON_HOST_CLOSE
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD hostclose");
#endif // DEBUG_WINKEY
#if defined(OPTION_WINKEY_BLINK_PTT_ON_HOST_OPEN)
          blink_ptt_dits_and_dahs("--");
#else
          boop_beep();
#endif
          config_dirty = 1;
#if defined(OPTION_WINKEY_2_SUPPORT) && !defined(OPTION_WINKEY_2_HOST_CLOSE_NO_SERIAL_PORT_RESET)
          primary_serial_port->end();
          primary_serial_port->begin(1200);
#endif
          break;
        case 0x04: // echo command
          winkey_status = WINKEY_ADMIN_COMMAND_ECHO;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD_ECHO");
#endif // DEBUG_WINKEY
          break;
        case 0x05: // paddle A2D
          winkey_port_write(WINKEY_RETURN_THIS_FOR_ADMIN_PADDLE_A2D, 0);
          // winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          winkey_status = WINKEY_ADMIN_PADDLE_A2D_PARM_1;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD paddleA2D");
#endif // DEBUG_WINKEY
          break;
        case 0x06: // speed A2D
          winkey_port_write(WINKEY_RETURN_THIS_FOR_ADMIN_SPEED_A2D, 0);
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD speedA2D");
#endif // DEBUG_WINKEY
          break;
        case 0x07: // Get values
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD winkey_admin_get_values");
#endif // DEBUG_WINKEY
          winkey_admin_get_values_command();
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          break;
        case 0x08: // reserved
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD0x08reserved-WTF");
#endif // DEBUG_WINKEY
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          break;
        case 0x09: // get cal on WK1, unimplemented on WK2, getMajorVersion on WK3
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMDgetcal");
#endif // DEBUG_WINKEY
#if defined(OPTION_WINKEY_2_SUPPORT)
          winkey_port_write(WINKEY_RETURN_THIS_FOR_ADMIN_GET_CAL_WK2, 1); // Docs say this should be 0, but this is a hack for compatibility
#else
          winkey_port_write(WINKEY_RETURN_THIS_FOR_ADMIN_GET_CAL_WK1, 1);
#endif
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          break;
#ifdef OPTION_WINKEY_2_SUPPORT
        case 0x0a: // set wk1 mode (10)
          wk2_mode = 1;
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD wk2_mode1");
#endif // DEBUG_WINKEY
          break;
        case 0x0b: // set wk2 mode (11)
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD wk2_mode2");
#endif // DEBUG_WINKEY
          beep();
          beep();
          wk2_mode = 2;
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          break;
        case 0x0c: // download EEPPROM 256 bytes (12)
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD winkey_eeprom_download");
#endif // DEBUG_WINKEY
          winkey_eeprom_download();
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          break;
        case 0x0d: // upload EEPROM 256 bytes (13)
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD uploadEEPROM");
#endif                                                // DEBUG_WINKEY
          winkey_status = WINKEY_UNSUPPORTED_COMMAND; // upload EEPROM 256 bytes
          winkey_parmcount = 256;
          break;
        case 0x0e: //(14)
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD WINKEY_SEND_MSG");
#endif // DEBUG_WINKEY
          winkey_status = WINKEY_SEND_MSG;
          break;
        case 0x0f: // load xmode (15)
          winkey_status = WINKEY_UNSUPPORTED_COMMAND;
          winkey_parmcount = 1;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD loadxmode");
#endif // DEBUG_WINKEY
          break;
        case 0x10: // reserved (16)
          winkey_port_write(zero, 0);
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          break;
        case 0x11: // set high baud rate (17)
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD sethighbaudrate");
#endif // DEBUG_WINKEY
          primary_serial_port->end();
          primary_serial_port->begin(9600);
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          break;
        case 0x12: // set low baud rate (18)
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD setlowbaudrate");
#endif // DEBUG_WINKEY
          primary_serial_port->end();
          primary_serial_port->begin(1200);
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          break;
#endif // OPTION_WINKEY_2_SUPPORT

#ifdef FEATURE_SO2R_BASE
        case 0xF0: // Send SO2R device information
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD getSO2Rdeviceinfo");
#endif

          static const uint8_t device_info[] = {
              0xAA, 0x55, 0xCC, 0x33, // Header
              1, 0, 0,                // SO2R Major, minor, patch
              1, 0,                   // Protocol major, minor
              0,                      // capabilities - bit 0 is stereo reverse, others undefined
          };

          for (uint8_t i = 0; i < sizeof(device_info); i++)
          {
            winkey_port_write(device_info[i], 1);
          }

          static const byte *name = (byte *)SO2R_DEVICE_NAME;
          for (uint8_t i = 0; i < sizeof(SO2R_DEVICE_NAME); i++)
          {
            winkey_port_write(name[i], 1);
          }

          static const byte *version = (byte *)CODE_VERSION;
          for (uint8_t i = 0; i < sizeof(CODE_VERSION); i++)
          {
            winkey_port_write(version[i], 1);
          }
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          break;

        case 0xFF: // No-op
          debug_serial_port->println("service_winkey:NO-OP");
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
          break;

#endif // FEATURE_SO2R_BASE

        default:
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD->NOCMD");
#endif // DEBUG_WINKEY
          break;
        }
      }
      else
      {
        if (winkey_status == WINKEY_ADMIN_COMMAND_ECHO)
        {
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD echoabyte.");
#endif // DEBUG_WINKEY
          winkey_port_write(incoming_serial_byte, 1);
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
        }
      }

      if (winkey_status == WINKEY_KEYING_COMPENSATION_COMMAND)
      {
#ifdef DEBUG_WINKEY
        debug_serial_port->println("service_winkey:ADMIN_CMD WINKEY_KEYING_COMPENSATION_COMMAND byte");
#endif // DEBUG_WINKEY
        configuration.keying_compensation = incoming_serial_byte;
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_FIRST_EXTENSION_COMMAND)
      {
#ifdef DEBUG_WINKEY
        debug_serial_port->println("service_winkey:ADMIN_COMMAND WINKEY_FIRST_EXTENSION_COMMAND byte");
#endif // DEBUG_WINKEY
        first_extension_time = incoming_serial_byte;
#ifdef DEBUG_WINKEY_PROTOCOL_USING_CW
        send_char('X', KEYER_NORMAL);
#endif
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_PAUSE_COMMAND)
      {
        if (incoming_serial_byte)
        {
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD WINKEY_PAUSE_COMMANDpause");
#endif // DEBUG_WINKEY
          pause_sending_buffer = 1;
        }
        else
        {
#ifdef DEBUG_WINKEY
          debug_serial_port->println("service_winkey:ADMIN_CMD WINKEY_PAUSE_COMMANDunpause");
#endif // DEBUG_WINKEY
          pause_sending_buffer = 0;
        }
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_KEY_COMMAND)
      {
#ifdef FEATURE_MEMORIES
        repeat_memory = 255;
#endif
        sending_mode = AUTOMATIC_SENDING;
        if (incoming_serial_byte)
        {
          tx_and_sidetone_key(1);
        }
        else
        {
          tx_and_sidetone_key(0);
        }
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_DAH_TO_DIT_RATIO_COMMAND)
      {
        winkey_dah_to_dit_ratio_command(incoming_serial_byte);
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_WEIGHTING_COMMAND)
      {
        winkey_weighting_command(incoming_serial_byte);
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_PTT_TIMES_PARM1_COMMAND)
      {
        winkey_ptt_times_parm1_command(incoming_serial_byte);
        winkey_status = WINKEY_PTT_TIMES_PARM2_COMMAND;
      }
      else
      {
        if (winkey_status == WINKEY_PTT_TIMES_PARM2_COMMAND)
        {
          winkey_ptt_times_parm2_command(incoming_serial_byte);
          winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
        }
      }

      if (winkey_status == WINKEY_SET_POT_PARM1_COMMAND)
      {
        winkey_set_pot_parm1_command(incoming_serial_byte);
        winkey_status = WINKEY_SET_POT_PARM2_COMMAND;
      }
      else
      {
        if (winkey_status == WINKEY_SET_POT_PARM2_COMMAND)
        {
          winkey_set_pot_parm2_command(incoming_serial_byte);
          winkey_status = WINKEY_SET_POT_PARM3_COMMAND;
        }
        else
        {
          if (winkey_status == WINKEY_SET_POT_PARM3_COMMAND)
          {                                                     // third parm is max read value from pot, depending on wiring
            winkey_set_pot_parm3_command(incoming_serial_byte); // WK2 protocol just ignores this third parm
            winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;      // this is taken care of in winkey_set_pot_parm3()
          }
        }
      }

      if (winkey_status == WINKEY_SETMODE_COMMAND)
      {
        winkey_setmode_command(incoming_serial_byte);
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_SOFTWARE_PADDLE_COMMAND)
      {
#ifdef FEATURE_MEMORIES
        repeat_memory = 255;
#endif
        switch (incoming_serial_byte)
        {
        case 0:
          winkey_dit_invoke = 0;
          winkey_dah_invoke = 0;
          break;
        case 1:
          winkey_dit_invoke = 0;
          winkey_dah_invoke = 1;
          break;
        case 2:
          winkey_dit_invoke = 1;
          winkey_dah_invoke = 0;
          break;
        case 3:
          winkey_dah_invoke = 1;
          winkey_dit_invoke = 1;
          break;
        }
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_SIDETONE_FREQ_COMMAND)
      {
        winkey_sidetone_freq_command(incoming_serial_byte);
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_ADMIN_PADDLE_A2D_PARM_3)
      {
#ifdef DEBUG_WINKEY
        debug_serial_port->println("service_winkey:ADMIN_COMMAND WINKEY_ADMIN_PADDLE_A2D_PARM_3 byte");
#endif // DEBUG_WINKEY
        winkey_status = WINKEY_NO_COMMAND_IN_PROGRESS;
      }

      if (winkey_status == WINKEY_ADMIN_PADDLE_A2D_PARM_2)
      {
#ifdef DEBUG_WINKEY
        debug_serial_port->println("service_winkey:ADMIN_COMMAND WINKEY_ADMIN_PADDLE_A2D_PARM_2 byte");
#endif // DEBUG_WINKEY
        winkey_status = WINKEY_ADMIN_PADDLE_A2D_PARM_3;
      }
      if (winkey_status == WINKEY_ADMIN_PADDLE_A2D_PARM_1)
      {
        winkey_status = WINKEY_ADMIN_PADDLE_A2D_PARM_2;
#ifdef DEBUG_WINKEY
        debug_serial_port->println("service_winkey:ADMIN_COMMAND WINKEY_ADMIN_PADDLE_A2D_PARM_1 byte");
#endif // DEBUG_WINKEY
      }

    } // else (winkey_status == WINKEY_NO_COMMAND_IN_PROGRESS)
  } // if (action == SERVICE_SERIAL_BYTE
}
#endif // FEATURE_WINKEY_EMULATION

//-------------------------------------------------------------------------------------------------------