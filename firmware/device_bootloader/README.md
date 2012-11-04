# Bootloader Protocol

 Command | Host to IOIO                                                         | IOIO to Host
---------|----------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------
 0x00    | **HardReset**<br>Followed by 4 bytes:<br>4B magic 'I', 'O', 'I', 'O' | **EstablishConnection**<br>Followed by 28 bytes:<br>4B magic 'B', 'O', 'O', 'T'<br>8B hardwareId<br>8B booltloaderId<br>8B platformId
 0x01    | **CheckInterface**<br>Followed by:<br>8B: interfaceId                | **CheckInterfaceResponse**<br>Followed by 1 byte with the format:<br>`XXXXXXXs`<br>`s` - supported
 0x02    | **ReadFingerprint**<br>No arguments.                                 | **FingerPrint**<br>Followed by a 16B fingerprint.
 0x03    | **WriteFingerprint**<br>Followed by a 16B fingerprint.               | Reserved.
 0x04    | **WriteImage**<br>Followed by:<br>4B little-endian size.<br>*size*B contents of a `.ioio` file. | **Checksum**<br>Followed by 2B: 16-bit little-endian unsigned sum of all the bytes in the `.ioio` file.

