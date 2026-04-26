#ifndef OPTIMAL_H
#define OPTIMAL_H

#include "decision.h"

void optimal_prepare(TaskDescription* tasks, int task_count);

void optimal_free();

int optimal_decision(const DecisionFactors* factors);

#endif