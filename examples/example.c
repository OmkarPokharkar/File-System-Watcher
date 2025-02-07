#include <stdio.h>
#include <stdlib.h>
#include "fswatcher.h" // Include the header for the library
#include <sys/inotify.h> // Include the header for inotify constants
#include <time.h>

// Callback function to handle events
void event_handler(fs_event_t *event) {
    char time_str[26];
    ctime_r(&event->timestamp, time_str);
    time_str[24] = '\0'; // Remove the newline character

    printf("Event detected:\n");
    printf("Path: %s\n", event->path);
    printf("User ID: %d\n", event->uid);
    printf("Timestamp: %s\n", time_str);

    if (event->mask & IN_CREATE) {
        printf("File created\n");
    } else if (event->mask & IN_MODIFY) {
        printf("File modified\n");
    } else if (event->mask & IN_DELETE) {
        printf("File deleted\n");
    } else if (event->mask & IN_MOVED_FROM) {
        printf("File moved from\n");
    } else if (event->mask & IN_MOVED_TO) {
        printf("File moved to\n");
    } else if (event->mask & IN_ATTRIB) {
        printf("File attributes changed\n");
    }
}

int main() {
    // Initialize the first watcher
    fs_watcher_t *watcher1 = fs_watcher_init();
    if (!watcher1) {
        fprintf(stderr, "Failed to initialize file system watcher 1\n");
        return EXIT_FAILURE;
    }

    // Add a directory to watch for the first watcher
    if (fs_watcher_add(watcher1, "/path/to/watch1", event_handler) != 0) {
        fprintf(stderr, "Failed to add watch for /path/to/watch1\n");
        fs_watcher_destroy(watcher1);
        return EXIT_FAILURE;
    }

    // Start the event loop in a separate thread for the first watcher
    if (fs_watcher_start(watcher1) != 0) {
        fprintf(stderr, "Failed to start watcher 1\n");
        fs_watcher_destroy(watcher1);
        return EXIT_FAILURE;
    }

    // Initialize the second watcher
    fs_watcher_t *watcher2 = fs_watcher_init();
    if (!watcher2) {
        fprintf(stderr, "Failed to initialize file system watcher 2\n");
        fs_watcher_destroy(watcher1);
        return EXIT_FAILURE;
    }

    // Add a directory to watch for the second watcher
    if (fs_watcher_add(watcher2, "/path/to/watch2", event_handler) != 0) {
        fprintf(stderr, "Failed to add watch for /path/to/watch2\n");
        fs_watcher_destroy(watcher2);
        fs_watcher_destroy(watcher1);
        return EXIT_FAILURE;
    }

    // Start the event loop in a separate thread for the second watcher
    if (fs_watcher_start(watcher2) != 0) {
        fprintf(stderr, "Failed to start watcher 2\n");
        fs_watcher_destroy(watcher2);
        fs_watcher_destroy(watcher1);
        return EXIT_FAILURE;
    }

    // Main thread can perform other tasks here
    // ...

    // Wait for the user to press Enter to stop the watchers
    getchar();

    // Stop the watchers
    fs_watcher_stop(watcher1);
    fs_watcher_stop(watcher2);

    // Clean up
    fs_watcher_destroy(watcher1);
    fs_watcher_destroy(watcher2);
    return EXIT_SUCCESS;
}
