# Arduino-KIMlib
KIMaip KNX / EIB Arduino Library

This design for KNX I2C Interface module KIMaip, it consists of a micro controller running the certified KNX communication stack KAIstack and the high performance KNX interface to the KNX bus - KAIphys. This design enables direct usage of the high power supply from the bus.

It is easy to read and write KNX communication object or KNX parameter into KIMaip device.

You can write a program logic with Arduino IDE (C++), next step, you can use ETS for link the KNX object to group address and download application, configuration and address into KIMaip device.

This library is intended to use the module directly to an application controller and is thus optimized as basis for end devices.

In the library folder there is an ETS (V5) file that contains the KIMaip device to copy and use in your project.
The ETS file is the same one used to load the DEMO application used to make the examples contained in the library work.
The device present in it must be configured according to the needs of your project:
    - General configurations;
    - Type of communication object (1Bit, 2Bit, 1Byte ... etc ..);
    - User Parameter preset value.

You can buy KIMaip KNX / EIB shield for your experiment whit Arduino.
Full version: https://www.ebay.it/itm/324815210159
Light version: https://www.ebay.it/itm/324939880226

Enjoy. :-)
