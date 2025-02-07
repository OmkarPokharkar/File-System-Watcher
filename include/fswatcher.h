#ifndef FSWATCHER_H
#define FSWATCHER_H

#include <stdint.h>
#include <time.h>
#include <pthread.h>

// Opaque type for the file system watcher
typedef struct fs_watcher_t fs_watcher_t;

// Struct to hold event details
typedef struct {
    const char *path;
    uint32_t mask;
    uid_t uid;
    time_t timestamp;
} fs_event_t;

// Callback function type
typedef void (*fs_event_callback_t)(fs_event_t *event);

// Initialize the file system watcher
fs_watcher_t* fs_watcher_init(void);

// Add a directory to watch
int fs_watcher_add(fs_watcher_t *watcher, const char *path, fs_event_callback_t callback);

// Start the event loop in a separate thread
int fs_watcher_start(fs_watcher_t *watcher);

// Stop the event loop and shutdown the thread
void fs_watcher_stop(fs_watcher_t *watcher);

// Destroy the file system watcher and free resources
void fs_watcher_destroy(fs_watcher_t *watcher);

#endif // FSWATCHER_H
