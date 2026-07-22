# kilix-game-kit

`kilix-game-kit` is the thin integration layer for C games and apps running
under Kilix, Pleb, and Plebian OS. One recursively checked-out submodule pins
and builds the shared terminal, input, software-rendering, audio, and state
stack:

- `kitty-terminal-session` (including framebuffer, input, and keyboard)
- `soft-raster`
- `pcm-mixer` and `pcmmix-bank`
- `kilix-state`

The archive adds only a fixed-step clock with bounded catch-up. A separate
test archive provides PTY I/O, byte matching, stable FNV-1a hashes, and
tolerance-aware RGBA diffs. Game-specific rules, controls, art, simulation,
and sound meaning stay in each game.

## Checkout and verify

```sh
git clone --recurse-submodules \
  https://github.com/itsmygithubacct/kilix-game-kit.git
cd kilix-game-kit
make test
make sanitize
make test-deps
```

`tools/check-submodules.sh` fails for missing, conflicted, or locally advanced
dependency checkouts. The Git links are the dependency lock; updates are
ordinary reviewed submodule commits.

## Consume from a game

Add this repository as `third_party/kilix-game-kit`, initialize recursively,
then include its Make fragment:

```make
include third_party/kilix-game-kit/mk/game-kit.mk
CPPFLAGS += $(KILIX_GAME_KIT_CPPFLAGS)

game: $(GAME_OBJECTS) $(KILIX_GAME_KIT_LIB)
	$(CC) -o $@ $(GAME_OBJECTS) $(KILIX_GAME_KIT_LIB) \
		$(KILIX_GAME_KIT_LDLIBS)
```

Consumers can include `kilix_game_kit.h` for the complete runtime stack or
the individual public headers. Since the static archive stores dependencies
as separate objects, the linker pulls only the APIs a game uses.

## Fixed-step clock

```c
kilix_game_clock clock;
kilix_game_clock_init(&clock, NULL); /* 60 Hz, max 8 catch-up steps */

for (;;) {
    kilix_game_frame frame = kilix_game_clock_advance(
        &clock, kilix_game_monotonic_ns());
    for (uint32_t i = 0; i < frame.steps; ++i)
        update(kilix_game_clock_step_seconds(&clock));
    render(frame.alpha);
}
```

Long stalls are clamped and excess accumulated steps are dropped, preventing
a permanent “spiral of death.” The clock reports dropped time for diagnostics
and accepts caller-supplied timestamps, so simulation timing is deterministic
in tests.

## Test helpers

Link `libkilix-game-test.a` with `-lutil` to create fixed-size PTYs and assert
terminal mode negotiation without a real terminal. `kilix_test_hash64()`
provides stable binary/golden hashes, while `kilix_test_diff_rgba()` reports
pixel differences at a selectable per-channel tolerance.

## License

MIT. Pinned dependencies retain their own licenses.
