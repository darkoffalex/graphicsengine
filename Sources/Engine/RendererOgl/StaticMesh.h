#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "StaticMeshPart.h"

namespace ogl
{
	/**
	 * \brief Статический меш. Используется для отрисовки не анимированной геометрии
	 * \details Состояит из частей (как минимум одной), обладает положенем в пространстве
	 */
	class StaticMesh
	{
	private:
		std::vector<StaticMeshPart> parts_;  // Части

	public:
		bool isRendering;   // Рендерится ли меш

		glm::vec3 origin;   // Локальный центр объекта
		glm::vec3 rotation; // Поворот по всем осям
		glm::vec3 position; // Положение в пространстве
		glm::vec3 scale;    // Масштаб

		/**
		 * \brief Конструктор
		 * \param part Первая часть статического меша
		 */
		StaticMesh(StaticMeshPart part);

		/**
		 * \brief Конструктор
		 * \param parts Части статического меша (массив)
		 */
		StaticMesh(const std::vector<StaticMeshPart>& parts);

		/**
		 * \brief Деструктор
		 */
		~StaticMesh() = default;

		/**
		 * \brief Получить части
		 * \return Массив частей меша
		 */
		std::vector<StaticMeshPart>& getParts();

		/**
		 * \brief Получить матрицу вращения 4*4
		 * \return Матрица
		 */
		glm::mat4 getRotationMatrix4x4() const;

		/**
		 * \brief Получить матрицу вращения 3*3
		 * \return Матрица
		 */
		glm::mat3 getRotationMatrix3x3() const;

		/**
		 * \brief Получить матрицу сдвига
		 * \return Матрица
		 */
		glm::mat4 getTranslationMatrix() const;

		/**
		 * \brief Получить матрицу масштаба
		 * \return Матрица
		 */
		glm::mat4 getScaleMatrix() const;

		/**
		 * \brief Получить матрицу модели
		 * \details Результат перемножения всех матриц с учетом локального центра
		 * \return Матрица
		 */
		glm::mat4 getModelMatrix() const;
	};

	/**
	 * \brief Умный указатель на меш
	 * \details Этот указатель возвращает метод добавления геометрии в рендерере
	 */
	typedef std::shared_ptr<StaticMesh> StaticMeshPtr;
}
