#include <MainApplication/Utils/Keyboard.hpp>

#include <Core/CoreMacros.hpp>
#include <Core/Log/Log.hpp>

#if defined( OS_WINDOWS )
#include <Windows.h>
#elif defined( OS_LINUX )
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <xcb/xcb.h>
#elif defined( OS_MACOS )

#if defined (DEBUG)
    #   undef DEBUG
    #   define DEBUG 0
    #   include <Carbon/Carbon.h>
    #   undef DEBUG
    #   define DEBUG
#else
    #   define DEBUG 0
    #   include <Carbon/Carbon.h>
    #   undef DEBUG
#endif

#endif

namespace Ra
{
    namespace Gui
    {
#if defined( OS_WINDOWS )

        bool isKeyPressed( Key key )
        {
            int vkey = 0;
            switch (key)
            {
                case Key_A:          vkey = 'A';           break;
                case Key_B:          vkey = 'B';           break;
                case Key_C:          vkey = 'C';           break;
                case Key_D:          vkey = 'D';           break;
                case Key_E:          vkey = 'E';           break;
                case Key_F:          vkey = 'F';           break;
                case Key_G:          vkey = 'G';           break;
                case Key_H:          vkey = 'H';           break;
                case Key_I:          vkey = 'I';           break;
                case Key_J:          vkey = 'J';           break;
                case Key_K:          vkey = 'K';           break;
                case Key_L:          vkey = 'L';           break;
                case Key_M:          vkey = 'M';           break;
                case Key_N:          vkey = 'N';           break;
                case Key_O:          vkey = 'O';           break;
                case Key_P:          vkey = 'P';           break;
                case Key_Q:          vkey = 'Q';           break;
                case Key_R:          vkey = 'R';           break;
                case Key_S:          vkey = 'S';           break;
                case Key_T:          vkey = 'T';           break;
                case Key_U:          vkey = 'U';           break;
                case Key_V:          vkey = 'V';           break;
                case Key_W:          vkey = 'W';           break;
                case Key_X:          vkey = 'X';           break;
                case Key_Y:          vkey = 'Y';           break;
                case Key_Z:          vkey = 'Z';           break;
                case Key_Num0:       vkey = '0';           break;
                case Key_Num1:       vkey = '1';           break;
                case Key_Num2:       vkey = '2';           break;
                case Key_Num3:       vkey = '3';           break;
                case Key_Num4:       vkey = '4';           break;
                case Key_Num5:       vkey = '5';           break;
                case Key_Num6:       vkey = '6';           break;
                case Key_Num7:       vkey = '7';           break;
                case Key_Num8:       vkey = '8';           break;
                case Key_Num9:       vkey = '9';           break;
                case Key_Escape:     vkey = VK_ESCAPE;     break;
                case Key_LControl:   vkey = VK_LCONTROL;   break;
                case Key_LShift:     vkey = VK_LSHIFT;     break;
                case Key_LAlt:       vkey = VK_LMENU;      break;
                case Key_LSystem:    vkey = VK_LWIN;       break;
                case Key_RControl:   vkey = VK_RCONTROL;   break;
                case Key_RShift:     vkey = VK_RSHIFT;     break;
                case Key_RAlt:       vkey = VK_RMENU;      break;
                case Key_RSystem:    vkey = VK_RWIN;       break;
                case Key_Menu:       vkey = VK_APPS;       break;
                case Key_LBracket:   vkey = VK_OEM_4;      break;
                case Key_RBracket:   vkey = VK_OEM_6;      break;
                case Key_SemiColon:  vkey = VK_OEM_1;      break;
                case Key_Comma:      vkey = VK_OEM_COMMA;  break;
                case Key_Period:     vkey = VK_OEM_PERIOD; break;
                case Key_Quote:      vkey = VK_OEM_7;      break;
                case Key_Slash:      vkey = VK_OEM_2;      break;
                case Key_BackSlash:  vkey = VK_OEM_5;      break;
                case Key_Tilde:      vkey = VK_OEM_3;      break;
                case Key_Equal:      vkey = VK_OEM_PLUS;   break;
                case Key_Dash:       vkey = VK_OEM_MINUS;  break;
                case Key_Space:      vkey = VK_SPACE;      break;
                case Key_Return:     vkey = VK_RETURN;     break;
                case Key_BackSpace:  vkey = VK_BACK;       break;
                case Key_Tab:        vkey = VK_TAB;        break;
                case Key_PageUp:     vkey = VK_PRIOR;      break;
                case Key_PageDown:   vkey = VK_NEXT;       break;
                case Key_End:        vkey = VK_END;        break;
                case Key_Home:       vkey = VK_HOME;       break;
                case Key_Insert:     vkey = VK_INSERT;     break;
                case Key_Delete:     vkey = VK_DELETE;     break;
                case Key_Add:        vkey = VK_ADD;        break;
                case Key_Subtract:   vkey = VK_SUBTRACT;   break;
                case Key_Multiply:   vkey = VK_MULTIPLY;   break;
                case Key_Divide:     vkey = VK_DIVIDE;     break;
                case Key_Left:       vkey = VK_LEFT;       break;
                case Key_Right:      vkey = VK_RIGHT;      break;
                case Key_Up:         vkey = VK_UP;         break;
                case Key_Down:       vkey = VK_DOWN;       break;
                case Key_Numpad0:    vkey = VK_NUMPAD0;    break;
                case Key_Numpad1:    vkey = VK_NUMPAD1;    break;
                case Key_Numpad2:    vkey = VK_NUMPAD2;    break;
                case Key_Numpad3:    vkey = VK_NUMPAD3;    break;
                case Key_Numpad4:    vkey = VK_NUMPAD4;    break;
                case Key_Numpad5:    vkey = VK_NUMPAD5;    break;
                case Key_Numpad6:    vkey = VK_NUMPAD6;    break;
                case Key_Numpad7:    vkey = VK_NUMPAD7;    break;
                case Key_Numpad8:    vkey = VK_NUMPAD8;    break;
                case Key_Numpad9:    vkey = VK_NUMPAD9;    break;
                case Key_F1:         vkey = VK_F1;         break;
                case Key_F2:         vkey = VK_F2;         break;
                case Key_F3:         vkey = VK_F3;         break;
                case Key_F4:         vkey = VK_F4;         break;
                case Key_F5:         vkey = VK_F5;         break;
                case Key_F6:         vkey = VK_F6;         break;
                case Key_F7:         vkey = VK_F7;         break;
                case Key_F8:         vkey = VK_F8;         break;
                case Key_F9:         vkey = VK_F9;         break;
                case Key_F10:        vkey = VK_F10;        break;
                case Key_F11:        vkey = VK_F11;        break;
                case Key_F12:        vkey = VK_F12;        break;
                case Key_F13:        vkey = VK_F13;        break;
                case Key_F14:        vkey = VK_F14;        break;
                case Key_F15:        vkey = VK_F15;        break;
                case Key_Pause:      vkey = VK_PAUSE;      break;
                default:             vkey = 0;             break;
            }

            return ( GetAsyncKeyState( vkey ) & 0x8000 ) != 0;
        }

#elif defined( OS_LINUX )

