////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "inputclass.h"


void InputClass::KeyDown(UINT input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
}


void InputClass::KeyUp(UINT input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
}


bool InputClass::IsKeyDown(UINT key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}
