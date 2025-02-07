#include "fswatcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <pthread.h>

struct fs_watcher_t {
    int inotify_fd;
    int epoll_fd;
    fs_event_callback_t callback;
    pthread_t thread;
    int running;
    pthread_mutex_t lock;
    int lock_initialized;
};

static void log_error(const char *message) {
    perror(message);
}

static void* fs_watcher_thread_func(void *arg) {
    fs_watcher_t *watcher = (fs_watcher_t *)arg;
    struct epoll_event events[10];
    char buffer[1024];

    while (1) {
        pthread_mutex_lock(&watcher->lock);
        if (!watcher->running) {
            pthread_mutex_unlock(&watcher->lock);
            break;
        }
        pthread_mutex_unlock(&watcher->lock);

        int n = epoll_wait(watcher->epoll_fd, events, 10, -1);
        if (n == -1) {
            log_error("epoll_wait");
            break;
        }

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == watcher->inotify_fd) {
                int length = read(watcher->inotify_fd, buffer, sizeof(buffer));
                if (length < 0 && errno != EAGAIN) {
                    log_error("read");
                    break;
                }

                for (char *ptr = buffer; ptr < buffer + length; ) {
                    struct inotify_event *event = (struct inotify_event *) ptr;
                    fs_event_t fs_event;
                    fs_event.path = event->name;
                    fs_event.mask = event->mask;
                    fs_event.uid = getuid(); // Get the user ID
                    fs_event.timestamp = time(NULL); // Get the current timestamp

                    watcher->callback(&fs_event);
                    ptr += sizeof(struct inotify_event) + event->len;
                }
            }
        }
    }

    return NULL;
}

fs_watcher_t* fs_watcher_init(void) {
    fs_watcher_t *watcher = malloc(sizeof(fs_watcher_t));
    if (!watcher) {
        log_error("malloc");
        return NULL;
    }

    watcher->inotify_fd = inotify_init1(IN_NONBLOCK);
    if (watcher->inotify_fd == -1) {
        log_error("inotify_init1");
        free(watcher);
        return NULL;
    }

    watcher->epoll_fd = epoll_create1(0);
    if (watcher->epoll_fd == -1) {
        log_error("epoll_create1");
        close(watcher->inotify_fd);
        free(watcher);
        return NULL;
    }

    watcher->running = 0;
    watcher->lock_initialized = 0;
    if (pthread_mutex_init(&watcher->lock, NULL) != 0) {
        log_error("pthread_mutex_init");
        close(watcher->inotify_fd);
        close(watcher->epoll_fd);
        free(watcher);
        return NULL;
    }
    watcher->lock_initialized = 1;
    return watcher;
}

int fs_watcher_add(fs_watcher_t *watcher, const char *path, fs_event_callback_t callback) {
    int wd = inotify_add_watch(watcher->inotify_fd, path, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_ATTRIB);
    if (wd == -1) {
        log_error("inotify_add_watch");
        return -1;
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = watcher->inotify_fd;
    if (epoll_ctl(watcher->epoll_fd, EPOLL_CTL_ADD, watcher->inotify_fd, &event) == -1) {
        log_error("epoll_ctl");
        inotify_rm_watch(watcher->inotify_fd, wd);
        return -1;
    }

    watcher->callback = callback;
    return 0;
}

int fs_watcher_start(fs_watcher_t *watcher) {
    pthread_mutex_lock(&watcher->lock);
    if (watcher->running) {
        pthread_mutex_unlock(&watcher->lock);
        return -1; // Already running
    }
    watcher->running = 1;
    pthread_mutex_unlock(&watcher->lock);

    if (pthread_create(&watcher->thread, NULL, fs_watcher_thread_func, watcher) != 0) {
        log_error("pthread_create");
        watcher->running = 0;
        return -1;
    }
    return 0;
}

void fs_watcher_stop(fs_watcher_t *watcher) {
    pthread_mutex_lock(&watcher->lock);
    watcher->running = 0;
    pthread_mutex_unlock(&watcher->lock);
    pthread_join(watcher->thread, NULL);
}

void fs_watcher_destroy(fs_watcher_t *watcher) {
    fs_watcher_stop(watcher);
    close(watcher->inotify_fd);
    close(watcher->epoll_fd);
    if (watcher->lock_initialized) {
        pthread_mutex_destroy(&watcher->lock);
    }
    free(watcher);
}
