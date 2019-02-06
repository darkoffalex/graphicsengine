#include "StaticGeometryResource.h"
#include <map>
#include <algorithm>
#include <iterator>
#include <iostream>

namespace ogl
{
	/**
	 * \brief Проинициализирован ли GLEW
	 */
	extern bool _isGlewInitialised;

	/**
	* \brief Пересчитать нормали и тенгенты вершин
	* \param vertices Указатель на массив вершин
	* \param indices Индексы
	* \param calcTangents Считать так же и тангенты
	* \param ccw Обход вершин против часовой стрелки
	*/
	void StaticGeometryResource::recalcNormalsForIndexed(std::vector<Vertex>* vertices, const std::vector<glm::uint32>& indices, bool calcTangents, bool ccw)
	{
		// Ассоциативные массив (нормали и ангенты)
		// Каждый индекс будет связан суммой нормалей (и тангентов) полигонов в которых участвует вершина под этим индексом
		std::map<glm::uint32, glm::vec3> normals;
		std::map<glm::uint32, glm::vec3> tangents;

		// Временный полигон
		PolygonIndexed polygon;

		// Пройтись по всем индексам
		// Для формирования массива связи каждой вершины и суммы нормалей ее полигонов
		for (unsigned int i = 0; i < indices.size(); i++)
		{
			// Заполнять полигон
			polygon.indices.push_back(indices[i]);

			// Когда 3 вершины были внесены (полигон построен)
			if ((i + 1) % 3 == 0) {

				// Пройтись по этим вершинам (их индексам)
				for (auto index : polygon.indices)
				{
					// Если в ассоциативном массие нету такого ключа, добавить
					// новую нормаль (и тангент, если надо) текущего полигона
					if (normals.find(index) == normals.end())
					{
						normals[index] = polygon.calculateNormal(*vertices);
						if (calcTangents) tangents[index] = polygon.calculateNormal(*vertices);
					}
					// Если в ассоциативном массие есть такой ключ, добавить
					// к существующей нормали нормаль (и тангент, если надо) текущего полигона
					else
					{
						normals[index] += polygon.calculateNormal(*vertices);
						if (calcTangents) tangents[index] += polygon.calculateNormal(*vertices);
					}
				}

				// Cбросить полигон
				polygon.indices.clear();
			}
		}

		// Пройтись по ассоциативному массиву индексов для установки значений вершинам в массиве
		std::map<glm::uint32, glm::vec3>::iterator it;
		for (it = normals.begin(); it != normals.end(); ++it)
		{
			// Индекс вершины
			glm::uint32 index = it->first;
			// Сумма нормалей
			glm::vec3 normalSum = it->second;

			// Установить новое значение нормали и тангента (если нужно)
			(*vertices)[index].normal = glm::normalize(normalSum);
			if(calcTangents) (*vertices)[index].tangent = glm::normalize(tangents[index]);
		}
	}

	/**
	* \brief Пересчитать нормали и тангенты вершин не индексированной геометрии
	* \param vertices Вершины
	* \param calcTangents Тангенты
	* \param ccw Обход вершин против часовой стрелки
	*/
	void StaticGeometryResource::recalcNormalsForNonIndexed(std::vector<Vertex>* vertices, bool calcTangents, bool ccw)
	{
		// Временный полигон
		Polygon polygon;
		PolygonIndexed polygonIndexed;

		// Пройтись по всем индексам
		// Для формирования массива связи каждой вершины и полигонов образованных ею
		// Т.е выясняем в каких полигонах участвует вершина с конкретным индексом
		for (unsigned int i = 0; i < vertices->size(); i++)
		{
			// Заполнять полигон (включая индексы)
			polygon.vertices.push_back((*vertices)[i]);
			polygonIndexed.indices.push_back(i);

			// Когда 3 вершины были внесены (полигон построен)
			if ((i + 1) % 3 == 0) {

				// Посчитать нормаль и тангент полигона (если нужно)
				glm::vec3 normal = polygon.calculateNormal(ccw);
				glm::vec3 tangent = calcTangents ? polygon.calculateUVTangent() : glm::vec3(0.0f, 0.0f, 0.0f);

				// Пройти по индексам вершин полигона - обновить данные в массиве
				for(auto index : polygonIndexed.indices){
					(*vertices)[index].normal = normal;
					(*vertices)[index].tangent = tangent;
				}

				// Cбросить полигон
				polygon.vertices.clear();
				polygonIndexed.indices.clear();
			}
		}
	}

