#pragma once

#include <stdint.h>

/*
 * The virtual counter, not the physical one the LionsOS benchmark reads: a
 * kernel built without hypervisor support only lets EL0 at CNTVCT_EL0. Both
 * tick at CNTFRQ_EL0.
 */
static inline uint64_t read_cntvct(void)
{
    uint64_t value;

    asm volatile(
        "mrs %0, cntvct_el0"
        : "=r"(value)
    );

    return value;
}

static inline uint64_t read_cntfrq(void)
{
    uint64_t value;

    asm volatile(
        "mrs %0, cntfrq_el0"
        : "=r"(value)
    );

    return value;
}

// I need this to be long double in order to have accurate measurements.
static inline long double ticks_to_ns(long double ticks, uint64_t freq)
{
    return (ticks * 1000000000) / freq;
}

#ifdef PAGER_INSTRUMENTATION

/* Faults there is room to record. */
#define PAGER_MAX_SAMPLES 50000

/* When the benchmark took each fault and when it got control back. */
extern uint64_t before_faults[PAGER_MAX_SAMPLES];
extern uint64_t after_faults[PAGER_MAX_SAMPLES];
/* Faults recorded so far. */
extern uint64_t fault_idx;

#endif
