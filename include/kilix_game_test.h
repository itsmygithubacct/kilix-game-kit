#ifndef KILIX_GAME_TEST_H
#define KILIX_GAME_TEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kilix_test_pty {
    int master_fd;
    int slave_fd;
} kilix_test_pty;

typedef struct kilix_test_image_diff {
    size_t differing_pixels;
    uint8_t maximum_channel_delta;
} kilix_test_image_diff;

bool kilix_test_pty_open(kilix_test_pty *pty, unsigned short columns,
                         unsigned short rows, unsigned short pixel_width,
                         unsigned short pixel_height);
void kilix_test_pty_close(kilix_test_pty *pty);
bool kilix_test_write_all(int fd, const void *bytes, size_t byte_count);
ssize_t kilix_test_read_available(int fd, void *bytes, size_t capacity,
                                  int idle_timeout_ms);
bool kilix_test_contains(const void *bytes, size_t byte_count,
                         const void *needle, size_t needle_size);

uint64_t kilix_test_hash64(const void *bytes, size_t byte_count);
kilix_test_image_diff kilix_test_diff_rgba(const uint8_t *first,
                                           const uint8_t *second,
                                           size_t pixel_count,
                                           uint8_t tolerance);

#ifdef __cplusplus
}
#endif

#endif
