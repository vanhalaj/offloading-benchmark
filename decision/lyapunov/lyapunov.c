/*
 * Online algorithm for offloading decision, utilizing Lyapunov optimization. 
 * 
 * Based on formulas presented in: 
 * D. Huang, P. Wang and D. Niyato. A dynamic offloading algorithm for mobile computing. 
 * IEEE Transactions on Wireless Communications, 11(6):1991–1995, 2012.
 * doi: 10.1109/TWC.2012.041912.110912
 */

#include "lyapunov.h"
#include <math.h>
#include <stdio.h>

double v = 800.0;
double rho = 0.1;

// Set by the simulator with: amount of tasks / total delay of full local execution
double arrival_rate = 21.2914340090;

static double X1 = 0.0;
static double X2 = 0.0;

int lyapunov_decision(const DecisionFactors* factors)
{
	// TODO? Since tasks have no actual deadlines, just use local delay estimate with some multiplier 
	double delay_constraint = 1.5 * factors->delay_local;

	double sigma_local = factors->delay_local * arrival_rate;
	double sigma_remote = factors->delay_offloaded * arrival_rate;

	double eta_local = (factors->delay_local > delay_constraint) ? 1.0 : 0.0;
	double eta_remote = (factors->delay_offloaded > delay_constraint) ? 1.0 : 0.0;
	
	double f_local =
		sigma_local * X1 +
		eta_local * X2 +
		v * factors->energy_local;

	double f_remote =
		sigma_remote * X1 +
		eta_remote * X2 +
		v * factors->energy_offloaded;

	int decision = f_remote < f_local;

	// update state
	double delay = decision ? factors->delay_offloaded : factors->delay_local;
	double sigma = decision ? sigma_remote : sigma_local;
	double eta = (delay > delay_constraint) ? 1.0 : 0.0;
	X1 = fmax(X1 - 1.0, 0.0) + sigma;
	X2 = fmax(X2 - rho, 0.0) + eta;

	return decision;
}

void set_arrival_rate(double rate)
{
	arrival_rate = rate;
}

void set_v(double value)
{
	v = value;
}

double get_v(void)
{
	return v;
}
