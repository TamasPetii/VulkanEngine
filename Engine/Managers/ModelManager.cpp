#include "ModelManager.h"
#include "Engine/Timer/Timer.h"

ModelManager::ModelManager(std::shared_ptr<ImageManager> imageManager) : 
    imageManager(imageManager)
{
}

std::shared_ptr<Model> ModelManager::LoadModel(const std::string& path)
{
    if (models.find(path) != models.end())
        return models.at(path);

    std::shared_ptr<Model> model = std::make_shared<Model>(imageManager);

    Timer timer{};
    if (!model->Load(path))
        return nullptr;

    std::cout << "-----------------------------------------\n";
    std::cout << "Model Loaded: " << timer.GetElapsedTime() << "\n";
    std::cout << "-----------------------------------------\n";

    models[path] = model;
    return models.at(path);
}

std::shared_ptr<Model> ModelManager::GetModel(const std::string& path)
{
    if (models.find(path) == models.end())
        return nullptr;

    return models.at(path);
}
