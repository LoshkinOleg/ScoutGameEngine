#pragma once
/*
namespace hs
{
    class Projectile
    {
    public:
        bool InFlight() const
        {
            return timer_ > -PARTICLES_STATE_LIFETIME_;
        }
        void Init(const glm::vec3 color)
        {
            color_ = color;
            glGenVertexArrays(1, &VAO_);
            glBindVertexArray(VAO_);
            glGenBuffers(1, &VBO_);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            glBufferData(GL_ARRAY_BUFFER, positions_.size() * sizeof(glm::vec2), positions_.data(), GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        }
        void Launch(const glm::vec2 firingTankPos, const float firingTankGunPos)
        {
            // Launch projectile.
            timer_ = PROJECTILE_STATE_LIFETIME_;
            avgPos_ = ZERO_VEC3;
            for(size_t i = 0; i < NR_OF_PARTICLES_FOR_PROJECTILE; i++)
            {
                const float x = glm::cos((float)i);
                const float y = glm::sin((float)i);
                positions_[i] = firingTankPos + glm::vec2(x, y) * PROJECTILE_STATE_PIXELS_SPREAD_;
                avgPos_ += positions_[i];
            }
            avgPos_ /= NR_OF_PARTICLES_FOR_PROJECTILE;
            dir_ = glm::normalize(glm::vec2(glm::cos(firingTankGunPos), glm::sin(firingTankGunPos))); // TODO: is this normalize necessary?
        }
        void Update(const Rectangle enemyHitbox, const float dt, std::function<void()> onHit, Shader& shader)
        {
            if(InFlight())
            {
                if(timer_ > 0.0f) // Projectile is in it's projectile state.
                {
                    const bool contact = enemyHitbox.Intersect(avgPos_); // Check for collision with an enemy tank.

                    avgPos_ = ZERO_VEC3;
                    for(size_t i = 0; i < NR_OF_PARTICLES_FOR_PROJECTILE; i++)
                    {
                        positions_[i] += dir_ * dt * PROJECTILE_MOV_SPEED_;
                        avgPos_ += positions_[i];
                    }
                    avgPos_ /= NR_OF_PARTICLES_FOR_PROJECTILE;

                    if(contact)
                    {
                        timer_ = 0.0f; // Put projectile into it's particles state immediately.
                        onHit();
                    }
                }

                // Upload to gpu and issue draw call.
                shader.Bind();
                shader.SetFloat({ "timer", timer_ });
                shader.SetVec3({ "color", color_ });
                glBindVertexArray(VAO_);
                glBindBuffer(GL_ARRAY_BUFFER, VBO_);
                glBufferSubData(GL_ARRAY_BUFFER, 0, positions_.size() * sizeof(glm::vec2), positions_.data());
                glDrawArrays(GL_POINTS, 0, (GLsizei)positions_.size());

                timer_ -= dt; // Must be at end of Draw for dir_ to be generated.
            }
        }
        void Destroy()
        {
            glDeleteBuffers(1, &VBO_);
            glDeleteVertexArrays(1, &VAO_);
        }
    private:
        constexpr static const float PROJECTILE_MOV_SPEED_ = 20.0f;
        constexpr static const float PROJECTILE_STATE_LIFETIME_ = 1.0f;
        constexpr static const float PROJECTILE_STATE_PIXELS_SPREAD_ = 0.033f;

        constexpr static const float PARTICLES_STATE_LIFETIME_ = 0.5f;

        unsigned int VAO_ = 0, VBO_ = 0;
        std::array<glm::vec2, NR_OF_PARTICLES_FOR_PROJECTILE> positions_ = {};
        glm::vec2 dir_ = ZERO_VEC3;
        glm::vec2 avgPos_ = ZERO_VEC3;
        glm::vec3 color_ = ONE_VEC3;

        // Uniforms.
        float timer_ = -PARTICLES_STATE_LIFETIME_ - 0.1f; // ----PARTICLE--- 0 ++++++PROJECTILE++++
    };
}//!hs

*/