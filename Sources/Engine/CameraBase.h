#pragma once

#include <glm/glm.hpp>

/**
 * \brief Тип пространства
 * \details Некоторые API используют право-ручное а некоторые лево-ручное пространство, от этого завсят матрицы
 */
enum CoordSpaceType{
	LEFT_HANDED,
	RIGHT_HANDED
};

/**
 * \brief Базовая камера
 * \details Позволяет получить матрицы проекции и вида
 */
class CameraBase
{
private:
	float zNear_;          // Ближняя грань отсечения
	float zFar_;           // Дальняя грань отсечения
	float aspectRatio_;    // Пропорции экрана

public:
	glm::vec3 position;    // Положение в пространстве
	glm::vec3 rotation;    // Вращение в пространстве
	float FOV;             // Угол обзора

	/**
	 * \brief Конструктор
	 * \param aspectRatio Пропорции экрана
	 * \param zNear Ближняя грань отсечения
	 * \param zFar Дальняя грань отсечения
	 * \param fov Угол обзора
	 */
	CameraBase(float aspectRatio, float zNear = 0.1f, float zFar = 100.0f, float fov = 45.0f);

	/**
	 * \brief Деструктор
	 */
	virtual ~CameraBase() = default;

	/**
	 * \brief Установка пропорций экрана
	 * \param aspectRatio Пропорции (отношение ширины к длине)
	 */
	void setAspectRatio(float aspectRatio);

	/**
	 * \brief Установить ближнюю грань (ближе этого расстояния все отбрасывается)
	 * \param zNear Ближняя грань отсечения
	 */
	void setZNear(float zNear);

	/**
	 * \brief Установить дальнюю грань (дальше этого расстояния все отбрасывается)
	 * \param zFar Дальняя грань отсечения
	 */
	void setZFar(float zFar);

	/**
	 * \brief Получить матрицу проекции
	 * \param coordSpaceType Тип пространства (право-ручное или лево-ручное)
	 * \details При помощи данной матрицы точки примитивов из 3Д-пространства проецируются на 2Д-экран (для дальнейшей растеризации)
	 * \return Матрица
	 */
	glm::mat4 getProjectionMatrix(CoordSpaceType coordSpaceType = CoordSpaceType::RIGHT_HANDED) const;

	/**
	 * \brief Получить направление камеры (куда смотрит камера)
	 * \return Вектор
	 */
	glm::vec3 getDirection() const;

	/**
	 * \brief Получить вектор-вправо от камеры
	 * \return Вектор
	 */
	glm::vec3 getRight() const;

	/**
	 * \brief Получить матрицу вида
	 * \param coordSpaceType Тип пространства (право-ручное или лево-ручное)
	 * \details При помощи данной матрицы координаты всех точек меняются так, как если бы центр мира был там, где камера
	 * \return Матрица
	 */
	glm::mat4 getViewMatrix(CoordSpaceType coordSpaceType = CoordSpaceType::RIGHT_HANDED) const;
};