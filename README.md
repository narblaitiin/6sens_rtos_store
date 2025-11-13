# Code for 6Sens Project : test of MDBT50Q SoC ADC (AIN0 - P0.02) and DS3231 RTC (I2C)

## Overview
This first code allows us to convert a voltage and digital values and adds processing to it:

 - take sample of sensor level (Analog-to-Digital), from filtering and ampliflying part of PCB (from geophone sensor to analog P0.02 of MDBT50Q)
 - get timestamp from DS3231 RTC (I2C device)
 - store the different values in an area of partitioned external QSPI flash memory MX25R64

This allows us to test the analog part of PCB and the internal ADC of the MDBT50Q. The final goal will be to send the samples to a lorawan server and clear the memory location once a day.

## Building and Running
The following commands clean build folder, build and flash the sample:

**Command to use**
```
west build -t pristine

west build -p always -b mdbt50q_lora_dev applications/nrf52840_rtos_adc

west flash --runner jlink
```

## Downloading satress data

By default, at startup, the sensor dumps its memory, which is normally in the form of files `/lfs/geophone_<nb>.dat`.

The data is sent in base64 format, and a typical log is as follows:

```
FILE:/lfs/geophone_000.dat
D:<base64 encoded data - chunk 1>
D:<base64 encoded data - chunk 2>
...
D:<base64 encoded data - chunk n>
TOTAL_ENCODED:89128
```

To retrieve the data, the following steps should be followed: 
- `python3 download_data.py`: the script automatically connects to the sensor and retrieves the files, storing them in the chosen directory (default: `lfs`).
- Press the reset button on the sensor, and the data will be downloaded.
- `python3 spectrogram.py` to display the spectrogram and calculated metrics, `python3 spectrogram.py --raw` to display the raw signal as well.