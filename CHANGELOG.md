# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres
to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
