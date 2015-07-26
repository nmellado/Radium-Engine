#ifndef RADIUMENGINE_ENTITYMANAGER_HPP
#define RADIUMENGINE_ENTITYMANAGER_HPP

#include <memory>
#include <vector>
#include <map>
#include <string>

#include <Core/Utils/Singleton.hpp>
#include <Core/Index/IndexMap.hpp>

namespace Ra { namespace Engine { class Entity; } }

namespace Ra { namespace Engine {

class EntityManager
{
public:
	/// CONSTRUCTOR
	EntityManager();

	/// DESTRUCTOR
	virtual ~EntityManager();

    /**
     * @brief Get or create (if does not exist) an entity given its name
     * @param name The name of the entity to ger or create
     * @return The entity.
     */
    Entity* getOrCreateEntity(const std::string& name);

    /**
     * @brief Create an entity (kind of a factory).
	 * A generic name (Entity_idx) is given to the entity.
     * Manager has the pointer ownership.
     * @return The created entity.
     */
    Entity* createEntity();

	/**
	 * @brief Check wether an entity with a given name exists or not.
	 * @param name The name of the entity to find
	 * @return true if the entity exists, false otherwise
	 */
	bool entityExists(const std::string& name) const;

    /**
     * @brief Remove an entity given its index. Also deletes the pointer.
     * @param idx Index if the entity to remove.
     */
    void removeEntity(Core::Index idx);

    /**
     * @brief Remove a given entity. Also deletes the pointer.
     * @param entity The entity to remove.
     */
    void removeEntity(Entity* entity);

    /**
     * @brief Get an entity given its index.
     * @param idx Index of the component to retrieve.
     * @return The entity if found in the map, nullptr otherwise.
     */
    Entity* getEntity(Core::Index idx) const;

    /**
     * @brief Get all entities from the manager.
     * This might be usefull to be able to display and navigate through them
     * in a GUI for example.
     * @return A list containing all entities from the manager.
     */
    std::vector<Entity*> getEntities() const;

private:

    /**
     * @brief Create an entity given its name
     * @param name Name of the entity to create
     * @return The created entity
     */
    Entity* createEntity(const std::string& name);

    /**
     * @brief Get an entity given its name.
     * @param name Name of the entity to retrieve.
     * @return The entity if found in the map, nullptr otherwise.
     */
    Entity* getEntity(const std::string& name) const;

private:
    Core::IndexMap<std::shared_ptr<Entity>> m_entities;
	std::map<std::string, Core::Index> m_entitiesName;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENTITYMANAGER_HPP
