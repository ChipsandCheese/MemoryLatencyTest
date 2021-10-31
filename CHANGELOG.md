# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres
to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## v0.3.1 (2021-10-30)

### Added

- Windows i386 version! Should run on anything that supports x86 and MMX.

### Changed

- Executables are now suffixed with their cpu instruction set

### Fixed

- Test now bails early if it fails to allocate memory

## v0.3.0 (2021-10-29)

### Added

- Help option (`-h`)
- Option to set max region size in megabytes (`-m`)
- Option to set base iterations to run for each region size (`-i`)
- Option to force the C version of the test (`-c`)
- Assembly version of the critical section of the test
- 384 KB now included in default region sizes

### Changed

- x86 and x86-64 platforms now run the assembly version of the test by default
- Use 32-bit data types when running on x86
- Use `time_t` for calculation step

### Fixed

- Test now ensures that memory was properly allocated before proceeding
- Errors are now printed to sterr rather than stdout

## v0.2.0 (2021-10-22)

### Added

- Parameter handling - options will be added in a future release
- "Press ENTER key to continue" on Windows

### Changed

- Significantly improve precision of latency calculations
- Don't print trailing zeroes of nanosecond counts and print 5 significant figures maximum
- New fine-grained heuristic for scaling iteration counts based on region size

### Removed

- Unused mobile wait mode

## v0.1.1 (2021-10-18)

### Changed

- Removed unused tz structs
- Use musl-gcc to build static Linux x86_64 executables, which should now run on the vast majority of Linux-based operating systems
