#include <math.h>
#include <stdio.h>
#include "display.h"


void render_color_buffer(void) {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        window_width * sizeof(uint32_t)
    );

    SDL_RenderTexture(
        renderer,
        color_buffer_texture,
        NULL,
        NULL
    );
}

void draw_grid(void) {
    for (int y = 0; y < window_height; y += 10) {
        for (int x = 0; x < window_width; x += 10) {
            color_buffer[(window_width * y) + x] = 0xFF444444;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
        color_buffer[(window_width * y) + x] = color;
    }
}

void draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int side_length = dx > dy ? dx : dy;

    float increment_x = dx / (float)side_length;
    float increment_y = dy / (float)side_length;

    float x = x0;
    float y = y0;

    for (int i = 0; i < side_length; i++) {
        draw_pixel(round(x), round(y), 0xFFFFFF00);

        if (x0 < x1) {
            x += increment_x;
        }
        if (x0 > x1) {
            x -= increment_x;
        } 
        if (y0 < y1) {
            y += increment_y;
        } 
        if (y0 > y1) {
            y -= increment_y;
        }
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

