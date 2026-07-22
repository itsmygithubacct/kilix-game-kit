#include "kilix_game_loop.h"
#include "kilix_game_test.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition)                                                      \
    do {                                                                      \
        if (!(condition)) {                                                   \
            (void)fprintf(stderr, "%s:%d: check failed: %s\n",             \
                          __FILE__, __LINE__, #condition);                     \
            return false;                                                     \
        }                                                                     \
    } while (false)

static bool test_fixed_step(void)
{
    kilix_game_clock_options options;
    kilix_game_clock clock;
    kilix_game_frame frame;

    kilix_game_clock_options_init(&options);
    options.step_ns = 10;
    options.max_frame_ns = 100;
    options.max_steps_per_frame = 3u;
    CHECK(kilix_game_clock_init(&clock, &options));
    frame = kilix_game_clock_advance(&clock, 1000);
    CHECK(frame.steps == 0u && frame.alpha == 0.0);
    frame = kilix_game_clock_advance(&clock, 1025);
    CHECK(frame.steps == 2u);
    CHECK(frame.alpha > 0.49 && frame.alpha < 0.51);
    frame = kilix_game_clock_advance(&clock, 1040);
    CHECK(frame.steps == 2u && frame.alpha == 0.0);
    frame = kilix_game_clock_advance(&clock, 1240);
    CHECK(frame.steps == 3u);
    CHECK(frame.frame_ns == 100);
    CHECK(frame.dropped_ns == 170);
    CHECK(kilix_game_clock_dropped_ns(&clock) == 170);
    CHECK(kilix_game_clock_total_steps(&clock) == 7u);
    CHECK(kilix_game_clock_step_seconds(&clock) == 1.0e-8);

    /* Backward clocks resynchronize without a negative frame. */
    frame = kilix_game_clock_advance(&clock, 1200);
    CHECK(frame.steps == 0u && frame.frame_ns == 0);
    frame = kilix_game_clock_advance(&clock, 1210);
    CHECK(frame.steps == 1u);
    return true;
}

static bool test_clock_validation_and_sleep(void)
{
    kilix_game_clock_options options;
    kilix_game_clock clock;
    int64_t now;

    kilix_game_clock_options_init(&options);
    options.step_ns = 0;
    CHECK(!kilix_game_clock_init(&clock, &options));
    options.step_ns = 20;
    options.max_frame_ns = 10;
    CHECK(!kilix_game_clock_init(&clock, &options));
    now = kilix_game_monotonic_ns();
    CHECK(now >= 0);
    CHECK(kilix_game_sleep_until_ns(now));
    CHECK(!kilix_game_sleep_until_ns(-1));
    return true;
}

static bool test_pty_and_golden_helpers(void)
{
    static const char message[] = "ordered terminal bytes";
    static const uint8_t first[] = {0u, 10u, 20u, 255u, 4u, 5u, 6u, 7u};
    static const uint8_t second[] = {0u, 12u, 20u, 255u, 4u, 5u, 9u, 7u};
    kilix_test_pty pty;
    kilix_test_image_diff difference;
    char received[64] = "";
    ssize_t count;

    CHECK(kilix_test_pty_open(&pty, 80u, 24u, 640u, 480u));
    CHECK(kilix_test_write_all(pty.slave_fd, message, sizeof message - 1u));
    count = kilix_test_read_available(pty.master_fd, received,
                                      sizeof received, 20);
    CHECK(count == (ssize_t)(sizeof message - 1u));
    CHECK(memcmp(received, message, sizeof message - 1u) == 0);
    CHECK(kilix_test_contains(received, (size_t)count, "terminal", 8u));
    CHECK(!kilix_test_contains(received, (size_t)count, "mouse", 5u));
    kilix_test_pty_close(&pty);

    CHECK(kilix_test_hash64("hello", 5u) == UINT64_C(0xa430d84680aabd0b));
    difference = kilix_test_diff_rgba(first, second, 2u, 1u);
    CHECK(difference.differing_pixels == 2u);
    CHECK(difference.maximum_channel_delta == 3u);
    difference = kilix_test_diff_rgba(first, second, 2u, 3u);
    CHECK(difference.differing_pixels == 0u);
    return true;
}

int main(void)
{
    CHECK(test_fixed_step());
    CHECK(test_clock_validation_and_sleep());
    CHECK(test_pty_and_golden_helpers());
    (void)puts("ok: kilix-game-kit");
    return 0;
}
