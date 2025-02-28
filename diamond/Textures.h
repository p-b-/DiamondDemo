#pragma once
#include <string>
#include <map>

class Textures
{
public:
	Textures();
	unsigned int CreateTexture(const char* pzFilepath, const std::string& name, bool useAlpha, bool flipVertically);
	unsigned int GetTextureIdFromName(const std::string& name);

private:

	std::map<std::string, unsigned int> m_mapTextureNameToId;
};

