#pragma once
#include <Windows.h>

namespace ogl
{
	/**
	* \brief Создать OpenGL контекст для отрисовки на окне
	* \param hWnd Окно
	* \return Идентификатор контекста
	*/
	HGLRC CreateContext(HWND hWnd);

	/**
	* \brief Уничтожение OpenGL контекста
	* \param context Указатель на хендл контекста
	*/
	void DeleteContext(HGLRC* context);

	/**
	* \brief Управление вертикальной синхронизацией
	* \param sync Состояние
	*/
	void EnableVSync(bool sync);
}
