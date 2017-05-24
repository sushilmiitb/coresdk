// #include <coreEngine/modelBuilder/ModelLoader.h>
// #include <vector>
// #include <coreEngine/conf/MathType.h>

// namespace cl{
// 	void ModelLoader::load_obj(const char* filename, std::vector<CL_Vec3> &vertices, std::vector<CL_Vec2> &uvs,
// 		std::vector<CL_Vec3> &normals, std::vector<CL_GLuint> &elements){

// 		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
// 		std::vector< CL_Vec3 > temp_vertices;
// 		std::vector< CL_Vec2 > temp_uvs;
// 		std::vector< CL_Vec3 > temp_normals;

// 		FILE * file = fopen(filename, "r");
// 		if (file == NULL){
// 			printf("Impossible to open the file !\n");
// 			return;
// 		}

// 		while (1){

// 			char lineHeader[128];
// 			// read the first word of the line
// 			int res = fscanf(file, "%s", lineHeader);
// 			if (res == EOF){
// 				break; // EOF = End Of File. Quit the loop.
// 			}

// 			// else : parse lineHeader
// 			if (strcmp(lineHeader, "v") == 0){
// 				CL_Vec3 vertex;
// 				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
// 				temp_vertices.push_back(vertex);
// 			}
// 			else if (strcmp(lineHeader, "vt") == 0){
// 				CL_Vec2 uv;
// 				fscanf(file, "%f %f\n", &uv.x, &uv.y);
// 				temp_uvs.push_back(uv);
// 			}
// 			else if (strcmp(lineHeader, "vn") == 0){
// 				glm::vec3 normal;
// 				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
// 				temp_normals.push_back(normal);
// 			}
// 			else if (strcmp(lineHeader, "f") == 0){
// 				std::string vertex1, vertex2, vertex3;
// 				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
// 				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
// 				if (matches != 9){
// 					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
// 					return;
// 				}
// 				vertexIndices.push_back(vertexIndex[0]);
// 				vertexIndices.push_back(vertexIndex[1]);
// 				vertexIndices.push_back(vertexIndex[2]);
// 				uvIndices.push_back(uvIndex[0]);
// 				uvIndices.push_back(uvIndex[1]);
// 				uvIndices.push_back(uvIndex[2]);
// 				normalIndices.push_back(normalIndex[0]);
// 				normalIndices.push_back(normalIndex[1]);
// 				normalIndices.push_back(normalIndex[2]);
// 			}
// 		}

// 		// For each vertex of each triangle
// 		for (unsigned int i = 0; i < vertexIndices.size(); i++){
// 			unsigned int vertexIndex = vertexIndices[i];
// 			CL_Vec3 vertex = temp_vertices[vertexIndex - 1];
// 			vertices.push_back(vertex);
// 		}

// 		// For each uv
// 		for (unsigned int i = 0; i < uvIndices.size(); i++){
// 			unsigned int uvIndex = uvIndices[i];
// 			CL_Vec2 uv = temp_uvs[uvIndex - 1];
// 			uvs.push_back(uv);
// 		}

// 		// For each normal
// 		for (unsigned int i = 0; i < normalIndices.size(); i++){
// 			unsigned int normalIndex = normalIndices[i];
// 			CL_Vec3 normal = temp_vertices[normalIndex - 1];
// 			normals.push_back(normal);
// 		}
// 	}
// }