# thread-safe-hashmap
Thread-safe hashmap

Features: insert, remove, operator[], at, clear, print, copy assignment, move assignment

Fixed-size C-style array is used to store elements, collisions are handled using chaining, hash function is implemented as modulus of the key with respect to array size

Thread-safety is gained using shared_timed_mutex, which is locked using shared_lock during read operations, and locked using unique_lock during write operations,
so that multiple readers can read at the same time, but only one writer writes at a time.

shared_timed_mutex is used to comply with C++14 standard. With C++17 standard shared_timed_mutex can be replaced with more efficient shared_mutex (which don't have timed locking support)

Hashmap is implemented as template class with key type and value type as template arguments

Supports table resizing

A main program is included that tests hashmap functionality

Use make command to build, options: prep (creates output directories), debug, release, clean: default is release

Output file is generated in debug/ or release/ directory based on build type

Works under windows, tested with Visual Studio 2015 (project and solution files are included)
