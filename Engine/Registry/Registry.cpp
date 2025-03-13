#include "Registry.h"
#include "Relationship.h"

Entity Registry::CreateEntity()
{
    Entity entity = NULL_ENTITY;

    if (destroyedEntities.empty())
    {
        entity = counter++;
        bitsetComponents.push_back(std::bitset<MAX_COMPONENTS>{});
    }
    else
    {
        auto it = destroyedEntities.begin();
        entity = *it;
        destroyedEntities.erase(it);
    }

    AddComponents<Relationship>(entity);

    return entity;
}

void Registry::DestroyEntity(Entity entity)
{
    //Remove the parent-child relations

    //Remove from all the component pools
    for (auto& [typeID, pool] : pools)
        pool->RemoveEntity(entity);
    
    destroyedEntities.insert(entity);
    bitsetComponents[entity].reset();
}
