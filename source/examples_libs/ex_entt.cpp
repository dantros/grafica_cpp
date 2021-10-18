/*
 * Extracted from https://github.com/skypjack/entt/tree/v3.8.1 and modified.
 */

#include <iostream>
#include <entt/entt.hpp>

struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float dx;
    float dy;
};

std::ostream& operator<<(std::ostream& os, const Position& position)
{
    os << "p(" << position.x << ", " << position.y << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Velocity& velocity)
{
    os << "v(" << velocity.dx << ", " << velocity.dy << ")";
    return os;
}

void update(entt::registry &registry)
{
    // getting a view of all the entities with "const Position" and "Velocity" components
    auto view = registry.view<const Position, Velocity>();

    std::cout << std::endl << "Use a callback" << std::endl;

    view.each([](const auto &pos, auto &vel)
        {
            std::cout << "pos = " << pos << "    |    vel = " << vel << std::endl;
        });


    std::cout << std::endl << "Use an extended callback" << std::endl;

    view.each([](const auto entity, const auto &pos, auto &vel)
        {
            std::cout << "entity = " << static_cast<unsigned int>(entity)
                << "    |    pos = " << pos << "    |    vel = " << vel << std::endl;
        });

    std::cout << std::endl << "Use a range-for" << std::endl;

    for(auto [entity, pos, vel]: view.each())
    {
        std::cout << "entity = " << static_cast<unsigned int>(entity)
            << "    |    pos = " << pos << "    |    vel = " << vel << std::endl;
    }

    std::cout << std::endl << "Use forward iterators and get only the components of interest" << std::endl;
    for(auto entity: view)
    {
        auto &pos = view.get<const Position>(entity);
        auto &vel = view.get<Velocity>(entity);
        
        std::cout << "entity = " << static_cast<unsigned int>(entity)
            << "    |    pos = " << pos << "    |    vel = " << vel << std::endl;
    }

    std::cout << std::endl << "Getting a view of all the entities with \"Position\" and translate them." << std::endl;
    auto view2 = registry.view<Position>();

    for (auto entity : view2)
    {
        auto& pos = view2.get<Position>(entity);

        pos.x += 10.0f;
        pos.y += 10.0f;

        std::cout << "entity = " << static_cast<unsigned int>(entity)
            << "    |    pos = " << pos << std::endl;
    }
}

int main()
{
    entt::registry registry;

    for(auto i = 0u; i < 10u; ++i)
    {
        // Note: entity is an unsigned integer. We always need the registry to work with it.
        const auto entity = registry.create();

        // Adding a position component
        registry.emplace<Position>(entity, i * 1.f, i * 1.f);

        if(i % 2 == 0)
        {
            // Adding a velocity component only on even entities
            registry.emplace<Velocity>(entity, i * .1f, i * .1f);
        }
    }

    update(registry);
}
