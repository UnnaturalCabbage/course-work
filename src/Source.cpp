#include <stdlib.h>
#include <random>
#include "DatingDB.h"

class DatingDBTestDriver {
private:
   DatingDB *db;
   std::string currUser = "";
   
   JsonDB::Model::Schema getCurrUser() {
      return this->db->DB::find(currUser);
   }

   struct TestCandidate {
      std::string firstname[4] = {"Bohdan", "Andry", "Kate", "Anny"};
      std::string lastname[4] = {"Skovoroda", "Schevchenko", "Gagarin", "Dan"};
      std::string sex[2] = {"male", "female"};
      std::time_t regDate[3] = {time(nullptr), time(nullptr), time(nullptr)};
      std::map<std::string, std::string> about {{
                                          "strong", "1"
                                       }, {
                                          "beautiful", "1"
                                       }, {
                                          "honest", "1"
                                       }, {
                                          "kind", "1"
                                       }, {
                                          "friendly", "1"
                                       }, {
                                          "rude", "1"
                                       }, {
                                          "stict", "1"
                                       }};
      std::map<std::string, std::string> requirements {{
                                          "strong", "1"
                                       }, {
                                          "beautiful", "1"
                                       }, {
                                          "honest", "1"
                                       }, {
                                          "kind", "1"
                                       }, {
                                          "friendly", "1"
                                       }, {
                                          "rude", "1"
                                       }, {
                                          "stict", "1"
                                       }};
   };
   
   int randNum(int limit) {
      std::random_device rd;
      std::default_random_engine engine(rd());
      std::uniform_int_distribution<int> uniform_dist(0, limit);
      return uniform_dist(engine);
   }
   
   std::map<std::string, std::string> getRandItems(std::map<std::string, std::string> map) {
      std::map<std::string, std::string> result;
      for (auto item : map) {
         if (rand() % 3 == 0) {
            result[item.first] = item.second;
         }
      }
      return result;
   }
   
public:
   DatingDBTestDriver(DatingDB *db) {
      this->db = db;
   }
   void fillDB(int num) {
      for (int i = 0; i < num; i++) {
         DatingDBTestDriver::TestCandidate testCand;
         JsonDB::Model::Schema cand {{
                                       "firstname", testCand.firstname[this->randNum(3)]
                                    }, {
                                       "lastname", testCand.lastname[this->randNum(3)]
                                    }, {
                                       "age", (std::rand() % 24) + 16
                                    }, {
                                       "sex", testCand.sex[this->randNum(1)]
                                    }, {
                                       "about", getRandItems(testCand.about)
                                    }, {
                                       "requirements", getRandItems(testCand.requirements)
                                    }, {
                                       "inbox", std::map<std::string, std::string>()
                                    }, {
                                       "sent", std::map<std::string, std::string>()
                                    }};
         this->db->save(cand);
         this->db->write();
         this->currUser = this->db->getId(this->db->find({{ "sex", "male" }})[0]);
      }
   }
   
   void changeAccount(std::string newId) {
      this->currUser = newId;
   }
   
   void displayMenu() {
      using namespace std;
      
      cout << "Мой профиль" << endl <<
              "Имя, фамилия: " << this->db->getFirstname(this->getCurrUser()) << " " << this->db->getLastname(this->getCurrUser()) << endl <<
              "Возраст: " << this->db->getAge(this->getCurrUser()) << endl <<
              "Пол: " << this->db->getSex(this->getCurrUser()) << endl;
      cout << "О мне:" << endl;
      for (auto item : this->db->getAbout(this->getCurrUser())) {
         cout << " -- " << item.first << endl;
      }
      cout << "Мои требования:" << endl;
      for (auto item : this->db->getReq(this->getCurrUser())) {
         cout << " -- " << item.first << endl;
      }
      cout << "1. Найти по имени" << endl
           << "2. Подобрать мне" << endl
           << "3. Отправить запрос" << endl
           << "4. Посмотреть входящие" << endl
           << "5. Посмотреть отправленные" << endl
           << "6. Принять запрос" << endl
           << "7. Поменять аккаунт" << endl;
      this->action(); 
   }
   
   void action() {
      using namespace std;
      
      int actionNum;
      cin >> actionNum;
      
      std::string name;
      std::string id;
      switch (actionNum) {
      case 1:
         cout << "Введите имя: ";
         cin >> name;
         for (auto item : this->db->find({{ "firstname", name }})) {
            cout << " -- " << this->db->getId(item) << " " << this->db->getFirstname(item) << " " << this->db->getLastname(item) << endl;
         }
         cout << endl << endl;
         this->displayMenu();
         break;
      case 2:
         cout << "У вас с этими людьми 100% совпадение по вашим требованиям: " << endl;
         for (auto item : this->db->match(this->getCurrUser(), 3)) {
            cout << " -- " << this->db->getId(item) << " " << this->db->getFirstname(item) << " " << this->db->getLastname(item) << endl;
         }
         cout << endl << endl;
         system("pause");
         this->displayMenu();
         break;
      case 3:
         cout << "Введите id: ";
         cin >> id;
         if (this->db->send(this->db->getId(this->getCurrUser()), id, "Привет, давай познакомимся!"))
            cout << "Запрос успешно отправлен" << endl << endl;
         else
            cout << "Не удалось отправить запрос" << endl << endl;
         this->displayMenu();
         break;
      case 4:
         cout << "Входящие запросы" << endl;
         for (auto item : this->db->getInbox(this->getCurrUser())) {
            auto id = item.first;
            auto msg = item.second;
            auto user = this->db->DB::find(id);
            cout << " -- " << this->db->getId(user) << " " << this->db->getFirstname(user) << " " << this->db->getLastname(user) << endl << msg << endl;
         }
         cout << endl;
         system("pause");
         this->displayMenu();
         break;
      case 5:
         cout << "Отправленные запросы" << endl;
         for (auto item : this->db->getSent(this->getCurrUser())) {
            auto id = item.first;
            auto msg = item.second;
            auto user = this->db->DB::find(id);
            cout << " -- " << id << " " << this->db->getFirstname(user) << " " << this->db->getLastname(user) << endl << msg << endl;
         }
         cout << endl;
         system("pause");
         this->displayMenu();
         break;
      case 6:
         cout << "Принять запрос от: ";
         cin >> id;
         if (this->db->accept(this->getCurrUser(), id))
            cout << "Запрос успешно принят!" << endl;
         else { 
            cout << "Не удалось принять запрос" << endl;
            this->displayMenu();
         }
         break;
      case 7:
         cout << "Введите id нового аккаунта: ";
         cin >> id;
         this->changeAccount(id);
         this->displayMenu();
         break;
      }
   }
};

int main() {
   using namespace std;
   
   setlocale(0, ""); 
   
   DatingDB users;
   DatingDBTestDriver testDriver(&users);
   testDriver.fillDB(20);
   testDriver.displayMenu(); 
   
   system("pause");
   return 0;
}