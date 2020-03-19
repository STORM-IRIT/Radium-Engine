#ifndef RADIUMENGINE_TIMELINE_CONFIG_HPP_
#define RADIUMENGINE_TIMELINE_CONFIG_HPP_

// fix timeline's width directly in animtimeline.ui

// first timeline position is fixed at startup with these margins (right/bottom)
// parent area :
//  ---------------------------------
// |                                 |
// |                                 |
// |                                 |
// |                                 |
// |             ------------        |
// |            |  timeline  | <---> |
// |             ------------        |
// |                                 |
// |                                 |
// |                                 |
//  ---------------------------------
#define TIMELINE_MARGIN_RIGHT 50 // unit : pixel
//  ---------------------------------
// |                                 |
// |                                 |
// |                                 |
// |                                 |
// |             ------------        |
// |            |  timeline  |       |
// |             ------------        |
// |                  ^              |
// |                  |              |
// |                  v              |
//  ---------------------------------
#define TIMELINE_MARGIN_DOWN 40 // unit : pixel

//  -----------------------     -----------------------
// |    |   timeline   ||  |   |   |   timeline    ||  |
//  -----------------------     -----------------------
//          ^                      ^ <--
//          |                      |
//        cursor                 cursor
// on ruler clicking, automove to nearest keyFrame or ruler scale
// if the distance with cursor is below this constant
#define TIMELINE_AUTO_SUGGEST_CURSOR_RADIUS 4 // unit : pixel

// zoom in/out speed factor (<wheelMouse> on ruler)
#define TIMELINE_ZOOM_SPEED 1.0 // unit : double (0.0 .. oo)

// slide ruler speed factor (<ctrl> + <wheelMouse> on ruler)
#define TIMELINE_SLIDE_SPEED 1.0 // unit : double (0.0 .. oo)

// todo : maybe set global static var
// frame per second to draw position of each frame in ruler
#define TIMELINE_FPS 60

// 500M bytes in RAM, max bytes for saving user anim for undo/redo stack
#define TIMELINE_BUFFER_SESSION_MAX_SIZE 500000000

// millisecond, auto save environment after delay
#define TIMELINE_DELAY_AUTO_SAVE 100

#endif // RADIUMENGINE_TIMELINE_CONFIG_HPP_
