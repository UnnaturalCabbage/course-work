#include "Model.h"

JsonDB::Model::Model(JsonDB::Model::Schema schema, std::string collection, std::string path) : schema(schema), collection(collection), path(path) {}

JsonDB::Model::Schema JsonDB::Model::Model::getModel() {
  return this->schema;
}

std::string JsonDB::Model::Model::getCollection() {
  return this->collection;
}

std::string JsonDB::Model::Model::getPath() {
  return this->path;
}


Json::Value JsonDB::Model::stringify(JsonDB::Model::Schema doc) {
  using namespace JsonDB;

  Json::Value result;

  try {
    for (auto item : this->schema) {
      std::string key = item.first;
      Model::SchemaItem defValue =  item.second;
      if (this->checkType<int>(key)) 
        result[key] = std::get<int>(Model::schemaContains(key, doc) ? doc[key] : defValue);
      else if (this->checkType<float>(key)) 
        result[key] = std::get<float>(Model::schemaContains(key, doc) ? doc[key] : defValue);
      else if (this->checkType<std::string>(key))
        result[key] = std::get<std::string>(Model::schemaContains(key, doc) ? doc[key] : defValue);
      else if (this->checkType<std::vector<std::string>>(key)) {
        auto value = std::get<std::vector<std::string>>(Model::schemaContains(key, doc) ? doc[key] : defValue);
        for (auto item : value) {
          result[key].append(item);
        }
      } else if (this->checkType<std::map<std::string, std::string>>(key)) {
        auto value = std::get<std::map<std::string,std::string>>(Model::schemaContains(key, doc) ? doc[key] : defValue);
        for (auto item : value) {
          result[key][item.first] = item.second;
        }
      }
    }
  } catch (std::exception e) {
    std::cout << e.what() << std::endl;
  }

  return result;
}

JsonDB::Model::SchemaItem JsonDB::Model::parseItem(std::string key, Json::Value jsonValue) {
  JsonDB::Model::SchemaItem result;
  
  Model::SchemaItem fieldType = this->schema[key];
  if (this->checkType<int>(key)) 
    result = jsonValue.asInt();
  else if (this->checkType<float>(key)) 
    result = jsonValue.asFloat();
  else if (this->checkType<std::string>(key))
    result = jsonValue.asString();
  else if (this->checkType<std::vector<std::string>>(key)) {
    std::vector<std::string> value;
    for (auto item : jsonValue) {
      value.push_back(item.asString());
    }
    result = value;
  } else if (this->checkType<std::map<std::string, std::string>>(key)) {
    std::map<std::string,std::string> value;
    for (auto objKey : jsonValue.getMemberNames()) {
      value[objKey] = jsonValue[objKey].asString();
    }
    result = value;
  }
  
  return result;
}

JsonDB::Model::Schema JsonDB::Model::parse(Json::Value json) {
  using namespace JsonDB;

  Model::Schema result;

  try {
    for (auto item : this->schema) {
      auto key = item.first;
      auto defValue = item.second;
      Json::Value jsonValue = json[key];
      if (this->checkType<int>(key)) 
        result[key] = !jsonValue.empty() > 0 ? jsonValue.asInt() : 0;
      else if (this->checkType<float>(key)) 
        result[key] = !jsonValue.empty() > 0 ? jsonValue.asFloat() : (float)0;
      else if (this->checkType<std::string>(key))
        result[key] = !jsonValue.empty() > 0 ? jsonValue.asString() : "";
      else if (this->checkType<std::vector<std::string>>(key)) {
        std::vector<std::string> value;
        for (auto item : jsonValue) {
          value.push_back(item.asString());
        }
        result[key] = value;
      } else if (this->checkType<std::map<std::string, std::string>>(key)) {
        std::map<std::string,std::string> value;
        for (auto objKey : jsonValue.getMemberNames()) {
          value[objKey] = jsonValue[objKey].asString();
        }
        result[key] = value;
      }
   }
  } catch(std::exception e) {
    std::cout << e.what() << std::endl;
  }

  return result;
};

bool JsonDB::Model::mapCompare(std::map<std::string,std::string> &lhs, std::map<std::string,std::string> &rhs, JsonDB::Model::CompareModes modes) {
  if (modes.oneOf) {
    for (auto item : lhs) {
      auto key = item.first;
      auto value = item.second;

      if (lhs.find(key) != rhs.end() && lhs[key].compare(rhs[key]) == 0) return true;
    }
    return false;
  }
  return lhs.size() == rhs.size()
         && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

bool JsonDB::Model::compareItem(std::string key, JsonDB::Model::SchemaItem left, JsonDB::Model::SchemaItem right, CompareModes modes) {
  if (this->checkType<int>(key)) {
    auto lValue = std::get<int>(left);
    auto rValue = std::get<int>(right);
    if (lValue != rValue) return false;
  } else if (this->checkType<float>(key)) {
    auto lValue = std::get<float>(left);
    auto rValue = std::get<float>(right);
    if (lValue != rValue) return false;
  } else if (this->checkType<std::string>(key)) { 
    auto lValue = std::get<std::string>(left);
    auto rValue = std::get<std::string>(right);
    if (lValue.compare(rValue) != 0) return false;
  } else if (this->checkType<std::vector<std::string>>(key)) {
    auto lValue = std::get<std::vector<std::string>>(left);
    auto rValue = std::get<std::vector<std::string>>(right);
    if (lValue != rValue) return false;
  } else if (this->checkType<std::map<std::string, std::string>>(key)) {
    auto lValue = std::get<std::map<std::string, std::string>>(left);
    auto rValue = std::get<std::map<std::string, std::string>>(right);
    if (!Model::mapCompare(lValue, rValue, modes)) return false;
  }
  
  return true;
}

bool JsonDB::Model::compare(JsonDB::Model::Schema left, JsonDB::Model::Schema right) {
  using namespace JsonDB;
  
  for (auto item : this->schema) {
    std::string key = item.first;
    if (!this->compareItem(key, left[key], right[key])) return false;
  }
  
  return true;
}

JsonDB::Model& JsonDB::Model::getInstance(JsonDB::Model::Schema schema, std::string collection, std::string path) {
  using namespace JsonDB;
  
  static std::map<std::string, std::shared_ptr<JsonDB::Model>> instances;
  std::string key = collection + path;

  if (auto it (instances.find(key)); it == instances.end()) {
    instances[key] = std::make_shared<Model>(schema, collection, path);  
  }
  return *instances[key];
}