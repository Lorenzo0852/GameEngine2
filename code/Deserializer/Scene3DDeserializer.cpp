#include <Deserializer/Scene3DDeserializer.h>

bool Scene3DDeserializer::Initialize()
{
	rapidxml::file<> * xmlFile = new rapidxml::file<>(path);
	rapidxml::xml_document<> * doc = new rapidxml::xml_document<>();
	doc->parse<0>(xmlFile->data());
	rapidxml::xml_node<>* registryNode = doc->first_node()->first_node("registry");
	while (registryNode != 0)
	{
		rapidxml::xml_node<>* childNode = registryNode->first_node();
		while (childNode != 0)
		{
			if (std::string(childNode->value()) == "Movement3DSystem")
			{
				registry->AddSystem<Movement3DSystem>();
			}
			if (std::string(childNode->value()) == "ModelRender3DSystem")
			{
				registry->AddSystem<ModelRender3DSystem>(*window);
			}
			if (std::string(childNode->value()) == "EntityStartup3DSystem")
			{
				registry->AddSystem<EntityStartup3DSystem>();
			}
			childNode = childNode->next_sibling();
		}
		registryNode = registryNode->next_sibling("registry");
	}
	rapidxml::xml_node<>* entitiesNode = doc->first_node()->first_node("entities");
	while (entitiesNode != 0)
	{
		rapidxml::xml_node<>* childNode = entitiesNode->first_node();
		while (childNode != 0)
		{
			Entity entity = registry->CreateEntity();

			rapidxml::xml_node<>* componentNode = childNode->first_node();
			while (componentNode != 0)
			{
				if (std::string(componentNode->name()) == "TransformComponent")
				{
					rapidxml::xml_node<>* positionNode = componentNode->first_node("position");

					float xPos = std::stof(std::string(positionNode->first_node("x")->value()));
					float yPos = std::stof(std::string(positionNode->first_node("y")->value()));
					float zPos = std::stof(std::string(positionNode->first_node("z")->value()));

					rapidxml::xml_node<>* rotationNode = componentNode->first_node("rotation");

					float xRot = std::stof(std::string(rotationNode->first_node("x")->value()));
					float yRot = std::stof(std::string(rotationNode->first_node("y")->value()));
					float zRot = std::stof(std::string(rotationNode->first_node("z")->value()));

					rapidxml::xml_node<>* scaleNode = componentNode->first_node("scale");

					float xSca = std::stof(std::string(scaleNode->first_node("x")->value()));
					float ySca = std::stof(std::string(scaleNode->first_node("y")->value()));
					float zSca = std::stof(std::string(scaleNode->first_node("z")->value()));

					rapidxml::xml_node<>* parentNode = componentNode->first_node("parent");
					if (std::string(parentNode->value()) == "null")
					{
						entity.AddComponent<TransformComponent>(glm::vec3(xPos, yPos, zPos), glm::vec3(xRot, yRot, zRot), glm::vec3(xSca, ySca, zSca));
					}
					else
					{
						for (auto entity : registry->GetSystem<EntityStartup3DSystem>().GetSystemEntities())
						{
							if (entity.GetComponent<Node3DComponent>().modelId == std::string(parentNode->value()))
							{
								entity.AddComponent<TransformComponent>(glm::vec3(xPos, yPos, zPos), glm::vec3(xRot, yRot, zRot), glm::vec3(xSca, ySca, zSca), &entity);
							}
						}
					}
				}

				if (std::string(componentNode->name()) == "RigidbodyComponent")
				{
					rapidxml::xml_node<>* velocityNode = componentNode->first_node("velocity");

					float xPos = std::stof(std::string(velocityNode->first_node("x")->value()));
					float yPos = std::stof(std::string(velocityNode->first_node("y")->value()));
					float zPos = std::stof(std::string(velocityNode->first_node("z")->value()));

					rapidxml::xml_node<>* angularVelNode = componentNode->first_node("angularvelocity");

					float xRot = std::stof(std::string(angularVelNode->first_node("x")->value()));
					float yRot = std::stof(std::string(angularVelNode->first_node("y")->value()));
					float zRot = std::stof(std::string(angularVelNode->first_node("z")->value()));

					rapidxml::xml_node<>* scaleNode = componentNode->first_node("scale");

					float xSca = std::stof(std::string(scaleNode->first_node("x")->value()));
					float ySca = std::stof(std::string(scaleNode->first_node("y")->value()));
					float zSca = std::stof(std::string(scaleNode->first_node("z")->value()));

					entity.AddComponent<RigidbodyComponent>(glm::vec3(xPos, yPos, zPos), glm::vec3(xRot, yRot, zRot));
				}

				if (std::string(componentNode->name()) == "Node3DComponent")
				{
					rapidxml::xml_node<>* nameNode = componentNode->first_node("name");
					std::string name = nameNode->value();

					std::shared_ptr< glt::Model  > clone(new glt::Model);
					clone->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());

					entity.AddComponent<Node3DComponent>(name, clone);
				}

				componentNode = componentNode->next_sibling();
			}
			childNode = childNode->next_sibling();
		}
		entitiesNode = entitiesNode->next_sibling("entities");
	}
	return true;
}

void Scene3DDeserializer::Run(float deltaTime)
{

}