#include "Engine.h"

#include "Gizmos.h"

class Game final : public sge::I_Application
{
private:
	sge::Gizmos::GizmoVector vector;
public:
	void Init() override
	{
		vector = sge::Gizmos::CreateVector(sge::RED);
	}
	void Update() override
	{
		vector.Schedule();
	}
	void Shutdown() override
	{
		
	}
};

int main(int argv, char** args)
{
	Game game;
	sge::Engine& engine = sge::Engine::Get();
	engine.Run(&game);
	return EXIT_SUCCESS;
}