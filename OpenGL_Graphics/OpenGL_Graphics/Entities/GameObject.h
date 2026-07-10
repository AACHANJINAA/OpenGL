#pragma once
#include <vector>
#include <memory>
#include <string>
#include "../Components/Component.h"

class GAMEOBJECT {
private:
    std::vector<std::unique_ptr<COMPONENT>> _components;
    std::string _name;

public:
    GAMEOBJECT(const std::string& name = "GameObject") : _name(name) {}
    ~GAMEOBJECT() = default;

    void start();
    void update(float delta_time);
    void late_update(float delta_time);
    void render();
    void end();

    void set_name(const std::string& name) { _name = name; }
    std::string get_name() const { return _name; }

    template<typename T, typename... Args>
    T* add_component(Args&&... args) {
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->set_owner(this);
        T* raw_ptr = comp.get();
        _components.push_back(std::move(comp));
        return raw_ptr;
    }

    template<typename T>
    T* get_component() {
        for (auto& component : _components) {
            T* ptr = dynamic_cast<T*>(component.get());
            if (ptr != nullptr) {
                return ptr;
            }
        }
        return nullptr;
    }
};
