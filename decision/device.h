#ifndef DEVICE_H
#define DEVICE_H

struct device_descriptions_t
{
	double cpu_freq_local; // hertz
	double cpu_freq_offloaded; // hertz
	double power_load; // watts
	double power_idle; // watts
	double power_transmitter; // watts
	double power_receiver; // watts
};
typedef struct device_descriptions_t DeviceDescriptions;

#endif