#include "ModelManager.h"
#include "Engine/Timer/Timer.h"

ModelManager::ModelManager(std::shared_ptr<ImageManager> imageManager) : 
    imageManager(imageManager)
{
}

std::shared_ptr<Model> ModelManager::LoadModel(const std::string& path)
{
    std::unique_lock<std::mutex> lock(loadMutex);

    std::cout << "Main thread ImmediateQueue memory: " << Vk::VulkanContext::GetContext()->GetImmediateQueue().get() << std::endl;

    if (models.find(path) != models.end())
        return models.at(path);

    std::shared_ptr<Model> model = std::make_shared<Model>(imageManager);
    models[path] = model;

    futures.emplace(path, std::async(std::launch::async, &Model::Load, models.at(path), path));
    
    return models.at(path);
}

std::shared_ptr<Model> ModelManager::GetModel(const std::string& path)
{
    if (models.find(path) == models.end())
        return nullptr;

    return models.at(path);
}

void ModelManager::Update()
{
    //Need the lock, might delete future from futures map while LoadModel inserts future in it.
    std::unique_lock<std::mutex> lock(loadMutex);
    
    for (auto it = futures.begin(); it != futures.end();) {
        if (it->second.valid() && it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            try {
                it->second.get();
                it = futures.erase(it);
                std::cout << "Async model loading thread finished successfuly" << "\n";
            }
            catch (const std::exception& e) {
                std::cout << "Async model loading thread error: " << e.what() << std::endl;
                it = futures.erase(it);
            }
        }
        else {
            ++it;
        }
    }

    for (auto& [path, model] : models)
    {
        if (futures.find(path) == futures.end() && model->state == LoadState::GpuUploaded)
            model->state = LoadState::Ready;
    }
}
