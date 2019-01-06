#pragma once

#include <vector>
#include <string>

#include "../RendererOgl/StaticGeometryResource.h"

/**
 * \brief Положение вершины
 */
struct VertexPosition
{
	float x;
	float y;
	float z;
};

/**
 * \brief UV координаты текстуры
 */
struct VertexTexCoords
{
	float u;
	float v;
};

/**
 * \brief Нормаль вершины
 */
struct VertexNormal
{
	float x;
	float y;
	float z;
};

/**
 * \brief Набор индексов вершины всех типов (положение, uv, нормаль)
 */
struct VertexIndices
{
	unsigned position;
	unsigned texCoords;
	unsigned normal;

	/**
	 * \brief Оператор сравнения для проверки добавления вершины в список
	 * \param other Другая структура
	 * \return Равно или нет
	 */
	bool operator==(const VertexIndices& other) const
	{
		return this->position == other.position &&
			this->normal == other.normal &&
			this->texCoords == other.texCoords;
	}
};

/**
 * \brief Полигон (обычно треугольник)
 * \details Содержит масив наоборов индексов (для всех вершин, обычно 3)
 */
struct Poly
{
	std::vector<VertexIndices> vertexIndices;
};

/**
 * \brief Класс загрузчика .obj геометрии
 */
class ObjLoader
{

private:
	std::vector<VertexPosition> positions_;   // Положения вершин
	std::vector<VertexTexCoords> texCoords_;  // Текстурные координаты вершин
	std::vector<VertexNormal> normals_;       // Норали вершин
	std::vector<Poly> faces_;                 // Полигоны (индексы)

public:
	/**
	 * \brief Загрузка данных из .obj файла
	 * \param path Путь к файлу
	 * \param withoutUV Без UV координат
	 */
	void LoadFromFile(std::string path, bool withoutUV = false);

	/**
	 * \brief Очистка данных
	 */
	void Clear();


	/**
	 * \brief Создать ресурс статической геометрии для OpenGL рендерера
	 * \param inverseOrder Изменить порядок следования вершин в полигонах (против/по часовой стрелке)
	 * \param recalcNormals Пересчитать нормали (дорогая операция)
	 * \return Указатель на ресурс для OpenGL рендерера
	 */
	ogl::StaticGeometryResourcePtr MakeOglRendererResource(bool inverseOrder = false, bool recalcNormals = false);

	/**
	 * \brief Конструктор по умолчанию
	 */
	ObjLoader() = default;

	/**
	 * \brief Деструктор по умолчанию
	 */
	~ObjLoader() = default;

};