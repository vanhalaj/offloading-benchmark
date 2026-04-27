#include <stdlib.h>
#include <math.h>
#include "rl.h"

#define BINS 10
#define ACTIONS 2

// Q(z, d)
static double Q[BINS][BINS][ACTIONS] = { 0 };

// RL parameters
static double lr = 0.1;
static double discount = 0.9;
static double epsilon = 0.05;

static double lambda = 0.0;
static double step_size = 0.01;

// --- Previous transition memory ---
static int prev_d_bin = -1;
static int prev_e_bin = -1;
static int prev_action = -1;

// --- Discretization ---
static int discretize(double value, double max)
{
    if (max <= 0.0) return 0;

    int bin = (int)(value / max * BINS);
    if (bin >= BINS) bin = BINS - 1;
    if (bin < 0) bin = 0;
    return bin;
}

// --- State z_t ---
static void get_state(const DecisionFactors* f, int* d_bin, int* e_bin)
{
    double delay_ratio = f->delay_offloaded / (f->delay_local + 1e-6);
    double energy_ratio = f->energy_offloaded / (f->energy_local + 1e-6);

    *d_bin = discretize(delay_ratio, 2.0);
    *e_bin = discretize(energy_ratio, 2.0);
}

static double compute_reward(const DecisionFactors* f, int action)
{
    double energy = action ? f->energy_offloaded : f->energy_local;
    double delay = action ? f->delay_offloaded : f->delay_local;

    double violation = delay - f->delay_local;
    if (violation < 0.0)
        violation = 0.0;

    double penalty = lambda * violation;

    return -(energy + penalty);
}


static void update_lambda(const DecisionFactors* f, int action)
{
    double delay = action ? f->delay_offloaded : f->delay_local;

    double violation = delay - f->delay_local;

    lambda += step_size * violation;

    if (lambda < 0.0)
        lambda = 0.0;
}


// --- MAIN RL FUNCTION ---
int reinforcement_learning_decision(const DecisionFactors* f)
{
    int d_bin, e_bin;
    get_state(f, &d_bin, &e_bin);

    // --- Q-learning update for previous transition ---
    if (prev_action != -1)
    {
        double reward = compute_reward(f, prev_action);

        double max_next = fmax(Q[d_bin][e_bin][0],
            Q[d_bin][e_bin][1]);

        Q[prev_d_bin][prev_e_bin][prev_action] +=
            lr * (reward + discount * max_next -
                Q[prev_d_bin][prev_e_bin][prev_action]);
    }

    // --- Choose action (eps-greedy) ---
    int action;

    if (f->delay_offloaded > f->delay_local * 1.25)
        action = 0;   // force local
    else {
        if (((double)rand() / RAND_MAX) < epsilon)
            action = rand() % ACTIONS;
        else
            action = (Q[d_bin][e_bin][1] > Q[d_bin][e_bin][0]) ? 1 : 0;
    }
    // Update lambda AFTER choosing action
    update_lambda(f, action);

    // Store transition for next step
    prev_d_bin = d_bin;
    prev_e_bin = e_bin;
    prev_action = action;

    return action;
}
