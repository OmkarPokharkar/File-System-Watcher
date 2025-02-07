# File-System-Watcher

## Overview
This C/C++ library provides a simple, efficient, and flexible way to monitor changes in directories and files on Linux. It uses inotify, a Linux kernel subsystem, to track file modifications, deletions, creations, and other events. Developers can use this library to trigger custom callbacks whenever a filesystem change is detected.

## Key Features
### Monitor Specific Directories or Entire Trees
- Users can specify a directory to watch, and optionally include subdirectories.

### Event-Based Notifications
Detect file/directory events such as:
- File created (IN_CREATE)
- File modified (IN_MODIFY)
- File deleted (IN_DELETE)
- File renamed (IN_MOVED_FROM / IN_MOVED_TO)
- Permission changes (IN_ATTRIB)
- More events supported by inotify

### Custom Callback Mechanism
- Users can register their own functions to handle specific events.

### Efficient Event Processing
- Uses an event loop (poll/select/epoll) to minimize CPU usage.

### Asynchronous & Multithreaded Support
- Can work in a separate thread to avoid blocking the main application.

### Logging & Debugging Support
- Optional logging mechanism to record filesystem events.

### Error Handling & Recovery
- Detects and recovers from errors (e.g., inotify limits, missing permissions).

### Simple API
- Minimal, easy-to-use function calls for adding/removing watchers.

## Build and Installation

### Prerequisites
- GCC
- Make
- pthread library

### Building the Library and Example
To build the static and dynamic libraries, as well as the example programs, run:
```sh
make
```

### Installing the Library
To install the library and header files, run:
```sh
sudo make install
```

### Creating an RPM Package
To create an RPM package, ensure you have `rpmbuild` installed and run:
```sh
rpmbuild -ba fswatcher.spec
```

This will generate an RPM package that you can install on your system.

## Usage

### Example Code
Refer to the [examples directory](examples) for a usage example of the library.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes. Ensure your code follows the project's coding standards and includes appropriate tests.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Author

Developed by [Omkar Pokharkar](https://github.com/OmkarPokharkar).