#ifndef LYAPUNOV_H
#define LYAPUNOV_H

#include "decision.h"

int lyapunov_decision(DecisionFactors factors);

void set_arrival_rate(double rate);

void set_v(double v);

double get_v(void);

#endif