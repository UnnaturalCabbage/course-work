#include "DBService.h"

JsonDB::DBService::DBService() {}
JsonDB::DBService::DBService(Model *model, std::vector<std::string> indexes = {}) : JsonDB::DB(model) {
  this->setIndexes(indexes);
}

void JsonDB::DBService::setIndexes(std::vector<std::string> indexes) {
  this->indexes = indexes;
}
std::vector<std::string> JsonDB::DBService::getIndexes() {
  return this->indexes;
}

std::vector<std::string> JsonDB::DBService::parseIndex(std::string index) {
  std::vector<std::string> result;
  char *key = std::strtok(&index[0], "_");
  while (key != NULL) {
    result.push_back(std::string(key));
    key = std::strtok(NULL, "_");
  }
  return result;
}

void JsonDB::DBService::findIndexes(JsonDB::Model::Schema doc, void (*cb)(Json::Value &target, std::string indexed, std::string id)) {
  Json::Value jsonDoc = this->model->stringify(doc);
  std::string id = jsonDoc["_id"].asString();
  for (std::string index : this->indexes) {
    auto parsed = this->parseIndex(index);
    std::string indexedValue = "";
    for (auto key : parsed) {
      std::regex exp(key);
      indexedValue = (indexedValue.empty() ? "" : indexedValue + "_") + jsonDoc[key].asString();
    }
    (*cb)(this->root[JsonDB::DBService::indexesField][index], indexedValue, id);
  }
}

void JsonDB::DBService::saveIndexes(JsonDB::Model::Schema doc) {
  this->findIndexes(doc, [](Json::Value &target, std::string indexed, std::string id) {
    target[indexed][id]["_id"] = id;
  });
}

void JsonDB::DBService::removeIndexes(JsonDB::Model::Schema doc) {
  this->findIndexes(doc, [](Json::Value &target, std::string indexed, std::string id) {
    target.removeMember(indexed);
  });
}

JsonDB::Model::Schema JsonDB::DBService::save(JsonDB::Model::Schema doc) {
  JsonDB::Model::Schema savedDoc = JsonDB::DB::save(doc);
  this->saveIndexes(savedDoc);
  return savedDoc;
}

JsonDB::Model::Schema JsonDB::DBService::remove(std::string id) {
  JsonDB::Model::Schema removedDoc = JsonDB::DB::remove(id);
  this->removeIndexes(removedDoc);
  return removedDoc;
}

std::vector<JsonDB::Model::Schema> JsonDB::DBService::linearSearch(JsonDB::Model::Schema query, Json::Value indexesColl) {
  bool useIndexes = indexesColl.size() > 0;
  std::vector<Model::Schema> result;
  Json::Value target = useIndexes ? indexesColl : this->root;
  for (auto id : target.getMemberNames()) {
    if (id.compare(JsonDB::DBService::indexesField) == 0) continue;
    Json::Value doc = target[id];
    bool passed = true;
    for (auto field : query) {
      auto key = field.first;
      auto searchedValue = field.second;
      JsonDB::Model::CompareModes modes;
      modes.oneOf = true;
      auto value = this->model->parseItem(key, useIndexes ? this->root[doc["_id"].asString()][key] : doc[key]);
      if (!this->model->compareItem(key, value, searchedValue, modes)) {
        passed = false;
        break;
      }
    }
    if (passed)
      result.push_back(this->model->parse(this->root[doc["_id"].asString()]));
  }
  return result;
}

std::vector<JsonDB::Model::Schema> JsonDB::DBService::find(Model::Schema query) {
  if (query.find("_id") != query.end())
    return { JsonDB::DB::find(std::get<std::string>(query["_id"])) };
  std::string validKeyIndex = "";
  std::string validValueIndex = "";
  for (auto item : query) {
    std::string key = item.first;
    std::string value;
    if (this->model->checkType<int>(key)) 
      value = std::to_string(std::get<int>(item.second));
    else if (this->model->checkType<float>(key)) 
      value = std::to_string(std::get<float>(item.second));
    else if (this->model->checkType<std::string>(key))
      value = std::get<std::string>(item.second);
    else continue;
    std::string keyIndex = (validKeyIndex.length() > 0 ? validKeyIndex + "_" : "") + key;
    std::string valueIndex = (validValueIndex.length() > 0 ? validValueIndex + "_" : "") + value;
    if (this->root[JsonDB::DBService::indexesField][keyIndex].isMember(valueIndex)) {
      validKeyIndex = keyIndex;
      validValueIndex = valueIndex;
    } else break;
  }
  if (validKeyIndex.empty())
    return this->linearSearch(query, "");
  return this->linearSearch(query, this->root[JsonDB::DBService::indexesField][validKeyIndex][validValueIndex]);
}