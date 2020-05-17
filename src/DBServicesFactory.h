#pragma once

#include "DBService.h"
#include "DatingDB.h"

namespace JsonDB {
class DBServicesFactory {
public:
   virtual DBService* createDB() = 0;
   virtual ~DBServicesFactory() = default;
};
}

class DatingDBFactory : public JsonDB::DBServicesFactory {
public:
   JsonDB::DBService* createDB() {
      return new DatingDB;
   }
};