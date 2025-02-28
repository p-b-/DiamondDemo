#pragma once

// Interface to input, internal to engine itself
class IEngineInputIntl {
public:
	virtual bool Initialise() = 0;
	virtual void DeinitialiseAndDelete() = 0;
	virtual void ProcessInput(float fDeltaTime) = 0;
	virtual void ShowBindingPanel() = 0;
};