        bool isKeyPressed( Key key )
        {
            KeySym keysym = 0;
            switch (key)
            {
                case Key_LShift:     keysym = XK_Shift_L;      break;
                case Key_RShift:     keysym = XK_Shift_R;      break;
                case Key_LControl:   keysym = XK_Control_L;    break;
                case Key_RControl:   keysym = XK_Control_R;    break;
                case Key_LAlt:       keysym = XK_Alt_L;        break;
                case Key_RAlt:       keysym = XK_Alt_R;        break;
                case Key_LSystem:    keysym = XK_Super_L;      break;
                case Key_RSystem:    keysym = XK_Super_R;      break;
                case Key_Menu:       keysym = XK_Menu;         break;
                case Key_Escape:     keysym = XK_Escape;       break;
                case Key_SemiColon:  keysym = XK_semicolon;    break;
                case Key_Slash:      keysym = XK_slash;        break;
                case Key_Equal:      keysym = XK_equal;        break;
                case Key_Dash:       keysym = XK_minus;        break;
                case Key_LBracket:   keysym = XK_bracketleft;  break;
                case Key_RBracket:   keysym = XK_bracketright; break;
                case Key_Comma:      keysym = XK_comma;        break;
                case Key_Period:     keysym = XK_period;       break;
                case Key_Quote:      keysym = XK_apostrophe;   break;
                case Key_BackSlash:  keysym = XK_backslash;    break;
                case Key_Tilde:      keysym = XK_grave;        break;
                case Key_Space:      keysym = XK_space;        break;
                case Key_Return:     keysym = XK_Return;       break;
                case Key_BackSpace:  keysym = XK_BackSpace;    break;
                case Key_Tab:        keysym = XK_Tab;          break;
                case Key_PageUp:     keysym = XK_Prior;        break;
                case Key_PageDown:   keysym = XK_Next;         break;
                case Key_End:        keysym = XK_End;          break;
                case Key_Home:       keysym = XK_Home;         break;
                case Key_Insert:     keysym = XK_Insert;       break;
                case Key_Delete:     keysym = XK_Delete;       break;
                case Key_Add:        keysym = XK_KP_Add;       break;
                case Key_Subtract:   keysym = XK_KP_Subtract;  break;
                case Key_Multiply:   keysym = XK_KP_Multiply;  break;
                case Key_Divide:     keysym = XK_KP_Divide;    break;
                case Key_Pause:      keysym = XK_Pause;        break;
                case Key_F1:         keysym = XK_F1;           break;
                case Key_F2:         keysym = XK_F2;           break;
                case Key_F3:         keysym = XK_F3;           break;
                case Key_F4:         keysym = XK_F4;           break;
                case Key_F5:         keysym = XK_F5;           break;
                case Key_F6:         keysym = XK_F6;           break;
                case Key_F7:         keysym = XK_F7;           break;
                case Key_F8:         keysym = XK_F8;           break;
                case Key_F9:         keysym = XK_F9;           break;
                case Key_F10:        keysym = XK_F10;          break;
                case Key_F11:        keysym = XK_F11;          break;
                case Key_F12:        keysym = XK_F12;          break;
                case Key_F13:        keysym = XK_F13;          break;
                case Key_F14:        keysym = XK_F14;          break;
                case Key_F15:        keysym = XK_F15;          break;
                case Key_Left:       keysym = XK_Left;         break;
                case Key_Right:      keysym = XK_Right;        break;
                case Key_Up:         keysym = XK_Up;           break;
                case Key_Down:       keysym = XK_Down;         break;
                case Key_Numpad0:    keysym = XK_KP_Insert;    break;
                case Key_Numpad1:    keysym = XK_KP_End;       break;
                case Key_Numpad2:    keysym = XK_KP_Down;      break;
                case Key_Numpad3:    keysym = XK_KP_Page_Down; break;
                case Key_Numpad4:    keysym = XK_KP_Left;      break;
                case Key_Numpad5:    keysym = XK_KP_Begin;     break;
                case Key_Numpad6:    keysym = XK_KP_Right;     break;
                case Key_Numpad7:    keysym = XK_KP_Home;      break;
                case Key_Numpad8:    keysym = XK_KP_Up;        break;
                case Key_Numpad9:    keysym = XK_KP_Page_Up;   break;
                case Key_A:          keysym = XK_a;            break;
                case Key_B:          keysym = XK_b;            break;
                case Key_C:          keysym = XK_c;            break;
                case Key_D:          keysym = XK_d;            break;
                case Key_E:          keysym = XK_e;            break;
                case Key_F:          keysym = XK_f;            break;
                case Key_G:          keysym = XK_g;            break;
                case Key_H:          keysym = XK_h;            break;
                case Key_I:          keysym = XK_i;            break;
                case Key_J:          keysym = XK_j;            break;
                case Key_K:          keysym = XK_k;            break;
                case Key_L:          keysym = XK_l;            break;
                case Key_M:          keysym = XK_m;            break;
                case Key_N:          keysym = XK_n;            break;
                case Key_O:          keysym = XK_o;            break;
                case Key_P:          keysym = XK_p;            break;
                case Key_Q:          keysym = XK_q;            break;
                case Key_R:          keysym = XK_r;            break;
                case Key_S:          keysym = XK_s;            break;
                case Key_T:          keysym = XK_t;            break;
                case Key_U:          keysym = XK_u;            break;
                case Key_V:          keysym = XK_v;            break;
                case Key_W:          keysym = XK_w;            break;
                case Key_X:          keysym = XK_x;            break;
                case Key_Y:          keysym = XK_y;            break;
                case Key_Z:          keysym = XK_z;            break;
                case Key_Num0:       keysym = XK_0;            break;
                case Key_Num1:       keysym = XK_1;            break;
                case Key_Num2:       keysym = XK_2;            break;
                case Key_Num3:       keysym = XK_3;            break;
                case Key_Num4:       keysym = XK_4;            break;
                case Key_Num5:       keysym = XK_5;            break;
                case Key_Num6:       keysym = XK_6;            break;
                case Key_Num7:       keysym = XK_7;            break;
                case Key_Num8:       keysym = XK_8;            break;
                case Key_Num9:       keysym = XK_9;            break;
                default:             keysym = 0;               break;
            }

            Display* dis = XOpenDisplay( nullptr );
            xcb_keycode_t keycode = XKeysymToKeycode( dis, keysym );
            XCloseDisplay( dis );

            xcb_connection_t* connection = xcb_connect( nullptr, nullptr );
            xcb_generic_error_t* error = nullptr;

            xcb_query_keymap_reply_t* keymap = xcb_query_keymap_reply(
                connection, xcb_query_keymap( connection ), &error );

            xcb_disconnect( connection );

            if ( error )
            {
                LOG( logERROR ) << "Could not query keymap in isKeyPressed()";
                return false;
            }

            // NOTE(Charly): No real idea what is going on there, but it seems to work :D
            return ( keymap->keys[keycode / 8] & ( 1 << ( keycode % 8 ) ) ) != 0;
        }
#elif defined( OS_MACOS )
        // FIXME (Mathias) On MacoX, symbol with ANSI in their name are given for an ANSI US keyboard ...
        // FIXME (Mathias) : WARNING : some symbols do'nt exist on Apple keyboard ...
        bool isKeyPressed( Key key )
        {
            unsigned short keysym = 0;
            switch (key)
            {
                case Key_LShift:     keysym = kVK_Shift;      break;
                case Key_RShift:     keysym = kVK_RightShift;      break;
                case Key_LControl:   keysym = kVK_Control;    break;
                case Key_RControl:   keysym = kVK_RightControl;    break;
                case Key_LAlt:       keysym = kVK_Option;        break;
                case Key_RAlt:       keysym = kVK_RightOption;        break;
                case Key_LSystem:    keysym = kVK_Command;      break;
                case Key_RSystem:    keysym = kVK_Command;      break;
                case Key_Menu:       keysym = kVK_Function;         break;
                case Key_Escape:     keysym = kVK_Escape;       break;
                case Key_SemiColon:  keysym = kVK_ANSI_Semicolon;    break;
                case Key_Slash:      keysym = kVK_ANSI_Slash;        break;
                case Key_Equal:      keysym = kVK_ANSI_Equal;        break;
                case Key_Dash:       keysym = kVK_ANSI_Minus;        break;
                case Key_LBracket:   keysym = kVK_ANSI_LeftBracket;  break;
                case Key_RBracket:   keysym = kVK_ANSI_RightBracket; break;
                case Key_Comma:      keysym = kVK_ANSI_Comma;        break;
                case Key_Period:     keysym = kVK_ANSI_Period;       break;
                case Key_Quote:      keysym = kVK_ANSI_Quote;   break;
                case Key_BackSlash:  keysym = kVK_ANSI_Backslash;    break;
                case Key_Tilde:      keysym = kVK_ANSI_Grave;        break;
                case Key_Space:      keysym = kVK_Space;        break;
                case Key_Return:     keysym = kVK_Return;       break;
                case Key_BackSpace:  keysym = kVK_Delete;    break;
                case Key_Tab:        keysym = kVK_Tab;          break;
                case Key_PageUp:     keysym = kVK_PageUp;        break;
                case Key_PageDown:   keysym = kVK_PageDown;         break;
                case Key_End:        keysym = kVK_End;          break;
                case Key_Home:       keysym = kVK_Home;         break;
                case Key_Insert:     keysym = kVK_F20;       break;
                case Key_Delete:     keysym = kVK_Delete;       break;
                case Key_Add:        keysym = kVK_ANSI_KeypadPlus;       break;
                case Key_Subtract:   keysym = kVK_ANSI_KeypadMinus;  break;
                case Key_Multiply:   keysym = kVK_ANSI_KeypadMultiply;  break;
                case Key_Divide:     keysym = kVK_ANSI_KeypadDivide;    break;
                case Key_Pause:      keysym = kVK_Mute;        break;
                case Key_F1:         keysym = kVK_F1;           break;
                case Key_F2:         keysym = kVK_F2;           break;
                case Key_F3:         keysym = kVK_F3;           break;
                case Key_F4:         keysym = kVK_F4;           break;
                case Key_F5:         keysym = kVK_F5;           break;
                case Key_F6:         keysym = kVK_F6;           break;
                case Key_F7:         keysym = kVK_F7;           break;
                case Key_F8:         keysym = kVK_F8;           break;
                case Key_F9:         keysym = kVK_F9;           break;
                case Key_F10:        keysym = kVK_F10;          break;
                case Key_F11:        keysym = kVK_F11;          break;
                case Key_F12:        keysym = kVK_F12;          break;
                case Key_F13:        keysym = kVK_F13;          break;
                case Key_F14:        keysym = kVK_F14;          break;
                case Key_F15:        keysym = kVK_F15;          break;
                case Key_Left:       keysym = kVK_LeftArrow;         break;
                case Key_Right:      keysym = kVK_RightArrow;        break;
                case Key_Up:         keysym = kVK_UpArrow;           break;
                case Key_Down:       keysym = kVK_DownArrow;         break;
                case Key_Numpad0:    keysym = kVK_ANSI_Keypad0;      break;
                case Key_Numpad1:    keysym = kVK_ANSI_Keypad1;      break;
                case Key_Numpad2:    keysym = kVK_ANSI_Keypad2;      break;
                case Key_Numpad3:    keysym = kVK_ANSI_Keypad3;      break;
                case Key_Numpad4:    keysym = kVK_ANSI_Keypad4;      break;
                case Key_Numpad5:    keysym = kVK_ANSI_Keypad5;      break;
                case Key_Numpad6:    keysym = kVK_ANSI_Keypad6;      break;
                case Key_Numpad7:    keysym = kVK_ANSI_Keypad7;      break;
                case Key_Numpad8:    keysym = kVK_ANSI_Keypad8;      break;
                case Key_Numpad9:    keysym = kVK_ANSI_Keypad9;      break;
                case Key_A:          keysym = kVK_ANSI_A;            break;
                case Key_B:          keysym = kVK_ANSI_B;            break;
                case Key_C:          keysym = kVK_ANSI_C;            break;
                case Key_D:          keysym = kVK_ANSI_D;            break;
                case Key_E:          keysym = kVK_ANSI_E;            break;
                case Key_F:          keysym = kVK_ANSI_F;            break;
                case Key_G:          keysym = kVK_ANSI_G;            break;
                case Key_H:          keysym = kVK_ANSI_H;            break;
                case Key_I:          keysym = kVK_ANSI_I;            break;
                case Key_J:          keysym = kVK_ANSI_J;            break;
                case Key_K:          keysym = kVK_ANSI_K;            break;
                case Key_L:          keysym = kVK_ANSI_L;            break;
                case Key_M:          keysym = kVK_ANSI_M;            break;
                case Key_N:          keysym = kVK_ANSI_N;            break;
                case Key_O:          keysym = kVK_ANSI_O;            break;
                case Key_P:          keysym = kVK_ANSI_P;            break;
                case Key_Q:          keysym = kVK_ANSI_Q;            break;
                case Key_R:          keysym = kVK_ANSI_R;            break;
                case Key_S:          keysym = kVK_ANSI_S;            break;
                case Key_T:          keysym = kVK_ANSI_T;            break;
                case Key_U:          keysym = kVK_ANSI_U;            break;
                case Key_V:          keysym = kVK_ANSI_V;            break;
                case Key_W:          keysym = kVK_ANSI_W;            break;
                case Key_X:          keysym = kVK_ANSI_X;            break;
                case Key_Y:          keysym = kVK_ANSI_Y;            break;
                case Key_Z:          keysym = kVK_ANSI_Z;            break;
                case Key_Num0:       keysym = kVK_ANSI_0;            break;
                case Key_Num1:       keysym = kVK_ANSI_1;            break;
                case Key_Num2:       keysym = kVK_ANSI_2;            break;
                case Key_Num3:       keysym = kVK_ANSI_3;            break;
                case Key_Num4:       keysym = kVK_ANSI_4;            break;
                case Key_Num5:       keysym = kVK_ANSI_5;            break;
                case Key_Num6:       keysym = kVK_ANSI_6;            break;
                case Key_Num7:       keysym = kVK_ANSI_7;            break;
                case Key_Num8:       keysym = kVK_ANSI_8;            break;
                case Key_Num9:       keysym = kVK_ANSI_9;            break;
                default:             keysym = 0;               break;
            }
            unsigned char keyMap[16];
            GetKeys((BigEndianUInt32*) &keyMap);
            return (0 != ((keyMap[ keysym >> 3] >> (keysym & 7)) & 1));
        }
#else
        bool isKeyPressed( Key key )
        {
            //LOG( logERROR ) << "isKeyPressed is not implemented for your OS.";
                return false;
        }
#endif
    }
}
