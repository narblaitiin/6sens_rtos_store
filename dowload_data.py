import pylink
import argparse
import time
import base64
from rich.progress import Progress

def connect(jlink_serial: int):
    print("Connecting to JLINK serial = {}".format(jlink_serial))
    jlink = pylink.JLink()
    jlink.open(serial_no=jlink_serial)
    if not jlink.connected():
        print("ERROR : JLink is not connected. Maybe the serial number is wrong ?")
        exit(1)
    
    print("Connecting to sastress board ")
    jlink.connect("NRF52840_XXAA")
    if not jlink.target_connected():
        print("ERROR : Could connect to JLINK debugger but not to the SASTRESS board.")
        exit(1)
        
    return jlink

def RTT_viewer(jlink: pylink.JLink):
    current_file = None
    jlink.rtt_start()
    print("Waiting for RTT to settle")
    while jlink.rtt_get_status() == 0:
        time.sleep(1)
    print("Done")
    p = Progress()
    p.start()
    t = None
    data = ""
    while True:
        logs = jlink.rtt_read(0,200)
        data += ''.join([chr(i) for i in logs])
        if "\n" not in data:
            continue
        else:
            lines = data.split("\n")
            data = lines.pop()
            for line in lines:
                if line.startswith("FILE:"):
                    if current_file:
                        current_file.close()
                        p.remove_task(t)
                    filename = line.split(":")[1]
                    print("Found new file :", filename)
                    current_file = open(f"lfs/{filename}", "wb")
                    t = p.add_task(filename, total=512*1024)
                    transmitting_data = True
                elif line.startswith("D:") and current_file:
                    b64 = line.split(":")[1]
                    decoded = base64.b64decode(b64)
                    current_file.write(decoded)
                    p.update(t, advance=100)

def main():
    parser = argparse.ArgumentParser(description='Read flash memory from nRF52840 using JLink')
    parser.add_argument('--serial', type=int, default=1050342163,
                      help='JLink debugger serial number')
    parser.add_argument('--reset-head', type=bool, help='Reset the sensor list ')
    args = parser.parse_args()
    jlink = connect(args.serial)
    RTT_viewer(jlink)

if __name__ == "__main__":
    main()