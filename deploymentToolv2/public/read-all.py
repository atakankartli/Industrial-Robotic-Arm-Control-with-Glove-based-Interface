#!/usr/bin/env python
import time
from ads1015 import ADS1015

CHANNELS = ['in0/ref', 'in1/ref', 'in2/ref']

ads1015 = ADS1015()
chip_type = ads1015.detect_chip_type()

ads1015.set_mode('single')
ads1015.set_programmable_gain(2.048)

if chip_type == 'ADS1015':
    ads1015.set_sample_rate(1600)
else:
    ads1015.set_sample_rate(860)

reference = ads1015.get_reference_voltage()

value = ads1015.get_compensated_voltage(channel='in0/ref', reference_voltage=reference)

print("{:6.3f}".format(value))
