#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <vector>

namespace ogl
{
	/**
	 * \brief Передаваемая на отрисовку вершина
	 */
	struct Vertex
	{
		glm::vec3 position;    // Положение в пространстве
		glm::vec3 color;       // Цвет
		glm::vec2 uv;          // Координаты текустуры
		glm::vec3 normal;      // Нормаль
		glm::vec3 tangent;     // Вектор касательной (тангент)
	};

	/**
	* \brief Полигон
	* \details Хранит в себе 3 вершины, может счиать нормали и тангенты
	*/
	struct Polygon
	{
		std::vector<Vertex> vertices;

		/**
		* \brief Подсчет нормали
		* \param ccw Обход вершин против часовой стрелки
		* \return Нормаль
		*/
		glm::vec3 calculateNormal(bool ccw = false);

		/**
		* \brief Подсчет тангента
		* \details Для подсчета должны быть заданы корректный UV координаты
		* \return Тангент
		*/
		glm::vec3 calculateUVTangent();
	};

	/**
	* \brief Полигон (индексированный)
	* \detais Не хранит фактических данных вершин, но хранит индексы каждой
	*/
	struct PolygonIndexed
	{
		std::vector<glm::uint32> indices;

		/**
		* \brief Подсчет нормали
		* \param vertices Ссылка на массив вершин
		* \param ccw Обход вершин против часовой стрелки
		* \return Нормаль
		*/
		glm::vec3 calculateNormal(const std::vector<Vertex>& vertices, bool ccw = false);

		/**
		* \brief Подсчет тангента
		* \details Для подсчета должны быть заданы корректный UV координаты
		* \param vertices Ссылка на массив вершин
		* \return Тангент
		*/
		glm::vec3 calculateUVTangent(const std::vector<Vertex>& vertices);
	};

	/**
	 * \brief Параметры материала
	 * \details Описывает насколько сильна будет та или иная составляющая при освещении белого цвета
	 */
	struct MaterialSettings
	{
		glm::vec3 ambientСolor;  // Фоновый цвет
		glm::vec3 diffuseColor;  // Цвет рассеянного света
		glm::vec3 specularColor; // Цвет бликов
		GLfloat shininess;       // Коэффициент размера бликового пятна (меньше - больше)
	};

	/**
	 * \brief Идентификаторы фрейм-буфера
	 */
	struct FrameBuffer
	{
		GLuint fboId;                     // ID
		GLuint depthStencilAttachmentId;  // Вложение глубины-трафарета (буфер глубины трафарета)
		GLuint colorAttachmentId;         // Вложение цвета (цветовой буфер, картинка)
		glm::ivec2 sizes;                 // Размеры (ширина высота)
		GLuint samples;                   // Кол-во семплов (используется при мульти-семплинге)
	};

	/**
	 * \brief Коэфициенты маппинга текстры
	 */
	struct TextureMapping
	{
		glm::vec2 offset;
		glm::vec2 origin;
		glm::vec2 scale;
		glm::mat2 rotation;
	};
}
