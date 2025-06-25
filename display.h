#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

extern SDL_Renderer* renderer;

extern int window_width;
extern int window_height;

extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;


void draw_grid(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
