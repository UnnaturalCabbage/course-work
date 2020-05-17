#include "DB.h"

JsonDB::DB::DB() {}

JsonDB::DB::DB(JsonDB::Model *model) {
  this->model = model;
  this->collection = this->model->getCollection();
  this->path = this->model->getPath();
  JsonDB::DB::connect();
}

void JsonDB::DB::setModel(JsonDB::Model* model) {
  this->model = model;
}
JsonDB::Model* JsonDB::DB::getModel() {
  return this->model;
}

Json::Value JsonDB::DB::getRoot() {
  return this->root;
}

size_t JsonDB::DB::generateHash(Json::Value doc) {
  std::hash<std::string> hash;
  std::string hashStr = "";
  for (auto key : doc.getMemberNames()) {
    if (doc[key].isString() || doc[key].isInt()) {
      hashStr = hashStr + doc[key].asString();
    }
  }
  return hash(hashStr);
}

void JsonDB::DB::connect() {
  Json::Value fileContent = this->read();
  this->root = fileContent[this->collection];
}

JsonDB::Model::Schema JsonDB::DB::save(JsonDB::Model::Schema doc) {
  Json::Value jsonDoc = this->model->stringify(doc);
  std::string id = jsonDoc["_id"].asString();
  if (id.empty()) {
    id = std::to_string(this->generateHash(jsonDoc));
    jsonDoc["_id"] = id;
  }
  this->root[id] = jsonDoc;
  return this->model->parse(this->root[id]);
}

JsonDB::Model::Schema JsonDB::DB::update(JsonDB::Model::Schema doc) {
  Json::Value jsonDoc = this->model->stringify(doc);
  std::string id = jsonDoc["_id"].asString();
  if (id.empty()) {
    id = std::to_string(this->generateHash(jsonDoc));
    jsonDoc["_id"] = id;
  }
  for (auto key : jsonDoc.getMemberNames()) {
    this->root[id][key] = jsonDoc[key];
  }
  return this->model->parse(this->root[id]);
}

JsonDB::Model::Schema JsonDB::DB::remove(std::string id) {
  Json::Value doc =  this->root[id];
  this->root.removeMember(id);
  return this->model->parse(doc);
}

JsonDB::Model::Schema JsonDB::DB::find(std::string id) {
  return this->model->parse(this->root[id]);
}

Json::Value JsonDB::DB::read(std::string path) {
  std::ifstream file(path);
  if (!file) {
    std::cerr << "Error: failed to open " << path <<  std::endl;
  }
  Json::Value fileContent;
  file >> fileContent;
  file.close();
  return fileContent;
}

Json::Value JsonDB::DB::read() {
  return JsonDB::DB::read(this->path);
}

void JsonDB::DB::write(std::string path) {
  Json::Value fileContent = JsonDB::DB::read(path);
  fileContent[this->collection] = this->root;
  std::ofstream file(path);
  Json::StreamWriterBuilder builder;
  std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
  writer->write(fileContent, &file);
  file.close();
}

void JsonDB::DB::write() {
  this->write(this->model->getPath());
}