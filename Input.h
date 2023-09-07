#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

struct Input {

    public:

        void UpdateInput(Event event, RenderWindow &window);

        /// Only call at start of frame
        void Reset();

        Event event; // used to get keyboard input later

        bool UIInteraction;

        Vector2f mousePos;

        bool lMousePressed = false;
        bool lMouseReleased = false;
        bool lMouseDown = false;
        bool rMousePressed = false;
        bool rMouseReleased = false;
        bool rMouseDown = false;
        bool backSpace = false;
        bool PReleased = false;

        // text stuff
        // because sfml does not handle this nice
        string typingInput = "";
        bool KeyboardInput = false;

};

#endif // INPUT_H
