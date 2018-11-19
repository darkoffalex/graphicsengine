#pragma once

#include <Windows.h>


/**
 * \brief Обработка событий системы
 * \param hWnd Хендл окна
 * \param message Сообщение
 * \param wParam Параметр сообщения
 * \param lParam Параметр сообщения
 * \return Код выполнения
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);