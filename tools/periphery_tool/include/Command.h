#pragma once
#include <cli/clilocalsession.h>
#include <utility>
#include <vector>

using namespace cli;
using namespace std;

class Device
{
public:
    explicit Device(string _name) : name(std::move(_name)) { cout << "Device " << name << " loaded" << endl; }
    virtual ~Device() { cout << "Device " << name << " unload" << endl; }
    const string& Name() const { return name; }
private:
    const string name;
};

using Factory = function<unique_ptr<Device>(Menu*)>;

class DeviceRegistry
{
public:
    static DeviceRegistry& Instance() { return instance; }
    void Register(const string& name, Factory factory) { devices.emplace_back(name, factory); }
    void SetMenu(Menu& _menu) { menu = &_menu; }
    void LoadAll() {
        for (auto& p: devices)
		_devices.emplace_back(p.second(menu));
    }
private:
    static DeviceRegistry instance;
    vector<pair<string, Factory>> devices;
    vector<unique_ptr<Device>> _devices;
    Menu* menu;
};

class Registration
{
public:
    Registration(const string& name, Factory factory)
    {
        DeviceRegistry::Instance().Register(name, std::move(factory));
    }
};

template <typename T, const char* NAME>
class RegisteredDevice : public Device
{
public:
    RegisteredDevice() : Device(NAME)
    {
        const Registration& dummy = registration;
        do { (void)(dummy); } while (false); // silence unused variable warning
    }
private:
    static unique_ptr<Device> Create(Menu* menu) { return make_unique<T>(menu); }
    static Registration registration;
};

template <typename T, const char* NAME>
Registration RegisteredDevice<T, NAME>::registration(NAME, &RegisteredDevice<T, NAME>::Create);
