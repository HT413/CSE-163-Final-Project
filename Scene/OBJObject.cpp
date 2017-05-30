#include "OBJObject.h"

float minX, minY, minZ;
float maxX, maxY, maxZ;

OBJObject::OBJObject(const char * filepath) {
	model = mat4(1.0f);
	minX = minY = minZ = INFINITY;
	maxX = maxY = maxZ = -INFINITY;
	cout << "Now parsing: " << filepath << endl;
	parse(filepath);

	// Finished parsing, now set up the buffer objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

OBJObject::~OBJObject() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &NBO);
}

void OBJObject::parse(const char * filepath) {
	FILE* fp;
	float x, y, z;
	pair<int, int> fnx, fny, fnz;
	int c1, c2;

	fp = fopen(filepath, "rb");

	if (fp == NULL){
		cerr << "Error loading file" << endl;
		std::getchar();
		exit(-1);
	}

	while (!feof(fp)){
		c1 = fgetc(fp);
		while (!(c1 == 'v' || c1 == 'f')){
			c1 = fgetc(fp);
			if (feof(fp))
				break;
		}
		c2 = fgetc(fp);

		if ((c1 == 'v') && (c2 == ' ')){
			fscanf(fp, "%f %f %f", &x, &y, &z);
			vertices.push_back(vec3(x, y, z));
			if (x < minX) minX = x;
			if (y < minY) minY = y;
			if (z < minZ) minZ = z;
			if (x > maxX) maxX = x;
			if (y > maxY) maxY = y;
			if (z > maxZ) maxZ = z;
		}
		else if ((c1 == 'v') && (c2 == 'n')){
			fscanf(fp, "%f %f %f", &x, &y, &z);
			normals.push_back(glm::normalize(vec3(x, y, z)));
		}
		else if (c1 == 'f')
		{
			fscanf(fp, "%d//%d %d//%d %d//%d", &fnx.first, &fnx.second, &fny.first, &fny.second, &fnz.first, &fnz.second);
			indices.push_back(fnx.first - 1);
			indices.push_back(fny.first - 1);
			indices.push_back(fnz.first - 1);
		}
	}

	fclose(fp); // Finished parsing
	cout << "Done parsing, now centering the object..." << endl;

	float avgX = (maxX + minX) / 2.0f;
	float avgY = (maxY + minY) / 2.0f;
	float avgZ = (maxZ + minZ) / 2.0f;
	float maxDimension = std::max((maxX - minX), std::max((maxY - minY), (maxZ - minZ)));
	maxDimension /= 5.f;

	// Loop through list of vertices and replace with the adjusted vertices
	for (int i = 0; i < vertices.size(); ++i) {
		vec3 shiftedVertex = (vertices[i] - vec3(avgX, avgY, avgZ)) / maxDimension;
		vertices[i] = shiftedVertex;
	}

	cout << "All done!" << endl;
}

void OBJObject::draw(GLuint shaderProgram) {
	if(usingPhong) mat->bindMaterial();
	else a_mat->bindMaterial();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &(model[0][0]));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}