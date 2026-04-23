/*
 * Greedy online algorithm for computation offloading decision.
 * Designed for stochastic task arrival and uses a fixed percentage for how much
 * bigger the offloaded energy and delay is allowed to be per task.
 * 
 * Somewhat based on the idea presented in:
 * S. Sundar and B. Liang. Offloading dependent tasks with communication delay and 
 * deadline constraint. IEEE INFOCOM 2018 - IEEE Conference on Computer Communications,
 * pages 37–45, 2018. doi: 10.1109/INFOCOM.2018.8486305.
 */

#include "greedy.h"

// the part over 1.0 is the actual allowed loss
double max_loss_difference = 1.25;

// positive means profit compared to full local execution
double accumulated_delay_profit = 0.0;
double accumulated_energy_profit = 0.0;

int greedy_decision(const DecisionFactors* factors)
{
	accumulated_delay_profit += factors->delay_local;
	accumulated_energy_profit += factors->energy_local;

	if ((factors->energy_offloaded < accumulated_energy_profit) 
		&& (factors->energy_offloaded < factors->energy_local * max_loss_difference))
	{
		if ((factors->delay_offloaded < accumulated_delay_profit) 
			&& (factors->delay_offloaded < factors->delay_local * max_loss_difference))
		{
			// offload
			accumulated_delay_profit -= factors->delay_offloaded;
			accumulated_energy_profit -= factors->energy_offloaded;

			return 1;
		}
	}
	
	// local execution
	accumulated_delay_profit -= factors->delay_local;
	accumulated_energy_profit -= factors->energy_local;

	return 0;
}

void set_max_loss(double ratio)
{
	max_loss_difference = ratio + 1.0;
}

double get_max_loss(void)
{
	return max_loss_difference - 1.0;
}
