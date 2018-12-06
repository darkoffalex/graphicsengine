#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <gl/glew.h>

namespace ogl
{
	/**
	 * \brief Тип источника
	 */
	enum LightType
	{
		POINT_LIGHT,        // Точечный источник (распространяется во всех направлениях)
		DIRECTIONAL_LIGHT,  // Нправленный-параллельный (бесконечно отдаленный, например солнце)
		SPOT_LIGHT          // Направленный из одной точки (фанарик, прожектор)
	};

	/**
	 * \brief Источник света
	 */
	class Light
	{
	private:
		LightType type_;         // Тип источника

	public:
		bool render;              // Отображать ли источник визуально
		bool shadows;             // Создает тени от объектов (ВНИМАНИЕ! такой может быть только один)
		float renderScale;        // Размер отображаемого объекта
		glm::vec3 position;       // Положение в пространстве (важно для POINT_LIGHT и SPOT_LIGHT)
		glm::vec3 rotation;       // Вращение (важно для DIRECTIONAL_LIGHT и для SPOT_LIGHT)
		glm::vec3 color;          // Цвет света
		GLfloat cutOffAngle;      // Внутренний угол отсечения для фонарика-прожектора
		GLfloat cutOffOuterAngle; // Внешний угол осечения для фонарика прожектора

		// Затухание
		// Эти коэффициенты определяют силу затухания света в зависимости от расстояния
		// Используемая в шейдере формула по умолчанию : 1 / 1 + (linear * дисанция) + (quadratic * дистанция^2)
		struct {
			glm::float32 linear;
			glm::float32 quadratic;
		} attenuation;

		/**
		 * \brief Конструктор
		 * \param type Тип источника
		 * \param vPosition Положение
		 * \param vRotation Вращение
		 * \param vColor Цвет
		 */
		Light(LightType type, glm::vec3 vPosition = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 vRotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 vColor = glm::vec3(1.0f, 1.0f, 1.0f));

		/**
		 * \brief Получить матрицу вращения
		 * \return Матрица
		 */
		glm::mat4 getRotationMatrix4x4() const;

		/**
		 * \brief Получить матрицу сдвига
		 * \return Матрица
		 */
		glm::mat4 getTranslationMatrix() const;

		/**
		 * \brief Получить матрицу модели
		 * \param additionalScale Дополнительный множитель размера
		 * \return Матрица
		 */
		glm::mat4 getModelMatrix(float additionalScale = 1.0f) const;

		/**
		 * \brief Получить матрицу вида для источника света
		 * \return Матрица
		 */
		glm::mat4 getViewMatrix() const;

		/**
		 * \brief Получить матрицу проекции с точки зрения источника
		 * \return Матрица
		 */
		glm::mat4 getProjectionMatrix(GLfloat zNear = 0.1f, GLfloat zFar = 7.0f) const;

		/**
		 * \brief Получить тип
		 * \return Идентификатор типа
		 */
		LightType getType() const;

		/**
		 * \brief Получить направление
		 * \return Направление
		 */
		glm::vec3 getDirection() const;

		/**
		 * \brief Деструктор
		 */
		~Light() = default;
	};

	/**
	 * \brief Умный указатель на источник
	 * \details Этот указатель возвращает метод добавления источников в рендерере
	 */
	typedef std::shared_ptr<Light> LightPtr;
}
