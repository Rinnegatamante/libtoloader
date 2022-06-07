/*
 * This file is part of Tinyiest Obj Loader
 * Copyright 2022 Rinnegatamante
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TINYIEST_OBJ_LOADER_H_
#define _TINYIEST_OBJ_LOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	float x;
	float y;
	float z;
} to_vec3;

typedef struct {
	float x;
	float y;
} to_vec2;

typedef struct {
	int x;
	int y;
	int z;
} to_ivec3;

typedef struct {
	to_vec3 *pos;
	to_vec2 *texcoord;
	to_vec3 *normals;
	int num_vertices;
} to_model;

enum {
	TO_OK, // No error
	TO_NOFILE // File not found
};

// Loads an obj model from file
int to_loadObj(const char *fname, to_model *res);

// Frees allocated memory for a loaded obj model
void to_freeObj(to_model mdl);

#ifdef __cplusplus
}
#endif

#endif
