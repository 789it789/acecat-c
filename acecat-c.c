#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int colors[4][3] = {
    {0, 0, 0},
    {164, 164, 164},
    {255, 255, 255},
    {129, 0, 129}
};
const int num_colors = 4;

void set_rgb_color(int r, int g, int b) {
    printf("\033[38;2;%d;%d;%dm", r, g, b);
}

void reset_color() {
    printf("\033[0m");
}

static inline double lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

void interpolate_color(double t, int *r, int *g, int *b) {
    if (t <= 0) {
        *r = colors[0][0];
        *g = colors[0][1];
        *b = colors[0][2];
        return;
    }
    if (t >= 1) {
        *r = colors[num_colors - 1][0];
        *g = colors[num_colors - 1][1];
        *b = colors[num_colors - 1][2];
        return;
    }
    double scaled = t * (num_colors - 1);
    int idx = (int)scaled;
    double local_t = scaled - idx;

    *r = (int)lerp(colors[idx][0], colors[idx+1][0], local_t);
    *g = (int)lerp(colors[idx][1], colors[idx+1][1], local_t);
    *b = (int)lerp(colors[idx][2], colors[idx+1][2], local_t);
}

#define MAX_LINES 8192
#define MAX_LINE_LEN 8192

int main() {
    char *lines[MAX_LINES];
    int line_count = 0;

    while (line_count < MAX_LINES) {
        char *buffer = malloc(MAX_LINE_LEN);
        if (!buffer) {
            fprintf(stderr, "Out of memory\n");
            return 1;
        }
        if (!fgets(buffer, MAX_LINE_LEN, stdin)) {
            free(buffer);
            break;
        }
        lines[line_count++] = buffer;
    }

    for (int i = 0; i < line_count; i++) {
        double t = (line_count == 1) ? 0.0 : (double)i / (line_count - 1);

        int r, g, b;
        interpolate_color(t, &r, &g, &b);

        set_rgb_color(r, g, b);
        fputs(lines[i], stdout);
        reset_color();
    }

    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }

    return 0;
}
