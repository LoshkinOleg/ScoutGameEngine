#pragma once
/*
namespace hs
{
    class Map
    {
    private:
        constexpr static const size_t CHUNK_RESOLUTION_ = 64;
        constexpr static const float MAP_TILE_MULTIPLIER_ = 20.0f; // Needs to be big enough for the player not to see the chunks updating.
        constexpr static const float TEXTURE_QUAD_SIDE_LEN_ = 2.0f; // Data in the quad VBO ranges from -1 to 1. Must take this into account when using the map.

        enum ChunkLocation: int
        {
            BOTTOM_LEFT = 0,
            BOTTOM_MIDDLE = 1,
            BOTTOM_RIGHT = 2,
            MIDDLE_LEFT = 3,
            MIDDLE_MIDDLE = 4,
            MIDDLE_RIGHT = 5,
            TOP_LEFT = 6,
            TOP_MIDDLE = 7,
            TOP_RIGHT = 8
        };

        struct MapChunk_
        {
            void Generate(const siv::BasicPerlinNoise<float>& perlinNoiseGenerator)
            {
                constexpr const float INCREMENT_ = 1.0f / (float)CHUNK_RESOLUTION_;
                float fx = 0.0f, fy = 0.0f;
                for(size_t y = 0; y < CHUNK_RESOLUTION_; y++)
                {
                    fx = 0.0f;
                    for(size_t x = 0; x < CHUNK_RESOLUTION_; x++)
                    {
                        const float generated = perlinNoiseGenerator.noise2D_0_1(fx + (offset.x / (MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_)), fy + (offset.y / (MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_)));
                        data[y][x] = generated;
                        fx += INCREMENT_;
                    }
                    fy += INCREMENT_;
                }
            }
            glm::vec2 WorldPos()
            {
                return glm::vec2(offset.x, offset.y);
            }

            std::array<std::array<float, CHUNK_RESOLUTION_>, CHUNK_RESOLUTION_> data = { {0} };
            glm::ivec2 offset = { 0,0 };
        };

    public:
        glm::vec2 ComputeTerrainIncline(const glm::vec2 tankPos) const
        {
            constexpr const int DISTANCE_BETWEEN_SAMPLES_ = CHUNK_RESOLUTION_ / CHUNK_RESOLUTION_;

            // Find chunk closest to player.
            std::array<std::pair<int, glm::vec2>, 9> relChunkPos;
            for(size_t i = 0; i < 9; i++)
            {
                relChunkPos[i] = { i, ToVec2(chunks_[i].offset) - tankPos };
            }
            std::sort(relChunkPos.begin(), relChunkPos.end(), [](const std::pair<int, glm::vec2> v, const std::pair<int, glm::vec2> u) { return ManhattanCompare(v.second, u.second); });

            // Determine the chunk's data's indices. The data at these indices correspond to the area the player is standing on.
            const ChunkLocation closestChunkLocation = (ChunkLocation)relChunkPos[0].first;
            const MapChunk_ *const closestChunk = &chunks_[closestChunkLocation];
            const glm::vec2 relTankPos = -relChunkPos[0].second;
            std::array<std::array<glm::ivec2, 3>, 3> chunkDataToSample;
            for(int y = -1; y < 2; y++)
            {
                for(int x = -1; x < 2; x++)
                {
                    chunkDataToSample[y + 1][x + 1].x = (int)RemapToRange(
                        MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_ * -0.5f,
                        MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_ * 0.5f,
                        0.0f,
                        (float)CHUNK_RESOLUTION_,
                        relTankPos.x + float(x * DISTANCE_BETWEEN_SAMPLES_));
                    chunkDataToSample[y + 1][x + 1].y = (int)RemapToRange(
                        MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_ * -0.5f,
                        MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_ * 0.5f,
                        0.0f,
                        (float)CHUNK_RESOLUTION_,
                        relTankPos.y + float(y * DISTANCE_BETWEEN_SAMPLES_));
                }
            }

            // Access the chunk's data to compute the local average incline.
            glm::vec2 accumulatedIncline = ZERO_VEC3;
            const float middleHeight = 1.0f - closestChunk->data[chunkDataToSample[1][1].y][chunkDataToSample[1][1].x];
            for(int y = -1; y < 2; y++)
            {
                for(int x = -1; x < 2; x++)
                {
                    if(x == 0 && y == 0) continue; // Middle chunk height - middle chunk height = 0, so ignore it.

                    glm::ivec2 dataToSample = chunkDataToSample[y + 1][x + 1];
                    const MapChunk_* chunkToSample = closestChunk;

                    const bool negX = dataToSample.x < 0;
                    const bool posX = dataToSample.x >= CHUNK_RESOLUTION_;
                    const bool negY = dataToSample.y < 0;
                    const bool posY = dataToSample.y >= CHUNK_RESOLUTION_;

                    if(negX)
                    {
                        dataToSample.x += CHUNK_RESOLUTION_;
                        if(negY) // Sample from bottom left neighbor.
                        {
                            dataToSample.y += CHUNK_RESOLUTION_;
                            assert // Sanity check.
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_LEFT &&
                                closestChunkLocation != ChunkLocation::BOTTOM_MIDDLE &&
                                closestChunkLocation != ChunkLocation::BOTTOM_RIGHT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_LEFT &&
                                closestChunkLocation != ChunkLocation::TOP_LEFT
                            );
                            chunkToSample = &chunks_[closestChunkLocation - 4];
                        }
                        else if(posY) // Sample from top left neighbor.
                        {
                            dataToSample.y -= CHUNK_RESOLUTION_;
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_LEFT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_LEFT &&
                                closestChunkLocation != ChunkLocation::TOP_LEFT &&
                                closestChunkLocation != ChunkLocation::TOP_MIDDLE &&
                                closestChunkLocation != ChunkLocation::TOP_RIGHT
                            );
                            chunkToSample = &chunks_[closestChunkLocation + 2];
                        }
                        else // Sample from left neighbor.
                        {
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_LEFT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_LEFT &&
                                closestChunkLocation != ChunkLocation::TOP_LEFT
                            );
                            chunkToSample = &chunks_[closestChunkLocation - 1];
                        }
                    }
                    else if(posX)
                    {
                        dataToSample.x -= CHUNK_RESOLUTION_;
                        if(negY) // Sample from bottom right neighbor.
                        {
                            dataToSample.y += CHUNK_RESOLUTION_;
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_LEFT &&
                                closestChunkLocation != ChunkLocation::BOTTOM_MIDDLE &&
                                closestChunkLocation != ChunkLocation::BOTTOM_RIGHT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_RIGHT &&
                                closestChunkLocation != ChunkLocation::TOP_RIGHT
                            );
                            chunkToSample = &chunks_[closestChunkLocation - 2];
                        }
                        else if(posY) // Sample from top right neighbor.
                        {
                            dataToSample.y -= CHUNK_RESOLUTION_;
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_RIGHT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_RIGHT &&
                                closestChunkLocation != ChunkLocation::TOP_RIGHT &&
                                closestChunkLocation != ChunkLocation::TOP_MIDDLE &&
                                closestChunkLocation != ChunkLocation::TOP_LEFT
                            );
                            chunkToSample = &chunks_[closestChunkLocation + 4];
                        }
                        else // Sample from right neighbor.
                        {
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_RIGHT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_RIGHT &&
                                closestChunkLocation != ChunkLocation::TOP_RIGHT
                            );
                            chunkToSample = &chunks_[closestChunkLocation + 1];
                        }
                    }
                    else
                    {
                        if(negY) // Sample from bottom neighbor.
                        {
                            dataToSample.y += CHUNK_RESOLUTION_;
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_LEFT &&
                                closestChunkLocation != ChunkLocation::BOTTOM_MIDDLE &&
                                closestChunkLocation != ChunkLocation::BOTTOM_RIGHT
                            );
                            chunkToSample = &chunks_[closestChunkLocation - 3];
                        }
                        else if(posY) // Sample from top neighbor.
                        {
                            dataToSample.y -= CHUNK_RESOLUTION_;
                            assert
                            (
                                closestChunkLocation != ChunkLocation::TOP_LEFT &&
                                closestChunkLocation != ChunkLocation::TOP_MIDDLE &&
                                closestChunkLocation != ChunkLocation::TOP_RIGHT
                            );
                            chunkToSample = &chunks_[closestChunkLocation + 3];
                        }
                    }

                    accumulatedIncline += -glm::normalize(glm::vec2(x, y)) * ((1.0f - chunkToSample->data[dataToSample.y][dataToSample.x]) - middleHeight);
                }
            }

            return accumulatedIncline / 8.0f;
        }

        void Update(const glm::vec2 playerPos, const unsigned int VAO)
        {
            glBindVertexArray(VAO);

            // Must be > 1 + (SQRT_OF_TWO / 2) to avoid updates fighting each other. Defines the radius of the circle the player can navigate in without triggering map updates.
            constexpr const float TOLERABLE_PLAYER_OFFSET = 1.0f + SQRT_OF_TWO * 0.5f;
            constexpr const float UPDATE_MAP_THRESHOLD = TOLERABLE_PLAYER_OFFSET * MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_;

            const bool updateRight = glm::length(chunks_[3].WorldPos() - playerPos) > UPDATE_MAP_THRESHOLD;
            const bool updateLeft = glm::length(chunks_[5].WorldPos() - playerPos) > UPDATE_MAP_THRESHOLD;
            const bool updateUp = glm::length(chunks_[1].WorldPos() - playerPos) > UPDATE_MAP_THRESHOLD;
            const bool updateDown = glm::length(chunks_[7].WorldPos() - playerPos) > UPDATE_MAP_THRESHOLD;

            if(updateRight) // Moving right.
            {
                chunks_[0] = chunks_[1];
                chunks_[1] = chunks_[2];
                chunks_[3] = chunks_[4];
                chunks_[4] = chunks_[5];
                chunks_[6] = chunks_[7];
                chunks_[7] = chunks_[8];
                chunks_[2].offset = chunks_[1].offset + glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[5].offset = chunks_[4].offset + glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[8].offset = chunks_[7].offset + glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[2].Generate(perlinGenerator_);
                chunks_[5].Generate(perlinGenerator_);
                chunks_[8].Generate(perlinGenerator_);
            }
            if(updateLeft) // Moving left.
            {
                chunks_[2] = chunks_[1];
                chunks_[1] = chunks_[0];
                chunks_[5] = chunks_[4];
                chunks_[4] = chunks_[3];
                chunks_[8] = chunks_[7];
                chunks_[7] = chunks_[6];
                chunks_[0].offset = chunks_[1].offset - glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[3].offset = chunks_[4].offset - glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[6].offset = chunks_[7].offset - glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[0].Generate(perlinGenerator_);
                chunks_[3].Generate(perlinGenerator_);
                chunks_[6].Generate(perlinGenerator_);
            }
            if(updateUp) // Moving up.
            {
                chunks_[0] = chunks_[3];
                chunks_[3] = chunks_[6];
                chunks_[1] = chunks_[4];
                chunks_[4] = chunks_[7];
                chunks_[2] = chunks_[5];
                chunks_[5] = chunks_[8];
                chunks_[6].offset = chunks_[3].offset + glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[7].offset = chunks_[4].offset + glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[8].offset = chunks_[5].offset + glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[6].Generate(perlinGenerator_);
                chunks_[7].Generate(perlinGenerator_);
                chunks_[8].Generate(perlinGenerator_);
            }
            if(updateDown) // Moving down.
            {
                chunks_[6] = chunks_[3];
                chunks_[3] = chunks_[0];
                chunks_[7] = chunks_[4];
                chunks_[4] = chunks_[1];
                chunks_[8] = chunks_[5];
                chunks_[5] = chunks_[2];
                chunks_[0].offset = chunks_[3].offset - glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[1].offset = chunks_[4].offset - glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[2].offset = chunks_[5].offset - glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[0].Generate(perlinGenerator_);
                chunks_[1].Generate(perlinGenerator_);
                chunks_[2].Generate(perlinGenerator_);
            }

            if(updateRight || updateLeft || updateDown || updateUp)
            {
                for(int i = 0; i < 9; i++)
                {
                    glBindTexture(GL_TEXTURE_2D, TEXs_[i]);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, CHUNK_RESOLUTION_, CHUNK_RESOLUTION_, 0, GL_RED, GL_FLOAT, &(chunks_[i].data)[0][0]);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }
            }

            // Draw map.
            shader_.Bind();
            for(int i = 0; i < 9; i++)
            {
                glm::mat4 model = glm::translate(IDENTITY_MAT4, glm::vec3(chunks_[i].offset.x, chunks_[i].offset.y, 0.0f));
                model = glm::scale(model, ONE_VEC3 * MAP_TILE_MULTIPLIER_);
                shader_.SetMat4({ "model", model });
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, TEXs_[i]);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        void Init(const glm::mat4& view)
        {
            Shader::Definition sdef;
            sdef.vertexPath = "../data/shaders/sprite.vert";
            sdef.fragmentPath = "../data/shaders/sprite_RED.frag";
            sdef.staticInts.insert({ "ALBEDO", 0 });
            sdef.dynamicMat4s.insert({ "view", &view });
            sdef.staticMat4s.insert({ "PROJECTION", ORTHO });
            shader_.Create(sdef);

            chunks_.resize(9);
            glGenTextures(9, TEXs_);
            for(int y = -1; y < 2; y++)
            {
                for(int x = -1; x < 2; x++)
                {
                    chunks_[(y + 1) * 3 + (x + 1)].offset = { x * MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, y * MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_ };
                    chunks_[(y + 1) * 3 + (x + 1)].Generate(perlinGenerator_);

                    glBindTexture(GL_TEXTURE_2D, TEXs_[(y + 1) * 3 + (x + 1)]);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, CHUNK_RESOLUTION_, CHUNK_RESOLUTION_, 0, GL_RED, GL_FLOAT, &(chunks_[(y + 1) * 3 + (x + 1)].data)[0][0]);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }
            }
            CheckGlError();
        }
        void Destroy()
        {
            glDeleteTextures(9, TEXs_);
            glDeleteProgram(shader_.GetPROGRAM());
        }

    private:
        unsigned int TEXs_[9] = { 0 };
        std::vector<MapChunk_> chunks_; // 0: LB, 1: MB, 2: RB, 3: LM, 4: MM, 5: RM, 6: LT, 7: MT, 8: RT 
        siv::BasicPerlinNoise<float> perlinGenerator_;
        Shader shader_;
    };
}//!hs

*/