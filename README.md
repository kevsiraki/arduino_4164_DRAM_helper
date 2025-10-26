# 4164 DRAM Controller Helpers

This project demonstrates how to interface a **4164 Dynamic RAM (DRAM)** chip with an **Arduino**, using it as a true external memory device for data storage and retrieval. The Arduino manages all row/column addressing, data I/O, write operations, and refresh cycles — providing full control of the 4164 chip without emulation or shadow memory.

## Overview

The sketch allows interactive reading and writing of DRAM contents via the Arduino Serial Monitor. All DRAM operations are performed using the proper **RAS**, **CAS**, and **WRITE** control signals, as well as dedicated data lines for input (`D`) and output (`Q`). The system periodically refreshes all rows to maintain data integrity, emulating a basic memory controller.

## Hardware Connections

| Signal  | Arduino Pin | Description                    |
| ------- | ----------- | ------------------------------ |
| RAS     | 4           | Row Address Strobe             |
| CAS     | 5           | Column Address Strobe          |
| WRITE   | 9           | Write Enable                   |
| D       | 6           | Data Input to DRAM             |
| Q       | 8           | Data Output from DRAM          |
| Q_LED   | 7           | LED indicator for output data  |
| CLK_LED | 12          | LED indicator for clock cycles |
| A0–A5   | A0–A5       | Address lines                  |
| A6–A7   | 10, 11      | Additional address lines       |

The DRAM’s address and data pins are directly driven by the Arduino, ensuring that all reads and writes occur at the hardware level.

## Features

* **True 4164 DRAM interfacing** – No software-level emulation or shadow memory.
* **Interactive serial control** – Send commands to inspect or modify memory contents.
* **Automatic refresh** – Periodic row refresh to maintain DRAM data.
* **Array-to-RAM copy utility** – Load predefined data patterns for testing.
* **LED feedback** – Optional visualization of data and timing activity.

## Serial Commands

| Command      | Description                                                                      |
| ------------ | -------------------------------------------------------------------------------- |
| `W row data` | Writes a 16-bit data string to the specified row (e.g., `W 3 1010101010101010`). |
| `R row`      | Reads a row directly from the DRAM and prints the bits.                          |
| `RS row`     | Reads a row slowly, showing LED activity for each bit.                           |
| `ARR`        | Prints the contents of the internal array (`arr[][]`).                           |
| `RAM`        | Reads and prints all DRAM rows directly from the chip.                           |
| `CPY`        | Copies the internal array contents to DRAM.                                      |

## Refresh Behavior

The sketch performs a full refresh cycle every two seconds:

```cpp
for (int row = 0; row < ROWS; row++) {
    setRowAddress(row);
    digitalWrite(RAS, LOW);
    delayMicroseconds(1);
    digitalWrite(RAS, HIGH);
}
```

This ensures that stored data remains valid in the DRAM over time.

## Verification

This implementation uses the DRAM chip exclusively for data storage. No variables or buffers in Arduino memory mirror or emulate the chip’s contents. All reads and writes go through hardware-level control signals to the physical 4164 chip.
