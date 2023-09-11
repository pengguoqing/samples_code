////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
  int i;


  // Initialize the keyboard state.
  for(i=0; i<256; i++)
  {
    m_keyboardState[i] = false;
  }
  
  return;
}


void InputClass::KeyDown(int keySymbol)
{
  if(keySymbol == 65307) { m_keyboardState[KEY_ESCAPE] = true; }
  
  return;
}


void InputClass::KeyUp(int keySymbol)
{
  if(keySymbol == 65307) { m_keyboardState[KEY_ESCAPE] = false; }
  
  return;
}


bool InputClass::IsEscapePressed() const
{
  return m_keyboardState[KEY_ESCAPE];
}

