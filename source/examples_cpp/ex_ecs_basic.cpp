

#include <iostream>
#include <string>
#include <vector>
#include <memory>

struct Component
{
    virtual void f() {}  // must be polymorphic to use runtime-checked dynamic_cast
};

using ComponentPtr = std::shared_ptr<Component>;

struct Entity
{
    std::string name;
    std::vector<ComponentPtr> components;
};

struct Mesh : Component
{
    int meshId;

    Mesh(int meshId_) : meshId(meshId_) {};
};

struct Audio : Component
{
    int trackId;

    Audio(int trackId_) : trackId(trackId_) {};
};

void render(const Entity &entity)
{
    bool hasMesh = false;
    for (auto component : entity.components)
    {
        Mesh* mesh = dynamic_cast<Mesh*>(component.get());
        if (mesh != nullptr)
        {
            std::cout << entity.name << " -> meshId = " << mesh->meshId << std::endl;
            hasMesh = true;
        }
    }

    if (not hasMesh)
    {
        std::cout << entity.name << " does not have a mesh." << std::endl;
    }
}

int main()
{
    Entity entityA{ "Entity A" };
    entityA.components.push_back(std::make_shared<Mesh>(10));
    entityA.components.push_back(std::make_shared<Audio>(15));

    Entity entityB{ "Entity B" };
    entityB.components.push_back(std::make_shared<Mesh>(20));

    Entity entityC{ "Entity C" };
    entityC.components.push_back(std::make_shared<Audio>(7));

    std::vector<Entity> entities{ entityA , entityB, entityC};

    for (auto& entity : entities)
    {
        render(entity);
    }

    // This approach has a number of undesired features.
}

