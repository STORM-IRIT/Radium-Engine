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
#endif

namespace Ra
{
    namespace Gui
    {
#if defined( OS_WINDOWS )

        bool isKeyPressed( Key key )
        {

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
#else
        bool isKeyPressed( Key key )
        {
            LOG( logERROR ) << "isKeyPressed is not implemented for your OS."
                return false;
        }
#endif
    }
}
