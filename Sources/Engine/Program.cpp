#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <Windows.h>
#include <chrono>
#include <STB/stb_image.h>
#include <clocale>

#include "RendererOgl/Renderer.h"
#include "RendererOgl/Tools.h"

#include "CameraControllable.h"
#include "Controls.h"
#include "Tools/FileTools.h"
#include "RendererOgl/Defaults.h"


// Время последнего кадра
std::chrono::time_point<std::chrono::high_resolution_clock> _lastFrameTime;

// Камера
CameraControllable * _pCamera;

// OpenGL рендерер
ogl::Renderer * _pRenderer;

// Набор ресурсов используемых в сцене
// Это просто тестовый пример (в реальных задачах будут использоваться различные динамические контейнеры)
struct{
	// Шейдеры
	struct {
		ogl::ShaderResourcePtr basicShader;
	} shaders;

	// Текстуры
	struct {
		ogl::TextureResourcePtr groundDiffuse;

		ogl::TextureResourcePtr cubeDiffuse;
		ogl::TextureResourcePtr cubeSpecular;
		ogl::TextureResourcePtr cubeBump;

		ogl::TextureResourcePtr wallDiffuse;
		ogl::TextureResourcePtr wallSpecular;
		ogl::TextureResourcePtr wallBump;

		ogl::TextureResourcePtr flashLight;
	} textures;

	// Геометрия
	struct {
		ogl::StaticGeometryResourcePtr ground;
		ogl::StaticGeometryResourcePtr wall;
		ogl::StaticGeometryResourcePtr cube;
	} geometry;
} _sceneResources;



/**
 * \brief Загрузка ресурсов
 */
void Load();

/**
 * \brief Инициализация сцены (добавление объектов и источников света)
 * \param pRenderer Указатель на рендерер
 */
void Init(ogl::Renderer * pRenderer);

/**
 * \brief Обновление положения объектов
 * \param frameDeltaMs Время кадра
 */
void Update(float frameDeltaMs);



/**
 * \brief Точка входа
 * \param argc Кол-во аргументов запуска
 * \param argv Аргументы запуска (строки)
 * \return Код завершения
 */
