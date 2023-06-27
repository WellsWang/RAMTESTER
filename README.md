# RAMTESTER
Tester for 4116/4164/41256 or compatible DRAM chip

## Source Code
This project is based on 4116 Tester developed by <uffe@frostbox-net>.

Original project could be found at: 
https://labs.frostbox.net/2020/03/24/4116-d-ram-tester-with-schematics-and-code/

This version was added following new features:
 *  4164/41256 or compatible support
 *  OLED display
 *  Serial output port
 *  In 4164/41256 mode, auto detect 4164 chip
 *  Chinese UI support

You can use Arduino IDE to compile and upload the code to ATMEGA328p MCU.

## Hardware Design
You may check the Schematic PNG file in repository for detail.

![Schematic_RAMTesterPro](https://github.com/WellsWang/RAMTESTER/assets/2504078/87bf67ea-378b-4380-833e-6582d58e168f)

BOM List:

![BOM](https://github.com/WellsWang/RAMTESTER/assets/2504078/572f9074-d9ca-4313-97e4-11d2101b7e8c)

## !!!NOTICE!!!
You should chose the **CORRECT** model via SW1 switch **BEFORE** you put the chip into the ZIFF socket.

Or, it will **DAMAGE** the chip for the wrong voltage settings!!!

Or, it will **DAMAGE** the chip for the wrong voltage settings!!!

Or, it will **DAMAGE** the chip for the wrong voltage settings!!!

## Prototypes
![prototype1](https://github.com/WellsWang/RAMTESTER/assets/2504078/6ddb3170-f2c0-49b3-86c5-ab4bb919a6cb)
![prototype2](https://github.com/WellsWang/RAMTESTER/assets/2504078/cd4b46e3-a0a3-41a7-8eb3-6eadbb56d041)
![prototype3](https://github.com/WellsWang/RAMTESTER/assets/2504078/ebc59f24-f384-41d6-b918-001b6c1a1ad7)

## License
Released under GPL v3 License.
See the LISENCE file for more details.
