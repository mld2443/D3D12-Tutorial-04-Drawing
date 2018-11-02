////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass(const InputClass&) = delete;
	InputClass& operator=(const InputClass&) = delete;

	InputClass() = default;
	~InputClass() = default;

	void KeyDown(UINT);
	void KeyUp(UINT);

	bool IsKeyDown(UINT);

private:
	vector<bool>	m_keys =	vector<bool>(256, false);
};
