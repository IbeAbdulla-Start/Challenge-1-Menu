#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "CPathAnimator.h"
#include "Tools/PathUtility.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"

#include "imgui.h"
#include <iostream>

#include <memory>

using namespace nou;

//Forward declaring our global resources for this demo.
std::unique_ptr<ShaderProgram> prog_texLit, prog_lit, prog_unlit;
std::unique_ptr<Mesh> duckMesh, boxMesh, recMesh,trashyMesh;
std::unique_ptr<Texture2D> duckTex,TrashyTex;
std::unique_ptr<Material> duckMat, unselectedMat, selectedMat, lineMat,recMat,trashyMat;

//Function to keep main clean
void LoadDefaultResources();

//Templated LERP function.
//(This will work for any type that supports addition and scalar multiplication.)
template<typename T>
T LERP(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}

float timeLoop = 12.f;
float timeLoopA = 5.f;

int main()
{

	bool isPressed = false;
	bool listPanel = true;

	bool timerDone = false;
	App::Init("Week 4 Tutorial - Paths and Curves", 800, 800);
	App::SetClearColor(glm::vec4(0.0f, 0.27f, 0.4f, 1.0f));

	App::InitImgui();

	LoadDefaultResources();

	//PathSampler::LERP = LERP<glm::vec3>;

	//Set up our camera.
	Entity camEntity = Entity::Create();
	auto& cam = camEntity.Add<CCamera>(camEntity);
	cam.Perspective(60.0f, 1.0f, 0.1f, 100.0f);
	camEntity.transform.m_pos = glm::vec3(0.0f, 0.0f, 4.0f);

	//Creating the duck entity.
	Entity duckEntity = Entity::Create();
	duckEntity.Add<CMeshRenderer>(duckEntity, *duckMesh, *duckMat);
	duckEntity.Add<CPathAnimator>(duckEntity);
	duckEntity.transform.m_scale = glm::vec3(0.003f, 0.003f, 0.003f);
	duckEntity.transform.m_pos = glm::vec3(0.0f, -1.0f, 0.0f);
	duckEntity.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	Entity RectangleE = Entity::Create();
	RectangleE.Add<CMeshRenderer>(RectangleE, *recMesh, *recMat);
	RectangleE.Add<CPathAnimator>(RectangleE);
	RectangleE.transform.m_pos = glm::vec3(0.0f, 2.0f, 0.0f);
	RectangleE.transform.m_scale = glm::vec3(2.0f, 0.2f, 1.0f);

	Entity TrashyE = Entity::Create();
	TrashyE.Add<CMeshRenderer>(TrashyE, *trashyMesh, *trashyMat);
	TrashyE.Add<CPathAnimator>(TrashyE);
	TrashyE.transform.m_pos = glm::vec3(-2.5f, 0.0f, 10.f);
	TrashyE.transform.m_rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TrashyE.transform.m_scale = glm::vec3(1.f,1.f, 1.f);
	

	//Setting up our container of waypoints.
	std::vector<std::unique_ptr<Entity>> points;
	points.push_back(Entity::Allocate());
	points.back()->Add<CMeshRenderer>(*points.back(), *boxMesh, *unselectedMat);
	points.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points.back()->transform.m_pos = glm::vec3(0.0f, 2.1f, 0.0f);

	points.push_back(Entity::Allocate());
	points.back()->Add<CMeshRenderer>(*points.back(), *boxMesh, *unselectedMat);
	points.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points.back()->transform.m_pos = glm::vec3(0.0f, -0.1f, 0.0f);

	std::vector<std::unique_ptr<Entity>> points2;
	points2.push_back(Entity::Allocate());
	points2.back()->Add<CMeshRenderer>(*points2.back(), *boxMesh, *unselectedMat);
	points2.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points2.back()->transform.m_pos = glm::vec3(-2.5f, 0.0f, 1.25f);

	points2.push_back(Entity::Allocate());
	points2.back()->Add<CMeshRenderer>(*points2.back(), *boxMesh, *unselectedMat);
	points2.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points2.back()->transform.m_pos = glm::vec3(-0.4, 0.0f, 1.25f);

	//points for scaling
	std::vector<std::unique_ptr<Entity>> pointsS;
	pointsS.push_back(Entity::Allocate());
	pointsS.back()->Add<CMeshRenderer>(*pointsS.back(), *boxMesh, *unselectedMat);
	pointsS.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	pointsS.back()->transform.m_pos = glm::vec3(0.8f, 0.8f, 1.0f); //SCALING VARIABLE
	pointsS.back()->transform.m_rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.5f, 0.0f));

	pointsS.push_back(Entity::Allocate());
	pointsS.back()->Add<CMeshRenderer>(*pointsS.back(), *boxMesh, *unselectedMat);
	pointsS.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	pointsS.back()->transform.m_pos = glm::vec3(2.5, 2.5f, 1.0f); //SCALING VARIABLE
	pointsS.back()->transform.m_rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, -0.5f, 0.0f));

	//Setting up our utility to draw the given path.
	PathSampler sampler = PathSampler();

	Entity pathDrawUtility = Entity::Create();
	pathDrawUtility.Add<CLineRenderer>(pathDrawUtility, sampler, *lineMat);

	App::Tick();

	
	//This is our main loop.
	while (!App::IsClosing() && !Input::GetKey(GLFW_KEY_ESCAPE))
	{
		//Start of the frame.
		App::FrameStart();
		float deltaTime = App::GetDeltaTime();

		
		//Updates the camera.
		camEntity.Get<CCamera>().Update();
		if (timeLoop > 0) {
			timerDone = false;
			timeLoop -= deltaTime;
			if (isPressed && !timerDone)

			{//Update our path animator.
				if (RectangleE.transform.m_pos.y > 0.0f)
				{
					RectangleE.Get<CPathAnimator>().Update(points, deltaTime);
				}
				else
				{
					//idk

					TrashyE.transform.RecomputeGlobal();
					TrashyE.Get<CMeshRenderer>().Draw();

					if (TrashyE.transform.m_pos.x < -0.5) {
						TrashyE.Get<CPathAnimator>().Update(points2, deltaTime);
					}
					else {
						//std::cout << "debug hit";
						TrashyE.Get<CPathAnimator>().UpdateScale(pointsS, deltaTime);

					}


				}
				//Update transformation matrices.
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i]->transform.RecomputeGlobal();
				}


				RectangleE.transform.RecomputeGlobal();

				//Draw everything.
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i]->Get<CMeshRenderer>().Draw();
				}


				RectangleE.Get<CMeshRenderer>().Draw();

				//Draw our path (for debugging/demo purposes).
				pathDrawUtility.Get<CLineRenderer>().Draw(points);
			}
		}
		else {
			timerDone = true;
			
			if (isPressed && timerDone)

			{//Update our path animator.
				
					//idk

					TrashyE.transform.RecomputeGlobal();
					TrashyE.Get<CMeshRenderer>().Draw();

					if (TrashyE.transform.m_pos.x > -2.4) {
						TrashyE.Get<CPathAnimator>().Update(points2, deltaTime);
					}
					else if (TrashyE.transform.m_pos.x > -2.4) {
						//std::cout << "debug hit";
						TrashyE.Get<CPathAnimator>().UpdateScale(pointsS, deltaTime);

					}
					else {

						if (RectangleE.transform.m_pos.y < 2.0f)
						{
							RectangleE.Get<CPathAnimator>().Update(points, deltaTime);
						}
					}
				
				//Update transformation matrices.
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i]->transform.RecomputeGlobal();
				}


				RectangleE.transform.RecomputeGlobal();

				//Draw everything.
				for (size_t i = 0; i < points.size(); ++i)
				{
					points[i]->Get<CMeshRenderer>().Draw();
				}


				RectangleE.Get<CMeshRenderer>().Draw();
			
				//Draw our path (for debugging/demo purposes).
				pathDrawUtility.Get<CLineRenderer>().Draw(points);

				if (timeLoopA > 0) {
					timeLoopA -= deltaTime;

				}
				else {
					timeLoop = 12.f;
					timeLoopA = 5.f;
				}
			}
			
			
		}
		//For Imgui...
		if (listPanel)
		{
			App::StartImgui();


			ImGui::Begin("Rubbish Rush", &listPanel, ImVec2(300, 200));

			if (!isPressed)
			{//Add a new waypoint!
				if (ImGui::Button("Start"))
				{
					isPressed = true;
					listPanel = false;
				}
			}

			ImGui::End();
			App::EndImgui();
		}

		//This sticks all the drawing we just did on the screen.
		App::SwapBuffers();
	}

	App::Cleanup();

	return 0;
}

