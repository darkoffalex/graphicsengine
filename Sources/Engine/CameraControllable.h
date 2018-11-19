#pragma once
#include "CameraBase.h"

/**
 * \brief Управляемая камера
 * \details Данный класс позволяет реализовать управление камерой при помощи кнопок и мыши
 */
class CameraControllable : public CameraBase
{
private:
	float speed_;        // Скорость перемещения
	float sensitivity_;  // Чувствительность

public:
	/**
	 * \brief Базовый вектор движения
	 * \details По задумке компоенты данного вектора при нажатии кнопок принимают значения: -1, 0, 1
	 */
	glm::vec3 movement;

	/**
	 * \brief Конструктор
	 * \param speed Скорость
	 * \param sensitivity Чувствительность
	 * \param aspectRatio Пропорции экрана
	 * \param zNear Ближняя грань отсечения
	 * \param zFar Дальняя гранб отсечения
	 * \param fov Угол обзора
	 */
	CameraControllable(float speed, float sensitivity, float aspectRatio, float zNear = 0.1f, float zFar = 100.0f, float fov = 45.0f);

	/**
	 * \brief Деструктор
	 */
	~CameraControllable() override = default;

	/**
	 * \brief Установить скорость перемещения
	 * \param speed Скорость (ед/с)
	 */
	void setSpeed(float speed);

	/**
	 * \brief Получить скорость перемещения
	 * \return Скорость (ед/с)
	 */
	float getSpeed() const;

	/**
	 * \brief Установить чувствительность
	 * \param sensitivity Чувствительность 
	 */
	void setSensitivity(float sensitivity);

	/**
	 * \brief Получить чувствительность
	 * \return Чувствительность 
	 */
	float getSensitivity() const;

	/**
	 * \brief Обновить положение в пространстве
	 * \details Приращение значений положения с учетом скорости и времени
	 * \param frameDeltaMs Время которое пршло за 1 кадр
	 */
	void updatePositions(float frameDeltaMs);
};

