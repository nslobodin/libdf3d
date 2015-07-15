#pragma once

namespace df3d { namespace base {

class DF3D_DLL MouseMotionEvent
{
public:
    bool leftPressed = false;
    bool rightPressed = false;
    int x = 0;
    int y = 0;
    int dx = 0;
    int dy = 0;
};

class DF3D_DLL MouseButtonEvent
{
public:
    enum class State
    {
        PRESSED,
        RELEASED
    };

    enum class Button
    {
        LEFT,
        RIGHT,
        MIDDLE
    };

    State state;
    Button button;
    int x = 0;
    int y = 0;
};

class DF3D_DLL MouseWheelEvent
{
public:
    float delta = 0;
};

class DF3D_DLL KeyboardEvent
{
public:
    enum class KeyCode
    {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        SPACE,

        F1,
        F2,

        UNDEFINED
    };

    KeyCode keycode;
};

class DF3D_DLL TouchEvent
{
public:
    enum class State
    {
        UP,
        DOWN,
        MOVING,
        CANCEL
    };

    int id = 0;
    int x = 0;
    int y = 0;
    int dx = 0;
    int dy = 0;
    State state;
};

} }