	/**
	* \brief Получить массив пересечений двух массивов
	* \param a Первый массив
	* \param b Второй массив
	* \return Массив пересечений
	*/
	std::vector<glm::uint32> StaticGeometryResource::getIntersections(std::vector<glm::uint32> a, std::vector<glm::uint32> b)
	{
		std::vector<glm::uint32> result;
		std::sort(a.begin(), a.end());
		std::sort(b.begin(), b.end());
		std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), back_inserter(result));
		return result;
	}

	/**
	* \brief Построить геометрию с учетом смежных граней
	* \details Метод может добавлять новые вершины в массив, следует это учитывать
	* \param vertices Вершины
	* \return Массив индексов
	*/
	std::vector<glm::uint32> StaticGeometryResource::buildAdjacency(std::vector<Vertex>* vertices, const std::vector<glm::uint32>& indices)
	{
		// Результирующие индексы с учетом смежностей
		std::vector<glm::uint32> resultIndices;
		// Список полигонов (формируется при проходе по индексам вершин)
		std::vector<PolygonIndexed> polygons;
		// Список пар, индекс вершины - соседних вершин
		std::map<glm::uint32, std::vector<glm::uint32>> neighbors;

		// Временный полигон
		PolygonIndexed polygon;

		// Пройтись по всем индексам
		// Для формирования массива связи каждой вершины и ее соседей
		for (unsigned int i = 0; i < indices.size(); i++)
		{
			// Заполнять полигон
			polygon.indices.push_back(indices[i]);

			// Когда 3 вершины были внесены (полигон построен)
			if ((i + 1) % 3 == 0) {

				// Пройтись по этим вершинам (их индексам)
				for (auto index : polygon.indices)
				{
					// Для каждой вершины снова пройтись по всем вершинам
					for (auto neighborIndex : polygon.indices)
					{
						// Только для соседних вершин (игнорировать текущую)
						if (neighborIndex != index)
						{
							// Добавить индексы соседних вершин
							if (neighbors.find(index) == neighbors.end()) neighbors[index] = { neighborIndex };
							else neighbors[index].push_back(neighborIndex);
						}
					}
				}

				// Добавить полигон в список полигонов
				polygons.push_back(polygon);

				// Cбросить полигон
				polygon.indices.clear();
			}
		}

		// Пройтись по всем полигонам
		for (auto polygonEntry : polygons)
		{
			// Индексы нового полигона с учетом смежных
			std::vector<glm::uint32> adjacent = {
				polygonEntry.indices[0],
				0,
				polygonEntry.indices[1],
				0,
				polygonEntry.indices[2],
				0
			};

			// Пройти по всем вершинам
			for (glm::uint32 i = 0; i < polygonEntry.indices.size(); i++)
			{
				// Текущая вершина и следуюшая
				glm::uint32 i0 = polygonEntry.indices[i];
				glm::uint32 i1 = polygonEntry.indices[(i + 1) % 3];

				// Найти пересечения соседних вершин
				auto intersections = getIntersections(neighbors[i0], neighbors[i1]);

				// Индекс смежного полигона для ребра i0-i1;
				glm::i32 intersection = -1;
				for (auto intersectionEntry : intersections)
				{
					// Если это не вершина этого полигона - значит это оно
					if (!polygonEntry.hasIndex(intersectionEntry))
					{
						intersection = static_cast<glm::i32>(intersectionEntry);
						break;
					}
				}

				// Если индекс смежного полигона получен - записать его
				if (intersection > -1) {
					adjacent[2 * i + 1] = static_cast<glm::uint32>(intersection);
				}
				// Если не получен - создать фантомную вершину и записать ее индекс
				else {
					Vertex v = {};
					v.phantom = 1;
					vertices->push_back(v);
					adjacent[2 * i + 1] = static_cast<glm::uint32>(vertices->size() - 1);
				}
			}

			// Добавить к общему результату
			resultIndices.insert(resultIndices.end(), adjacent.begin(), adjacent.end());
		}

		return resultIndices;
	}

	/**
	* \brief Конструктор
	* \param vertices Массив вершин
	* \param indices Массив индексов
	* \param storeData Хранить дубликат данных в оперативной памяти
	* \param calcNormals Вычислить нормали
	* \param calcTangents Вычислить тангенты
	* \param adjacency Построить геометрию со смежностями
	*/
	StaticGeometryResource::StaticGeometryResource(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, bool storeData, bool calcNormals, bool calcTangents, bool adjacency)
	{
		// Инициализация GLEW
		if (!_isGlewInitialised) {
			glewExperimental = GL_TRUE;
			_isGlewInitialised = glewInit() == GLEW_OK;
		}

		if (!_isGlewInitialised) {
			throw std::runtime_error("OpenGL:StaticGeometryResource: Glew is not initialised");
		}

		// Скопировать в хранимые массивы
		this->storedVertices_ = vertices;
		this->storedIndices_ = indices;

		// Кол-во индексов и вершин
		this->vertexCount_ = this->storedVertices_.size();
		this->indexCount_ = this->storedIndices_.size();

		// Используются ли индексы
		this->indexed_ = indices.size() > 0;

		// Если нужно посчитать нормали
		if (calcNormals || calcTangents) {
			// Для индексированной геометрии
			if(this->indexed_) StaticGeometryResource::recalcNormalsForIndexed(&(this->storedVertices_), this->storedIndices_, calcTangents);
			// Для не индексированной геометрии
			else StaticGeometryResource::recalcNormalsForNonIndexed(&(this->storedVertices_), calcTangents);
		}

		// Массив индексов геометрии со смежностями
		std::vector<GLuint> adjacentIndices;

		// Регистрация VAO, VBO, EBO
		glGenVertexArrays(1, &vaoId_);
		glGenBuffers(1, &vboId_);

		if (indexed_) {
			glGenBuffers(1, &eboId_);
		}

		// Работаем с VAO
		glBindVertexArray(vaoId_);

		// Если нужно строить смежные полигоны
		if(adjacency){
			adjacentIndices = StaticGeometryResource::buildAdjacency(&(this->storedVertices_), this->storedIndices_);
			this->indexCount_ = adjacentIndices.size();
			this->vertexCount_ = this->storedVertices_.size();
		}

		// Работаем с буфером вершин, помещаем в него данные
		glBindBuffer(GL_ARRAY_BUFFER, vboId_);
		glBufferData(GL_ARRAY_BUFFER, this->storedVertices_.size() * sizeof(Vertex), this->storedVertices_.data(), GL_STATIC_DRAW);

		// Работаем с буфером индексов, помещаем в него данные
		if (indexed_) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (adjacency ? adjacentIndices.size() : this->storedIndices_.size()) * sizeof(GLuint), adjacency ? adjacentIndices.data() : storedIndices_.data(), GL_STATIC_DRAW);
		}

		// Если хранить в опертивной памяти данные вершин не нужно - очистить
		if (!storeData) {
			this->storedVertices_.clear();
			this->storedVertices_.shrink_to_fit();

			this->storedIndices_.clear();
			this->storedIndices_.shrink_to_fit();
		}

		// Пояснения шейдеру как понимать данные из этого VBO (буфера вершин)

		// Аттрибут "положение"
		glVertexAttribPointer(
			0,                           // Номер положения (location у шейдера) 
			3,                           // Размер (сколько значений конктерного типа приходится на один атрибут) 
			GL_FLOAT,                    // Конкретный тип одного значения 
			GL_FALSE,                    // Не нормализовать 
			sizeof(Vertex),              // Размер шага (размер одной вершины) 
			reinterpret_cast<GLvoid*>(offsetof(Vertex, position)) // Сдвиг (с какого места в блоке данных начинается нужная часть)
		);

		// Аттрибут "цвет"
		glVertexAttribPointer(
			1,                           // Номер положения (location у шейдера) 
			3,                           // Размер (сколько значений конктерного типа приходится на один атрибут) 
			GL_FLOAT,                    // Конкретный тип одного значения 
			GL_FALSE,                    // Не нормализовать 
			sizeof(Vertex),              // Размер шага (размер одной вершины) 
			reinterpret_cast<GLvoid*>(offsetof(Vertex, color)) // Сдвиг (с какого места в блоке данных начинается нужная часть)
		);

		// Аттрибут "текстурные координаты для diffuse текстуры"
		glVertexAttribPointer(
			2,                           // Номер положения (location у шейдера) 
			2,                           // Размер (сколько значений конктерного типа приходится на один атрибут) 
			GL_FLOAT,                    // Конкретный тип одного значения 
			GL_FALSE,                    // Не нормализовать 
			sizeof(Vertex),              // Размер шага (размер одной вершины) 
			reinterpret_cast<GLvoid*>(offsetof(Vertex, uv)) // Сдвиг (с какого места в блоке данных начинается нужная часть)
		);

		// Аттрибут "текстурные координаты"
		glVertexAttribPointer(
			3,                           // Номер положения (location у шейдера) 
			3,                           // Размер (сколько значений конктерного типа приходится на один атрибут) 
			GL_FLOAT,                    // Конкретный тип одного значения 
			GL_FALSE,                    // Не нормализовать 
			sizeof(Vertex),              // Размер шага (размер одной вершины) 
			reinterpret_cast<GLvoid*>(offsetof(Vertex, normal)) // Сдвиг (с какого места в блоке данных начинается нужная часть)
		);

		// Аттрибут "текстурные координаты"
		glVertexAttribPointer(
			4,                           // Номер положения (location у шейдера) 
			3,                           // Размер (сколько значений конктерного типа приходится на один атрибут) 
			GL_FLOAT,                    // Конкретный тип одного значения 
			GL_FALSE,                    // Не нормализовать 
			sizeof(Vertex),              // Размер шага (размер одной вершины) 
			reinterpret_cast<GLvoid*>(offsetof(Vertex, tangent)) // Сдвиг (с какого места в блоке данных начинается нужная часть)
		);

		// Включить аттрибуты
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);

		// Завершаем работу с VAO
		glBindVertexArray(0);
	}

	/**
	* \brief Деструктор
	* \details Уничтожает OpenGL объекты (VAO, VBO, EBO)
	*/
	StaticGeometryResource::~StaticGeometryResource()
	{
		if (this->vboId_) glDeleteBuffers(1, &vboId_);
		if (this->eboId_) glDeleteBuffers(1, &eboId_);
		if (this->vaoId_) glDeleteBuffers(1, &vaoId_);
	}

	/**
	* \brief Используются ли индексы
	* \return Да или нет
	*/
	bool StaticGeometryResource::IsIndexed() const
	{
		return this->indexed_;
	}

	/**
	* \brief Получить кол-во вершин
	* \return Число вершин
	*/
	GLuint StaticGeometryResource::getVertexCount() const
	{
		return this->vertexCount_;
	}

	/**
	* \brief Получить кол-во индексов
	* \return Число индексов
	*/
	GLuint StaticGeometryResource::getIndexCount() const
	{
		return this->indexCount_;
	}

	/**
	* \brief Получить ID OpenGL объекта VAO (Vertex array object)
	* \return Число-идентификатор
	*/
	GLuint StaticGeometryResource::getVaoId() const
	{
		return this->vaoId_;
	}

	/**
	* \brief Получить ID OpenGL объекта VBO (Vertex buffer object)
	* \return Число-идентификатор
	*/
	GLuint StaticGeometryResource::getVboId() const
	{
		return this->vboId_;
	}

	/**
	* \brief Получить ID OpenGL объекта EBO (Element buffer object)
	* \return Число-идентификатор
	*/
	GLuint StaticGeometryResource::getEboId() const
	{
		return this->eboId_;
	}

	/**
	* \brief Получить массив хранимых вершин
	* \return Константная ссылка на массив
	*/
	const std::vector<Vertex>& StaticGeometryResource::getStoredVertices() const
	{
		return this->storedVertices_;
	}

	/**
	* \brief Создание ресурса
	* \param vertices Вершины
	* \param indices Индексы
	* \param storeData Хранить дубликат данных в оперативной памяти
	* \param calcNormals Вычислить нормали
	* \param calcTangents Вычислить тангенты
	* \param adjacency Построить геометрию со смежностями
	* \return Умный указатель на ресурс
	*/
	StaticGeometryResourcePtr MakeStaticGeometryResource(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, bool storeData, bool calcNormals, bool calcTangents, bool adjacency)
	{
		return std::make_shared<StaticGeometryResource>(vertices, indices, storeData, calcNormals, calcTangents, adjacency);
	}
}
