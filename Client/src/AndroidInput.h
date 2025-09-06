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
private:

};

