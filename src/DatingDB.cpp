#include "DatingDB.h"

DatingDB::DatingDB() : JsonDB::DBService(&JsonDB::Model::getInstance({{
                           "_id", std::string()
                        }, {
                           "firstname", std::string()
                        }, {
                           "lastname", std::string()
                        }, {
                           "age", int()
                        }, {
                           "sex", std::string()
                        }, {
                           "about", std::map<std::string, std::string>()
                        }, {
                           "requirements", std::map<std::string, std::string>()
                        }, {
                           "inbox", std::map<std::string, std::string>()
                        }, {
                           "sent", std::map<std::string, std::string>()
                        }}, "users", "./data.json"), std::vector<std::string> {"sex"}) {
                           
   this->archive = new DBService(&JsonDB::Model::getInstance({{
                           "_id", std::string()
                        }, {
                           "firstname", std::string()
                        }, {
                           "lastname", std::string()
                        }, {
                           "age", int()
                        }, {
                           "sex", std::string()
                        }, {
                           "pair", std::string()
                        }}, "archive", "./data.json"), std::vector<std::string> {"sex"});
}
DatingDB::~DatingDB() {
   delete this->archive;
   delete this->model;
}

std::string DatingDB::getId(JsonDB::Model::Schema person) {
   return std::get<std::string>(person["_id"]);
}

std::string DatingDB::getFirstname(JsonDB::Model::Schema person) {
   return std::get<std::string>(person["firstname"]);
}

std::string DatingDB::getLastname(JsonDB::Model::Schema person) {
   return std::get<std::string>(person["lastname"]);
}

int DatingDB::getAge(JsonDB::Model::Schema person) {
   return std::get<int>(person["age"]);
}

std::string DatingDB::getSex(JsonDB::Model::Schema person) {
   return std::get<std::string>(person["sex"]);
}

std::map<std::string, std::string> DatingDB::getAbout(JsonDB::Model::Schema person) {
   return std::get<std::map<std::string, std::string>>(person["about"]);
}

std::map<std::string, std::string> DatingDB::getReq(JsonDB::Model::Schema person) {
   return std::get<std::map<std::string, std::string>>(person["requirements"]);
}

std::map<std::string, std::string> DatingDB::getInbox(JsonDB::Model::Schema person) {
   return std::get<std::map<std::string, std::string>>(person["inbox"]);
}

std::map<std::string, std::string> DatingDB::getSent(JsonDB::Model::Schema person) {
   return std::get<std::map<std::string, std::string>>(person["sent"]);
}


std::vector<JsonDB::Model::Schema> DatingDB::match(JsonDB::Model::Schema person, const int limit) {
   std::vector<JsonDB::Model::Schema> result;
   
   auto searchResult = this->find({{
                                    "sex", this->getSex(person).compare("male") == 0 ? "female" : "male"
                                 }, {
                                    "about", this->getReq(person)
                                 }});
   result.insert(result.begin(), searchResult.begin(), std::min(searchResult.begin() + limit, searchResult.end()));
   
   return result;
}

bool DatingDB::send(std::string from, std::string to, std::string message) {
   auto fromUser = JsonDB::DB::find(from);
   auto toUser = JsonDB::DB::find(to);
   if (this->getId(toUser).empty()) return false;
   auto newSent = this->getSent(fromUser);
   newSent[this->getId(toUser)] = message;
   fromUser["sent"] = newSent;
   auto newInbox = this->getInbox(toUser);
   newInbox[this->getId(fromUser)] = message;
   toUser["inbox"] = newInbox;
   this->save(fromUser);
   this->save(toUser);
   this->write();
   return true;
}

bool DatingDB::accept(JsonDB::Model::Schema user, std::string id) {
   if (this->getInbox(user).find(id) == this->getInbox(user).end()) return false;
   JsonDB::Model::Schema userPair = this->DB::find(id);
   
   this->archive->save({{
      "_id", this->getId(user)
   }, {
      "firstname", this->getFirstname(user)
   }, {
      "lastname", this->getLastname(user)
   }, {
      "age", this->getAge(user)
   }, {
      "sex", this->getSex(user)
   }, {
      "pair", this->getId(userPair)
   }});
   this->archive->save({{
      "_id", this->getId(userPair)
   }, {
      "firstname", this->getFirstname(userPair)
   }, {
      "lastname", this->getLastname(userPair)
   }, {
      "age", this->getAge(userPair)
   }, {
      "sex", this->getSex(userPair)
   }, {
      "pair", this->getId(user)
   }});
   this->archive->write();
   this->remove(this->getId(user));
   this->remove(this->getId(userPair));
   this->write();
   
   return true;
}

std::string DatingDB::addUser(std::string firstname, std::string lastname, int age, std::string sex, std::map<std::string, std::string> about, std::map<std::string, std::string> req) {
   JsonDB::Model::Schema newUser = this->save({{
                  "firstname", firstname
               }, {
                  "lastname", lastname
               }, {
                  "age", age
               }, {
                  "sex", sex
               }, {
                  "about", about
               }, {
                  "requirements", req
               }});
   this->write();         
   return std::get<std::string>(newUser["_id"]);
}

void DatingDB::removeUser(std::string id) {
   this->remove(id);
   this->write();  
}