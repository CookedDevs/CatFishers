#pragma once
#include <string>
#include <iostream>

#ifdef _ANDROID_
#include <raymob.h>
#endif // _ANDROID_

class AndroidInput
{
public:
	static void ShowAndroidKeyboard();
	static void HideSoftKeyboard();
	static void GetSoftKeyboardInput(std::string& input, unsigned int size);
private:

};

