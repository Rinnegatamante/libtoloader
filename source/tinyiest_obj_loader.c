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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinyiest_obj_loader.h"

typedef struct {
	to_vec3 *pos;
	to_vec2 *texcoord;
	to_vec3 *normals;
	to_ivec3 *faces;
} to_rawmesh;

int to_loadObj(const char *fname, to_model *res) {
	// Opening model file from filesystem
	FILE *f = fopen(fname, "r");
	if (!f)
		return TO_NOFILE;
	fseek(f, 0, SEEK_END);
	int32_t msize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *mdl = (char *)malloc(msize);
	fread(mdl, 1, msize, f);
	fclose(f);
	
	// Initializing internal data struct
	to_rawmesh m;
	
	// Pre-parsing the file to allocate proper required data structs
	int num_pos = 0, num_texcoord = 0, num_normals = 0, num_vertices = 0;
	char *line, *p = mdl, *end = mdl + msize;
	char *s = strstr(mdl, "\n");
	for (;;) {
		line = p;
		if (s)
			s[0] = 0;
		if (line[0] == 'v') {
			if (line[1] == 't') { // Texture coord
				num_texcoord++;
			} else if (line[1] == 'n') { // Normal
				num_normals++;
			} else if (line[1] == ' ') { // Position
				num_pos++;
			}
		} else if (line[0] == 'f') { // Face
			int num_spaces = 0;
			char *space = strstr(line, " ");
			while (space) {
				space = strstr(space + 1, " ");
				num_spaces++;
			}
			if (num_spaces < 4) // Triangle
				num_vertices += 3;
			else // Quad
				num_vertices += 6;
		}
		if (!s)
			break;
		s[0] = '\n';
		if (s[1] == '\r')
			s++;
		p = s + 1;
		s = strstr(s + 1, "\n");
	}
	m.pos = (to_vec3 *)malloc(sizeof(to_vec3) * num_pos);
	m.texcoord = (to_vec2 *)malloc(sizeof(to_vec2) * num_texcoord);
	m.normals = (to_vec3 *)malloc(sizeof(to_vec3) * num_normals);
	m.faces = (to_ivec3 *)malloc(sizeof(to_ivec3) * num_vertices);
	
	// Parsing the file
	num_pos = num_texcoord = num_normals = num_vertices = 0;
	p = mdl;
	s = strstr(mdl, "\n");
	for (;;) {
		line = p;
		if (s)
			s[0] = 0;
		if (line[0] == 'v') {
			if (line[1] == 't') { // Texture coord
				sscanf(line, "vt %f %f", &m.texcoord[num_texcoord].x, &m.texcoord[num_texcoord].y);
				num_texcoord++;
			} else if (line[1] == 'n') { // Normal
				sscanf(line, "vn %f %f %f", &m.normals[num_normals].x, &m.normals[num_normals].y, &m.normals[num_normals].z);
				num_normals++;
			} else if (line[1] == ' ') { // Position
				sscanf(line, "v %f %f %f", &m.pos[num_pos].x, &m.pos[num_pos].y, &m.pos[num_pos].z);
				num_pos++;
			}
		} else if (line[0] == 'f') { // Face
			int num_spaces = 0;
			char *space = strstr(line, " ");
			while (space) {
				space = strstr(space + 1, " ");
				num_spaces++;
			}
			if (num_spaces < 4) {// Triangle
				sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
					&m.faces[num_vertices].x, &m.faces[num_vertices].y, &m.faces[num_vertices].z,
					&m.faces[num_vertices + 1].x, &m.faces[num_vertices + 1].y, &m.faces[num_vertices + 1].z,
					&m.faces[num_vertices + 2].x, &m.faces[num_vertices + 2].y, &m.faces[num_vertices + 2].z);
				num_vertices += 3;
			} else { // Quad
				sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
					&m.faces[num_vertices].x, &m.faces[num_vertices].y, &m.faces[num_vertices].z,
					&m.faces[num_vertices + 1].x, &m.faces[num_vertices + 1].y, &m.faces[num_vertices + 1].z,
					&m.faces[num_vertices + 2].x, &m.faces[num_vertices + 2].y, &m.faces[num_vertices + 2].z,
					&m.faces[num_vertices + 5].x, &m.faces[num_vertices + 5].y, &m.faces[num_vertices + 5].z);
					memcpy(&m.faces[num_vertices + 3].x, &m.faces[num_vertices + 2].x, sizeof(to_vec3));
					memcpy(&m.faces[num_vertices + 4].x, &m.faces[num_vertices].x, sizeof(to_vec3));
				num_vertices += 6;
			}
		}
		if (!s)
			break;
		if (s[1] == '\r')
			s++;
		p = s + 1;
		s = strstr(s + 1, "\n");
	}
	
	// Allocating final model required memory
	res->pos = (to_vec3 *)malloc(sizeof(to_vec3) * num_vertices);
	res->texcoord = (to_vec2 *)malloc(sizeof(to_vec2) * num_vertices);
	res->normals = (to_vec3 *)malloc(sizeof(to_vec3) * num_vertices);
	
	// Constructing final model data
	for (int i = 0; i < num_vertices; i++) {
		int pos_idx = m.faces[i].x - 1;
		int tex_idx = m.faces[i].y - 1;
		int nor_idx = m.faces[i].z - 1;
		memcpy(&res->pos[i].x, &m.pos[pos_idx].x, sizeof(to_vec3));
		memcpy(&res->texcoord[i].x, &m.texcoord[tex_idx].x, sizeof(to_vec2));
		memcpy(&res->normals[i].x, &m.normals[nor_idx].x, sizeof(to_vec3));
	}
	res->num_vertices = num_vertices;
	
	// Freeing temp data and returning loaded model
	free(mdl);
	free(m.faces);
	free(m.texcoord);
	free(m.normals);
	free(m.pos);
	return TO_OK;
}

void to_freeObj(to_model mdl) {
	free(mdl.pos);
	free(mdl.texcoord);
	free(mdl.normals);
}
