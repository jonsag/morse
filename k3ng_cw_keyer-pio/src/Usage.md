# K3NG Arduino CW Keyer

## Copyright

 Copyright 2011 - 2021 Anthony Good, K3NG  
 All trademarks referred to in source code and documentation are copyright their respective owners.  

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.  

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.  

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.  

## Commercial

If you offer a hardware kit using this software, show your appreciation by sending the author a complimentary kit or a bottle of bourbon ;-)  

Full documentation can be found at [https://github.com/k3ng/k3ng_cw_keyer/wiki](https://github.com/k3ng/k3ng_cw_keyer/wiki).  Please read it before requesting help.  

For help, please post on the Radio Artisan group: [https://groups.io/g/radioartisan](https://groups.io/g/radioartisan).  Please do not email the developer directly for support.  Thanks  

YouTube Channel: [https://www.youtube.com/channel/UC5o8UM1-heT5kJbwnJRkUYg](https://www.youtube.com/channel/UC5o8UM1-heT5kJbwnJRkUYg)  

2020 Recipient of the Amateur Radio Software Award [https://amateurradiosoftwareaward.github.io/](https://amateurradiosoftwareaward.github.io/)  

Wordsworth CW training method created by George Allison, K1IG  
English code training word lists from gen_cw_words.pl by Andy Stewart, KB1OIQ  

Command Line Interface ("CLI") (USB Port) (Note: turn on carriage return if using Arduino Serial Monitor program)  

    CW Keyboard: type what you want the keyer to send (all commands are preceded with a backslash ( \ )  

    \?     Help                                      (requires FEATURE_SERIAL_HELP)  
    \/     Paged Help                                (requires FEATURE_SERIAL_HELP)  
    \#     Play memory #                             (requires FEATURES_MEMORIES; play memories 1 - 10 (0 = memory 10) )  
    \a     Iambic A mode  
    \b     Iambic B mode  
    \c     Single Paddle mode  
    \d     Ultimatic mode (if OPTION_NO_ULTIMATIC not set)  
    \e#### Set serial number to ####  
    \f#### Set sidetone frequency to #### hertz  
    \g     Bug mode  
    \h     Toggle between CW and Hell sending                    (requires FEATURE_HELL)  
    \i     Transmit enable/disable  
    \j###  Dah to dit ratio (300 = 3.00, do \j alone to set to default)  
    \k     CW Training Module                                     (requires FEATURE_TRAINING_COMMAND_LINE_INTERFACE)  
    \l##   Set weighting (50 = normal, do \l alone to set to default)  
    \m###  Set Farnsworth speed  
    \n     Toggle paddle reverse  
    \o     Toggle sidetone on/off  
    \p#(#) Program memory #  
    \q##   Switch to QRSS mode, dit length ## seconds  
    \r     Switch to regular speed mode  
    \s     Status  
    \t     Tune mode  
    \u     Manual PTT toggle  
    \v     Toggle potentiometer active / inactive   (requires FEATURE_POTENTIOMETER)  
    \w###  Set speed in WPM  
    \x#    Switch to transmitter #  
    \y#    Change wordspace to # elements (# = 1 to 9)  
    \z     Autospace on/off  
    \+     Create prosign  
    \!##   Repeat play memory  
    \|#### Set memory repeat (milliseconds)  (backslash and pipe)  
    \*     Toggle paddle echo  
    \`     Toggle straight key echo  
    \^     Toggle wait for carriage return to send CW / send CW immediately  
    \&     Toggle CMOS Super Keyer Timing on/off  
    \%##   Set CMOS Super Keyer Timing %  
    \.     Toggle dit buffer on/off  
    \-     Toggle dah buffer on/off  
    \~     Reset unit  
    \;     Toggle cw send echo  
    \{     QLF mode on/off  
    \>     Send serial number, then increment  
    \<     Send current serial number  
    \(     Send current serial number in cut numbers  
    \)     Send serial number with cut numbers, then increment  
    \[     Set Quiet Paddle Interruption  
    \=     Toggle American Morse mode    (requires FEATURE_AMERICAN_MORSE)  
    \@     Mill Mode  
    \}#### Set potentiometer range - low ## / high ##  
    \"     Hold PTT active with buffered characters  
    \]     PTT Enable / Disable  
    \_     Beacon Mode at Boot Up Enable / Disable (requires FEATURE_BEACON_SETTING)  
    \;     CW Send Inhibit Enable / Disable  
    \\     Immediately clear the buffer, stop memory sending, etc.  
    \:     Extended CLLI commands  
            eepromdump              - do a byte dump of EEPROM for troubleshooting  
            saveeeprom <filename>   - store EEPROM in a file  
            loadeeprom <filename>   - load into EEPROM from a file  
            printlog                - print the SD card log  
            clearlog                - clear the SD card log  
            ls <directory>          - list files in SD card directory  
            cat <filename>          - print filename on SD card  
            pl <transmitter> <mS>   - Set PTT lead time  
            pt <transmitter> <mS>   - Set PTT tail time  
            af ###                  - Set autospace timing factor; 100 = 1.00  
            pf ###                  - Set paddle echo timing factor; 100 = 1.00  

Buttons  

    button 0: command mode / command mode exit  
    button 0 + left paddle:  increase cw speed  
    button 0 + right paddle: decrease cw speed  
    button 1 - 12 hold + left paddle: repeat memory  
    button 1 - 6 half second hold: switch to TX # 1 - 6  

Command Mode (press button0 to enter command mode and press again to exit)  

    A  Switch to Iambic A mode  
    B  Switch to Iambic B mode  
    C  Switch to Single Paddle Mode  
    D  Switch to Ultimatic mode (if OPTION_NO_ULTIMATIC not set)  

    E  Announce speed  
    F  Adjust sidetone frequency  
    G  Switch to bug mode  
    H  Set weighting and dah to dit ratio to defaults  
    I  TX enable / disable  
    J  Dah to dit ratio adjust  
    K  Toggle Dit and Dah Buffers on and off (Ultimatic Mode) (if OPTION_NO_ULTIMATIC is not set)  
    L  Adjust weighting  
    M  Change command mode speed  
    N  Toggle paddle reverse  
    O  Toggle sidetone on / off  
    P#(#) Program a memory  
    Q  Adjust keying compensation (left paddle = increase, right paddle = decrease)  
    R####  Set serial number to ####  
    S  Alphabet code practice (FEATURE_ALPHABET_SEND_PRACTICE)  
    T  Tune mode  
    U  Receive / Send Echo Practice  
    V  Toggle potentiometer active / inactive  
    W  Change speed  
    X  Exit command mode (you can also press the command button (button0) to exit)  
    Y#### Change memory repeat delay to #### mS  
    Z  Autospace On/Off  
    #  Play a memory without transmitting  
    =  Enable / disable PTT Line (-...-)  
    ?  Status  
         1. Speed in WPM  
         2. Keyer Mode (A = Iambic A, B = Iambic B, G = Bug, S = Single Paddle, U = Ultimatic)  
         3. Weighting  
         4. Dah to Dit Ratio  

Memory Macros  

    \#     Jump to memory #  
    \c     Play serial number with cut numbers, then increment  
    \d###  Delay for ### seconds  
    \e     Play serial number, then increment  
    \f#### Change sidetone to #### hertz (must be four digits - use leading zero below 1000 hz)  
    \h     Switch to Hell sending  
    \i#    Insert memory number  
    \l     Switch to CW (from Hell mode)  
    \n     Decrement serial number, do not send  
    \q##   Switch to QRSS mode, dit length ## seconds  
    \r     Switch to regular speed mode  
    \s     Insert space  
    \t###  Transmit for ### seconds (must be three digits, use leading zeros if necessary)  
    \u     Activate PTT  
    \v     Deactivate PTT  
    \w###  Set regular mode speed to ### WPM (must be three digits, use leading zeros if necessary)  
    \x#    Switch to transmitter # (1, 2, or 3)  
    \y#    Increase speed # WPM  
    \z#    Decrease speed # WPM  
    \+     Prosign the next two characters  

PS2 / USB Keyboard  

    CTRL-A           Iambic A  
    CTRL-B           Iambic B  
    CTRL-C           Single Paddle  
    CTRL-D           Ultimatic                            (if OPTION_NO_ULTIMATIC not set)  
    CTRL-E           Set Serial Number  
    CTRL-G           Bug  
    CTRL-H           Toggle Hell Mode On/Off              (requires FEATURE_HELL)  
    CTRL-I           TX enable / disable  
    CTRL-M           Set Farnsworth Speed (0 = disabled)  (requires FEATURE_FARNSWORTH)  
    CTRL-N           Paddle Reverse  
    CTRL-O           Toggle Sidetone On/Off  
    CTRL-S           CMOS Superkeyer Timing On/Off  
    CTRL-T           Tune  
    CTRL-U           Manual PTT Toggle  
    CTRL-W           Set WPM  
    CTRL-F1          Switch to TX #1  
    CTRL-F2          Switch to TX #2  
    CTRL-F3          Switch to TX #3  
    CTRL-F4          Switch to TX #4  
    CTRL-F5          Switch to TX #5  
    CTRL-F6          Switch to TX #6  
    END              Send serial number no increment  
    ESC              Stop sending and clear buffer  
    F1, F2, F3..     Play memory 1, 2, 3...  
    DOWN ARROW       Decrease WPM  
    HOME             Reset timing settings  
    INSERT           Send serial number and increment  
    LEFT ARROW       Decrease Dah to Dit Ratio  
    PGDN             Decrease Sidetone Frequency  
    PGUP             Increase Sidetone Frequency  
    RIGHT ARROW      Increase Dah to Dit Ratio  
    SCROLL LOCK      Prosign Next Two Characters  
    SHIFT-BACKSPACE  Decrement serial number  
    SHIFT-F1, F2...  Program Memory 1, 2...  
    ALT-F1, F2...    Repeat Memory 1, 2...  
    TAB              Pause Sending Immediately  
    UP ARROW         Increase WPM  
    Keypad /         Dit Paddle (USB Keyboard Only)  
    Keypad *         Dah Paddle (USB Keyboard Only)  
    Keypad ENTER     Tune / Straight Key (USB Keyboard Only)  

USB Mouse  

    Left Button      Dit  
    Right Button     Dah  
    Middle Button    Tune / Straight Key  

PS2 Keyboard Notes (FEATURE_PS2_KEYBOARD)  

    To use FEATURE_PS2_KEYBOARD you need the K3NG_PS2Keyboard.h and K3NG_PS2Keyboard.cpp library files from [https://github.com/k3ng/k3ng_cw_keyer/tree/master/libraries](https://github.com/k3ng/k3ng_cw_keyer/tree/master/libraries)  

    Some keyboards may require a reset sequence upon startup.  This is activated with OPTION_PS2_KEYBOARD_RESET.  

USB Keyboard Notes (FEATURE_USB_KEYBOARD)  

    To use a USB keyboard you need to download and install this library: [https://github.com/felis/USB_Host_Shield_2.0](https://github.com/felis/USB_Host_Shield_2.0). You may use an Arduino Mega  
    ADK board (which has a built in USB host interface, get or Circuits@Home USB shield [(http://www.circuitsathome.com/products-page/arduino-shields/usb-host-shield-2-0-for-arduino)](http://www.circuitsathome.com/products-page/arduino-shields/usb-host-shield-2-0-for-arduino), or built your own MAX3421 based USB port.  

    If you are using an Arduino Mega ADK with Arduino IDE older than version 1.5.5, you must customize the USB Host Shield Library settings.h file!  

    If you are using Arduino IDE older than version 1.5.5 and you experience a compiler error, you may need to add these lines to your keyer.h file:  

        #define HID_PROTOCOL_KEYBOARD 1  
        #define HID_PROTOCOL_MOUSE 2  

    (Thanks Raimo, DL1HTB, for the two notes above.)  

    Option Usb Computer Keyboard Emulation FEATURE_CW_COMPUTER_KEYBOARD  
    (Arduino Due, Leonardo only)  

       You can use your cw key as a computer keyboard. Your computer recognize the K3NG keyer as a normal keyboard.  
       Language available English and Italian (more languages to add)  
       Use following prosign to emulate Enter Key, Caps Lock, space and backspace:  
       Prosign AA "Enter"  
       Prosign DO "Caps Lock" (enable and disable)  
       "......" or more "Backspace"  
       "------" or more "Space"  

SIDETONE_SWITCH

       Enabling this feature and an external toggle switch  adds switch control for playing cw sidetone.  
       ST Switch status is displayed in the status command.  This feature will override the software control of the sidetone (\o).  

Useful Stuff

    Reset to defaults: squeeze both paddles at power up (good to use if you dorked up the speed and don't have the CLI)  
    Press the right paddle to enter straight key mode at power up  
    Press the left  paddle at power up to enter and stay forever in beacon mode  
