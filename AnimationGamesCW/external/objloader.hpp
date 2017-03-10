#ifndef OBJLOADER_H
#define OBJLOADER_H

bool loadSimpleOBJ(const char * path,std::vector<glm::vec3> & out_vertices,std::vector<unsigned int> & out_vertex_indices);

#endif
