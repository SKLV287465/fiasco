# L4Re Build System

This package contains the build system for the L4Re operating system.
It also provides the basic directory infrastructure for L4Re.

# Documentation

This package is part of the L4Re Operating System Framework. For
documentation and build instructions please refer to
[l4re.org](https://l4re.org).

Packages for additional parts of the L4Re operating system are located
in `pkg/` and `test/`. Please refer to the READMEs in each directory for
additional information about each package.

# Contributions

We welcome contributions. Please see the
[contributors guide](https://l4re.org/contributing/).

# License

Detailed licensing and copyright information can be found in the
[LICENSE](LICENSE.spdx) file. Licensing information for packages
in `pkg/` and `test/` are located in each subdirectory.

# What my changes do

This implementation is a page fault latency benchmarking setup measuring
L4Re's page fault latency of private anonymous memory caused from lazy
allocation. The benchmark lives in `pkg/page_fault_latency`.

# Building the benchmark

```
make bench
```

Each `make bench` wipes build then from scratch creates `build/kernel`
from the `arm64-rpi` kernel template, copies `globalconfig.out` to
benchmark config, build l4re and page_fault_latency and builds boot image.

```
make bench-inc     # incremental: keeps both build dirs and their configs
make bench-clean   # delete build/
make help          # all targets
```

Useful overrides: `BENCH_IMAGE` (`rawimage`, `uimage`, `elfimage`, `qemu`),
`BENCH_ENTRY`, `BENCH_JOBS`, `BENCH_CROSS`, `BENCH_BUILD_DIR`.

`make bench` re-asserts `globalconfig.out` in the kernel build directory on
every run; `make bench-inc` does not, so a manual `make menuconfig` in
`build/kernel` survives incremental builds.
