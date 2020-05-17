#pragma once

#include <iostream>
#include <string>
#include <map>
#include <json/json.h>
#include <variant>
#include <memory>

namespace JsonDB {
class Model {
public:
   typedef std::variant<float, int, std::string, std::vector<std::string>, std::map<std::string,std::string>> SchemaItem;
   typedef std::map<std::string, Model::SchemaItem> Schema;
   struct CompareModes {
     bool oneOf = false;
   };
private:
  Model::Schema schema;
  std::string collection;
  std::string path;

   static bool mapCompare(std::map<std::string,std::string> &lhs, std::map<std::string,std::string> &rhs, CompareModes modes = CompareModes());
   static bool schemaContains(std::string key, Model::Schema s) {
      return s.find(key) != s.end();
   }
public:
   Model(Model::Schema schema, std::string collection, std::string path);
   
   Model::Schema getModel();
   std::string getCollection();
   std::string getPath();
   
   template <typename T>
   bool checkType(std::string key) {
   Model::SchemaItem fieldType = this->schema[key]; 
   if (const auto ptr (std::get_if<T>(&fieldType)); ptr) 
      return true;
   return false;
   }
   
   /**
    * Трансформирует схему в Json документ. Поля которых нет в текущей моделе не будут включены!
    * 
    * @param doc схема
    * @return Json документ
   */
   Json::Value stringify(Model::Schema doc);
   /**
    * Трансформирует значение из поля Json объекта в Model::SchemaItem исходя из текущей модели
    * 
    * @param key ключ данного значение в моделе
    * @param jsonValue значение, которое надо трансформировать
    * @return трансформированное значение типа Model::SchemaItem
   */
   Model::SchemaItem parseItem(std::string key, Json::Value jsonValue);
   /**
    * Трансформирует весь Json документ в Model::Schema
    * 
    * @param json докумен, которое надо трансформировать
    * @return трансформированный документ типа Model::Schema
   */
   Model::Schema parse(Json::Value json);
   /**
    * Сравниевает два значения типа Model::SchemaItem между собой
    * 
    * @param key ключ данных значений в моделе
    * @param left первое значение для сравнивания
    * @param right второе значение для сравнивания
    * @param modes флаги, которые изменяет процесс сравнивания
    * @return одинаковы ли значение
   */
   bool compareItem(std::string key, Model::SchemaItem left, Model::SchemaItem right, CompareModes modes = CompareModes());
   /**
    * Сравниевает два документа типа Model::Schema между собой
    * 
    * @param left первое значение для сравнивания
    * @param right второе значение для сравнивания
    * @return одинаковы ли значение
   */
   bool compare(Model::Schema left, Model::Schema right);
   
   /**
    * Возвращает модель, которое подключенна к collection в файле по пути path. Если такой еще нет, то она будет создана
    * 
    * @param schema схема модели
    * @param collection коллекция к которой принадлежит схема
    * @param path путь по которума находится json файл
    * @return ссылка на модель
   */
   static Model& getInstance(Model::Schema schema, std::string collection, std::string path);
   
   Model(Model const&) = delete;
   void operator= (Model const&) = delete;
};
}