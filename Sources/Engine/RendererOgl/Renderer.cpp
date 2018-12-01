#include "Renderer.h"
#include "Defaults.h"
#include <algorithm>

namespace ogl
{
	/**
	* \brief Проинициализирован ли GLEW
	*/
	extern bool _isGlewInitialised;

	/**
	* \brief Инициализация кадрового буфера
	* \param width Ширина буфера
	* \param height Высота буфера
	* \param multisampling Создавать буфер для с учетом мульти-семплинга (сглаживание)
	* \param samples Кол-во точек подвыборки (семплов) на каждый пиксель
	*/
	void Renderer::initFrameBuffer(GLuint width, GLuint height, bool multisampling, GLuint samples)
	{
		// о б ы ч н ы й   ф р е й м - б у ф е р

		// Сохранить размеры в структуре
		this->frameBuffer_.sizes = { width,height };
		// Кол-во семплов в обычном буфере равно единице (хоть этот параметр и не используется, установим)
		this->frameBuffer_.samples = 1;

		// Создать объект фрейм-буфера и привязать его (работаем с фрейм-буфером)
		glGenFramebuffers(1, &(this->frameBuffer_.fboId));
		glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer_.fboId);

		// Создать объект текстуры для цветового вложения фрейм-буфера
		glGenTextures(1, &(this->frameBuffer_.colorAttachmentId));
		glBindTexture(GL_TEXTURE_2D, this->frameBuffer_.colorAttachmentId);
		// Выделить текстурную память нужного размера
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameBuffer_.sizes.x, frameBuffer_.sizes.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		// Фильтрация (линейная)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Отвязать текустуру (завершаем работу с текстурой)
		glBindTexture(GL_TEXTURE_2D, 0);