void LoadDefaultResources()
{
	//Load in some shaders.
	auto v_texLit = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
	auto f_texLit = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);

	std::vector<Shader*> texLit = { v_texLit.get(), f_texLit.get() };
	prog_texLit = std::make_unique<ShaderProgram>(texLit);

	auto v_lit = std::make_unique<Shader>("shaders/lit.vert", GL_VERTEX_SHADER);
	auto f_lit = std::make_unique<Shader>("shaders/lit.frag", GL_FRAGMENT_SHADER);

	std::vector<Shader*> lit = { v_lit.get(), f_lit.get() };
	prog_lit = std::make_unique<ShaderProgram>(lit);

	auto v_unlit = std::make_unique<Shader>("shaders/unlit.vert", GL_VERTEX_SHADER);
	auto f_unlit = std::make_unique<Shader>("shaders/unlit.frag", GL_FRAGMENT_SHADER);

	std::vector<Shader*> unlit = { v_unlit.get(), f_unlit.get() };
	prog_unlit = std::make_unique<ShaderProgram>(unlit);

	//Load in the ducky model.
	duckMesh = std::make_unique<Mesh>();
	GLTF::LoadMesh("duck/Duck.gltf", *duckMesh);
	duckTex = std::make_unique<Texture2D>("duck/DuckCM.png");
	
	//Set up Box
	boxMesh = std::make_unique<Mesh>();
	GLTF::LoadMesh("box/Box.gltf", *boxMesh);

	//Set up Rec
	recMesh = std::make_unique<Mesh>();
	GLTF::LoadMesh("Rectangle/rectangle.gltf", *recMesh);

	recMat = std::make_unique<Material>(*prog_lit);
	
	trashyMesh = std::make_unique<Mesh>();
	GLTF::LoadMesh("trashy/Trashy.gltf", *trashyMesh);
	TrashyTex = std::make_unique<Texture2D>("trashy/Trashy2.png");

	trashyMat = std::make_unique<Material>(*prog_texLit);
	
	trashyMat->AddTexture("albedo", *TrashyTex);


	duckMat = std::make_unique<Material>(*prog_texLit);
	duckMat->AddTexture("albedo", *duckTex);

	unselectedMat = std::make_unique<Material>(*prog_lit);
	unselectedMat->m_color = glm::vec3(0.5f, 0.5f, 0.5f);

	selectedMat = std::make_unique<Material>(*prog_lit);
	selectedMat->m_color = glm::vec3(1.0f, 0.0f, 0.0f);

	lineMat = std::make_unique<Material>(*prog_unlit);
	lineMat->m_color = glm::vec3(1.0f, 1.0f, 1.0f);
}