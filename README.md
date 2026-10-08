# Bitcask-Style Key-Value Storage Engine

An in-progress persistent, log-structured key-value storage engine built from scratch in **C++23**, inspired by the Bitcask design [paper.](https://riak.com/assets/bitcask-intro.pdf)

## Features

- **Log-Structured Storage**: Writes are appended to binary datafiles with a custom record format (CRC32 checksum, timestamp, length-prefixed keys and values).
- **Single-Seek Reads**: An in-memory hash index (`keyDir`) maps keys to file offsets for fast, single-seek retrieval.
- **Crash Recovery**: The index is rebuilt deterministically from disk on startup, ensuring memory never outpaces durability.
- **Datafile Rollover**: Active datafiles automatically roll over into immutable read-only segments upon reaching a size threshold.
- **Concurrency Control**: Utilizes `std::shared_mutex` to allow concurrent reads while cleanly serializing writes. Verified under multi-threaded tests.
- **Robust Error Handling**: Public APIs return `std::expected` for recoverable errors and throw on integrity violations.
- **Tested**: Comprehensive test suite built with **Catch2** covering store creation, reopening, rollover, and multi-threaded safety.

## Tech Stack

- **Language**: C++23
- **Build System**: CMake
- **Testing**: Catch2
- **Core Components**: `std::filesystem`, `std::shared_mutex`, `std::expected`, CRC32