#pragma once
#include "ShadowedScene.h"
#include "SceneEntity.h"
#include "SceneActions.h"
#include <vector>
#include <map>
class Diamond;
class Window;
class IGameLoop;

class DiamondScene : public ShadowedScene {
public:
	virtual ~DiamondScene();
	virtual bool Initialise(Textures* pTextureCtrl, Window* pWnd);
	virtual void Deinitialise();
	virtual void SetGameLoop(IGameLoop* pGameLoop);
	virtual void AnimateScene(float fDeltaTime);

	virtual void DrawScene(bool bForShadows);
	
	virtual void ControllerConnected();

	virtual void ProcessDigitalAction(unsigned int unAction, float fDeltaTime);
	virtual void ProcessAnalogAction(unsigned int unAction, float fAnalogValue, float fDeltaTime);
	virtual void ProcessAnalogAction(unsigned int unAction, float fAnalogValue1, float fAnalogValue2, float fDeltaTime);
	virtual void ProcessAnalogAction(unsigned int unAction, float fAnalogValue1, float fAnalogValue2, float fAnalogValue3, float fDeltaTime);

private:
	void ApplyAnalogMovement(float fX, float fY, float fZ, float fDeltaTime);

	void ProcessDigitalAction(SceneAction eAction, float fDeltaTime);
	void ProcessAnalogAction(SceneAction eAction, float fAnalogValue, float fDeltaTime);
	void ProcessAnalogAction(SceneAction eAction, float fAnalogValue1, float fAnalogValue2, float fDeltaTime);
	void ProcessAnalogAction(SceneAction eAction, float fAnalogValue1, float fAnalogValue2, float fAnalogValue3, float fDeltaTime);

private:
	void InitialiseCamera(Window* pWnd, glm::vec3 vecPos);
	void InitialiseLighting(glm::vec3 vecLightPos);
	void InitialiseDiamonds();
	void InitialiseTiles();
	void InitialiseSpheres(glm::vec3 vecPos);

	void NoCullForLightFrustum();
	void CullEntitiesFromFrustum();

private:
	std::vector<SceneEntity*> m_vcEntities;
	std::vector<SceneEntity*> m_vcEntitiesToDraw;

	std::vector<SceneEntity*> m_vcLights;

	SceneEntity* m_pseLight1;
	SceneEntity* m_pseLightingSphere;
	SceneEntity* m_pDiamond;
	SceneEntity* m_pseCamera;
	SceneEntity* m_pShere;
	Camera* m_pCam;
	IGameLoop* m_pGameLoop;

	bool m_bCameraLocked;
	glm::vec3 m_vecLightPosition;
	glm::vec3 m_vecRotateLightAround;

	unsigned int m_unNextEntityId = 1;
	unsigned int m_unDiamondEntity;

	bool m_bDisplayShadowDebug;

	unsigned int m_unLightSphereEntityId;
	unsigned int m_unDiamondEntityId;
	unsigned int m_unTileEntityId;
	unsigned int m_unSphereEntityId;
};