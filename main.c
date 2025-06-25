#define SDL_MAIN_USE_CALLBACKS 1

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "display.h"
#include "vector.h"


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int window_width = 800;
int window_height = 600;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

#define N_POINTS (9 * 9 * 9)
vec3_t cube_points[N_POINTS]; // 9x9x9 cube
vec2_t projected_points[N_POINTS];

float fov_factor = 128;

vec2_t project(vec3_t point) {
    vec2_t projected_point = {
        .x = (fov_factor * point.x),
        .y = (fov_factor * point.y)
    };
    return projected_point;
}

void update(void) {
    for (int i = 0; i < N_POINTS; i++) {
        vec3_t point = cube_points[i];

        vec2_t projected_point = project(point);

        projected_points[i] = projected_point;
    }
}



SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DisplayID display_id = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* display_mode = SDL_GetCurrentDisplayMode(display_id);

    if (!display_mode) {
        SDL_Log("Failed to get current display mode: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    window_width = display_mode->w;
    window_height = display_mode->h;
    SDL_Log("size: %d, %d", window_width, window_height);

    if (!SDL_CreateWindowAndRenderer(
        "SDL Window", 
        window_width,
        window_height,
        SDL_WINDOW_FULLSCREEN, 
        &window, &renderer
    )) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    color_buffer = malloc(sizeof(uint32_t) * window_width * window_height);

    if (!color_buffer) {
        SDL_Log("Memory allocation for color buffer failed: %s", SDL_GetError());
        return 1;
    }

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    if (!color_buffer_texture) {
        SDL_Log("Failed to create color buffer texture: %s", SDL_GetError());
        return 1;
    }

    int point_count = 0;

    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                vec3_t new_point = { .x = x, .y = y, .z = z };
                cube_points[point_count++] = new_point;
            }
        }
    }

    update();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  
    }

    if (SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_ESCAPE) {
            return SDL_APP_SUCCESS;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //draw_grid();

   for (int i = 0; i < N_POINTS; i++) {
        vec2_t projected_point = projected_points[i];
        draw_rect(
            projected_point.x + (window_width / 2),
            projected_point.y + (window_height / 2),
            4,
            4,
            0xFFFFFF00
        );
    }


    render_color_buffer();

    clear_color_buffer(0x0000000);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    free(color_buffer);
}