		// Создание буфера глубины-трафарета (используем render-буфер, не текстуру, т.к. выборка в шейдере не нужна)
		glGenRenderbuffers(1, &(this->frameBuffer_.depthStencilAttachmentId));
		glBindRenderbuffer(GL_RENDERBUFFER, this->frameBuffer_.depthStencilAttachmentId);
		// Выделить необходимое кол-во памяти
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameBuffer_.sizes.x, frameBuffer_.sizes.y);
		// Отвязать render-буфер
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Привязать текстуру к кадровому буферу в качестве нулевого цветового вложения
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer_.colorAttachmentId, 0);
		// Привязать render-буфер глубины-трафарета в качестве вложения глубины трафарета
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frameBuffer_.depthStencilAttachmentId);

		// Если фрейм-буфер не готов
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw std::runtime_error("OpenGL:Renderer: Frame buffer can't be initialized");
		}

		// Прекращаем работу с фрейм-буфером
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Если не нужен мульти-семплинг - выход из функции
		if (!multisampling) return;

		// ф р е й м - б у ф е р  с  у ч е т о м  M S A A

		// Сохранить размеры в структуре
		this->frameBufferMSAA_.sizes = { width,height };
		// Кол-во семплов
		this->frameBufferMSAA_.samples = samples;

		// Создать объект фрейм-буфера и привязать его (работаем с фрейм-буфером)
		glGenFramebuffers(1, &(this->frameBufferMSAA_.fboId));
		glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferMSAA_.fboId);

		// Создать объект текстуры для цветового вложения фрейм-буфера
		glGenTextures(1, &(this->frameBufferMSAA_.colorAttachmentId));
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->frameBufferMSAA_.colorAttachmentId);
		// Выделить текстурную память нужного размера
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->frameBufferMSAA_.samples, GL_RGB, width, height, GL_TRUE);
		// Отвязать текустуру (завершаем работу с текстурой)
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		// Создание буфера глубины-трафарета (используем render-буфер, не текстуру, т.к. выборка в шейдере не нужна)
		glGenRenderbuffers(1, &(this->frameBufferMSAA_.depthStencilAttachmentId));
		glBindRenderbuffer(GL_RENDERBUFFER, this->frameBufferMSAA_.depthStencilAttachmentId);
		// Выделить необходимое кол-во памяти
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, this->frameBufferMSAA_.samples, GL_DEPTH24_STENCIL8, frameBufferMSAA_.sizes.x, frameBufferMSAA_.sizes.y);
		// Отвязать render-буфер
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Привязать текстуру к кадровому буферу в качестве нулевого цветового вложения
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frameBufferMSAA_.colorAttachmentId, 0);
		// Привязать render-буфер глубины-трафарета в качестве вложения глубины трафарета
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frameBufferMSAA_.depthStencilAttachmentId);

		// Если фрейм-буфер не готов
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw std::runtime_error("OpenGL:Renderer: Frame buffer(MSAA) can't be initialized");
		}

		// Прекращаем работу с фрейм-буфером
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/**
	 * \brief Де-инициализация кадрового буффера
	 */
	void Renderer::freeFrameBuffer()
	{
		// Удалить все объекты буферов
		glDeleteFramebuffers(1, &(this->frameBuffer_.fboId));
		glDeleteRenderbuffers(1, &(this->frameBuffer_.depthStencilAttachmentId));
		glDeleteTextures(1, &(this->frameBuffer_.colorAttachmentId));

		// Обнулить идентификаторы и прочие значения
		this->frameBuffer_.fboId = 0;
		this->frameBuffer_.depthStencilAttachmentId = 0;
		this->frameBuffer_.colorAttachmentId = 0;
		this->frameBuffer_.sizes = {};
	}

	/**
	* \brief Конструктор
	* \param hwnd Хендл WinAPI окна
	* \param shaderBasic Основной шейдер
	* \param shaderPostProcessing Шейдер для пост-обработки
	* \param msaa Активировать мульти-семплинг
	* \param samples Кол-во семплов
	*/
	Renderer::Renderer(HWND hwnd, ShaderResourcePtr shaderBasic, ShaderResourcePtr shaderPostProcessing, bool msaa, GLuint samples) :
		hwnd_(hwnd),
		viewMatrix_(glm::mat4(1)),
		projectionMatrix_(glm::mat4(1)),
		shaderBasic_(shaderBasic),
		shaderPostProc_(shaderPostProcessing),
		enableMSAA_(msaa),
		cameraPosition(glm::vec3(0.0f, 0.0f, 0.0f))
	{
		// Получение размеров области вида
		RECT clientRect;
		GetClientRect(this->hwnd_, &clientRect);
		this->viewPort.width = static_cast<GLuint>(clientRect.right);
		this->viewPort.height = static_cast<GLuint>(clientRect.bottom);

		// Инициализация GLEW
		if (!_isGlewInitialised) {
			glewExperimental = GL_TRUE;
			_isGlewInitialised = glewInit() == GLEW_OK;
		}

		if (!_isGlewInitialised) {
			throw std::runtime_error("OpenGL:Renderer: Glew is not initialised");
		}

		// Инициализация фрейм-буфера для пост-процессинга
		this->initFrameBuffer(this->viewPort.width, this->viewPort.height, this->enableMSAA_, samples);

		// Активация/деактивация мульти-семплинга
		if (enableMSAA_) glEnable(GL_MULTISAMPLE); else glDisable(GL_MULTISAMPLE);

		// Установка размеров области вида
		glViewport(0, 0, this->viewPort.width, this->viewPort.height);


		// Включить режим смешивания цветов (по умолчанию)
		glEnable(GL_BLEND);
		// Функция смешивания (по умолчанию)
		// Цвет, который накладывается поверх другого, множится на свой альфа-канал
		// Цвет, на который накладывается другой цвет, множится на единицу минус альфа канал наложенного цвета
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Значения цветов при смешивании (наложении) складываются
		glBlendEquation(GL_FUNC_ADD);

		// В ключить тест трафарета (по умолчанию)
		glEnable(GL_STENCIL_TEST);
		// Если тест трафарета и тест глубины пройден - заменить значение эталоном сравнения из glStencilFunc (поведение по умолч.)
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		// Тест трафарета считается пройденым, если значение у фрагмента равно единице (по умолчанию)
		glStencilFunc(GL_EQUAL, 1, 0xFF);

		// Передними считаются грани описаные по часовой стрелке
		glFrontFace(GL_CW);
		// Включить отсечение граней
		glEnable(GL_CULL_FACE);
		// Отсекать задние грани
		glCullFace(GL_BACK);

		// Текстура по умолчанию для diffuse, specular (белый пиксель)
		GLubyte whitePixel[] = { 255,255,255 };
		this->defaultTextures_.diffuse = MakeTextureResource(whitePixel, 1, 1, 24, false);
		this->defaultTextures_.specular = MakeTextureResource(whitePixel, 1, 1, 24, false);
		this->defaultTextures_.flashLightTexture = MakeTextureResource(whitePixel, 1, 1, 24, false);

		// Текстура по умолчанию для detail (прозрачный пиксель)
		GLubyte transparentPixel[] = { 0,0,0,0 };
		this->defaultTextures_.detail = MakeTextureResource(transparentPixel, 1, 1, 32, false);

		// Текстура по умолчанию для bump (синеватый пиксель, соответствует нормали [0,0,1])
		GLubyte bluePixel[] = { 126,126,255 };
		this->defaultTextures_.bump = MakeTextureResource(bluePixel, 1, 1, 24, false);

		// Геметрия по умолчанию (для визуализации различных объектов)
		this->defaultGeometry_.cube = MakeStaticGeometryResource(
			defaults::GetVertices(defaults::DefaultGeometryType::CUBE, 0.1f), 
			defaults::GetIndices(defaults::DefaultGeometryType::CUBE));

		this->defaultGeometry_.quad = MakeStaticGeometryResource(
			defaults::GetVertices(defaults::DefaultGeometryType::PLANE, 2.0f),
			defaults::GetIndices(defaults::DefaultGeometryType::PLANE));

		this->defaultGeometry_.skybox = MakeStaticGeometryResource(
			defaults::GetVertices(defaults::DefaultGeometryType::CUBE_SKYBOX),
			{});

		// Шейдер по умолчанию для объектов сплошного цвета
		this->shaderSolidColor_ = MakeShaderResource(defaults::GetShaderSource(defaults::DefaultShaderType::SOLID_COLORED));
		// Шейдер по умолчанию для скайбокса
		this->shaderSkybox_ = MakeShaderResource(defaults::GetShaderSource(defaults::DefaultShaderType::SKYBOX));
	}

	/**
	* \brief Освобождение памяти
	*/
	Renderer::~Renderer()
	{
		this->freeFrameBuffer();
	}

	/**
	* \brief Установить матрицу вида
	* \param matrix Матрица
	*/
	void Renderer::setViewMatrix(const glm::mat4& matrix)
	{
		this->viewMatrix_ = matrix;
	}

	/**
	* \brief Установить матрицу проекции
	* \param matrix Матрица
	*/
	void Renderer::setProjectionMatrix(const glm::mat4& matrix)
	{
		this->projectionMatrix_ = matrix;
	}

	/**
	* \brief Добавить статический меш в список
	* \param mesh Объект статического меша
	* \return Указатель на меш в списке
	*/
	StaticMeshPtr Renderer::addStaticMesh(const StaticMesh& mesh)
	{
		this->staticMeshes_.push_back(std::make_shared<StaticMesh>(mesh));
		return this->staticMeshes_[this->staticMeshes_.size() - 1];
	}

	/**
	* \brief Удаление статического меша из списка
	* \param meshPtr Указатель на меш в списке
	*/
	void Renderer::removeStaticMesh(StaticMeshPtr& meshPtr)
	{
		// Переместить в конец списка элемент с указанным адресом и получить итератор
		auto newEnd = std::remove_if(this->staticMeshes_.begin(), this->staticMeshes_.end(), [&meshPtr](const StaticMeshPtr& entry)
		{
			return meshPtr.get() == entry.get();
		});

		// Удалить объект из массива
		this->staticMeshes_.erase(newEnd, staticMeshes_.end());

		// Обнулить указатель
		meshPtr = nullptr;
	}

	/**
	* \brief Добавить источник света
	* \param light Объект источника света
	* \return Указатель на источник в списке
	*/
	LightPtr Renderer::addLight(const Light& light)
	{
		this->lights_.push_back(std::make_shared<Light>(light));
		return this->lights_[this->lights_.size() - 1];
	}

	/**
	* \brief Удалить источник света из списка
	* \param lightPtr Указатель на источник в списке
	*/
	void Renderer::removeLight(LightPtr& lightPtr)
	{
		// Переместить в конец списка элемент с указанным адресом и получить итератор
		auto newEnd = std::remove_if(this->lights_.begin(), this->lights_.end(), [&lightPtr](const LightPtr& entry)
		{
			return lightPtr.get() == entry.get();
		});

		// Удалить объект из массива
		this->lights_.erase(newEnd, lights_.end());

		// Обнулить указатель
		lightPtr = nullptr;
	}

	/**
	* \brief Получить массив статических мешей
	* \return Ссылка на массив указателей
	*/
	std::vector<StaticMeshPtr>& Renderer::getStaticMeshes()
	{
		return this->staticMeshes_;
	}

	/**
	* \brief Получить массив источников освещения
	* \return Ссылка на массив указателей
	*/
	std::vector<LightPtr>& Renderer::getLights()
	{
		return this->lights_;
	}

	/**
	* \brief Рисование кадра
	* \param clearColor Цвет очистки кадра
	* \param clearMask Параметры очистки
	*/
	void Renderer::drawFrame(glm::vec4 clearColor, GLbitfield clearMask)
	{
		// Инициализация GLEW
		if (!_isGlewInitialised) {
			glewExperimental = GL_TRUE;
			_isGlewInitialised = glewInit() == GLEW_OK;
		}

		if (!_isGlewInitialised) {
			throw std::runtime_error("OpenGL:Renderer: Glew is not initialised");
		}

		// П Р О Х О Д - 1

		// Если мульти-семплинг включен - рендерить в MSAA фрейм-буфер
		if(this->enableMSAA_){
			glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferMSAA_.fboId);
		}
		// Если мульти-семплинг отключен - рандерить в основной фрейм-буфер
		else{
			glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer_.fboId);
		}


		// Установка параметров очистки экрана
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(clearMask);

		// Включить тест глубины
		glEnable(GL_DEPTH_TEST);

		// Идентификаторы основных шейдеров
		GLuint solidColorShaderID = this->shaderSolidColor_->getId();
		GLuint basicShaderID = this->shaderBasic_->getId();
		GLuint postProcShaderID = this->shaderPostProc_->getId();
		GLuint skyboxShaderID = this->shaderSkybox_->getId();

		// с к а й - б о к с
		if(this->backgroundTexture != nullptr)
		{
			// Рисование скайбоксы
			// Использовать соответствующий шейдер
			glUseProgram(skyboxShaderID);

			// Чтобы скайбокс был статичен относительно камеры из видовой матрицы нужно убрать данные о перемещении
			glm::mat4 viewSkybox = glm::mat4(glm::mat3(this->viewMatrix_));

			// Передача матриц проекции и вида в шейдер
			glUniformMatrix4fv(glGetUniformLocation(skyboxShaderID, "view"), 1, GL_FALSE, glm::value_ptr(viewSkybox));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShaderID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projectionMatrix_));

			// Выключить тест глубины
			glDisable(GL_DEPTH_TEST);

			// Привязать VAO скайбокса
			glBindVertexArray(this->defaultGeometry_.skybox->getVaoId());
			glDrawArrays(GL_TRIANGLES, 0, this->defaultGeometry_.skybox->getVertexCount());
			// Применить текстуру скайбокса
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, this->backgroundTexture->getId());
			glUniform1i(glGetUniformLocation(skyboxShaderID, "skybox"), 0);
			// Отвязать VAO
			glBindVertexArray(0);

			// Включить тест глубины обратно
			glEnable(GL_DEPTH_TEST);
		}

		// с и с т е м н ы е  о б ъ е к т ы

		// Рисование системных объектов (источники света и прочее)
		// Использовать однотонный шейдер
		glUseProgram(solidColorShaderID);

		// Передача матриц проекции и вида в шейдер
		glUniformMatrix4fv(glGetUniformLocation(solidColorShaderID, "view"), 1, GL_FALSE, glm::value_ptr(this->viewMatrix_));
		glUniformMatrix4fv(glGetUniformLocation(solidColorShaderID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projectionMatrix_));

		// Включить тест трафарета
		glEnable(GL_STENCIL_TEST);

		// Проход по всем источникам освещения для их отображения
		for (auto light : this->lights_)
		{
			// Если источник света должен быть отображен
			if (light->render)
			{
				// о б ъ е к т

				// Тест будет пройден в любом случае (какое бы значение не было в буфере)
				// После прохождения теста в буфер будет внесено значение 1 (поведение из glStencilOp)
				glStencilFunc(GL_ALWAYS, 1, 0xFF);

				// Матрица модели
				glm::mat4 mdodelMatrix = light->getModelMatrix();
				glUniformMatrix4fv(glGetUniformLocation(solidColorShaderID, "model"), 1, GL_FALSE, glm::value_ptr(mdodelMatrix));

				// Передать цвет
				glUniform3fv(glGetUniformLocation(solidColorShaderID, "lightColor"), 1, glm::value_ptr(light->color));

				// Привязать VAO
				glBindVertexArray(this->defaultGeometry_.cube->getVaoId());
				glDrawElements(GL_TRIANGLES, this->defaultGeometry_.cube->getIndexCount(), GL_UNSIGNED_INT, nullptr);
				glBindVertexArray(0);

				// о б в о д к а

				// Тест считается пройденным если в буфере не 1
				// После прохождения теста в буфер будет внесено значение 1 (поведение из glStencilOp)
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

				// Матрица модели (чуть увеличенный вариант)
				mdodelMatrix = light->getModelMatrix(1.1f);
				glUniformMatrix4fv(glGetUniformLocation(solidColorShaderID, "model"), 1, GL_FALSE, glm::value_ptr(mdodelMatrix));

				// Передать цвет
				glm::vec3 col = {1.0f,0.68f,0.0f};
				glUniform3fv(glGetUniformLocation(solidColorShaderID, "lightColor"), 1, glm::value_ptr(col));

				// Привязать VAO
				glBindVertexArray(this->defaultGeometry_.cube->getVaoId());
				glDrawElements(GL_TRIANGLES, this->defaultGeometry_.cube->getIndexCount(), GL_UNSIGNED_INT, nullptr);
				glBindVertexArray(0);
			}
		}

		// о с н о в н ы е  о б ъ е к т ы

		// Рисование основной сцены
		// Использовать основной шейдер
		glUseProgram(basicShaderID);

		// Для основной сцены тест трафарета не производится
		glDisable(GL_STENCIL_TEST);

		// Кол-во обработанных источников всех типов
		unsigned pointLigths = 0;
		unsigned directionalLights = 0;
		unsigned spotLights = 0;

		// Проход по всем источникам освещения для передачи их данных в шейдер
		for (auto light : this->lights_)
		{
			// Для точечных источников
			if (light->getType() == LightType::POINT_LIGHT)
			{
				// Базовое имя переменной
				std::string varBaseName = "pointLights[" + std::to_string(pointLigths) + "].";

				// Передать позицию источника
				glUniform3fv(glGetUniformLocation(basicShaderID, std::string(varBaseName + "position").c_str()), 1, glm::value_ptr(light->position));
				// Передать цвет источника
				glUniform3fv(glGetUniformLocation(basicShaderID, std::string(varBaseName + "color").c_str()), 1, glm::value_ptr(light->color));
				// Передать линейный коэффициент затухания
				glUniform1f(glGetUniformLocation(basicShaderID, std::string(varBaseName + "linear").c_str()), light->attenuation.linear);
				// Передать квадратичный коэффициент затухания
				glUniform1f(glGetUniformLocation(basicShaderID, std::string(varBaseName + "quadratic").c_str()), light->attenuation.quadratic);

				// Увеличить итератор
				pointLigths++;
			}
			// Для направленных источников
			else if (light->getType() == LightType::DIRECTIONAL_LIGHT)
			{
				// Базовое имя переменной
				std::string varBaseName = "directLights[" + std::to_string(directionalLights) + "].";

				// Передать направление источника
				glUniform3fv(glGetUniformLocation(basicShaderID, std::string(varBaseName + "direction").c_str()), 1, glm::value_ptr(light->getDirection()));
				// Передать цвет источника
				glUniform3fv(glGetUniformLocation(basicShaderID, std::string(varBaseName + "color").c_str()), 1, glm::value_ptr(light->color));

				// Увеличить итератор
				directionalLights++;
			}
			// Для типа "фонарик-прожектор"
			else if (light->getType() == LightType::SPOT_LIGHT)
			{
				// Базовое имя переменной
				std::string varBaseName = "spotLights[" + std::to_string(spotLights) + "].";

				// Передать позицию источника
				glUniform3fv(glGetUniformLocation(basicShaderID, std::string(varBaseName + "position").c_str()), 1, glm::value_ptr(light->position));
				// Передать направление источника
				glUniform3fv(glGetUniformLocation(basicShaderID, std::string(varBaseName + "direction").c_str()), 1, glm::value_ptr(light->getDirection()));
				// Передать цвет источника
				glUniform3fv(glGetUniformLocation(basicShaderID, std::string(varBaseName + "color").c_str()), 1, glm::value_ptr(light->color));
				// Передать косинус угола отсечения (внутренний)
				glUniform1f(glGetUniformLocation(basicShaderID, std::string(varBaseName + "cutOffCos").c_str()), glm::cos(glm::radians(light->cutOffAngle)));
				// Передать косинус угола отсечения (внешний)
				glUniform1f(glGetUniformLocation(basicShaderID, std::string(varBaseName + "cutOffOuterCos").c_str()), glm::cos(glm::radians(light->cutOffOuterAngle)));
				// Передать линейный коэффициент затухания
				glUniform1f(glGetUniformLocation(basicShaderID, std::string(varBaseName + "linear").c_str()), light->attenuation.linear);
				// Передать квадратичный коэффициент затухания
				glUniform1f(glGetUniformLocation(basicShaderID, std::string(varBaseName + "quadratic").c_str()), light->attenuation.quadratic);
				// Матрица модели
				glm::mat4 mdodelMatrix = light->getModelMatrix();
				glUniformMatrix4fv(glGetUniformLocation(basicShaderID, std::string(varBaseName + "modelMatrix").c_str()), 1, GL_FALSE, glm::value_ptr(mdodelMatrix));
				// Увеличить итератор
				spotLights++;
			}
		}

		// Передать матрицу вида и проекции
		glUniformMatrix4fv(glGetUniformLocation(basicShaderID, "view"), 1, GL_FALSE, glm::value_ptr(this->viewMatrix_));
		glUniformMatrix4fv(glGetUniformLocation(basicShaderID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projectionMatrix_));

		// Передать положение камеры (для бликов/отражений)
		glUniform3fv(glGetUniformLocation(basicShaderID, "eyePosition"), 1, glm::value_ptr(this->cameraPosition));

		// Пройтись по всем статическим мешам
		for(auto staticMesh : this->staticMeshes_)
		{
			// Пройтись по всем частям меша
			for(auto part : staticMesh->getParts())
			{
				// Пеередать матрицу мрдели в шейдер
				glm::mat4 mdodelMatrix = staticMesh->getModelMatrix();
				glUniformMatrix4fv(glGetUniformLocation(basicShaderID, "model"), 1, GL_FALSE, glm::value_ptr(mdodelMatrix));


				// Получить ID'ы текстур (если установлены - их, если нет, тех что по умолчанию)
				GLuint diffuseTextureId = part.diffuseTexture.resource != nullptr ? part.diffuseTexture.resource->getId() : this->defaultTextures_.diffuse->getId();
				GLuint detailTextureId = part.detailTexture.resource != nullptr ? part.detailTexture.resource->getId() : this->defaultTextures_.detail->getId();
				GLuint specularTextureId = part.specularTexture.resource != nullptr ? part.specularTexture.resource->getId() : this->defaultTextures_.specular->getId();
				GLuint bumpTextureId = part.bumpTexture.resource != nullptr ? part.bumpTexture.resource->getId() : this->defaultTextures_.bump->getId();
				GLuint flashLightTextureId = this->flashLightTexture != nullptr ? this->flashLightTexture->getId() : this->defaultTextures_.flashLightTexture->getId();


				// Передача коэффициентов мапинга текстур в шейдер
				// Diffuse
				glUniform2fv(glGetUniformLocation(basicShaderID, "texMappingDiffuse.offset"), 1, glm::value_ptr(part.diffuseTexture.offset));
				glUniform2fv(glGetUniformLocation(basicShaderID, "texMappingDiffuse.scale"), 1, glm::value_ptr(part.diffuseTexture.scale));
				glm::mat2 rotDiffuse = part.diffuseTexture.getRotMatrix();
				glUniformMatrix2fv(glGetUniformLocation(basicShaderID, "texMappingDiffuse.rot"), 1, GL_FALSE, glm::value_ptr(rotDiffuse));

				// Detail
				glUniform2fv(glGetUniformLocation(basicShaderID, "texMappingDetail.offset"), 1, glm::value_ptr(part.detailTexture.offset));
				glUniform2fv(glGetUniformLocation(basicShaderID, "texMappingDetail.scale"), 1, glm::value_ptr(part.detailTexture.scale));
				glm::mat2 rotDetail = part.detailTexture.getRotMatrix();
				glUniformMatrix2fv(glGetUniformLocation(basicShaderID, "texMappingDetail.rot"), 1, GL_FALSE, glm::value_ptr(rotDetail));

				// Specular
				glUniform2fv(glGetUniformLocation(basicShaderID, "texMappingSpecular.offset"), 1, glm::value_ptr(part.specularTexture.offset));
				glUniform2fv(glGetUniformLocation(basicShaderID, "texMappingSpecular.scale"), 1, glm::value_ptr(part.specularTexture.scale));
				glm::mat2 rotSpecular = part.specularTexture.getRotMatrix();
				glUniformMatrix2fv(glGetUniformLocation(basicShaderID, "texMappingSpecular.rot"), 1, GL_FALSE, glm::value_ptr(rotSpecular));

				// Bump
				glUniform2fv(glGetUniformLocation(basicShaderID, "texMappingBump.offset"), 1, glm::value_ptr(part.bumpTexture.offset));
				glUniform2fv(glGetUniformLocation(basicShaderID, "texMappingBump.scale"), 1, glm::value_ptr(part.bumpTexture.scale));
				glm::mat2 rotBump = part.bumpTexture.getRotMatrix();
				glUniformMatrix2fv(glGetUniformLocation(basicShaderID, "texMappingBump.rot"), 1, GL_FALSE, glm::value_ptr(rotBump));


				// Активация и передача текстур в шейдер
				// Diffuse
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseTextureId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, part.diffuseTexture.wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, part.diffuseTexture.wrapT);
				glUniform1i(glGetUniformLocation(basicShaderID, "diffuseTexture"), 0);

				// Detail
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, detailTextureId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, part.detailTexture.wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, part.detailTexture.wrapT);
				glUniform1i(glGetUniformLocation(basicShaderID, "detailTexture"), 1);

				// Specular
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, specularTextureId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, part.specularTexture.wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, part.specularTexture.wrapT);
				glUniform1i(glGetUniformLocation(basicShaderID, "specularTexture"), 2);

				// Bump
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, bumpTextureId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, part.bumpTexture.wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, part.bumpTexture.wrapT);
				glUniform1i(glGetUniformLocation(basicShaderID, "bumpTexture"), 3);

				// Flashlight
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, flashLightTextureId);
				glUniform1i(glGetUniformLocation(basicShaderID, "flashlightTexture"), 4);


				// Передать параметры материала
				glUniform3fv(glGetUniformLocation(basicShaderID, "material.ambientColor"), 1, glm::value_ptr(part.material.ambientСolor));
				glUniform3fv(glGetUniformLocation(basicShaderID, "material.diffuseColor"), 1, glm::value_ptr(part.material.diffuseColor));
				glUniform3fv(glGetUniformLocation(basicShaderID, "material.specularColor"), 1, glm::value_ptr(part.material.specularColor));
				glUniform1f(glGetUniformLocation(basicShaderID, "material.shininess"), part.material.shininess);


				// Привязать VAO
				glBindVertexArray(part.getGeometry()->getVaoId());

				// Рисовать либо индексированную либо не-индексированную геометрию
				if (part.getGeometry()->IsIndexed()) {
					glDrawElements(GL_TRIANGLES, part.getGeometry()->getIndexCount(), GL_UNSIGNED_INT, nullptr);
				}
				else {
					glDrawArrays(GL_TRIANGLES, 0, part.getGeometry()->getVertexCount());
				}

				// Отвязка VAO
				glBindVertexArray(0);
			}
		}


		// П Р О Х О Д - 2

		// Если мультисемплинг включен, необходимо осуществить перенос из MSAA буфера в обычной (для дальнейшего пост-процессинга)
		if(this->enableMSAA_){
			glBindFramebuffer(GL_READ_FRAMEBUFFER, this->frameBufferMSAA_.fboId);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->frameBuffer_.fboId);
			glBlitFramebuffer(0, 0, this->frameBufferMSAA_.sizes.x, this->frameBufferMSAA_.sizes.y, 0, 0, this->frameBuffer_.sizes.x, this->frameBuffer_.sizes.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		// Активировать фрейм-буфер окна (рендеринг на поверхность окна)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Установка параметров очистки экрана (белый фон, очищать только цвет, отключить тест глубины)
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);

		// Использовать шейдер пост-обработки
		glUseProgram(postProcShaderID);

		// Привязать VAO (геометрия квадрата)
		glBindVertexArray(this->defaultGeometry_.quad->getVaoId());

		// Нацепить текстуру фреймбуфера на квадрат
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->frameBuffer_.colorAttachmentId);
		glUniform1i(glGetUniformLocation(postProcShaderID, "screenTexture"), 0);

		// Отрисовать VAO
		glDrawElements(GL_TRIANGLES, this->defaultGeometry_.quad->getIndexCount(), GL_UNSIGNED_INT, nullptr);

		// Отвязать VAO
		glBindVertexArray(0);

		// Смена буферов
		SwapBuffers(GetDC(this->hwnd_));
	}
}
