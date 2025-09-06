#include "AndroidInput.h"

#ifdef _ANDROID_
#include <android/log.h>
#endif // _ANDROID_

void AndroidInput::ShowAndroidKeyboard()
{
#ifdef _ANDROID_
	ShowSoftKeyboard();
#endif // _ANDROID_
}

void AndroidInput::HideSoftKeyboard()
{
#ifdef _ANDROID_
	HideSoftKeyboard();
#endif // _ANDROID_
}