int main(int argc, char* argv[])
{
	try
	{
		// Получение хендла исполняемого модуля
		HINSTANCE hInstance = GetModuleHandle(nullptr);

		// Регистрация класса окна
		WNDCLASSEX classInfo;
		classInfo.cbSize = sizeof(WNDCLASSEX);
		classInfo.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		classInfo.cbClsExtra = 0;
		classInfo.cbWndExtra = 0;
		classInfo.hInstance = hInstance;
		classInfo.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		classInfo.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
		classInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
		classInfo.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
		classInfo.lpszMenuName = nullptr;
		classInfo.lpszClassName = L"GraphicsAppWndClass";
		classInfo.lpfnWndProc = WndProc;

		// Если не удалось зарегистрировать класс
		if (!RegisterClassEx(&classInfo)) {
			throw std::runtime_error("ERROR: Can't register window class.");
		}

		// Создание окна
		HWND hWnd = CreateWindow(
			classInfo.lpszClassName,
			L"Graphics",
			WS_OVERLAPPEDWINDOW,
			0, 0,
			1280, 768,
			NULL,
			NULL,
			hInstance,
			NULL);

		// Если не удалось создать окно
		if (!hWnd) {
			throw std::runtime_error("ERROR: Can't create main application window.");
		}

		// Показать окно
		ShowWindow(hWnd, SW_SHOWNORMAL);

		// Создание OpenGL контекста
		// Необходимо для инициализации GLEW
		ogl::CreateContext(hWnd);

		// Вертикальная синхронизация (отключить)
		ogl::EnableVSync(false);

		// Загрузить необходимые ресурсы (шейдеры, текстуры, прочее)
		Load();

		// Создать рендерер
		_pRenderer = new ogl::Renderer(hWnd, _sceneResources.shaders.basicShader);

		// Создать камеру
		_pCamera = new CameraControllable(1.5f, 0.3f, _pRenderer->viewPort.getAspectRatio());
		_pCamera->position = { 0.0f,0.0f,3.0f };
		_pCamera->rotation.x = -10.0f;

		// Инициализация рисуемых объектов
		Init(_pRenderer);

		// Оконное сообщение (пустая структура)
		MSG msg = {};

		// Текущее время
		_lastFrameTime = std::chrono::high_resolution_clock::now();

		// Вечный цикл (работает пока не пришло сообщение WM_QUIT)
		while (true)
		{
			// Если получено сообщение
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT) {
					break;
				}
			}

			// Время текущего кадра (текущей итерации)
			std::chrono::time_point<std::chrono::high_resolution_clock> currentFrameTime = std::chrono::high_resolution_clock::now();
			// Сколько микросекунд прошло с последней итерации
			// 1 миллисекунда = 1000 микросекунд = 1000000 наносекунд
			int64_t delta = std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTime - _lastFrameTime).count();
			// Перевести в миллисекунды
			float deltaMs = static_cast<float>(delta) / 1000;

			// Подсчет FPS
			/*
			std::string deltaStr = std::string("Graphics (").append(std::to_string((1/deltaMs)*1000)).append(" FPS)");
			SetWindowTextA(hWnd, deltaStr.c_str());
			*/

			// Обновление
			Update(deltaMs);

			// Нарисовать кадр
			//_pRenderer->drawFrame(glm::vec4(0.2f, 0.4f, 0.6f, 1.0f));
			_pRenderer->drawFrame(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

			// Обновить время последнего кадра
			_lastFrameTime = currentFrameTime;
		}

		// Очистить память посе выхода из цикла
		delete _pRenderer;
		delete _pCamera;
	}
	catch (std::exception const &ex)
	{
		std::cout << ex.what() << std::endl;
		system("pause");
	}
}

/**
* \brief Загрузка ресурсов
*/
void Load()
{
	// Ш Е Й Д Е Р Ы

	// Загрузить шейдер по умолчанию
	std::string shaderSource = LoadStringFromFile(ShadersDir().append("basic.glsl"));
	_sceneResources.shaders.basicShader = ogl::MakeShaderResource(shaderSource);

	// Г Е О М Е Т Р И Я

	// Геометрия используемая для пола
	_sceneResources.geometry.ground = ogl::MakeStaticGeometryResource({
		{ { 10.0f, 0.0f, -10.0f },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f } },
		{ { 10.0f, 0.0f, 10.0f },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
		{ { -10.0f, 0.0f, 10.0f },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
		{ { -10.0f,  0.0f, -10.0f },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f } },
	}, { 0,1,2, 0,2,3 }, false, true, true);

	_sceneResources.geometry.wall = ogl::MakeStaticGeometryResource({
		{ { 5.0f, 5.0f, 0.0f },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f } },
		{ { 5.0f, -5.0f, 0.0f },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
		{ { -5.0f, -5.0f, 0.0f },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
		{ { -5.0f, 5.0f,  0.0f },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f } },
	}, { 0,1,2, 0,2,3 }, false, true, true);

	// Т Е К С Т У Р Ы

	int width, height, bpp;
	unsigned char* textureBytes;
	stbi_set_flip_vertically_on_load(false);

	// Текустура пола
	textureBytes = stbi_load(ExeDir().append("..\\Textures\\floor.jpg").c_str(), &width, &height, &bpp, 3);
	_sceneResources.textures.groundDiffuse = ogl::MakeTextureResource(textureBytes, static_cast<GLuint>(width), static_cast<GLuint>(height), static_cast<GLuint>(bpp), true);
	stbi_image_free(textureBytes);

	// Текстура стены
	textureBytes = stbi_load(ExeDir().append("..\\Textures\\brickwall.jpg").c_str(), &width, &height, &bpp, 3);
	_sceneResources.textures.wallDiffuse = ogl::MakeTextureResource(textureBytes, static_cast<GLuint>(width), static_cast<GLuint>(height), static_cast<GLuint>(bpp), true);
	stbi_image_free(textureBytes);

	textureBytes = stbi_load(ExeDir().append("..\\Textures\\brickwall_normal.jpg").c_str(), &width, &height, &bpp, 3);
	_sceneResources.textures.wallBump = ogl::MakeTextureResource(textureBytes, static_cast<GLuint>(width), static_cast<GLuint>(height), static_cast<GLuint>(bpp), true);
	stbi_image_free(textureBytes);

	// Текстура фонарика
	stbi_set_flip_vertically_on_load(true);
	textureBytes = stbi_load(ExeDir().append("..\\Textures\\flashlight-test.jpg").c_str(), &width, &height, &bpp, 3);
	_sceneResources.textures.flashLight = ogl::MakeTextureResource(textureBytes, static_cast<GLuint>(width), static_cast<GLuint>(height), static_cast<GLuint>(bpp), true);
	stbi_image_free(textureBytes);
}

