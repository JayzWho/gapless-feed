# gapless-feed

A market data feed handler for MoldUDP64 / Nasdaq TotalView-ITCH 5.0, in C++23.

An exchange broadcasts market data as numbered messages in UDP packets, and UDP loses,
duplicates and reorders packets. The receiver in this project watches the sequence numbers,
holds back what arrives after a gap, asks the retransmission server for the missing range, and
hands every message to its consumer exactly once and in order. The result is checked message
by message against a reference decoded straight from a real Nasdaq historical file.

**Status:** project skeleton. The build, tests and CI are in place; the protocol code is not
written yet.

## Build and test

Needs Linux, CMake 3.25 or newer, and a C++23 compiler. GCC 15 and Clang 21 are the ones
tested. GoogleTest is taken from the system when installed and downloaded otherwise.

```sh
cmake --workflow --preset debug      # configure, build and test with ASan + UBSan
cmake --workflow --preset release    # the same, optimized and without sanitizers
```

Each preset builds into `build/<preset>/`. To use Clang, set `CXX=clang++` the first time a
preset is configured.

Format the code before committing; CI rejects unformatted files.

```sh
scripts/format.sh
```

## Data

The project runs on one file from Nasdaq's public sample directory: the full trading day of
2019-01-30, a 4.8 GB download that decompresses to about 11 GB.

```sh
scripts/fetch_data.sh                # downloads, checks and decompresses into Data/
```

## Layout

| Path | Contents |
|---|---|
| `include/gapless/` | Library headers |
| `src/` | Library sources |
| `apps/` | Command-line tools |
| `tests/` | Unit tests |
| `scripts/` | Data download and formatting scripts |
| `Docs/` | Project documents |
