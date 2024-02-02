#include "WareHouse.h"
#include "Action.h"
#include "Customer.h"
#include "Order.h"
#include "Volunteer.h"
using namespace std;
#include <fstream>
#include <iostream>
using std:: string;
#include <sstream>
#include <istream>


WareHouse::WareHouse(const string &configFilePath)
: isOpen(false),actionsLog(),volunteers(),pendingOrders(),inProcessOrders(),completedOrders(),customers() ,customerCounter(0),volunteerCounter(0),orderCounter(0){
    parseText(configFilePath);
};

    
void WareHouse::start(){
    open();
    
    while(isOpen){
        std::string input;
        std::cout << "Enter action: ";
        std::getline(std::cin, input);

        // Extract the action (first word)
        std::istringstream iss(input);
        std::string action;
        iss >> action;
        // Perform actions based on the first word
        if (action == "step") {
            // Extract the number for step action
            int number;
            if (iss >> number) {
                SimulateStep* step = new SimulateStep(number);
                addAction(step);
            } 
        } 
        else if (action == "order") {
            // Extract the number for order action
            int number;
            if (iss >> number) {
                AddOrder* order = new AddOrder(number);
                addAction(order);
            }      
        }  
        else if (action =="customer"){  
            std::string name; 
            std::string type;
            int distance;
            int maxOrd;  
            if(iss >> name >> type >> distance >> maxOrd){
                AddCustomer* cust = new AddCustomer(name,type, distance, maxOrd);
                addAction(cust);
            }            
        }
        else if (action =="orderStatus"){  
            int number;
            if (iss >> number) {
                PrintOrderStatus* print = new PrintOrderStatus(number);
                addAction(print);
            }    
        }
        else if (action =="customerStatus"){  
            int number;
            if (iss >> number) {
                PrintCustomerStatus* print = new PrintCustomerStatus(number);
                addAction(print);
            }    
        }
        else if (action =="volunteerStatus"){  
            int number;
            if (iss >> number) {
                PrintVolunteerStatus* print = new PrintVolunteerStatus(number);
                addAction(print);
            }  
        }
        else if (action =="log"){  
            PrintActionsLog* log = new PrintActionsLog();
            addAction(log);
        }
        else if (action =="close"){ 
            Close* close = new Close();
            addAction(close);
        }

        else if (action =="backup"){  
            BackupWareHouse* back = new BackupWareHouse();
            addAction(back);
        }

        else if (action =="restore"){  
            RestoreWareHouse* restore = new RestoreWareHouse();
            addAction(restore);
        }
        else{
            std::cout << "try again!" << std::endl;
        }

    }

};

void WareHouse::addOrder(Order* order){
    pendingOrders.push_back(order);
    orderCounter++;
}

void WareHouse::addAction(BaseAction* action){
    action->act(*this);
    actionsLog.push_back(action);
};

Customer& WareHouse::getCustomer(int customerId) const {
        Customer* no_cus = nullptr;
        // Iterate through the vector to search for the customer
        for (Customer* cus : customers) {
            // Check if the customerId of the current customer matches the given customerId
            if (cus->getId() == customerId) {
                // Return a reference to the found customer
                return *cus;
            }
        }
        return *no_cus;
    };

Volunteer& WareHouse::getVolunteer(int volunteerId) const {
        // Iterate through the vector to search for the customer
        Volunteer* no_voli = nullptr;
        for (Volunteer* voli : volunteers) {
            // Check if the customerId of the current customer matches the given customerId
            if (voli->getId() == volunteerId) {
                // Return a reference to the found customer
                return *voli;
            }
        }
        return *no_voli;
    };

Order& WareHouse::getOrder(int orderId) const {
        // Iterate through the vector to search for the customer
        Order *no_ori = nullptr;
       for(Order* ord : pendingOrders){
                if(ord->getId() == orderId){
                    return *ord;
                }
        }
         for(Order* ord : inProcessOrders){
                    if(ord->getId() == orderId){
                        return *ord;
                    }
         }
        for(Order* ord : completedOrders){
                    if(ord->getId() == orderId){
                        return *ord;
                    }
        }
        return *no_ori;
    };


void WareHouse::parseText(const string &configFilePath){
        ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "Error opening the configuration file." << std::endl;
        return;
    }

    customerCounter = 0;
    volunteerCounter = 0;
    std::string line;
    while (getline(configFile, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "customer") {
            // Parse customer
            std::string name;
            std::string customerType;
            int distance;
            int maxOrders;

            iss >> name >> customerType >> distance >> maxOrders;

            Customer *newCustomer = nullptr;
            if (customerType == "soldier") {
                newCustomer = new SoldierCustomer(customers.size(), name,
                                                  distance, maxOrders);
            } else if (customerType == "civilian") {
                newCustomer = new CivilianCustomer(customers.size(), name,
                                                   distance, maxOrders);
            } else {
                std::cerr << "Unknown customer type: " << customerType
                          << std::endl;
            }

            // add customer to warehouse
            if (newCustomer) {
                AddCustomer (name,customerType,distance,maxOrders).act(*this);
            }
        } else if (type == "volunteer") {
            // Parse volunteer
            std::string name;
            std::string volunteerType;
            int coolDown;
            int maxDistance;
            int distancePerStep;
            int maxOrders = -1;

            iss >> name >> volunteerType >> coolDown;

            Volunteer *newVolunteer = nullptr;
            if (volunteerType == "collector") {
                newVolunteer =
                    new CollectorVolunteer(volunteers.size(), name, coolDown);
            } else if (volunteerType == "limited_collector") {
                iss >> maxOrders;
                newVolunteer = new LimitedCollectorVolunteer(
                    volunteers.size(), name, coolDown, maxOrders);
            } else if (volunteerType == "driver") {
                iss >> maxDistance >> distancePerStep;
                newVolunteer = new DriverVolunteer(
                    volunteers.size(), name, maxDistance, distancePerStep);
            } else if (volunteerType == "limited_driver") {
                iss >> maxDistance >> distancePerStep >> maxOrders;
                newVolunteer = new LimitedDriverVolunteer(
                    volunteers.size(), name, maxDistance, distancePerStep,
                    maxOrders);
            } else {
                std::cerr << "Unknown volunteer type: " << volunteerType
                          << std::endl;
            }

            // add to warehouse
            if (newVolunteer) {
                volunteerCounter++;
                volunteers.push_back(newVolunteer);
            }
        }
    }

    configFile.close();
}
    
    

