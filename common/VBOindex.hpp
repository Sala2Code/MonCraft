#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <map>
#include <string.h> 

#include "include.hpp"

struct PackedVertex{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator<(const PackedVertex that) const{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
	};
};


inline bool getSimilarVertexIndex_fast( PackedVertex & packed, std::map<PackedVertex,unsigned short> & VertexToOutIndex,unsigned short & result){
	std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if ( it == VertexToOutIndex.end() ){
		return false;
	}else{
		result = it->second;
		return true;
	}
}

inline void indexVBO(
        std::vector<glm::vec3> &in_vertices,std::vector<glm::vec2> &in_uvs,std::vector<glm::vec3> &in_normals,
        std::vector<unsigned short> &out_indices,std::vector<glm::vec3> &out_vertices,std::vector<glm::vec2> &out_uvs,std::vector<glm::vec3> &out_normals
){
	std::map<PackedVertex,unsigned short> VertexToOutIndex;
	for ( unsigned int i=0; i<in_vertices.size(); i++ ){
		PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};
		unsigned short index;
		bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

		if ( found ){
			out_indices.push_back( index );
		}else{
			out_vertices.push_back( in_vertices[i]);
			out_uvs.push_back( in_uvs[i]);
			out_normals.push_back( in_normals[i]);
			unsigned short newindex = (unsigned short)out_vertices.size() - 1;
			out_indices.push_back( newindex );
			VertexToOutIndex[ packed ] = newindex;
		}
	}
}


#endif