/**
* \brief Инициализация сцены (добавление объектов и источников света)
* \param pRenderer Указатель на рендерер
*/
void Init(ogl::Renderer* pRenderer)
{
	// Добавить текстуру фонарика
	_pRenderer->flashLightTexture = _sceneResources.textures.flashLight;

	// Добавить к отрисовке пол, настроить его текстуру и положение
	ogl::StaticMeshPtr groundMesh = pRenderer->addStaticMesh(ogl::StaticMesh(ogl::StaticMeshPart(_sceneResources.geometry.ground)));
	groundMesh->getParts()[0].diffuseTexture.resource = _sceneResources.textures.groundDiffuse;
	groundMesh->getParts()[0].diffuseTexture.scale = { 10.0, 10.0f };
	groundMesh->getParts()[0].material = ogl::defaults::GetMaterialSetings(ogl::defaults::DefaultMaterialType::DEFAULT);
	groundMesh->position.y = -1.0f;

	// Добавить к отрисовке стену, настроить текстуру и положение
	ogl::StaticMeshPtr wallMesh = pRenderer->addStaticMesh(ogl::StaticMesh(ogl::StaticMeshPart(_sceneResources.geometry.wall)));
	wallMesh->getParts()[0].diffuseTexture.resource = _sceneResources.textures.wallDiffuse;
	wallMesh->getParts()[0].diffuseTexture.scale = { 5.0, 5.0f };
	wallMesh->getParts()[0].bumpTexture.resource = _sceneResources.textures.wallBump;
	wallMesh->getParts()[0].bumpTexture.scale = { 5.0, 5.0f };
	wallMesh->position = { 0.0f,4.0f,-2.0f };

	// Добавить источник света, настроить его положение
	ogl::LightPtr centralLight = pRenderer->addLight(ogl::Light(ogl::LightType::SPOT_LIGHT, { 0.0f,0.0f,0.0f }, { -90.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }));
	centralLight->render = true;
}

/**
* \brief Обновление положения объектов
* \param frameDeltaMs Время кадра
*/
void Update(float frameDeltaMs)
{
	_pRenderer->getLights()[0]->rotation.x += 0.05f * frameDeltaMs;

	// Если есть камера
	if(_pCamera != nullptr)
	{
		// Обновить положение камеры
		_pCamera->updatePositions(frameDeltaMs);

		// Матрицы проекции и вида согласно настройкам камеры
		_pRenderer->setProjectionMatrix(_pCamera->getProjectionMatrix());
		_pRenderer->setViewMatrix(_pCamera->getViewMatrix());

		// Положение камеры
		_pRenderer->cameraPosition = _pCamera->position;
	}
}
