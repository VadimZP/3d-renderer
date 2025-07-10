#define SDL_MAIN_USE_CALLBACKS 1

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int window_width = 800;
int window_height = 600;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };

float fov_factor = 640;


vec2_t project(vec3_t point) {
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z
    };
    return projected_point;
}

uint64_t previous_frame_time = 0;

void update(void) {
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (time_to_wait > 0) {
        SDL_Delay(time_to_wait);
    }
    previous_frame_time = SDL_GetTicks();

    triangles_to_render = NULL;

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            transformed_vertex.z -= camera_position.z;

            vec2_t projected_point = project(transformed_vertex);

            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }

        //triangles_to_render[i] = projected_triangle;
        array_push(triangles_to_render, projected_triangle);
    }
}



SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    //load_obj_file_data("C:\\CBProjects\\3drenderer\\assets\\cube.obj");

    //return SDL_APP_FAILURE;

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

    load_obj_file_data("C:\\CBProjects\\3drenderer\\assets\\f22.obj");

    vec3_t a = { 2.5, 6.4, 3.0 };

    float a_length = vec3_length(a);
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

    draw_grid();
    update();

    int num_triangles = array_length(triangles_to_render);

   for (int i = 0; i < num_triangles; i++) {
       triangle_t triangle = triangles_to_render[i];

       for (int j = 0; j <= 2; j++) {
           draw_rect(
               triangle.points[j].x,
               triangle.points[j].y,
               4,
               4,
               0xFFFFFF00
           );
       }

       draw_triangle(
           triangle.points[0].x,
           triangle.points[0].y,
           triangle.points[1].x,
           triangle.points[1].y,
           triangle.points[2].x,
           triangle.points[2].y,
           0xFFFFFF00
       );
    }

    // Clear the array of triangles to render every frame loop
    array_free(triangles_to_render);

    render_color_buffer();

    clear_color_buffer(0x0000000);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}
