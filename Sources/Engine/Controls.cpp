#include "Controls.h"
#include "CameraControllable.h"

// Камера
extern CameraControllable * _pCamera;

// Положение мыши
static glm::ivec2 _mousePositions;


/**
* \brief Обработка событий системы
* \param hWnd Хендл окна
* \param message Сообщение
* \param wParam Параметр сообщения
* \param lParam Параметр сообщения
* \return Код выполнения
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		// При нажатии кнопок (WASD)
		switch (wParam)
		{
		case 0x57: // S
			_pCamera->movement.z = 1.0f;
			break;
		case 0x41: // D
			_pCamera->movement.x = 1.0f;
			break;
		case 0x53: // W
			_pCamera->movement.z = -1.0f;
			break;
		case 0x44: // A
			_pCamera->movement.x = -1.0f;
			break;
		case VK_SPACE:
			_pCamera->movement.y = 1.0f;
			break;
		case 0x43: // C
			_pCamera->movement.y = -1.0f;
			break;
		default:
			break;
		}
		break;

	case WM_KEYUP:
		// При отжатии кнопок (WASD)
		switch (wParam)
		{
		case 0x57: // S
			_pCamera->movement.z = 0.0f;
			break;
		case 0x41: // D
			_pCamera->movement.x = 0.0f;
			break;
		case 0x53: // W
			_pCamera->movement.z = 0.0f;
			break;
		case 0x44: // A
			_pCamera->movement.x = 0.0f;
			break;
		case VK_SPACE:
			_pCamera->movement.y = 0.0f;
			break;
		case 0x43: // C
			_pCamera->movement.y = 0.0f;
			break;
		default:
			break;
		}
		break;

	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
		// При нажатии любой кнопки мыши
		_mousePositions.x = static_cast<int>(LOWORD(lParam));
		_mousePositions.y = static_cast<int>(HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		// При движении мыши
		// Если зажата левая кнопка мыши
		if (wParam & MK_LBUTTON) {
			int posx = static_cast<int>(LOWORD(lParam));
			int posy = static_cast<int>(HIWORD(lParam));
			_pCamera->rotation.x += static_cast<float>(_mousePositions.y - posy) * _pCamera->getSensitivity();
			_pCamera->rotation.y += static_cast<float>(_mousePositions.x - posx) * _pCamera->getSensitivity();
			_mousePositions.x = posx;
			_mousePositions.y = posy;
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}