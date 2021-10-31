# Memory Latency Test

Originally developed by [clamchowder](https://github.com/clamchowder/) and maintained here by other Chips & Cheese staff.

## Download

Get the latest (semi) stable release from the GitHub releases page [here](https://github.com/ChipsandCheese/MemoryLatencyTest/releases/). Windows and Linux binaries are both available. Bleeding edge binaries can be found in the artifacts of the Build with Meson workflow [here](https://github.com/ChipsandCheese/MemoryLatencyTest/actions/workflows/build-project.yml).

## Support

| Platform       | Required Instruction Sets |
|----------------|---------------------------|
| Linux x86_64   | x86_64, MMX, SSE, SSE2    |
| Windows x86_64 | x86_64, MMX, SSE, SSE2    |
| Windows i386   | x86, MMX                  |

## Compile

Run the following:

```bash
$ mkdir build && cd build
$ meson ..
$ meson compile
```

The compiled binary will be in `build/src`.
