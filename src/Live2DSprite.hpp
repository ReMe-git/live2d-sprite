#pragma once

class Live2DSprite {
private:

public:
	Live2DSprite();
	~Live2DSprite();
	void InitializeCubism();
	void SetExecuteAbsolutePath();
	bool InitializeSystem();
	void Release();
	void LoadModel(const std::string modelDirectoryName);
	bool Update();
}; // Live2DSprite
