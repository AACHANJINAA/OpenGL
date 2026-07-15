#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class GAMEOBJECT;

// Base command interface for Undo transactions
class COMMAND {
public:
    virtual ~COMMAND() = default;
    virtual void undo() = 0;
};

// Undo transaction for Spawning shapes
class SPAWNCOMMAND : public COMMAND {
private:
    std::shared_ptr<GAMEOBJECT> _go;
public:
    SPAWNCOMMAND(const std::shared_ptr<GAMEOBJECT>& go);
    void undo() override;
};

// Undo transaction for Deleting objects
class DELETECOMMAND : public COMMAND {
private:
    std::vector<std::shared_ptr<GAMEOBJECT>> _deleted_gos;
public:
    DELETECOMMAND(const std::vector<std::shared_ptr<GAMEOBJECT>>& list);
    void undo() override;
};

// Snapshot for Transform coordinates
struct TransformSnapshot {
    std::shared_ptr<GAMEOBJECT> go;
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;
};

// Undo transaction for moving/rotating/scaling objects
class TRANSFORMCOMMAND : public COMMAND {
private:
    std::vector<TransformSnapshot> _snapshots;
public:
    TRANSFORMCOMMAND(const std::vector<TransformSnapshot>& snapshots);
    void undo() override;
};
