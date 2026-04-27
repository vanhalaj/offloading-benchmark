#ifndef RL_H
#define RL_H

#include "decision.h"

int reinforcement_learning_decision(const DecisionFactors* factors);

int rl_load_q_table(const char* file_name);

int rl_save_q_table(const char* file_name);

#endif