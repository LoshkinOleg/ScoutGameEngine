#pragma once
/*
namespace hs
{
    class A_Tank
    {
    public:
        Rectangle GetHitBox() const
        {
            return hitbox_;
        }
        glm::vec2 GetPos() const
        {
            return pos_;
        }
    protected:
        
    };

    class AiTank: public A_Tank
    {
    public:
        void Kill()
        {
            isDead_ = true;
        }
        void Init(const glm::vec2 startingPos, const glm::vec3 color, A_Tank* playerTank)
        {
            playerTank_ = playerTank;
            pos_ = startingPos;
            color_ = color;
            hitbox_.bottomLeft = startingPos - ToVec2(ONE_VEC3); // ONE because texture quad goes from -1;-1 to 1;1
            hitbox_.topRight = startingPos + ToVec2(ONE_VEC3);
            projectile_.Init(color);
        }
        void Update(const float dt, const glm::vec2 playerPos, const unsigned int VAO, Shader& tankShader, Shader& projectleShader, const unsigned int TEXs[2])
        {
            if(!isDead_)
            {
                reloadTimer_ -= dt;

                if(reloadTimer_ < 0.0f)
                {
                    reloadTimer_ = RELOAD_TIME_;
                    projectile_.Launch(pos_, gunRot_);
                }
                projectile_.Update
                (
                    playerTank_->GetHitBox(),
                    dt,
                    []() {},
                    projectleShader
                );

                // Rotate tank.
                bodyRot_ += PI * dt * TURN_MULT_ * 0.5f;

                // Move tank.
                glm::vec2 dir = glm::vec2(glm::cos(bodyRot_), glm::sin(bodyRot_));
                pos_ += dir * dt * TANK_SPEED_;
                hitbox_.bottomLeft = pos_ - ToVec2(ONE_VEC3);
                hitbox_.topRight = pos_ + ToVec2(ONE_VEC3);

                // Rotate tank's gun.
                const glm::vec2 relPlayerPos = playerPos - pos_;
                gunRot_ = glm::atan(relPlayerPos.y / relPlayerPos.x);
                if(relPlayerPos.x < 0.0f) gunRot_ += PI;

                glBindVertexArray(VAO);

                tankShader.Bind();
                glActiveTexture(GL_TEXTURE0);

                // Draw tank body.
                glm::mat4 model = glm::translate(IDENTITY_MAT4, ToVec3(pos_));
                model = glm::rotate(model, bodyRot_, FRONT_VEC3);
                tankShader.SetMat4({ "model", model });
                tankShader.SetVec3({ "color", color_ });
                glBindTexture(GL_TEXTURE_2D, TEXs[0]);
                glDrawArrays(GL_TRIANGLES, 0, 6);

                // Draw tank gun.
                model = glm::translate(IDENTITY_MAT4, ToVec3(pos_));
                model = glm::rotate(model, gunRot_, FRONT_VEC3);
                model = glm::scale(model, GUN_SCALE_);
                tankShader.SetMat4({ "model", model });
                glBindTexture(GL_TEXTURE_2D, TEXs[1]);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        void Destroy()
        {
            projectile_.Destroy();
        }
    private:

        constexpr static const float RELOAD_TIME_ = 1.5f;

        Projectile projectile_;
        float reloadTimer_ = RELOAD_TIME_;
        bool isDead_ = false;
        A_Tank* playerTank_ = nullptr;
    };

    class PlayerTank: public A_Tank
    {
        constexpr static const glm::vec3 GUN_SCALE_ = glm::vec3(1.5f, 0.5f, 1.0f);
        constexpr static const float TURN_MULT_ = 1.0f;
        constexpr static const float TANK_SPEED_ = 5.0f;

        glm::vec2 pos_ = ZERO_VEC3;
        float bodyRot_ = 0.0f;
        float gunRot_ = 0.0f;
        Rectangle hitbox_ = {};
        glm::vec3 color_ = WHITE;
        std::vector<Projectile> projectilePool_ = std::vector<Projectile>(5);
        std::vector<AiTank*> enemies_;
        DrawableVector movementVector_;

    public:
        void Init(const glm::vec2 startingPos, const glm::vec3 color, std::vector<AiTank*> enemies, const glm::mat4& view)
        {
            enemies_ = enemies;
            pos_ = startingPos;
            color_ = color;
            hitbox_.bottomLeft = startingPos - ToVec2(ONE_VEC3); // ONE because texture quad goes from -1;-1 to 1;1
            hitbox_.topRight = startingPos + ToVec2(ONE_VEC3);
            for(auto& projectile : projectilePool_)
            {
                projectile.Init(color);
            }
            movementVector_.Init(GREEN, view);
        }
        void Update(const bool d, const bool w, const bool a, const bool s, const bool lmb, const float dt, const glm::vec2 relMousePos, const unsigned int VAO, unsigned int TEXs[2], Shader& tankShader, Shader& projectileShader, const Map& map)
        {
            // Rotate tank.
            if(d != a)
            {
                float rotation = PI * dt * TURN_MULT_;
                if(d) rotation = -rotation;
                if((a || d) && s) rotation = -rotation;
                bodyRot_ += rotation;
            }

            // Move tank.
            glm::vec2 movementVec = ZERO_VEC2;
            if(w != s)
            {
                glm::vec2 dir = glm::vec2(glm::cos(bodyRot_), glm::sin(bodyRot_));
                if(s) dir = -dir;
                constexpr const float INCLINE_EFFECT_MULTIPLIER = 100.0f;
                movementVec = (dir * dt * TANK_SPEED_) + (map.ComputeTerrainIncline(pos_) * dt * INCLINE_EFFECT_MULTIPLIER);
                pos_ += movementVec; // (dir * dt * TANK_SPEED_) + (map.ComputeTerrainIncline(pos_) * dt * INCLINE_EFFECT_MULTIPLIER);
                hitbox_.bottomLeft = pos_ - ToVec2(ONE_VEC3);
                hitbox_.topRight = pos_ + ToVec2(ONE_VEC3);
            }
            movementVector_.Update(pos_, pos_ + movementVec);

            // Rotate gun.
            gunRot_ = glm::atan(relMousePos.y / relMousePos.x);
            if(relMousePos.x < 0.0f) gunRot_ += PI;

            // Launch a projectile.
            bool projectileLaunched = false;
            for(auto& projectile : projectilePool_)
            {
                if(lmb && !projectileLaunched)
                {
                    if(!projectile.InFlight())
                    {
                        projectile.Launch(pos_, gunRot_);
                        projectileLaunched = true;
                    }
                }
                projectile.Update
                (
                    enemies_.front()->GetHitBox(),
                    dt,
                    std::bind(&AiTank::Kill, enemies_.front()),
                    projectileShader
                );
            }

            glBindVertexArray(VAO);

            tankShader.Bind();
            glActiveTexture(GL_TEXTURE0);

            // Draw tank body.
            glm::mat4 model = glm::translate(IDENTITY_MAT4, ToVec3(pos_));
            model = glm::rotate(model, bodyRot_, FRONT_VEC3);
            tankShader.SetMat4({ "model", model });
            tankShader.SetVec3({ "color", color_ });
            glBindTexture(GL_TEXTURE_2D, TEXs[0]);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Draw tank gun.
            model = glm::translate(IDENTITY_MAT4, ToVec3(pos_));
            model = glm::rotate(model, gunRot_, FRONT_VEC3);
            model = glm::scale(model, GUN_SCALE_);
            tankShader.SetMat4({ "model", model });
            glBindTexture(GL_TEXTURE_2D, TEXs[1]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        void Destroy()
        {
            for(auto& projectile : projectilePool_)
            {
                projectile.Destroy();
            }
            movementVector_.Destroy();
        }
    };
}//!hs

*/