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

void swap(triangle_t* a, triangle_t* b) {
    triangle_t temp = *a;
    *a = *b;
    *b = temp;
}


void bubbleSort(triangle_t arr[], int n) {
    int i, j;
    bool swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = false;
        for (j = n - 1; j > 0 + i; j--) {
            if (arr[j].avg_depth > arr[j - 1].avg_depth) {
                swap(&arr[j], &arr[j - 1]);
                swapped = true;
            }
        }

        if (swapped == false)
            break;
    }
}


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int window_width = 800;
int window_height = 600;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { 0, 0, 0 };

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


        vec3_t transformed_vertices[3];

        for (int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];



     /*       transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);*/

            transformed_vertex.z += 5;

            transformed_vertices[j] = transformed_vertex;
        }

        vec3_t vector_a = transformed_vertices[0]; /*    A              */
        vec3_t vector_b = transformed_vertices[1]; /*   / \             */
        vec3_t vector_c = transformed_vertices[2]; /*  C---B (clokwise) */

        // B - A
        vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        // C - A
        vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ab);
        vec3_normalize(&vector_ac);

        // Compute the face normal (using cross product to find perpendicular)
        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&normal);

        vec3_t camera_ray = vec3_sub(camera_position, vector_a);

        float dot_normal_camera = vec3_dot(camera_ray, normal);

        vec2_t projected_points[3];

        if (dot_normal_camera > 0) {
            for (int j = 0; j < 3; j++)
            {
                projected_points[j] = project(transformed_vertices[j]);

                projected_points[j].x += (window_width / 2);
                projected_points[j].y += (window_height / 2);
            }

            triangle_t projected_triangle = {
                .points = {
                    { projected_points[0].x ,  projected_points[0].y},
                    { projected_points[1].x ,  projected_points[1].y},
                    { projected_points[2].x ,  projected_points[2].y }
                },
                .color = mesh_face.color,
                .avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0
            };


            array_push(triangles_to_render, projected_triangle);
        }
        bubbleSort(triangles_to_render, array_length(triangles_to_render));
    }


    //for (int i = 0; i < array_length(triangles_to_render); i++) {
    //    printf("z: %f \n", triangles_to_render[i].avg_depth);
    //}
    //printf("================= \n");
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

    load_cube_mesh_data();
    //load_obj_file_data("C:\\CBProjects\\3drenderer\\assets\\cube.obj");

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

       draw_filled_triangle(
           triangle.points[0].x,
           triangle.points[0].y,
           triangle.points[1].x,
           triangle.points[1].y,
           triangle.points[2].x,
           triangle.points[2].y,
           triangle.color
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
