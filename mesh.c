
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mesh.h"
#include "array.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0 }
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
   {.x = -1, .y = -1, .z = -1 }, // 1
   {.x = -1, .y = 1, .z = -1 }, // 2
   {.x = 1, .y = 1, .z = -1 }, // 3
   {.x = 1, .y = -1, .z = -1 }, // 4
   {.x = 1, .y = 1, .z = 1 }, // 5
   {.x = 1, .y = -1, .z = 1 }, // 6
   {.x = -1, .y = 1, .z = 1 }, // 7
   {.x = -1, .y = -1, .z = 1 } // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3 },
    {.a = 1, .b = 3, .c = 4 },
    // right
    {.a = 4, .b = 3, .c = 5 },
    {.a = 4, .b = 5, .c = 6 },
    // back
    {.a = 6, .b = 5, .c = 7 },
    {.a = 6, .b = 7, .c = 8 },
    // left
    {.a = 8, .b = 7, .c = 2 },
    {.a = 8, .b = 2, .c = 1 },
    // top
    {.a = 2, .b = 7, .c = 5 },
    {.a = 2, .b = 5, .c = 3 },
    // bottom
    {.a = 6, .b = 8, .c = 1 },
    {.a = 6, .b = 1, .c = 4 },
};

void load_cube_mesh_data(void) {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }

    for (int i = 0; i < N_CUBE_FACES; i++) {
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }
}

void load_obj_file_data(char* filename) {
    FILE* fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL) {
        printf("Not able to open the file");
        return;
    }
    
    char myString[50];
    char trimmedString[50];

    vec3_t* vertices = NULL;
    face_t* faces = NULL;

    while (fgets(myString, 50, fptr)) {
        if (myString[0] == 'v' && myString[1] == ' ') {
            int j = 2;
            for (int i = 0; i < 50; i++) {
                trimmedString[i] = myString[j];
                j++;
            }

            vec3_t vertex = { 0, 0, 0 };
            char* myPtr = strtok(trimmedString, " ");
            int k = 0;
            while (myPtr != NULL) {
                float number = atof(myPtr);
                if (k == 0) {
                    vertex.x = number;
                }
                if (k == 1) {
                    vertex.y = number;
                }
                if (k == 2) {
                    vertex.z = number;
                }
                myPtr = strtok(NULL, " ");
                k++;
            }
            array_push(vertices, vertex);
            k = 0;
        }

        if (myString[0] == 'f' && myString[1] == ' ') {
            int j = 2;
            for (int i = 0; i < 50; i++) {
                trimmedString[i] = myString[j];
                j++;
            }

            printf("%s\n", trimmedString);

            face_t face = { 0, 0, 0 };
            char* myPtr = strtok(trimmedString, "/ ");
            int k = 1;
            while (myPtr != NULL) {
                int number = atoi(myPtr);

                if (k == 1) {
                    printf("%s is under %d\n", myPtr, k);
                    face.a = number;
                }
                if (k == 4) {
                    printf("%s is under %d\n", myPtr, k);
                    face.b = number;
                }
                if (k == 7) {
                    printf("%s is under %d\n", myPtr, k);
                    face.c = number;
                }
                myPtr = strtok(NULL, "/ ");
                k++;

            }
            printf("%d\n", face.a);
            array_push(faces, face);
            k = 0;
        }
    }

    mesh.vertices = vertices;
    mesh.faces = faces;

    fclose(fptr);
}