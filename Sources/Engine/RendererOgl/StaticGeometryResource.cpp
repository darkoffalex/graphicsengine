﻿#include "StaticGeometryResource.h"

namespace ogl
{
	/**
	 * \brief Проинициализирован ли GLEW
	 */
	extern bool _isGlewInitialised;

	/**
	* \brief Вычисление нормалей и тангентов для хранимой (или временно-хранимой) геометрии
	* \param calcNormals Вычислить норамали для каждой вершины (усредненные нормали смежных полигонов)
	* \param calcTangents Вычислить тангенты для каждой вершины (косательные к полигонам, завсящие от UV вектора)
	*/
	void StaticGeometryResource::calculateAdditionalVectorsForStored(bool calcNormals, bool calcTangents)
	{
		// Если не нужно ничего вычислять - выходим из функции
		if(!calcNormals && !calcTangents) return;

		// Если геометрия индексирована
		if (indexed_)
		{
			// Массив полигонов
			std::vector<Polygon> polygons;

			// Полигон и счет индексов
			Polygon poly = {};
			auto count = 0;

			// Пройтись по всем индексам и заполнить массив полигонов
			for (auto index : this->storedIndices_) {
				
				// Добавляем индекс
				poly.indices[count] = index;

				// Положение
				poly.vertices[count] = glm::vec3(
					this->storedVertices_[index].position.x, 
					this->storedVertices_[index].position.y, 
					this->storedVertices_[index].position.z);

				// Поскольку тангенты нужны для normal-mapping'а
				// используем UV координаты именно bump текстуры
				poly.uvs[count] = glm::vec2(
					this->storedVertices_[index].uv.x, 
					this->storedVertices_[index].uv.y);

				// Увеличить счетчик
				count++;

				// Если 3-ий индекс был добавлен, добавить заполненный полигон и сбросить счет
				if (count > 2) {
					polygons.push_back(poly);
					count = 0;
					poly = {};
				}
			}

			// Еще раз пройтись по всем индексам, чтобы найти в каких полигонах участвует вершина с конкретным индексом
			for (auto index : this->storedIndices_) {

				// Сумма нормалей всех полигонов в которых участвует вершина
				glm::vec3 normalSum(0.0f, 0.0f, 0.0f);

				// Сумма тангентов всех полигонов в которых участвует вершина
				glm::vec3 tangentSum(0.0f, 0.0f, 0.0f);

				// Пройтись по полигонам и сложить их нормали и тангенты
				for (auto polygonEntry : polygons) {
					if (polygonEntry.hasIndex(index))
					{
						glm::vec3 n = calcNormals ? polygonEntry.getNormal() : glm::vec3(0.0f, 0.0f, 0.0f);
						glm::vec3 t = calcTangents ? polygonEntry.getUVTangent() : glm::vec3(0.0f, 0.0f, 0.0f);
						normalSum += n;
						tangentSum += t;
					}
				}

				// Добавить нормаль вершине
				if(calcNormals){
					glm::vec3 normal = glm::normalize(normalSum);
					this->storedVertices_[index].normal = { normal.x,normal.y,normal.z };
				}

				// Добавить тангент к вершине
				if(calcTangents){
					glm::vec3 tangent = glm::normalize(tangentSum);
					this->storedVertices_[index].tangent = { tangent.x, tangent.y, tangent.z };
				}
			}
		}
		// Если геометрия не индексирована
		else
		{
			// Полигон и счет индексов
			Polygon poly = {};
			auto count = 0;

			// Пройтись по всем вершинам и на каждом полигоне посчитать нормали и тангеты для каждой вершины
			for (unsigned int i = 0; i < this->storedVertices_.size(); i++)
			{
				// Индекс
				poly.indices[count] = i;

				// Положение
				poly.vertices[count] = glm::vec3(
					this->storedVertices_[i].position.x, 
					this->storedVertices_[i].position.y, 
					this->storedVertices_[i].position.z);

				// UV координаты
				poly.uvs[count] = glm::vec2(
					this->storedVertices_[i].uv.x, 
					this->storedVertices_[i].uv.y);

				// Увеличить счетчик
				count++;

				// Если 3-ий индекс был добавлен, можно посчитать нормали
				// и тангенты для полигона (присвоив их его вершинам) и перейти к следующему
				if (count > 2) {
					
					glm::vec3 n = calcNormals ? poly.getNormal() : glm::vec3(0.0f, 0.0f, 0.0f);
					glm::vec3 t = calcTangents ? poly.getUVTangent() : glm::vec3(0.0f, 0.0f, 0.0f);

					for(unsigned int v = 0; v < 3; v++){
						this->storedVertices_[poly.indices[v]].normal = n;
						this->storedVertices_[poly.indices[v]].tangent = t;
					}

					count = 0;
					poly = {};
				}
			}
		}
	}

	/**
	* \brief Конструктор
	* \param vertices Массив вершин
	* \param indices Массив индексов
	* \param storeData Хранить дубликат данных в оперативной памяти
	* \param calcNormals Вычислить нормали
	* \param calcTangents Вычислить тангенты
	*/
	StaticGeometryResource::StaticGeometryResource(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, bool storeData, bool calcNormals, bool calcTangents)
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

		// Если нужно получить нормали
		if (calcNormals || calcTangents) this->calculateAdditionalVectorsForStored(calcNormals, calcTangents);

		// Регистрация VAO, VBO, EBO
		glGenVertexArrays(1, &vaoId_);
		glGenBuffers(1, &vboId_);

		if (indexed_) {
			glGenBuffers(1, &eboId_);
		}

		// Работаем с VAO
		glBindVertexArray(vaoId_);

		// Работаем с буфером вершин, помещаем в него данные
		glBindBuffer(GL_ARRAY_BUFFER, vboId_);
		glBufferData(GL_ARRAY_BUFFER, this->storedVertices_.size() * sizeof(Vertex), this->storedVertices_.data(), GL_STATIC_DRAW);

		// Работаем с буфером индексов, помещаем в него данные
		if (indexed_) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->storedIndices_.size() * sizeof(GLuint), this->storedIndices_.data(), GL_STATIC_DRAW);
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
	* \return Умный указатель на ресурс
	*/
	StaticGeometryResourcePtr MakeStaticGeometryResource(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, bool storeData, bool calcNormals, bool calcTangents)
	{
		return std::make_shared<StaticGeometryResource>(vertices, indices, storeData, calcNormals, calcTangents);
	}
}
