#include <stdio.h>
#include <stdlib.h>

int colors[4][3] = {
    {0, 0, 0},
    {164, 164, 164},
    {255, 255, 255},
    {129, 0, 129}
};

static double lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

void interpolate_color(double t, int *r, int *g, int *b) {
    if (t <= 0) {
        *r = colors[0][0]; *g = colors[0][1]; *b = colors[0][2];
        return;
    }
    if (t >= 1) {
        *r = colors[3][0]; *g = colors[3][1]; *b = colors[3][2];
        return;
    }
    double scaled = t * 3;
    int idx = (int)scaled;
    double local_t = scaled - idx;
    *r = (int)lerp(colors[idx][0], colors[idx+1][0], local_t);
    *g = (int)lerp(colors[idx][1], colors[idx+1][1], local_t);
    *b = (int)lerp(colors[idx][2], colors[idx+1][2], local_t);
}

void set_color(int r, int g, int b) {
    printf("\033[38;2;%d;%d;%dm", r, g, b);
}

void reset_color() {
    printf("\033[0m");
}

int main(int argc, char *argv[]) {
    FILE *f = stdin;
    if (argc > 1) {
        f = fopen(argv[1], "rb");
        if (!f) {
            perror("Failed to open file");
            return 1;
        }
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = NULL;
    if (size > 0) {
        buffer = malloc(size + 1);
        if (!buffer) {
            perror("malloc failed");
            if (f != stdin) fclose(f);
            return 1;
        }
        fread(buffer, 1, size, f);
        buffer[size] = '\0';
    } else {
        // For stdin or unknown size, read dynamically
        size_t cap = 8192, len = 0;
        buffer = malloc(cap);
        if (!buffer) {
            perror("malloc failed");
            if (f != stdin) fclose(f);
            return 1;
        }
        int c;
        while ((c = fgetc(f)) != EOF) {
            if (len + 1 >= cap) {
                cap *= 2;
                char *tmp = realloc(buffer, cap);
                if (!tmp) {
                    perror("realloc failed");
                    free(buffer);
                    if (f != stdin) fclose(f);
                    return 1;
                }
                buffer = tmp;
            }
            buffer[len++] = c;
        }
        buffer[len] = '\0';
        size = len;
    }

    if (f != stdin) fclose(f);

    int lines_count = 0;
    for (long i = 0; i < size; i++) {
        if (buffer[i] == '\n') lines_count++;
        }
        if (size > 0 && buffer[size-1] != '\n') {
            lines_count++;
        }
        char **lines = malloc(lines_count * sizeof(char *));
    if (!lines) {
        perror("malloc failed");
        free(buffer);
        return 1;
    }

    int line_idx = 0;
    lines[0] = buffer;
    for (long i = 0; i < size; i++) {
        if (buffer[i] == '\n') {
            buffer[i] = '\0';
            if (line_idx + 1 < lines_count)
                lines[++line_idx] = buffer + i + 1;
        }
    }

    for (int i = 0; i < lines_count; i++) {
        double t = (lines_count == 1) ? 0.0 : (double)i / (lines_count - 1);
        int r, g, b;
        interpolate_color(t, &r, &g, &b);
        set_color(r, g, b);
        printf("%s\n", lines[i]);
        reset_color();
    }

    free(lines);
    free(buffer);
    return 0;
}
