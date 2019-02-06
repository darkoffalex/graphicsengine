#include "Types.h"

namespace ogl
{
	/**
	* \brief Подсчет нормали
	* \param ccw Обход вершин против часовой стрелки
	* \return Нормаль
	*/
	glm::vec3 Polygon::calculateNormal(bool ccw)
	{
		glm::vec3 edge1 = vertices[1].position - vertices[0].position;
		glm::vec3 edge2 = vertices[2].position - vertices[0].position;
		return glm::normalize(ccw ? glm::cross(edge1, edge2) : glm::cross(edge2, edge1));
	}

	/**
	* \brief Подсчет тангента
	* \details Для подсчета должны быть заданы корректный UV координаты
	* \return Тангент
	*/
	glm::vec3 Polygon::calculateUVTangent()
	{
		// Грани полигона (в виде векторов)
		glm::vec3 edge1 = vertices[1].position - vertices[0].position;
		glm::vec3 edge2 = vertices[2].position - vertices[0].position;

		// Дельта UV для каждой грани
		glm::vec2 deltaUV1 = vertices[1].uv - vertices[0].uv;
		glm::vec2 deltaUV2 = vertices[2].uv - vertices[0].uv;

		// Подсчет тангента
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		return glm::normalize(glm::vec3(
			f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
			f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
			f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)));
	}

	/**
	* \brief Подсчет нормали
	* \param vertices Ссылка на массив вершин
	* \param ccw Обход вершин против часовой стрелки
	* \return Нормаль
	*/
	glm::vec3 PolygonIndexed::calculateNormal(const std::vector<Vertex>& vertices, bool ccw)
	{
		//TODO: Оптимизировать. Не слишком оптимальное решение (излишнее копирование)

		Polygon p;

		for (auto index : this->indices) {
			p.vertices.push_back(vertices[index]);
		}

		return p.calculateNormal(ccw);
	}

	/**
	* \brief Подсчет тангента
	* \details Для подсчета должны быть заданы корректный UV координаты
	* \param vertices Ссылка на массив вершин
	* \return Тангент
	*/
	glm::vec3 PolygonIndexed::calculateUVTangent(const std::vector<Vertex>& vertices)
	{
		//TODO: Оптимизировать. Не слишком оптимальное решение (излишнее копирование)

		Polygon p;

		for (auto index : this->indices) {
			p.vertices.push_back(vertices[index]);
		}

		return p.calculateUVTangent();
	}

	/**
	* \brief Есть ли такой индекс
	* \param index Индекс
	* \return Да или нет
	*/
	bool PolygonIndexed::hasIndex(glm::uint32 index)
	{
		return std::find(this->indices.begin(), this->indices.end(), index) != this->indices.end();
	}
}
