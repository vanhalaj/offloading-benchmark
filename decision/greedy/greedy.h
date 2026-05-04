#ifndef GREEDY_H
#define GREEDY_H

#include "decision.h"

int greedy_decision(const DecisionFactors* factors);

/*!
 * @brief Max allowed per-task energy and delay loss to limit greediness
 * @param ratio value over 0.0
 */
void set_max_loss(double ratio);

double get_max_loss(void);

void greedy_reset(void);

#endif