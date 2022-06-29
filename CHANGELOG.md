# Changelog

All notable changes to this project will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).


## [Unreleased]

## [3.0.0] - 2022-06-29
### Added
- Extend `FrequencyManager` API.

### Changed
- Remove sleep in `UsbStream::close_device`.
- Add safety checks during the reading of serial port (usb) messages (#31).

### Removed
- ThreadsafeTimeseries (use the time_series package instead).

### Fixed
- Make the package compile on macOS.


## [2.0.0] - 2021-03-02

There is no changelog for this or earlier versions.


[Unreleased]: https://github.com/machines-in-motion/real_time_tools/compare/v3.0.0...HEAD
[3.0.0]: https://github.com/machines-in-motion/real_time_tools/compare/v2.0.0...v3.0.0
[2.0.0]: https://github.com/machines-in-motion/real_time_tools/releases/tag/v2.0.0
