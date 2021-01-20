#pragma once
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
