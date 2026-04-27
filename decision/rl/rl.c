#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rl.h"

#define BINS 10
#define ACTIONS 2

#define USE_SMOOTH_AVERAGE_DELAY 1

static double Q[BINS][BINS][ACTIONS] = { 0 };

static double lr = 0.1;
static double discount = 0.9;
static double epsilon = 0.05;

static double lambda = 0.0;
static double step_size = 0.05;

static int prev_d_bin = -1;
static int prev_e_bin = -1;
static int prev_action = -1;

static int discretize(double value, double max)
{
    if (max <= 0.0) return 0;

    int bin = (int)(value / max * BINS);
    if (bin >= BINS) bin = BINS - 1;
    if (bin < 0) bin = 0;
    return bin;
}

static void get_state(const DecisionFactors* f, int* d_bin, int* e_bin)
{
    double delay_ratio = f->delay_offloaded / (f->delay_local);
    double energy_ratio = f->energy_offloaded / (f->energy_local);

    *d_bin = discretize(delay_ratio, 2.0);
    *e_bin = discretize(energy_ratio, 0.5);
    //printf("%f,%f,%d,%d\n", delay_ratio, energy_ratio, *d_bin, *e_bin);
    //printf("%d,%f,%f,%f\n", f->task.task_input_size, f->task.task_computation_size, f->energy_local, f->energy_offloaded);
}

static double compute_reward(const DecisionFactors* f, int action)
{
    double energy = action ? f->energy_offloaded : f->energy_local;
    double delay = action ? f->delay_offloaded : f->delay_local;

    double violation = (delay - f->delay_local);// / f->delay_local;
    return -energy - lambda * violation;
}

static void update_lambda(const DecisionFactors* f, int action)
{
    double delay = action ? f->delay_offloaded : f->delay_local;

#if USE_SMOOTH_AVERAGE_DELAY
    static double avg_delay = 0.0;
    static double beta = 0.01; // smoothing factor

    avg_delay = (1 - beta) * avg_delay + beta * delay;

    double violation = avg_delay - f->delay_local;
    lambda += step_size * violation;
#else
    double violation = delay - f->delay_local;

    lambda += step_size * violation;
#endif

    if (lambda < 0.0)
        lambda = 0.0;
}

int reinforcement_learning_decision(const DecisionFactors* f)
{
    int d_bin, e_bin;
    get_state(f, &d_bin, &e_bin);

    // update Q-table for previous transition
    if (prev_action != -1)
    {
        double reward = compute_reward(f, prev_action);

        double max_next = fmax(Q[d_bin][e_bin][0], Q[d_bin][e_bin][1]);

        Q[prev_d_bin][prev_e_bin][prev_action] +=
            lr * (reward + discount * max_next - Q[prev_d_bin][prev_e_bin][prev_action]);
    }

    // eps-greedy choosing
    int action;
    if (f->delay_offloaded > f->delay_local * 1.5)
        action = 0;   // force local
    else 
    {
        if (((double)rand() / RAND_MAX) < epsilon)
            action = rand() % ACTIONS;
        else
            action = (Q[d_bin][e_bin][1] > Q[d_bin][e_bin][0]) ? 1 : 0;
    }

    update_lambda(f, action);
    prev_d_bin = d_bin;
    prev_e_bin = e_bin;
    prev_action = action;

    return action;
}
