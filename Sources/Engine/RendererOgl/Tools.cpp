#include "Tools.h"
#include <gl/glew.h>
#include <stdexcept>

namespace ogl
{
	/**
	 * \brief Проинициализирован ли GLEW
	 */
	bool _isGlewInitialised = false;

	/**
	* \brief Создать OpenGL контекст для отрисовки на окне
	* \param hWnd Окно
	* \return Идентификатор контекста
	*/
	HGLRC CreateContext(HWND hWnd)
	{
		// Описываем необходимый формат пикселей
		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		// Контекст устройства связанный с окном
		HDC hdc = GetDC(hWnd);

		// Номер формата пикселей
		int pixelFormatID = ChoosePixelFormat(hdc, &pfd);

		if (!pixelFormatID) {
			throw std::runtime_error("ERROR: Can't find suitable pixel format");
		}

		// Найти наиболее подходящее описание формата
		// Если в этом описании чего-то не хватает до требуемого формата - исключение
		PIXELFORMATDESCRIPTOR bestMatchPfd;
		DescribePixelFormat(hdc, pixelFormatID, sizeof(PIXELFORMATDESCRIPTOR), &bestMatchPfd);

		if (bestMatchPfd.cDepthBits < pfd.cDepthBits) {
			throw std::runtime_error("ERROR: Can't find suitable pixel format");
		}

		if (!SetPixelFormat(hdc, pixelFormatID, &pfd)) {
			throw std::runtime_error("ERROR: Can't set required pixel format");
		}

		// OpenGL контекст
		HGLRC context = wglCreateContext(hdc);

		// Сделать текущим
		wglMakeCurrent(hdc, context);
		return context;
	}

	/**
	* \brief Уничтожение OpenGL контекста
	* \param context Указатель на хендл контекста
	*/
	void DeleteContext(HGLRC* context)
	{
		if (context)
		{
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(*context);
			*context = nullptr;
		}
	}

	/**
	* \brief Управление вертикальной синхронизацией
	* \param sync Состояние
	*/
	void EnableVSync(bool sync)
	{
		// Расширение (строка со списком)
		const char *extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

		// Если есть нужное расширение
		if (strstr(extensions, "WGL_EXT_swap_control"))
		{
			// Объявляем тип указателя функции управления вертикальной синхронизацией
			typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
			// Указатель пуст
			PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = nullptr;
			// Получить указатель на функцию
			wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
			// Если удалось получить - использовать
			if (wglSwapIntervalEXT)
				wglSwapIntervalEXT(sync);
		}
	}
}
