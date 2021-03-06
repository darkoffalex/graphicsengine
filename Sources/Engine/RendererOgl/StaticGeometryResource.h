﻿#pragma once

#include <vector>
#include <memory>

#include "Types.h"

namespace ogl
{
	/**
	 * \brief Ресурс статического меша
	 * \details Хранит информацию о загруженных вершинах. Может быть многократно использован, но не копируется
	 */
	class StaticGeometryResource
	{
	private:
		GLuint vaoId_;               // ID объекта массива вершин (VAO)
		GLuint vboId_;               // ID объекта вершинного буфера (VBO)
		GLuint eboId_;               // ID объекта индексного/элементного буфера (EBO)

		GLuint vertexCount_;         // Кол-во вершин
		GLuint indexCount_;          // Кол-во индексов

		bool indexed_;               // Рисовать как индексированную геометрию

		// На случай, если нужен будет доступ к уже загруженой в видео-память геометрии
		// дубликат массива вершин и индексов может храниться в следующих массивах

		std::vector<Vertex> storedVertices_; // Хранимые вершины
		std::vector<GLuint> storedIndices_;  // Хранимые индексы

		/**
		* \brief Запрет копирования через инициализацию
		* \param other Ссылка на копируемый объекта
		*/
		StaticGeometryResource(const StaticGeometryResource& other) = delete;

		/**
		* \brief Запрект копирования через присваивание
		* \param other Ссылка на копируемый объекта
		*/
		void StaticGeometryResource::operator=(const StaticGeometryResource& other) = delete;

		/**
		 * \brief Пересчитать нормали и тенгенты вершин индексированной геометрии
		 * \param vertices Указатель на массив вершин
		 * \param indices Индексы
		 * \param calcTangents Считать так же и тангенты
		 * \param ccw Обход вершин против часовой стрелки
		 */
		static void recalcNormalsForIndexed(std::vector<Vertex>* vertices, const std::vector<glm::uint32>& indices, bool calcTangents = false, bool ccw = false);

		/**
		 * \brief Пересчитать нормали и тангенты вершин не индексированной геометрии
		 * \param vertices Вершины
		 * \param calcTangents Тангенты
		 * \param ccw Обход вершин против часовой стрелки
		 */
		static void recalcNormalsForNonIndexed(std::vector<Vertex>* vertices, bool calcTangents = false, bool ccw = false);

		/**
		 * \brief Получить массив пересечений двух массивов
		 * \param a Первый массив
		 * \param b Второй массив
		 * \return Массив пересечений
		 */
		static std::vector<glm::uint32> getIntersections(std::vector<glm::uint32> a, std::vector<glm::uint32> b);

		/**
		* \brief Построить геометрию с учетом смежных граней
		* \details Метод может добавлять новые вершины в массив, следует это учитывать
		* \param vertices Вершины
		* \return Массив индексов
		*/
		static std::vector<glm::uint32> buildAdjacency(std::vector<Vertex>* vertices, const std::vector<glm::uint32>& indices);

	public:

		/**
		 * \brief Конструктор
		 * \param vertices Массив вершин
		 * \param indices Массив индексов
		 * \param storeData Хранить дубликат данных в оперативной памяти
		 * \param calcNormals Вычислить нормали
		 * \param calcTangents Вычислить тангенты
		 * \param adjacency Построить геометрию со смежностями
		 */
		StaticGeometryResource(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices = {}, bool storeData = false, bool calcNormals = false, bool calcTangents = false, bool adjacency = false);

		/**
		 * \brief Деструктор
		 * \details Уничтожает OpenGL объекты (VAO, VBO, EBO)
		 */
		~StaticGeometryResource();

		/**
		 * \brief Используются ли индексы
		 * \return Да или нет
		 */
		bool IsIndexed() const;

		/**
		 * \brief Получить кол-во вершин
		 * \return Число вершин
		 */
		GLuint getVertexCount() const;

		/**
		 * \brief Получить кол-во индексов
		 * \return Число индексов
		 */
		GLuint getIndexCount() const;

		/**
		 * \brief Получить ID OpenGL объекта VAO (Vertex array object)
		 * \return Число-идентификатор
		 */
		GLuint getVaoId() const;

		/**
		 * \brief Получить ID OpenGL объекта VBO (Vertex buffer object)
		 * \return Число-идентификатор
		 */
		GLuint getVboId() const;

		/**
		 * \brief Получить ID OpenGL объекта EBO (Element buffer object)
		 * \return Число-идентификатор
		 */
		GLuint getEboId() const;

		/**
		 * \brief Получить массив хранимых вершин
		 * \return Константная ссылка на массив
		 */
		const std::vector<Vertex>& getStoredVertices() const;
	};

	/**
	 * \brief Тип для умного указателя на ресурс
	 * \details Ресурс лучше не создавать как объект, предпочтительнее использовать функцию создания ресурса и данный тип
	 */
	typedef std::shared_ptr<StaticGeometryResource> StaticGeometryResourcePtr;

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
	StaticGeometryResourcePtr MakeStaticGeometryResource(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, bool storeData = false, bool calcNormals = false, bool calcTangents = false, bool adjacency = false);
}