#include "Types.h"

namespace ogl
{
	/**
	* \brief Вычисление нормали
	* \param ccw Треугольник построен по часовой и против часовой стрелки
	* \return Вектор нормали
	*/
	glm::vec3 Polygon::getNormal(bool ccw) const
	{
		glm::vec3 edge1 = vertices[1] - vertices[0];
		glm::vec3 edge2 = vertices[2] - vertices[0];
		return glm::normalize(ccw ? glm::cross(edge1, edge1) : glm::cross(edge2, edge1));
	}

	/**
	* \brief Вычисление тангента
	* \return Вектор тангента
	* \details Полученный вектор зависит от UV координат и ориентации текстуры на полигоне
	*/
	glm::vec3 Polygon::getUVTangent() const
	{
		// Грани полигона (в виде векторов)
		glm::vec3 edge1 = vertices[1] - vertices[0];
		glm::vec3 edge2 = vertices[2] - vertices[0];

		// Дельта UV для каждой грани
		glm::vec2 deltaUV1 = uvs[1] - uvs[0];
		glm::vec2 deltaUV2 = uvs[2] - uvs[0];

		// Подсчет тангента
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		return glm::normalize(glm::vec3(
			f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
			f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
			f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)));
	}

	/**
	* \brief Есть ли такой индекс у полигона
	* \param index Индекс
	* \return Да или нет
	*/
	bool Polygon::hasIndex(GLuint index) const
	{
		return indices[0] == index || indices[1] == index || indices[2] == index;
	}
}