const vector<BaseAction*>& WareHouse::getActions() const{
    return actionsLog;
}


void WareHouse::close(){
    isOpen=false;
    //.....
}

void WareHouse::open(){
    std::cout << "Warehouse is open!" << std::endl;
    isOpen=true;
}

int WareHouse::getOrderCounter() const{
    return orderCounter;
};

int WareHouse::getVolunteerCounter() const{
    return volunteerCounter;
};

int WareHouse::getCustomerCounter() const{
    return customerCounter;
};


void WareHouse::fromPendingToinProcess(int orderID){
    for (auto it = pendingOrders.begin(); it != pendingOrders.end();++it){
        if((*it)->getId() == orderID ){
            inProcessOrders.push_back(*it);
            pendingOrders.erase(it);
            break;
        }
    }
};


void WareHouse::frominProcessToPending(int orderID){
    for (auto it = inProcessOrders.begin(); it != inProcessOrders.end();++it){
        if((*it)->getId() == orderID ){
           pendingOrders.push_back(*it);
           inProcessOrders.erase(it);
        }
    }
};

void WareHouse::frominProcessToCompleted(int orderID){
    for (auto it = inProcessOrders.begin(); it != inProcessOrders.end();++it){
        if((*it)->getId() == orderID){
           completedOrders.push_back(*it);
           inProcessOrders.erase(it);
        }
    }
};

void WareHouse::deleteVolunteer(Volunteer* volunteer){
    for (auto it = volunteers.begin(); it != volunteers.end();++it){
        if((*it)->getId() == volunteer->getId()){
           volunteers.erase(it);
           break;
        }
    }
}

void WareHouse::addCustomer(Customer* customer){
    customers.push_back(customer);
    customerCounter++;
};

vector<Customer*> WareHouse::getCustomerVector() const{
    return customers;
};

vector<Order*> WareHouse::getinProcessOrdersVector() const{
    return inProcessOrders;
};

vector<Order*> WareHouse::getCompletedOrdersVector() const{
    return completedOrders;
};

vector<Order*> WareHouse::getPendingOrderVector() const{
    return pendingOrders;
};

vector<BaseAction*> WareHouse::getActionsLog() const{
    return actionsLog;
};

vector<Volunteer*> WareHouse::getvolunteersVector() const{
    return volunteers;
};

WareHouse:: ~WareHouse(){
    for(auto customer : customers){
        delete customer;
    }
    customers.clear();

    for(auto voli : volunteers){
        delete voli;
    }
    volunteers.clear();

    for(auto ori : pendingOrders){
        delete ori;
    }
    pendingOrders.clear();

    for(auto ori : inProcessOrders){
        delete ori;
    }
    inProcessOrders.clear();

    for(auto ori : completedOrders){
        delete ori;
    }
    completedOrders.clear();

    for(auto action : actionsLog){
        delete action;
    }
    actionsLog.clear();

};

WareHouse::WareHouse(const WareHouse &other) {
    isOpen = other.isOpen;
    customerCounter = other.customerCounter;
    volunteerCounter = other.volunteerCounter;
    orderCounter = other.orderCounter;

    // Copy actionsLog
    for (BaseAction *action : other.actionsLog) {
        actionsLog.push_back(action->clone());
    }

    // Copy volunteers
    for (Volunteer *volunteer : other.volunteers) {
        volunteers.push_back(volunteer->clone());
    }

    // Copy pendingOrders
    for (Order *order : other.pendingOrders) {
        pendingOrders.push_back(order->clone());
    }

    // Copy inProcessOrders
    for (Order *order : other.inProcessOrders) {
        inProcessOrders.push_back(order->clone());
    }

    // Copy completedOrders
    for (Order *order : other.completedOrders) {
        completedOrders.push_back(order->clone());
    }

    // Copy customers
    for (Customer *customer : other.customers) {
        customers.push_back(customer->clone());
    }
}




// Move assignment operator
WareHouse& WareHouse::operator=(WareHouse &&other) noexcept {
    if (this == &other)
        return *this;

    // Move member variables
    isOpen = std::move(other.isOpen);
    actionsLog = std::move(other.actionsLog);
    volunteers = std::move(other.volunteers);
    pendingOrders = std::move(other.pendingOrders);
    inProcessOrders = std::move(other.inProcessOrders);
    completedOrders = std::move(other.completedOrders);
    customers = std::move(other.customers);
    customerCounter = std::move(other.customerCounter);
    volunteerCounter = std::move(other.volunteerCounter);
    orderCounter = std::move(other.orderCounter);

    // Reset the moved object
    other.isOpen = false;
    other.customerCounter = 0;
    other.volunteerCounter = 0;
    other.orderCounter = 0;
    other.actionsLog.clear();
    other.volunteers.clear();
    other.pendingOrders.clear();
    other.inProcessOrders.clear();
    other.completedOrders.clear();
    other.customers.clear();

    return *this;
}







