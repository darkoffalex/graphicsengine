#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>

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
	 * \brief Полигон. Используется в основном для вычисления нормалей
	 * \details Полезен при формировании буфера геометрии (для вычисления нормалей и тенгентов)
	 */
	struct Polygon
	{
		glm::vec3 vertices[3];
		GLuint indices[3];
		glm::vec2 uvs[3];

		/**
		 * \brief Вычисление нормали
		 * \param ccw Треугольник построен по часовой и против часовой стрелки
		 * \return Вектор нормали
		 */
		glm::vec3 getNormal(bool ccw = false) const;

		/**
		 * \brief Вычисление тангента
		 * \return Вектор тангента
		 * \details Полученный вектор зависит от UV координат и ориентации текстуры на полигоне
		 */
		glm::vec3 getUVTangent() const;

		/**
		 * \brief Есть ли такой индекс у полигона
		 * \param index Индекс
		 * \return Да или нет
		 */
		bool hasIndex(GLuint index) const;
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
	 * \details Общий размер структуры должен быть кратен 16, для этого используется выравнивание
	 */
	struct Std140TextureMapping
	{
		glm::vec2 offset;    // Сдвиг текстуры
		glm::vec2 origin;    // Центральная точка
		glm::vec4 scale;     // Масштабирование (vec2 + 8 байт выравнивания)
		glm::mat4 rotation;  // Поворот (используется только часть 2*2)

		Std140TextureMapping(glm::vec2 offsetIn, glm::vec2 originIn, glm::vec2 scaleIn, glm::mat4 rotationIn) :
			offset(offsetIn),
			origin(originIn),
			scale(scaleIn.x, scaleIn.y, 0.0f, 0.0f),
			rotation(rotationIn) {}
	};

	/**
	 * \brief Параметры точечного источника освещения
	 * \details Общий размер структуры должен быть кратен 16, для этого используется выравнивание
	 */
	struct Std140PointLightSettings
	{
		glm::vec4 position;  // Положение (vec3 + 4 байта выравнивания)
		glm::vec4 color;     // Цвет (vec3 + 4 байта выравнивания)
		GLfloat linear;      // Линейкный коэффициент затухания
		GLfloat quadratic;   // Квадратичный коэффициент затухания
		glm::vec2 padding;   // 8 байт выравнивания (для кратности ощего размера)

		Std140PointLightSettings(glm::vec3 positionIn, glm::vec3 colorIn, GLfloat linearIn, GLfloat quadraticIn) :
			position({ positionIn.x,positionIn.y,positionIn.z,0.0f }),
			color({ colorIn.r,colorIn.g,colorIn.b,0.0f }),
			linear(linearIn),
			quadratic(quadraticIn) {}
	};
}
