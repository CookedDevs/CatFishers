#include "AndroidInput.h"
#include "Client.h"

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

void AndroidInput::GetSoftKeyboardInput(std::string& input)
{
#ifdef _ANDROID_
    std::vector<char> buffer(input.begin(), input.end());
    buffer.push_back('\0');
    SoftKeyboardEditText(buffer.data(), buffer.size());
    input = buffer.data();
#endif // _ANDROID_
}

bool AndroidInput::Joystick()
{
#ifdef _ANDROID_
    Vector2 circlePosition = { 100.f, (float)GetScreenHeight() - 110.f };
    float biggerRadius = 70.f;

    Vector2 smallerCirclePosition = { 100.f, (float)GetScreenHeight() - 110.f };
    float smallerRadius = 30.f;

    float angle = 0.0f;
    float dx = 0.0f, dy = 0.0f, dxx = 0.0f, dyy = 0.0f;

    smallerCirclePosition = GetMousePosition();

    if (CheckCollisionPointCircle(smallerCirclePosition, circlePosition, biggerRadius * 2) && GetTouchPointCount() > 0)
    {
        if (!CheckCollisionPointCircle(smallerCirclePosition, circlePosition, biggerRadius))
        {
            dx = smallerCirclePosition.x - circlePosition.x;
            dy = smallerCirclePosition.y - circlePosition.y;

            angle = atan2f(dy, dx);

            dxx = (biggerRadius)*cosf(angle);
            dyy = (biggerRadius)*sinf(angle);

            smallerCirclePosition.x = circlePosition.x + dxx;
            smallerCirclePosition.y = circlePosition.y + dyy;
        }

        unsigned int deadZoneSize = 8;

        if (smallerCirclePosition.x <= circlePosition.x - deadZoneSize) Client::SetKey('A', true); else Client::SetKey('A', false);
        if (smallerCirclePosition.x >= circlePosition.x + deadZoneSize) Client::SetKey('D', true); else Client::SetKey('D', false);
        if (smallerCirclePosition.y <= circlePosition.y - deadZoneSize) Client::SetKey('W', true); else Client::SetKey('W', false);
        if (smallerCirclePosition.y >= circlePosition.y + deadZoneSize) Client::SetKey('S', true); else Client::SetKey('S', false);

        DrawCircleV(circlePosition, biggerRadius, LIGHTGRAY);
        DrawCircleV(smallerCirclePosition, smallerRadius, RED);
        return true;
    }
#endif
    return false;
}