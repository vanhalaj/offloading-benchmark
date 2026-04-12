#include "lyapunov.h"
#include <math.h>
#include <stdio.h>

double v = 50.0;
double rho = 0.1;

double arrival_rate = 21.2914340090; // jos laskis alkuun paljonko menee kaikkien tehtävien paikallisessa suorituksessa, josta jakamalla tehtävien määrällä saadaan tää

static double X1 = 0.0;
static double X2 = 0.0;

int lyapunov_decision(DecisionFactors factors)
{
	double delay_constraint = 1.5 * factors.delay_local; // TODO ?

	double sigma_local = factors.delay_local * arrival_rate;
	double sigma_remote = factors.delay_offloaded * arrival_rate;

	double eta_local = (factors.delay_local > delay_constraint) ? 1.0 : 0.0;
	double eta_remote = (factors.delay_offloaded > delay_constraint) ? 1.0 : 0.0;
	
	double f_local =
		sigma_local * X1 +
		eta_local * X2 +
		v * factors.energy_local;

	double f_remote =
		sigma_remote * X1 +
		eta_remote * X2 +
		v * factors.energy_offloaded;

	//printf("Lyapunov: local %f remote %f\n", f_local, f_remote);
	int decision = f_remote < f_local;

	// update state
	double delay = decision ? factors.delay_offloaded : factors.delay_local;
	double sigma = decision ? sigma_remote : sigma_local;
	double eta = (delay > delay_constraint) ? 1.0 : 0.0;
	X1 = fmax(X1 - 1.0, 0.0) + sigma;
	X2 = fmax(X2 - rho, 0.0) + eta;

	return decision;
}

void set_v(double value)
{
	v = value;
}

double get_v(void)
{
	return v;
}
