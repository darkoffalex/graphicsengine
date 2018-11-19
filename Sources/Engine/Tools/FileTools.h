#pragma once
#include <string>

/**
* \brief Путь к рабочему каталогу
* \return Строка содержащая путь к директории
*/
std::string WorkingDir();

/**
* \brief Путь к каталогу с исполняемым файлом (директория содержащая запущенный .exe)
* \return Строка содержащая путь к директории
*/
std::string ExeDir();

/**
* \brief Абсолютный путь к папке с шейдеами
* \return Строка содержащая путь к директории
*/
std::string ShadersDir();

/**
* \brief Загрузка текстового файла в строку
* \return Строка с содержимым файла
*/
std::string LoadStringFromFile(std::string filepath);