#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include "minor_pf.h"
#include <sys/mman.h>
#include "timer.h"
#define PAGE_SIZE       4096ULL

#define NUM_PAGES       16384ULL

#define TEST_VADDR_R      0x40000000ULL
#define TEST_VADDR_W      0x50000000ULL


#define WARMUP_SAMPLES  10

#ifdef PAGER_INSTRUMENTATION
uint64_t before_faults[PAGER_MAX_SAMPLES];
uint64_t after_faults[PAGER_MAX_SAMPLES];
uint64_t fault_idx = 0;
#endif


__attribute__((weak)) void minor_pf_pass_begin(void)
{
}

__attribute__((weak)) void minor_pf_pass_end(const char *name)
{
    (void)name;
}

/*
 * MAP_FIXED rather than the MAP_FIXED_NOREPLACE the LionsOS benchmark asks
 * for: L4Re's mmap does not know the latter and would take the address as a
 * hint.
 */
static volatile uint8_t *map_region(uint64_t vaddr)
{
    void *base = mmap(
        (void *)vaddr,
        NUM_PAGES * PAGE_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
        -1,
        0
    );

    if (base == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    return (volatile uint8_t *)base;
}

static int benchmark_read(uint64_t freq)
{
    volatile uint8_t *base = map_region(TEST_VADDR_R);
    if (!base)
        return 1;

    printf("\nRunning READ benchmark...\n");
    minor_pf_pass_begin();
    uint64_t start = read_cntvct();
    for (size_t i = 0; i < NUM_PAGES; i++) {
        volatile uint8_t *page = base + i * PAGE_SIZE;

        #ifdef PAGER_INSTRUMENTATION
        before_faults[fault_idx] = read_cntvct();
        #endif
        uint8_t value = *page;
        (void)value;
        #ifdef PAGER_INSTRUMENTATION
        after_faults[fault_idx] = read_cntvct();
        fault_idx++;
        #endif
    }
    uint64_t end = read_cntvct();
    minor_pf_pass_end("READ");

    long double average = ticks_to_ns(end - start, freq) / NUM_PAGES;
    printf("read average latency: %.3Lf ns\n", average);
    return 0;
}


static int benchmark_write(uint64_t freq)
{
    volatile uint8_t *base = map_region(TEST_VADDR_W);
    if (!base)
        return 1;

    printf("\nRunning WRITE benchmark...\n");
    minor_pf_pass_begin();
    uint64_t start = read_cntvct();
    for (size_t i = 0; i < NUM_PAGES; i++) {

        volatile uint8_t *page =
            base + i * PAGE_SIZE;
        #ifdef PAGER_INSTRUMENTATION
        before_faults[fault_idx] = read_cntvct();
        #endif
        *page = 42;
        #ifdef PAGER_INSTRUMENTATION
        after_faults[fault_idx] = read_cntvct();
        fault_idx++;
        #endif
    }
    uint64_t end = read_cntvct();
    minor_pf_pass_end("WRITE");

    long double average = ticks_to_ns(end - start, freq) / NUM_PAGES;
    printf("write average latency: %.3Lf ns\n", average);
    return 0;
}


int minor_pf(void)
{
    uint64_t freq = read_cntfrq();
    printf("pf latency benchmark:\n");
    printf("CNTFRQ_EL0 : %lu Hz\n", freq);
    printf("page size  : %llu bytes\n", PAGE_SIZE);
    printf("pages      : %llu\n", NUM_PAGES);
    printf("size       : %llu MiB\n",
           (NUM_PAGES * PAGE_SIZE) / (1024 * 1024));

    /*
     * IMPORTANT:
     *
     * The region must start completely unmapped.
     *
     * Do not touch TEST_VADDR before benchmark_read().
     */

    if (benchmark_read(freq))
        return 1;

    if (benchmark_write(freq))
        return 1;

    #ifdef PAGER_INSTRUMENTATION
    /* The first NUM_PAGES samples are the READ pass, the rest the WRITE pass. */
    printf("\nper-fault latency (ticks):\n");
    for (uint64_t i = 0; i < fault_idx; i++)
        printf("%lu\n", after_faults[i] - before_faults[i]);
    #endif

    printf("benchmark done\n");

    return 0;
}
