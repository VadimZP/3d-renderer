#include <math.h>

#include "vector.h"

float vec2_length(vec2_t v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(vec2_t a, vec2_t b) {
	vec2_t v = { .x = a.x + b.x, .y = a.y + b.y };
	return v;
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
	vec2_t v = { .x = a.x - b.x, .y = a.y - b.y };
	return v;
}

vec2_t vec2_mul(vec2_t v, float factor) {
	vec2_t new_vector = { .x = v.x * factor, .y = v.y * factor };
	return new_vector;
}

float vec2_dot(vec2_t a, vec2_t b) {
	return a.x * b.x + a.y * b.y;
}

void vec2_normalize(vec2_t* v) {
	float l = vec2_length(*v);
	v->x /= l;
	v->y /= l;
}


float vec3_length(vec3_t v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
	vec3_t v = { .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z };
	return v;
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
	vec3_t v = { .x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z };
	return v;
}

vec3_t vec3_mul(vec3_t v, float factor) {
	vec3_t new_vector = { .x = v.x * factor, .y = v.y * factor, .z = v.z * factor };
	return new_vector;
}

float vec3_dot(vec3_t a, vec3_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
	vec3_t result = {
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x
	};
	return result;
}

void vec3_normalize(vec3_t* v) {
	float l = vec3_length(*v);
	v->x /= l;
	v->y /= l;
	v->z /= l;
}


vec3_t vec3_rotate_x(vec3_t v, float angle) {
	if (!angle) {
		return v;
	}
	vec3_t rotated_vector = {
		.x = v.x,
		.y = v.y * cos(angle) - v.z * sin(angle),
		.z = v.y * sin(angle) + v.z * cos(angle),
	};

	return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
	if (!angle) {
		return v;
	}
	vec3_t rotated_vector = {
		.x = v.x * cos(angle) - v.z * sin(angle),
		.y = v.y,
		.z = v.x * sin(angle) + v.z * cos(angle),
	};

	return rotated_vector;
}


vec3_t vec3_rotate_z(vec3_t v, float angle) {
	if (!angle) {
		return v;
	}
	vec3_t rotated_vector = {
		.x = v.x * cos(angle) - v.y * sin(angle),
		.y = v.x * sin(angle) + v.y * cos(angle),
		.z = v.z,
	};

	return rotated_vector;
}
