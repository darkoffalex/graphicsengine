﻿#pragma once

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
}
