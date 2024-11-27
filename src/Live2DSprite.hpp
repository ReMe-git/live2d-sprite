#pragma once

class Live2DSprite {
private:

public:
	Live2DSprite();
	~Live2DSprite();
	void InitializeCubism();
	void SetExecuteAbsolutePath();
	void SetModelDirectory(const std::string modelDirectoryName);
	bool InitializeSystem();
	void Release();
	void LoadModel(const std::string modelName);
	void Update();
}; // Live2DSprite
