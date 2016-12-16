# OpenThread on CC2538 Example

This directory contains example platform drivers for the [Texas
Instruments CC2538][cc2538].

[cc2538]: http://www.ti.com/product/CC2538

The example platform drivers are intended to present the minimal code
necessary to support OpenThread.  As a result, the example platform
drivers do not necessarily highlight the platform's full capabilities.

## Toolchain

Download and install the [GNU toolchain for ARM
Cortex-M][gnu-toolchain].

[gnu-toolchain]: https://launchpad.net/gcc-arm-embedded

## Build Examples

```bash
$ cd <path-to-openthread>
$ ./bootstrap
$ make -f examples/Makefile-kw2x
```

After a successful build, the `elf` files are found in
`<path-to-openthread>/output/bin`.  You can convert them to `bin`
files using `arm-none-eabi-objcopy`:
```bash
$ arm-none-eabi-objcopy -O binary arm-none-eabi-ot-cli-ftd arm-none-eabi-ot-cli-ftd.bin
```

## Flash Binaries

Compiled binaries may be flashed onto the CC2538 using the [Serial
Bootloader Interface][cc2538-bsl] or [JTAG interface][jtag].  The
[cc2538-bsl.py script][cc2538-bsl-tool] provides a convenient method
for flashing a CC2538 via the UART.

[cc2538-bsl]: http://www.ti.com/lit/an/swra466a/swra466a.pdf
[cc2538-bsl-tool]: https://github.com/JelmerT/cc2538-bsl
[jtag]: https://en.wikipedia.org/wiki/JTAG

## Interact

1. Open terminal to `/dev/ttyACM0` (serial port settings: 115200 8-N-1).
2. Type `help` for list of commands.

```bash
> help
help
channel
childtimeout
contextreusedelay
extaddr
extpanid
ipaddr
keysequence
leaderweight
masterkey
mode
netdataregister
networkidtimeout
networkname
panid
ping
prefix
releaserouterid
rloc16
route
routerupgradethreshold
scan
start
state
stop
whitelist
```


1. add "#pragma GCC diagnostic ignored "-Wpedantic"" in MK21D5.h to avoid C99 does not support unnamed union or struct for c lanaguage.

2. PortConfig.h 
98   //#warning "No target defined!"

3. Source/GPIO/GPIO.h
40 #include "MK21D5.h"

4. Source/NVIC/Interrupt.c
26 #include "MK21D5.h"

5. NVIC.h

78 #define EnableInterrupts() __asm(" CPSIE i"); // asm -> __asm

84 #define DisableInterrupts() __asm(" CPSID i");

6. RNG.C
   "mmcau_interface.h"  ->  "MMCAU_Interface.h"

7. virtual_com.c 
589    unsigned char const *pBuf = NULL; // add NULL

8. voliate static -> static voliate

9. usb_dci.c add header file:   46 #include "Interrupt.h" 

11. virtual_com.c
   - add CDC_App_callback()
   - extern bool_t CDC_Transmit(unsigned char const *pBuf, usbPacketSize_t bufLen, void (*pfCallBack)(unsigned char const *pBuf));
     change index_t -> usbPacketSize (uint8_t -> uint16_t) otherwise, when buflen > 255, cannot display all char

12. LED.c
    37 #include "LED.h"  ->  "Led.h"
