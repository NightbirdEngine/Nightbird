#include <Core/Engine.h>

#include <Core/SceneObject.h>
#include <Core/Renderer.h>
#include <Core/ModelManager.h>
#include <Core/GlfwWindow.h>
#include <Vulkan/DescriptorPool.h>

#include <Core/ProjectRegistration.h>

#include <EditorRenderTarget.h>

#include <iostream>

#include <rttr/library.h>

int main(int argc, char** argv)
{
	rttr::library project("Project");
	bool projectLoaded = project.load();
	if (projectLoaded)
	{
		std::cout << "Loaded Project shared library via RTTR with " << project.get_types().size() << " types" << std::endl;
	}
	else
	{
		std::cout << "Failed to load Project shared library via RTTR" << std::endl;
	}

	Nightbird::Engine engine;
	
	Nightbird::EditorRenderTarget renderTarget(engine.GetRenderer(), engine.GetRenderer()->GetInstance(), engine.GetRenderer()->GetDevice(), engine.GetRenderer()->GetSwapChain(), engine.GetRenderer()->GetRenderPass(), engine.GetGlfwWindow()->Get(), engine.GetScene(), engine.GetModelManager());
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	engine.Run();
}