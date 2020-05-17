#pragma once

#include <time.h>
#include <functional>
#include "DBService.h"

class DatingDB : public JsonDB::DBService {
private:
   JsonDB::DBService *archive;
public:
   DatingDB();
   ~DatingDB();
   
   std::string getId(JsonDB::Model::Schema person);
   std::string getFirstname(JsonDB::Model::Schema person);
   std::string getLastname(JsonDB::Model::Schema person);
   int getAge(JsonDB::Model::Schema person);
   std::string getSex(JsonDB::Model::Schema person);
   std::map<std::string, std::string> getAbout(JsonDB::Model::Schema person);
   std::map<std::string, std::string> getReq(JsonDB::Model::Schema person);
   std::map<std::string, std::string> getInbox(JsonDB::Model::Schema person);
   std::map<std::string, std::string> getSent(JsonDB::Model::Schema person);
   
   bool accept(JsonDB::Model::Schema user, std::string id);
   std::vector<JsonDB::Model::Schema> match(JsonDB::Model::Schema person, const int limit = 3);
   bool send(std::string from, std::string to, std::string message);
};