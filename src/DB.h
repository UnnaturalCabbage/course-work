#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <json/json.h>
#include "Model.h"

namespace JsonDB {
class DB {
protected:
   Json::Value root;
   std::string collection;
   std::string path;
   Model *model;
   
   size_t generateHash(Json::Value doc);
public:
   DB();
   DB(Model *model);
   virtual ~DB() = default;
   
   void setModel(Model* model);
   Model* getModel();
   Json::Value getRoot();
   
   /**
    * Подлючение базы данных к коллекции находящейся в файле по пути path, и запись ее в динамическую память
   */
   void connect();
   /**
    * Сохранение документа в динамической памяти
    * @param doc Документ, который нужно сохранить. Если такой документ существует, то он будет перезаписан.
    * Сохраняет только те поля, которые присутствуют в текущей моделе.
    * @return Документ, который сохранился.
   */
   virtual Model::Schema save(Model::Schema doc);
   /**
    * Обновление документа в динамической памяти
    * 
    * @param doc Документ, который нужно обновить. Если такой документ существуют, то его поля обновяться на поля из doc, если нет, то создаст.
    * Сохраняет только те поля, которые присутствуют в текущей моделе.
    * @return Документ, который обновился.
   */
   virtual Model::Schema update(Model::Schema doc);
   /**
    * Удаляет документ из динамической пямяти
    * 
    * @param id id докуента, который нужно удалить.
    * @return Документ, который был удален.
   */
   virtual Model::Schema remove(std::string id);
   /**
    * Получает документ из базы по id
    * 
    * @param id id докуента, который нужно получить.
    * @return Документ, который был получен.
   */
   Model::Schema find(std::string id);
   /**
    * Читает Json из файла и возвращает его
    * 
    * @param path путь к json файлу
    * @return Json в представлении класса Json::Value (библиотека JsonCpp)
   */
   Json::Value read(std::string path);
   Json::Value read();
   /**
    * Записывает json из динамической памяти в файл
    * 
    * @param path путь к json файлу
   */
   void write(std::string path);
   void write();
};
}