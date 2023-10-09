#pragma once
#include "pch.h"
#include "OBJ_Loader.h"

class Mesh
{
private:
	std::vector<GLfloat> vertexData;
	std::vector<GLfloat> colorData;
	std::vector<GLuint> indexData;

	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;

public:
	Mesh()
	{
		vao = NULL;
		vbo[0] = NULL;
		vbo[1] = NULL;
		ebo = NULL;
	}

	~Mesh()
	{

	}

	GLvoid LoadFromFile(const char* fileName)
	{
		objl::Loader loader;
		if (!loader.LoadFile(fileName))
			return;

		for (int i = 0; i < loader.LoadedMeshes.size(); ++i)
		{
			objl::Mesh m = loader.LoadedMeshes[i];

			for (int j = 0; j < m.Vertices.size(); ++j)
			{
				objl::Vector3 p = m.Vertices[j].Position;
				objl::Vector3 n = m.Vertices[j].Normal;

				vertexData.push_back(p.X);
				vertexData.push_back(p.Y);
				vertexData.push_back(p.Z);
				colorData.push_back(n.X);
				colorData.push_back(n.Y);
				colorData.push_back(n.Z);
			}

			for (int j = 0; j < m.Indices.size(); ++j)
			{
				GLuint u = m.Indices[j];
				indexData.push_back(u);
			}
		}
	}

	void AppendVertex(const glm::vec3& v)
	{
		vertexData.push_back(v.x);
		vertexData.push_back(v.y);
		vertexData.push_back(v.z);
	}

	void AppendColor(const glm::vec3& v)
	{
		colorData.push_back(v.x);
		colorData.push_back(v.y);
		colorData.push_back(v.z);
	}

	void AppendIndex(GLuint v)
	{
		indexData.push_back(v);
	}

	void AppendIndex(GLuint v0, GLuint v1, GLuint v2)
	{
		indexData.push_back(v0);
		indexData.push_back(v1);
		indexData.push_back(v2);
	}

	void ClearIndexes()
	{
		indexData.clear();
	}

	void MakeArrayBuffers()
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(2, vbo);
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexData.size(), vertexData.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colorData.size(), colorData.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexData.size(), indexData.data(), GL_STATIC_DRAW);
	}

	void ValidateVBOs()
	{
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertexData.size(), vertexData.data());

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * colorData.size(), colorData.data());
	}

	void ValidateEBO()
	{
		glBindVertexArray(vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexData.size(), indexData.data(), GL_STATIC_DRAW);
	}

	void Draw(GLenum mode)
	{
		glBindVertexArray(vao);
		glDrawElements(mode, indexData.size(), GL_UNSIGNED_INT, 0);
	}

	void DrawPolygon()
	{
		glBindVertexArray(vao);
		if (indexData.size() == 1)
			Draw(GL_POINTS);
		else if (indexData.size() < 3)
			Draw(GL_LINES);
		else
			Draw(GL_POLYGON);
	}

	void DrawElements()
	{
		Draw(GL_TRIANGLES);
	}
};