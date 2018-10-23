import os
import mmap
import struct
from time import sleep
from onl.platform.base import *
from onl.platform.dell import *

# Write value to PCI Bus
def pci_set_value(resource, val, offset):
    fd=os.open(resource,os.O_RDWR)
    mm=mmap.mmap(fd,0)
    mm.seek(offset)
    mm.write(struct.pack('I',val))
    mm.close()
    os.close(fd)


class OnlPlatform_x86_64_dell_z9100_c2538_r0(OnlPlatformDell,
                                              OnlPlatformPortConfig_32x100):
    PLATFORM='x86-64-dell-z9100-c2538-r0'
    MODEL="Z9100"
    SYS_OBJECT_ID=".9100"

    def baseconfig(self):
        self.insmod('optoe')

        ########### initialize I2C buses ###########
        self.new_i2c_devices([

            # initialize mux (PCA9547)
            ('pca9547', 0x70, 1),

            # initiate switches (PCA9548)
            ('pca9548', 0x71, 4),
            ('pca9548', 0x71, 6),
            ('pca9548', 0x71, 7),
            ('pca9548', 0x71, 8),
            ('pca9548', 0x71, 9)

            ])

        # Set SMBus Controller 2.0 SPGT re to 0x00000005 to tune 80KHz freq
        pci_set_value(
            "/sys/devices/pci0000:00/0000:00:13.0/resource0",
            0x00000005, 0x300
        )
        sleep(0.05)

        # initialize QSFP port 1~32
        self.new_i2c_devices([
                ('optoe1', 0x50, 42),
                ('optoe1', 0x50, 43),
                ('optoe1', 0x50, 44),
                ('optoe1', 0x50, 45),
                ('optoe1', 0x50, 46),
                ('optoe1', 0x50, 47),
                ('optoe1', 0x50, 48),
                ('optoe1', 0x50, 49),
                ('optoe1', 0x50, 34),
                ('optoe1', 0x50, 35),
                ('optoe1', 0x50, 36),
                ('optoe1', 0x50, 37),
                ('optoe1', 0x50, 39),
                ('optoe1', 0x50, 38),
                ('optoe1', 0x50, 41),
                ('optoe1', 0x50, 40),
                ('optoe1', 0x50, 26),
                ('optoe1', 0x50, 27),
                ('optoe1', 0x50, 28),
                ('optoe1', 0x50, 29),
                ('optoe1', 0x50, 30),
                ('optoe1', 0x50, 31),
                ('optoe1', 0x50, 32),
                ('optoe1', 0x50, 33),
                ('optoe1', 0x50, 18),
                ('optoe1', 0x50, 19),
                ('optoe1', 0x50, 20),
                ('optoe1', 0x50, 21),
                ('optoe1', 0x50, 22),
                ('optoe1', 0x50, 23),
                ('optoe1', 0x50, 24),
                ('optoe1', 0x50, 25)
                ])

        subprocess.call('echo port1 > /sys/bus/i2c/devices/42-0050/port_name', shell=True)
        subprocess.call('echo port2 > /sys/bus/i2c/devices/43-0050/port_name', shell=True)
        subprocess.call('echo port3 > /sys/bus/i2c/devices/44-0050/port_name', shell=True)
        subprocess.call('echo port4 > /sys/bus/i2c/devices/45-0050/port_name', shell=True)
        subprocess.call('echo port5 > /sys/bus/i2c/devices/46-0050/port_name', shell=True)
        subprocess.call('echo port6 > /sys/bus/i2c/devices/47-0050/port_name', shell=True)
        subprocess.call('echo port7 > /sys/bus/i2c/devices/48-0050/port_name', shell=True)
        subprocess.call('echo port8 > /sys/bus/i2c/devices/49-0050/port_name', shell=True)
        subprocess.call('echo port9 > /sys/bus/i2c/devices/34-0050/port_name', shell=True)
        subprocess.call('echo port10 > /sys/bus/i2c/devices/35-0050/port_name', shell=True)
        subprocess.call('echo port11 > /sys/bus/i2c/devices/36-0050/port_name', shell=True)
        subprocess.call('echo port12 > /sys/bus/i2c/devices/37-0050/port_name', shell=True)
        subprocess.call('echo port13 > /sys/bus/i2c/devices/39-0050/port_name', shell=True)
        subprocess.call('echo port14 > /sys/bus/i2c/devices/38-0050/port_name', shell=True)
        subprocess.call('echo port15 > /sys/bus/i2c/devices/41-0050/port_name', shell=True)
        subprocess.call('echo port16 > /sys/bus/i2c/devices/40-0050/port_name', shell=True)
        subprocess.call('echo port17 > /sys/bus/i2c/devices/26-0050/port_name', shell=True)
        subprocess.call('echo port18 > /sys/bus/i2c/devices/27-0050/port_name', shell=True)
        subprocess.call('echo port19 > /sys/bus/i2c/devices/28-0050/port_name', shell=True)
        subprocess.call('echo port20 > /sys/bus/i2c/devices/29-0050/port_name', shell=True)
        subprocess.call('echo port21 > /sys/bus/i2c/devices/30-0050/port_name', shell=True)
        subprocess.call('echo port22 > /sys/bus/i2c/devices/31-0050/port_name', shell=True)
        subprocess.call('echo port23 > /sys/bus/i2c/devices/32-0050/port_name', shell=True)
        subprocess.call('echo port24 > /sys/bus/i2c/devices/33-0050/port_name', shell=True)
        subprocess.call('echo port25 > /sys/bus/i2c/devices/18-0050/port_name', shell=True)
        subprocess.call('echo port26 > /sys/bus/i2c/devices/19-0050/port_name', shell=True)
        subprocess.call('echo port27 > /sys/bus/i2c/devices/20-0050/port_name', shell=True)
        subprocess.call('echo port28 > /sys/bus/i2c/devices/21-0050/port_name', shell=True)
        subprocess.call('echo port29 > /sys/bus/i2c/devices/22-0050/port_name', shell=True)
        subprocess.call('echo port30 > /sys/bus/i2c/devices/23-0050/port_name', shell=True)
        subprocess.call('echo port31 > /sys/bus/i2c/devices/24-0050/port_name', shell=True)
        subprocess.call('echo port32 > /sys/bus/i2c/devices/25-0050/port_name', shell=True)

        # initialize SFP port 33-34
        self.new_i2c_devices([
                ('24c02', 0x50, 11),
                ('24c02', 0x50, 12)
                ])

        subprocess.call('echo port33 > /sys/bus/i2c/devices/11-0050/port_name', shell=True)
        subprocess.call('echo port34 > /sys/bus/i2c/devices/12-0050/port_name', shell=True)

        return True
