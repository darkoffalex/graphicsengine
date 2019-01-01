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
	* \brief Инициализация uniform-буферов
	* \details В uniform буферах содержатся данные передаваемые в шейдер
	*/
	void Renderer::initUniformBuffers()
	{
		if (this->shaders_.shaderGBuffer_)
		{
			// Получить идентификаторы uniform-блоков объявленых в шейдере
			GLuint viewProjectionGbrUboBindingID = glGetUniformBlockIndex(this->shaders_.shaderGBuffer_->getId(), "viewProjectionMatrices");
			GLuint modelGbrUboBindingID = glGetUniformBlockIndex(this->shaders_.shaderGBuffer_->getId(), "modelMatrix");
			GLuint textureMappingUboBindingID = glGetUniformBlockIndex(this->shaders_.shaderGBuffer_->getId(), "textureMapping");
			//GLuint materislSettingsUboBindingID = glGetUniformBlockIndex(this->shaders_.shaderGBuffer_->getId(), "materialSettings");
			GLuint positionsUboBindingID = glGetUniformBlockIndex(this->shaders_.shaderLighting_->getId(), "positions");
			GLuint pointLightsUboBindingID = glGetUniformBlockIndex(this->shaders_.shaderLighting_->getId(), "ptLightsUniform");
			GLuint dirLightsUboBindingID = glGetUniformBlockIndex(this->shaders_.shaderLighting_->getId(), "dirLightsUniform");
			GLuint spotLightsUboBindingID = glGetUniformBlockIndex(this->shaders_.shaderLighting_->getId(), "spotLightsUniform");

			// Задаем номера связок этих uniform-блоков у шейдеров
			glUniformBlockBinding(this->shaders_.shaderGBuffer_->getId(), viewProjectionGbrUboBindingID, 0);
			glUniformBlockBinding(this->shaders_.shaderGBuffer_->getId(), modelGbrUboBindingID, 1);
			glUniformBlockBinding(this->shaders_.shaderGBuffer_->getId(), textureMappingUboBindingID, 2);
			//glUniformBlockBinding(this->shaders_.shaderGBuffer_->getId(), materislSettingsUboBindingID, 3);
			glUniformBlockBinding(this->shaders_.shaderLighting_->getId(), positionsUboBindingID, 4);
			glUniformBlockBinding(this->shaders_.shaderLighting_->getId(), pointLightsUboBindingID, 5);
			glUniformBlockBinding(this->shaders_.shaderLighting_->getId(), dirLightsUboBindingID, 6);
			glUniformBlockBinding(this->shaders_.shaderLighting_->getId(), spotLightsUboBindingID, 7);

			// Создать UBO-буфер для матриц вида-проекции и выделить память
			glGenBuffers(1, &uboViewProjection_);
			glBindBuffer(GL_UNIFORM_BUFFER, uboViewProjection_);
			glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			// Создать UBO-буфер для матрицы модели и выделить память
			glGenBuffers(1, &uboModel_);
			glBindBuffer(GL_UNIFORM_BUFFER, uboModel_);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			// Создать UBO-буфер для параметров маппинга текстуры и выделить память
			glGenBuffers(1, &uboTextureMapping_);
			glBindBuffer(GL_UNIFORM_BUFFER, uboTextureMapping_);
			glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(Std140TextureMapping), nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			// Создать UBO-буфер для параметров маппинга текстуры и выделить память
			/*
			glGenBuffers(1, &uboMaterialSettings_);
			glBindBuffer(GL_UNIFORM_BUFFER, uboMaterialSettings_);
			glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4) + sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			*/

			// Создать UBO-буфер для положений (шейдер освещения)
			glGenBuffers(1, &uboPositions_);
			glBindBuffer(GL_UNIFORM_BUFFER, uboPositions_);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			// Создать UBO-буфер для точечных источников освещения (шейдер освещения)
			glGenBuffers(1, &uboPointLights_);
			glBindBuffer(GL_UNIFORM_BUFFER, uboPointLights_);
			glBufferData(GL_UNIFORM_BUFFER, (sizeof(glm::vec4) * 2 + sizeof(GLfloat) * 4) * MAX_POINT_LIGHTS, nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			// Создать UBO-буфер для направленных источников освещения (шейдер освещения)
			glGenBuffers(1, &uboDirLights_);
			glBindBuffer(GL_UNIFORM_BUFFER, uboDirLights_);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 2 * MAX_DIRECT_LIGHTS, nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			// Создать UBO-буфер для источников освещения типа "фонарик" (шейдер освещения)
			glGenBuffers(1, &uboSpotLights_);
			glBindBuffer(GL_UNIFORM_BUFFER, uboSpotLights_);
			glBufferData(GL_UNIFORM_BUFFER, 128 * MAX_SPOT_LIGHTS, nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			// Связать область памяти буфера с точками uniform-привязки
			glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->uboViewProjection_, 0, 2 * sizeof(glm::mat4));
			glBindBufferRange(GL_UNIFORM_BUFFER, 1, this->uboModel_, 0, sizeof(glm::mat4));
			glBindBufferRange(GL_UNIFORM_BUFFER, 2, this->uboTextureMapping_, 0, 3 * sizeof(Std140TextureMapping));
			//glBindBufferRange(GL_UNIFORM_BUFFER, 3, this->uboMaterialSettings_, 0, 3 * sizeof(glm::vec4) + sizeof(GLfloat));
			glBindBufferRange(GL_UNIFORM_BUFFER, 4, this->uboPositions_, 0, sizeof(glm::vec4));
			glBindBufferRange(GL_UNIFORM_BUFFER, 5, this->uboPointLights_, 0, (sizeof(glm::vec4) * 2 + sizeof(GLfloat) * 4) * MAX_POINT_LIGHTS);
			glBindBufferRange(GL_UNIFORM_BUFFER, 6, this->uboDirLights_, 0, sizeof(glm::vec4) * 2 * MAX_DIRECT_LIGHTS);
			glBindBufferRange(GL_UNIFORM_BUFFER, 7, this->uboSpotLights_, 0, 128 * MAX_SPOT_LIGHTS);

		}
	}

	/**
	* \brief Очистка uniform-буферов
	*/
	void Renderer::freeUniformBuffers() const
	{
		if (uboViewProjection_) glDeleteBuffers(1, &uboViewProjection_);
		if (uboModel_) glDeleteBuffers(1, &uboModel_);
		if (uboTextureMapping_) glDeleteBuffers(1, &uboTextureMapping_);
		if (uboPositions_) glDeleteBuffers(1, &uboPositions_);
		if (uboPointLights_) glDeleteBuffers(1, &uboPointLights_);
		if (uboDirLights_) glDeleteBuffers(1, &uboDirLights_);
		if (uboSpotLights_) glDeleteBuffers(1, &uboSpotLights_);
	}

	/**
	* \brief Инициализация G-буффера
	* \param width Ширина буфера
	* \param height Высота буфера
	*/
	void Renderer::initGBuffer(GLuint width, GLuint height)
	{
		// Сохранить размеры в структуре
		this->gBuffer_.sizes = { width,height };

		// Создать объект фрейм-буфера и привязать его (работаем с фрейм-буфером)
		glGenFramebuffers(1, &gBuffer_.gBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer_.gBufferId);

		// Создать цветовое вложение для хранения положения фрагментов
		glGenTextures(1, &gBuffer_.gPositionAttachmentId);
		glBindTexture(GL_TEXTURE_2D, gBuffer_.gPositionAttachmentId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Создать цветовое вложение для хранения нормалей фрагментов
		glGenTextures(1, &gBuffer_.gNormalAttachmentId);
		glBindTexture(GL_TEXTURE_2D, gBuffer_.gNormalAttachmentId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Создать цветовое вложение для хранения цвета и интенсивности отражения (RGB - цвет, A - отражения)
		glGenTextures(1, &gBuffer_.gAlbedoSpecAttachmentId);
		glBindTexture(GL_TEXTURE_2D, gBuffer_.gAlbedoSpecAttachmentId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Создание буфера глубины-трафарета (используем render-буфер, не текстуру, т.к. выборка в шейдере не нужна)
		glGenRenderbuffers(1, &gBuffer_.depthStencilAttachmentId);
		glBindRenderbuffer(GL_RENDERBUFFER, gBuffer_.depthStencilAttachmentId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Собрать фрейм-буфер используя выше-описанные компоненты
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer_.gPositionAttachmentId, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer_.gNormalAttachmentId, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer_.gAlbedoSpecAttachmentId, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gBuffer_.depthStencilAttachmentId);

		// Указать какие вложения будут использованы для рендеринга
		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		// Если фрейм-буфер не готов
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw std::runtime_error("OpenGL:Renderer: G-buffer can't be initialized");
		}

		// Прекращаем работу с фрейм-буфером
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/**
	* \brief Очистка G-буфера
	*/
	void Renderer::freeGBuffer()
	{
		GLuint textures[3] = { this->gBuffer_.gPositionAttachmentId,this->gBuffer_.gNormalAttachmentId,this->gBuffer_.gAlbedoSpecAttachmentId };
		glDeleteTextures(3, textures);
		glDeleteRenderbuffers(1, &gBuffer_.depthStencilAttachmentId);
		glDeleteFramebuffers(1, &gBuffer_.gBufferId);
		this->gBuffer_.sizes = { 0,0 };
	}

	/**
	* \brief Инициализация фрейм-буфера
	* \param width Ширина буфера
	* \param height Высота буфера
	*/
	void Renderer::initFrameBuffer(GLuint width, GLuint height)
	{
		// Сохранить размеры в структуре
		this->frameBuffer_.sizes = { width,height };

		// Создать объект фрейм-буфера и привязать его (работаем с фрейм-буфером)
		glGenFramebuffers(1, &(this->frameBuffer_.frameBufferId));
		glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer_.frameBufferId);

		// Создать объект текстуры для цветового вложения фрейм-буфера
		glGenTextures(1, &(this->frameBuffer_.colorAttachmentId));
		glBindTexture(GL_TEXTURE_2D, this->frameBuffer_.colorAttachmentId);
		// Выделить текстурную память нужного размера
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameBuffer_.sizes.width, frameBuffer_.sizes.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		// Фильтрация (линейная)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Отвязать текустуру (завершаем работу с текстурой)
		glBindTexture(GL_TEXTURE_2D, 0);

		// Создание буфера глубины-трафарета (используем render-буфер, не текстуру, т.к. выборка в шейдере не нужна)
		glGenRenderbuffers(1, &(this->frameBuffer_.depthStencilAttachmentId));
		glBindRenderbuffer(GL_RENDERBUFFER, this->frameBuffer_.depthStencilAttachmentId);
		// Выделить необходимое кол-во памяти
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameBuffer_.sizes.width, frameBuffer_.sizes.height);
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
	}

	/**
	* \brief Очистка фрейм-буфера
	*/
	void Renderer::freeFrameBuffer()
	{
		// Удалить все объекты буфера
		glDeleteRenderbuffers(1, &(this->frameBuffer_.depthStencilAttachmentId));
		glDeleteTextures(1, &(this->frameBuffer_.colorAttachmentId));
		glDeleteFramebuffers(1, &(this->frameBuffer_.frameBufferId));

		// Обнулить идентификаторы и прочие значения
		this->frameBuffer_.frameBufferId = 0;
		this->frameBuffer_.depthStencilAttachmentId = 0;
		this->frameBuffer_.colorAttachmentId = 0;
		this->frameBuffer_.sizes = {};
	}

	/**
	* \brief Проход для рендеринга геометрии (рендеринг в G-буфер)
	* \param shaderID шейдер для рендеринга в G-буфре
	* \param clearColor Цвет очистки
	* \param clearMask Маска очистки
	*/
	void Renderer::renderPassGeometry(GLuint shaderID, glm::vec4 clearColor, GLbitfield clearMask)
	{
		// Установка размеров области вида
		glViewport(0, 0, this->viewPort.width, this->viewPort.height);

		// Активировать G-буфер (рендеринг в G-буфер)
		glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer_.gBufferId);

		// Установка параметров очистки экрана
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(clearMask);

		// Использовать шейдер
		glUseProgram(shaderID);

		// Обновляем области UBO-буфера содержащие матрицы проекции
		glBindBuffer(GL_UNIFORM_BUFFER, uboViewProjection_);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(this->projectionMatrix_));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(this->viewMatrix_));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Включить тест глубины
		glEnable(GL_DEPTH_TEST);

		// Отключить тест трафарета
		glDisable(GL_STENCIL_TEST);

		// Пройтись по всем статическим мешам
		for (auto staticMesh : this->staticMeshes_)
		{
			// Пройтись по всем частям меша
			for (auto part : staticMesh->getParts())
			{
				// Получить матрицу модели
				glm::mat4 mdodelMatrix = staticMesh->getModelMatrix();

				// Обновить UBO-буфер, содержащий матрицу модели в шейдере
				glBindBuffer(GL_UNIFORM_BUFFER, uboModel_);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mdodelMatrix));
				glBindBuffer(GL_UNIFORM_BUFFER, 0);


				// Получить структуры коэфициентов маппинга
				Std140TextureMapping diffuseTextureMapping = { part.diffuseTexture.offset,{ 0.0f,0.0f },part.diffuseTexture.scale,part.diffuseTexture.getRotMatrix() };
				Std140TextureMapping speculaTextureMapping = { part.specularTexture.offset,{ 0.0f,0.0f },part.specularTexture.scale,part.specularTexture.getRotMatrix() };
				Std140TextureMapping bumpTextureMapping = { part.bumpTexture.offset,{ 0.0f,0.0f },part.bumpTexture.scale,part.bumpTexture.getRotMatrix() };

				// Обновляем области UBO-буфера коэфициенты маппинга
				glBindBuffer(GL_UNIFORM_BUFFER, uboTextureMapping_);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Std140TextureMapping), &diffuseTextureMapping);
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Std140TextureMapping), sizeof(Std140TextureMapping), &speculaTextureMapping);
				glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(Std140TextureMapping), sizeof(Std140TextureMapping), &bumpTextureMapping);
				glBindBuffer(GL_UNIFORM_BUFFER, 0);


				// Получить ID'ы текстур (если установлены - их, если нет, тех что по умолчанию)
				GLuint diffuseTextureId = part.diffuseTexture.resource != nullptr ? part.diffuseTexture.resource->getId() : this->defaultTextures_.diffuse->getId();
				GLuint specularTextureId = part.specularTexture.resource != nullptr ? part.specularTexture.resource->getId() : this->defaultTextures_.specular->getId();
				GLuint bumpTextureId = part.bumpTexture.resource != nullptr ? part.bumpTexture.resource->getId() : this->defaultTextures_.bump->getId();

				// Активация и передача текстур в шейдер
				// Diffuse
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseTextureId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, part.diffuseTexture.wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, part.diffuseTexture.wrapT);
				glUniform1i(glGetUniformLocation(shaderID, "diffuseTexture"), 0);

				// Specular
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, specularTextureId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, part.specularTexture.wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, part.specularTexture.wrapT);
				glUniform1i(glGetUniformLocation(shaderID, "specularTexture"), 1);

				// Bump
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, bumpTextureId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, part.bumpTexture.wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, part.bumpTexture.wrapT);
				glUniform1i(glGetUniformLocation(shaderID, "bumpTexture"), 2);


				// Обновляем области UBO-буфера параметров материала
				/*
				glBindBuffer(GL_UNIFORM_BUFFER, uboMaterialSettings_);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(part.material.ambientСolor));
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(part.material.diffuseColor));
				glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(part.material.specularColor));
				glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(GLfloat), &part.material.shininess);
				glBindBuffer(GL_UNIFORM_BUFFER, 0);
				*/

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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/**
	* \brief Проход для рендеринга освещенности (рендеринг во фрейм-буфер)
	* \param shaderID шейдер для рендеринга во фрейм-буфер
	* \param cameraPosition Положение камеры
	* \param clearColor Цвет очистки
	* \param clearMask Маска очистки
	*/
	void Renderer::renderPassLighting(GLuint shaderID, const glm::vec3& cameraPosition, glm::vec4 clearColor, GLbitfield clearMask) const
	{
		// Установка размеров области вида
		glViewport(0, 0, this->viewPort.width, this->viewPort.height);

		// Активировать фрейм-буфер (рендеринг во фрейм-буфер)
		glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer_.frameBufferId);

		// Установка параметров очистки экрана
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(clearMask);

		// Использовать шейдер
		glUseProgram(shaderID);

		// Обновляем области UBO-буфера содержащие положение камеры
		glBindBuffer(GL_UNIFORM_BUFFER, uboPositions_);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(cameraPosition));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Отключить тест глубины
		glDisable(GL_DEPTH_TEST);

		// Отключить тест трафарета
		glDisable(GL_STENCIL_TEST);

		// Привязать VAO (геометрия квадрата)
		glBindVertexArray(this->defaultGeometry_.quad->getVaoId());

		// Передать значения цвета-бликовости фрагментов в шейдер
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->gBuffer_.gAlbedoSpecAttachmentId);
		glUniform1i(glGetUniformLocation(shaderID, "albedoSpecularTexture"), 0);

		// Передать значения положений фрагментов в шейдер
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->gBuffer_.gPositionAttachmentId);
		glUniform1i(glGetUniformLocation(shaderID, "positionTexture"), 1);

		// Передать значения нормалей фрагментов в шейдер
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, this->gBuffer_.gNormalAttachmentId);
		glUniform1i(glGetUniformLocation(shaderID, "normalTexture"), 2);

		// Если не пуст массив источников света
		if(!this->lights_.empty())
		{
			GLuint pointLights = 0;         // Итератор точечных источников
			//GLuint directionalLights = 0;   // Итератор направленных источников
			//GLuint spotLights = 0;          // Итератор фонариков-прожекторов

			// Пройти по всем источникам
			for(auto light : this->lights_)
			{
				// Если это точечный источник
				if(light->getType() == LightType::POINT_LIGHT)
				{
					// Создать структуру формата std 140 
					Std140PointLightSettings settings(light->position,light->color,light->attenuation.linear,light->attenuation.quadratic);

					// Обновить часть буфера соответствующую этому источнику света
					glBindBuffer(GL_UNIFORM_BUFFER, uboPointLights_);
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Std140PointLightSettings)*pointLights, sizeof(Std140PointLightSettings), &settings);
					glBindBuffer(GL_UNIFORM_BUFFER, 0);
					
					//Увеличить итератор точечных источников
					pointLights++;
				}
			}
		}

		// Отрисовать VAO
		glDrawElements(GL_TRIANGLES, this->defaultGeometry_.quad->getIndexCount(), GL_UNSIGNED_INT, nullptr);

		// Отвязать VAO
		glBindVertexArray(0);
	}

	/**
	* \brief Проход рендеринга для финального представления (рендеринг в основной буфер)
	* \param shaderID шейдер для рендеринга во фрейм-буфер
	* \param clearColor Цвет очистки
	* \param clearMask Маска очистки
	*/
	void Renderer::renderPassFinal(GLuint shaderID, glm::vec4 clearColor, GLbitfield clearMask) const
	{
		// Установка размеров области вида
		glViewport(0, 0, this->viewPort.width, this->viewPort.height);

		// Активировать основной буфер (окна, оконной системы)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Установка параметров очистки экрана
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(clearMask);

		// Использовать шейдер
		glUseProgram(shaderID);

		// Отключить тест глубины
		glDisable(GL_DEPTH_TEST);

		// Отключить тест трафарета
		glDisable(GL_STENCIL_TEST);

		// Привязать VAO (геометрия квадрата)
		glBindVertexArray(this->defaultGeometry_.quad->getVaoId());

		// Нацепить текстуру фреймбуфера на квадрат
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->frameBuffer_.colorAttachmentId);
		glUniform1i(glGetUniformLocation(shaderID, "screenTexture"), 0);

		// Отрисовать VAO
		glDrawElements(GL_TRIANGLES, this->defaultGeometry_.quad->getIndexCount(), GL_UNSIGNED_INT, nullptr);

		// Отвязать VAO
		glBindVertexArray(0);

		// Смена буферов окна
		SwapBuffers(GetDC(this->hwnd_));
	}

	/**
	* \brief Конструктор
	* \param hwnd Хендл WinAPI окна
	* \param geometry Шейдер для рендеринга геометрии
	* \param lightning Шейдер для подсчета освещенности
	* \param postProcessing Шейдер для пост-обработки
	*/
	Renderer::Renderer(HWND hwnd, ShaderResourcePtr geometry, ShaderResourcePtr lightning, ShaderResourcePtr postProcessing) :
		hwnd_(hwnd),
		viewMatrix_(glm::mat4(1)),
		projectionMatrix_(glm::mat4(1)),
		uboViewProjection_(0),
		uboModel_(0),
		cameraPosition(glm::vec3(0.0f, 0.0f, 0.0f))
	{
		// Инициализация GLEW
		if (!_isGlewInitialised) {
			glewExperimental = GL_TRUE;
			_isGlewInitialised = glewInit() == GLEW_OK;
		}

		if (!_isGlewInitialised) {
			throw std::runtime_error("OpenGL:Renderer: Glew is not initialised");
		}

		if (geometry == nullptr || lightning == nullptr || postProcessing == nullptr) {
			throw std::runtime_error("OpenGL:Renderer: Not all shaders initialized");
		}

		// v i e w - p o r t

		// Получение размеров области вида
		RECT clientRect;
		GetClientRect(this->hwnd_, &clientRect);
		this->viewPort.width = static_cast<GLuint>(clientRect.right);
		this->viewPort.height = static_cast<GLuint>(clientRect.bottom);

		// Установка размеров области вида
		glViewport(0, 0, this->viewPort.width, this->viewPort.height);

		// Инициализация G-буфера и фрейм-буфера
		this->initGBuffer(this->viewPort.width, this->viewPort.height);
		this->initFrameBuffer(this->viewPort.width, this->viewPort.height);

		// с г л а ж и в а н и е

		// Не используем мульти-семплинг
		glDisable(GL_MULTISAMPLE);

		// с м е ш и в а н и е  ц в е т о в (а л ь ф а - к а н а л)

		// Поскольку у нас отложенное затенение, смешивания по альфа-каналу можно (нужно) выключить
		glDisable(GL_BLEND);
		// Функция смешивания (по умолчанию, не используется, поскольку GL_BLEND отключен)
		// Цвет, который накладывается поверх другого, множится на свой альфа-канал
		// Цвет, на который накладывается другой цвет, множится на единицу минус альфа канал наложенного цвета
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Значения цветов при смешивании (наложении) складываются (не используется, поскольку GL_BLEND отключен)
		glBlendEquation(GL_FUNC_ADD);

		// т е с т  т р а ф а р е т а

		// В ключить тест трафарета (по умолчанию)
		glEnable(GL_STENCIL_TEST);
		// Если тест трафарета и тест глубины пройден - заменить значение эталоном сравнения из glStencilFunc (поведение по умолч.)
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		// Тест трафарета считается пройденым, если значение у фрагмента равно единице (по умолчанию)
		glStencilFunc(GL_EQUAL, 1, 0xFF);

		// о т с е ч е н и е  г р а н е й

		// Передними считаются грани описаные по часовой стрелке
		glFrontFace(GL_CW);
		// Включить отсечение граней
		glEnable(GL_CULL_FACE);
		// Отсекать задние грани
		glCullFace(GL_BACK);

		// г е о м е т р и я  п о  у м о л ч а н и ю

		this->defaultGeometry_.cube = MakeStaticGeometryResource(
			defaults::GetVertices(defaults::DefaultGeometryType::CUBE, 0.3f),
			defaults::GetIndices(defaults::DefaultGeometryType::CUBE));

		this->defaultGeometry_.quad = MakeStaticGeometryResource(
			defaults::GetVertices(defaults::DefaultGeometryType::PLANE, 2.0f),
			defaults::GetIndices(defaults::DefaultGeometryType::PLANE));

		// т е к с т у р ы  п о  у м о л ч а н и ю

		GLubyte whitePixel[] = { 255,255,255 }; // белый пиксель
		GLubyte bluePixel[] = { 127,127,255 };  // синеватый пиксель, соответствует нормали [0,0,1]
		this->defaultTextures_.diffuse = MakeTextureResource(whitePixel, 1, 1, 24, false);
		this->defaultTextures_.specular = MakeTextureResource(whitePixel, 1, 1, 24, false);
		this->defaultTextures_.bump = MakeTextureResource(bluePixel, 1, 1, 24, false);

		// ш е й д е р ы

		this->shaders_.shaderGBuffer_ = geometry;
		this->shaders_.shaderLighting_ = lightning;
		this->shaders_.shaderPostProcessing_ = postProcessing;

		// u n i f o r m - б у ф е р ы

		// Инициализация uniform-буфера (инициализация возможна только после того как задан шейдер)
		this->initUniformBuffers();
	}

	/**
	* \brief Освобождение памяти
	*/
	Renderer::~Renderer()
	{
		// Уничтожение uniform-буферов
		this->freeUniformBuffers();

		// Уничтожение G-буфера и фрейм-буфера
		this->freeGBuffer();
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

		// Получить ID'ы всех необходимых шейдеров
		GLuint geometryShaderID = this->shaders_.shaderGBuffer_->getId();
		GLuint lightingShaderID = this->shaders_.shaderLighting_->getId();
		GLuint postProcessingShaderID = this->shaders_.shaderPostProcessing_->getId();

		// Отрендерить кадр с геометрией, записать значения положений, нормалей, цветов фрагментов в G-буфер
		this->renderPassGeometry(geometryShaderID, { 0.0f,0.0f,0.0f,0.0f }, clearMask);
		// Используя данные из G-буфера произвести расчет освещенности (запись во фрейм-буфер)
		this->renderPassLighting(lightingShaderID, this->cameraPosition, clearColor, GL_COLOR_BUFFER_BIT);
		// Осуществить пост-обработку полученного кадра, на основе цветового вложения фрейм-буфера (запись в основной буфер)
		this->renderPassFinal(postProcessingShaderID, clearColor, GL_COLOR_BUFFER_BIT);
	}
}
