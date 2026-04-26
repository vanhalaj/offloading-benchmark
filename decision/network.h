#ifndef NETWORK_H
#define NETWORK_H

struct network_description_t 
{
	double latency_avg; // one way latency (seconds)
	double latency_var;
	double bandwidth_up; // bits per second
	double bandwidth_down; // bits per second
} typedef NetworkDescription;

struct network_state_t
{
	double latency;
	double r_up, r_down;
} typedef NetworkState;

#endif