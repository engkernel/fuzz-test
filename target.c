/*
 * Minimal, deliberately-vulnerable test target for exercising the engine's
 * afl-compile -> afl-fuzz pipeline end-to-end.
 *
 * The bug: a classic stack buffer overflow, reachable on the FIRST input
 * that happens to start with the 4 bytes "FUZZ" followed by more than 16
 * characters before a newline. AFL will find this in well under a minute
 * even from a cold/empty corpus - this is intentional, so your first test
 * run gets a real crash to verify crash-detection and artifact capture,
 * not just a clean multi-hour run with nothing to show for it.
 *
 * Supports BOTH invocation modes your driver can produce:
 *   - file mode:  ./target <path-to-input-file>   (the "@@" case)
 *   - stdin mode: ./target                         (reads stdin directly)
 * so you can test is_stdin_mode branching against the same binary.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void vulnerable_copy(const char *input) {
    char small_buffer[16];
    if (strncmp(input, "FUZZ", 4) == 0) {
        // Deliberately unsafe: strcpy with no bounds check once the magic
        // prefix is seen. Anything past 4+15 chars before input's own NUL
        // overflows small_buffer.
        strcpy(small_buffer, input + 4);
        printf("Matched prefix, copied: %s\n", small_buffer);
    } else {
        printf("No match, len=%zu\n", strlen(input));
    }
}

int main(int argc, char **argv) {
    char buf[256];
    size_t len;

    if (argc > 1) {
        // file mode: argv[1] is the AFL-provided input file path
        FILE *f = fopen(argv[1], "rb");
        if (!f) {
            perror("fopen");
            return 1;
        }
        len = fread(buf, 1, sizeof(buf) - 1, f);
        fclose(f);
    } else {
        // stdin mode
        len = fread(buf, 1, sizeof(buf) - 1, stdin);
    }

    buf[len] = '\0';
    vulnerable_copy(buf);
    return 0;
}
