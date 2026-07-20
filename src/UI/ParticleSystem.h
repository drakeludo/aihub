#pragma once

#include <vector>
#include <random>
#include <imgui.h>

struct Particle {
    ImVec2 position;
    ImVec2 velocity;
    ImVec4 color;
    float size;
    float lifetime;
    float age;
    
    bool isAlive() const {
        return age < lifetime;
    }
    
    void update(float dt) {
        age += dt;
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
        
        // Fade out
        float alpha = 1.0f - (age / lifetime);
        color.w = alpha;
    }
};

class ParticleSystem {
public:
    ParticleSystem(int maxParticles = 100) 
        : maxParticles_(maxParticles) {
        particles_.reserve(maxParticles);
    }
    
    void emit(const ImVec2& position, const ImVec4& color, float size = 2.0f) {
        if (particles_.size() >= maxParticles_) {
            return;
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> angleDist(0.0f, 6.28318f);
        std::uniform_real_distribution<float> speedDist(20.0f, 100.0f);
        std::uniform_real_distribution<float> lifeDist(0.5f, 2.0f);
        
        Particle p;
        p.position = position;
        
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        p.velocity = ImVec2(std::cos(angle) * speed, std::sin(angle) * speed);
        
        p.color = color;
        p.size = size;
        p.lifetime = lifeDist(gen);
        p.age = 0.0f;
        
        particles_.push_back(p);
    }
    
    void update(float dt) {
        for (auto& p : particles_) {
            p.update(dt);
        }
        
        // Remove dead particles
        particles_.erase(
            std::remove_if(particles_.begin(), particles_.end(),
                [](const Particle& p) { return !p.isAlive(); }),
            particles_.end()
        );
    }
    
    void render(ImDrawList* drawList) {
        for (const auto& p : particles_) {
            ImU32 col = ImGui::ColorConvertFloat4ToU32(p.color);
            drawList->AddCircleFilled(p.position, p.size, col);
        }
    }
    
    void clear() {
        particles_.clear();
    }
    
    size_t getParticleCount() const {
        return particles_.size();
    }
    
private:
    std::vector<Particle> particles_;
    size_t maxParticles_;
};
