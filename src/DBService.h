#pragma once

#include <regex>
#include <string.h>
#include "DB.h"

namespace JsonDB {
class DBService : public DB {
private:
   std::vector<std::string> indexes;
   inline static const std::string indexesField = "_indexes";

   std::vector<std::string> parseIndex(std::string index);
   void findIndexes(Model::Schema doc, void (*cb)(Json::Value &target, std::string indexed, std::string id));
   void saveIndexes(Model::Schema doc);
   void removeIndexes(Model::Schema doc);
   std::vector<Model::Schema> linearSearch(JsonDB::Model::Schema query, Json::Value indexesColl);
public:
   DBService();
   DBService(Model *model, std::vector<std::string> indexes);
   
   void setIndexes(std::vector<std::string> indexes);
   std::vector<std::string> getIndexes();
   
   Model::Schema save(Model::Schema doc);
   JsonDB::Model::Schema remove(std::string id);
   
   std::vector<Model::Schema> find(Model::Schema query);
};
}