﻿#include "Renderer.h"
#include "Defaults.h"
#include <algorithm>

namespace ogl
{
	/**
	* \brief Проинициализирован ли GLEW
	*/
	extern bool _isGlewInitialised;

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

		// Пеередать матрицы вида и проекции в шейдер
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projectionMatrix_));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(this->viewMatrix_));

		// Передать положение камеры (для бликов/отражений)
		glUniform3fv(glGetUniformLocation(shaderID, "cameraPosition"), 1, glm::value_ptr(this->cameraPosition));

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
				// Передать матрицу модели в шейдер
				glm::mat4 mdodelMatrix = staticMesh->getModelMatrix();
				glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(mdodelMatrix));

				// Получить структуры коэфициентов маппинга
				TextureMapping diffuseTextureMapping = { part.diffuseTexture.offset,{ 0.0f,0.0f },part.diffuseTexture.scale,part.diffuseTexture.getRotMatrix() };
				TextureMapping speculaTextureMapping = { part.specularTexture.offset,{ 0.0f,0.0f },part.specularTexture.scale,part.specularTexture.getRotMatrix() };
				TextureMapping bumpTextureMapping = { part.bumpTexture.offset,{ 0.0f, 0.0f },part.bumpTexture.scale,part.bumpTexture.getRotMatrix() };
				TextureMapping displaceTextureMapping = { part.displacementTexture.offset, {0.0f, 0.0f}, part.displacementTexture.scale, part.displacementTexture.getRotMatrix() };

				// Отправить маппинг текстур в шейдер
				this->texMappingToShader(shaderID, diffuseTextureMapping, "diffuseTexMapping");
				this->texMappingToShader(shaderID, speculaTextureMapping, "specularTexMapping");
				this->texMappingToShader(shaderID, bumpTextureMapping, "bumpTexMapping");
				this->texMappingToShader(shaderID, displaceTextureMapping, "displaceTextureMapping");


				// Получить ID'ы текстур (если установлены - их, если нет, тех что по умолчанию)
				GLuint diffuseTextureId = part.diffuseTexture.resource != nullptr ? part.diffuseTexture.resource->getId() : this->defaultTextures_.diffuse->getId();
				GLuint specularTextureId = part.specularTexture.resource != nullptr ? part.specularTexture.resource->getId() : this->defaultTextures_.specular->getId();
				GLuint bumpTextureId = part.bumpTexture.resource != nullptr ? part.bumpTexture.resource->getId() : this->defaultTextures_.bump->getId();
				GLuint displacementTextureId = part.displacementTexture.resource != nullptr ? part.displacementTexture.resource->getId() : this->defaultTextures_.displace->getId();

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

				// Displacement (paralax)
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, displacementTextureId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, part.displacementTexture.wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, part.displacementTexture.wrapT);
				glUniform1i(glGetUniformLocation(shaderID, "displaceTexture"), 3);

				// Привязать VAO
				glBindVertexArray(part.getGeometry()->getVaoId());

				// Рисовать либо индексированную либо не-индексированную геометрию
				if (part.getGeometry()->IsIndexed()) {
					glDrawElements(GL_TRIANGLES_ADJACENCY, part.getGeometry()->getIndexCount(), GL_UNSIGNED_INT, nullptr);
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
	* \brief Проход для построения теневых объемов и записи инаформации о тени в stencil-буфер
	* \param light Источник освещения
	* \param shaderID Шейдер для построения теневых объемов
	*/
	void Renderer::renderPassShadows(LightPtr light, GLuint shaderID)
	{
		// Включить тест глубины
		glEnable(GL_DEPTH_TEST);
		// Отключить отбрасывание граней (нам нужны обе стороны теневого объема)
		glDisable(GL_CULL_FACE);
		// Отключаеи запись в Z-буфер
		glDepthMask(GL_FALSE);
		// Предотвращаем отсечение бесконечно далеких фрагментов
		glEnable(GL_DEPTH_CLAMP);
		// Полигональный свдиг глубины (во избежании z-figting'а полигонов теневого объема и геометрии)
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.0f, 100.0f);

		// Установка размеров области вида
		glViewport(0, 0, this->viewPort.width, this->viewPort.height);

		// Активировать фрейм-буфер (рендеринг во фрейм-буфер)
		glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer_.frameBufferId);

		// Очистить stencil буфер
		glClear(GL_STENCIL_BUFFER_BIT);

		// Отключить рисование в цветовой буфер (тени рисуются только в stencil, при этом учитывая Z-буфер)
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		// Активировать stencil-тест, но он должен всегда проходить успешно
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0xff);

		// Алгоритм Z-fail. В начале рисуем нелицевые полигоны теневого объема, увелививая stencil значение для них на 1
		// Затем, рисуя лицевые, от того что есть единицу. В итоге затененных областях остануться единицы
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		// Использовать шейдер
		glUseProgram(shaderID);

		// Передать матрицы вида, проекции, положение источника освещения в шейдер
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projectionMatrix_));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(this->viewMatrix_));
		glUniform3fv(glGetUniformLocation(shaderID, "lightPosition"), 1, glm::value_ptr(light->position));

		// Пройтись по всем статическим мешам
		for (auto staticMesh : this->staticMeshes_)
		{
			// Пройтись по всем частям меша
			for (auto part : staticMesh->getParts())
			{
				// Передать матрицу модели в шейдер
				glm::mat4 mdodelMatrix = staticMesh->getModelMatrix();
				glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(mdodelMatrix));

				// Привязать VAO
				glBindVertexArray(part.getGeometry()->getVaoId());

				// Рисовать только индексированную геометрию со смежностями
				if (part.getGeometry()->IsIndexed()) {
					glDrawElements(GL_TRIANGLES_ADJACENCY, part.getGeometry()->getIndexCount(), GL_UNSIGNED_INT, nullptr);
				}

				// Отвязка VAO
				glBindVertexArray(0);
			}
		}


		// Возвращаем тест глубины в исходное состояние
		glDisable(GL_DEPTH_TEST);
		// Возвращаем culling в исходное состояние
		glEnable(GL_CULL_FACE);
		// Возвращаем запись в z-буфер в исходное состояние
		glDepthMask(GL_TRUE);
		// Отключаем клэмпинг бесконечно далеких фрагментов (возвращаем в исходное)
		glDisable(GL_DEPTH_CLAMP);
		// Полигональный свдиг глубины (возвращаем старое значение)
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0, 0);
		// Снова включить рисование в цветовой буфер
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		// Отключить тест трафарета
		glDisable(GL_STENCIL_TEST);
	}

	/**
	* \brief Проход рендеринга освещенности (один источником света)
	* \details Данный метод вызывается многократно (для нескольких источников), результаты буфера суммируются
	* \param light Источник света
	* \param shaderID Шейдер для рендеринга освещения
	* \param cameraPosition Положение камеры
	* \param clearColor Цвет очистки
	* \param clearMask Маска очистки
	* \param clear Очистить
	*/
	void Renderer::renderPassLighting(LightPtr light, GLuint shaderID, const glm::vec3& cameraPosition, glm::vec4 clearColor, GLbitfield clearMask, bool clear) const
	{
		// Установка размеров области вида
		glViewport(0, 0, this->viewPort.width, this->viewPort.height);

		// Активировать фрейм-буфер (рендеринг во фрейм-буфер)
		glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer_.frameBufferId);

		// Включить тест трафарета
		glEnable(GL_STENCIL_TEST);
		// Тест трафарета считается пройденым если значение в нем равно нулю
		glStencilFunc(GL_EQUAL, 0x0, 0xFF);
		// Не обновлять тест трафарета
		//glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);

		// Включть аддтимвное смешивание (предыдущй цвет складывается с текущим)
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		// Если нужно очистить буфер
		// Поскольку данный проход выполняется для каждого источника с последующим аддиивным смешиванием, буффер следует очищать только в первый раз
		if(clear)
		{
			// Установка параметров очистки экрана
			glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
			// Очистить экран
			glClear(clearMask);
		}


		// Использовать шейдер
		glUseProgram(shaderID);

		// Передать положение камеры (для бликов/отражений)
		glUniform3fv(glGetUniformLocation(shaderID, "cameraPosition"), 1, glm::value_ptr(this->cameraPosition));

		// Отключить тест глубины
		glDisable(GL_DEPTH_TEST);

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

		// Передать в шейдер тип источника освещения
		glUniform1ui(glGetUniformLocation(shaderID, "light.type"), static_cast<GLuint>(light->getType()));

		// Передать параметры источника освещения в шейдер (в зависимости от типа)
		switch (light->getType())
		{
		case LightType::POINT_LIGHT:
		default:
			glUniform3fv(glGetUniformLocation(shaderID, "light.position"), 1, glm::value_ptr(light->position));
			glUniform3fv(glGetUniformLocation(shaderID, "light.color"), 1, glm::value_ptr(light->color));
			glUniform1f(glGetUniformLocation(shaderID, "light.linear"), light->attenuation.linear);
			glUniform1f(glGetUniformLocation(shaderID, "light.quadratic"), light->attenuation.quadratic);
			break;

		case LightType::DIRECTIONAL_LIGHT:
			glUniform3fv(glGetUniformLocation(shaderID, "light.direction"), 1, glm::value_ptr(light->getDirection()));
			glUniform3fv(glGetUniformLocation(shaderID, "light.color"), 1, glm::value_ptr(light->color));
			break;

		case LightType::SPOT_LIGHT:
			glUniform3fv(glGetUniformLocation(shaderID, "light.position"), 1, glm::value_ptr(light->position));
			glUniform3fv(glGetUniformLocation(shaderID, "light.direction"), 1, glm::value_ptr(light->getDirection()));
			glUniform3fv(glGetUniformLocation(shaderID, "light.color"), 1, glm::value_ptr(light->color));
			glUniform1f(glGetUniformLocation(shaderID, "light.cutOffCos"), glm::cos(glm::radians(light->cutOffAngle)));
			glUniform1f(glGetUniformLocation(shaderID, "light.cutOffOuterCos"), glm::cos(glm::radians(light->cutOffOuterAngle)));
			glUniform1f(glGetUniformLocation(shaderID, "light.linear"), light->attenuation.linear);
			glUniform1f(glGetUniformLocation(shaderID, "light.quadratic"), light->attenuation.quadratic);
			glUniformMatrix4fv(glGetUniformLocation(shaderID, "light.modelMatrix"), 1, GL_FALSE, glm::value_ptr(light->getModelMatrix()));
			break;
		}

		// Отрисовать VAO
		glDrawElements(GL_TRIANGLES, this->defaultGeometry_.quad->getIndexCount(), GL_UNSIGNED_INT, nullptr);

		// Отвязать VAO
		glBindVertexArray(0);

		// Отключить смешивание
		glDisable(GL_BLEND);

		// Отключить тест трафарета
		glDisable(GL_STENCIL_TEST);
	}

	/**
	* \brief Проход для рендеринга системных объектов (напр. источники света)
	* \param shaderID
	*/
	void Renderer::renderPassSysObjects(GLuint shaderID) const
	{
		// Установка размеров области вида
		glViewport(0, 0, this->viewPort.width, this->viewPort.height);

		// Использовать шейдер
		glUseProgram(shaderID);

		// Скопировать значения глубины из G-буфера во фрейм-буфер (чтобы объекты не рендерелись поверх всего подряд)
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, this->gBuffer_.gBufferId);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->frameBuffer_.frameBufferId);
		//glBlitFramebuffer(0, 0, this->viewPort.width, this->viewPort.height, 0, 0, this->viewPort.width, this->viewPort.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		// Активировать фрейм-буфер (рендеринг во фрейм-буфер)
		glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer_.frameBufferId);

		// Включить тест глубины
		glEnable(GL_DEPTH_TEST);

		// Включить тест трафарета
		//glEnable(GL_STENCIL_TEST);

		// Передача матриц проекции и вида в шейдер
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(this->viewMatrix_));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projectionMatrix_));

		// Проход по всем источникам освещения для их отображения
		for (auto light : this->lights_)
		{
			// Если источник света должен быть отображен
			if (light->render)
			{
				// Матрица модели
				glm::mat4 mdodelMatrix = light->getModelMatrix();
				glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(mdodelMatrix));

				// Передать цвет
				glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, glm::value_ptr(light->color));

				// Привязать VAO
				glBindVertexArray(this->defaultGeometry_.cube->getVaoId());
				glDrawElements(GL_TRIANGLES, this->defaultGeometry_.cube->getIndexCount(), GL_UNSIGNED_INT, nullptr);
				glBindVertexArray(0);
			}
		}
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
	* \brief Передать в шейдер структуру маппинга текстуры
	* \param shaderId ID шейдера
	* \param mapping Структура маппинга
	* \param uniformName Наименование uniform переменной
	*/
	void Renderer::texMappingToShader(GLuint shaderId, const TextureMapping& mapping, std::string uniformName) const
	{
		glUniform2fv(glGetUniformLocation(shaderId, std::string(uniformName + ".offset").c_str()), 1, glm::value_ptr(mapping.offset));
		glUniform2fv(glGetUniformLocation(shaderId, std::string(uniformName + ".origin").c_str()), 1, glm::value_ptr(mapping.origin));
		glUniform2fv(glGetUniformLocation(shaderId, std::string(uniformName + ".scale").c_str()), 1, glm::value_ptr(mapping.scale));
		glUniformMatrix2fv(glGetUniformLocation(shaderId, std::string(uniformName + ".rotation").c_str()), 1, GL_FALSE, glm::value_ptr(mapping.rotation));
	}

	/**
	* \brief Конструктор
	* \param hwnd Хендл WinAPI окна
	* \param geometry Шейдер для рендеринга геометрии
	* \param lightning Шейдер для подсчета освещенности
	* \param postProcessing Шейдер для пост-обработки
	* \param shadows Шейдер для построения теневых объемов
	*/
	Renderer::Renderer(HWND hwnd, ShaderResourcePtr geometry, ShaderResourcePtr lightning, ShaderResourcePtr postProcessing, ShaderResourcePtr shadows) :
		hwnd_(hwnd),
		viewMatrix_(glm::mat4(1)),
		projectionMatrix_(glm::mat4(1)),
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
			defaults::GetVertices(defaults::DefaultGeometryType::CUBE, 0.1f),
			defaults::GetIndices(defaults::DefaultGeometryType::CUBE));

		this->defaultGeometry_.quad = MakeStaticGeometryResource(
			defaults::GetVertices(defaults::DefaultGeometryType::PLANE, 2.0f),
			defaults::GetIndices(defaults::DefaultGeometryType::PLANE));

		// т е к с т у р ы  п о  у м о л ч а н и ю

		GLubyte whitePixel[] = { 255,255,255 }; // белый пиксель
		GLubyte blackPixel[] = { 0,0,0 };
		GLubyte bluePixel[] = { 128,128,255 };  // синеватый пиксель, соответствует нормали [0,0,1]
		this->defaultTextures_.diffuse = MakeTextureResource(whitePixel, 1, 1, 24, false);
		this->defaultTextures_.specular = MakeTextureResource(whitePixel, 1, 1, 24, false);
		this->defaultTextures_.bump = MakeTextureResource(bluePixel, 1, 1, 24, false);
		this->defaultTextures_.displace = MakeTextureResource(blackPixel, 1, 1, 24, false);

		// ш е й д е р ы

		this->shaders_.shaderGBuffer_ = geometry;
		this->shaders_.shaderLighting_ = lightning;
		this->shaders_.shaderPostProcessing_ = postProcessing;
		this->shaders_.shaderSolidColor_ = MakeShaderResource(defaults::GetShaderSource(defaults::DefaultShaderType::SOLID_COLORED));
		this->shaders_.shaderShadowVolumes_ = shadows;
	}

	/**
	* \brief Освобождение памяти
	*/
	Renderer::~Renderer()
	{
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
		GLuint solidColorShaderID = this->shaders_.shaderSolidColor_->getId();
		GLuint shadowShaderID = this->shaders_.shaderShadowVolumes_->getId();

		// Отрендерить кадр с геометрией, записать значения положений, нормалей, цветов фрагментов в G-буфер
		this->renderPassGeometry(geometryShaderID, { 0.0f,0.0f,0.0f,0.0f }, clearMask);

		// Скопировать значения глубины из G-буфера во фрейм-буфер (чтобы объекты не рендерелись поверх всего подряд)
		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->gBuffer_.gBufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->frameBuffer_.frameBufferId);
		glBlitFramebuffer(0, 0, this->viewPort.width, this->viewPort.height, 0, 0, this->viewPort.width, this->viewPort.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		// Пройти по всем источникам
		for(unsigned int i = 0; i < this->lights_.size(); i++)
		{
			// Если источник установлен и валиден
			if (this->lights_[i] != nullptr){

				this->renderPassShadows(
					this->lights_[i],
					shadowShaderID
				);

				// Посчитать освещенность для источника, наложить на имеющийся в фрейм-буфере
				this->renderPassLighting(
					this->lights_[i],     // Источник
					lightingShaderID,     // Шейдер
					this->cameraPosition, // Положение камеры
					clearColor,           // Цвет очистки
					GL_COLOR_BUFFER_BIT,  // Очищать цветовой буфер
					i == 0                // Поскольку очистка произведена выше, очищать не нужно
				);
			}
		}

		// Отрендерить системные объекты (при необходимости)
		this->renderPassSysObjects(solidColorShaderID);
		// Осуществить пост-обработку полученного кадра, на основе цветового вложения фрейм-буфера (запись в основной буфер)
		this->renderPassFinal(postProcessingShaderID, clearColor, GL_COLOR_BUFFER_BIT);
	}
}
