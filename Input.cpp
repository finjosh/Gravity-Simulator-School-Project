#include "Input.h"

void Input::UpdateInput(Event event, RenderWindow &window)
{
    Input::event = event;

    if (event.type == Event::TextEntered) {
        if (event.text.unicode < 128) {
            // SFML sucks it makes a space instead of new line
            // return unicode is 13 therefor it should add a '\n' but it does not so I need this
            if (event.text.unicode == 13) {
                Input::typingInput.push_back('\n');
                Input::KeyboardInput = true;
            } else {
                Input::typingInput.push_back(static_cast<char>(event.text.unicode));
                Input::KeyboardInput = true;
            }
        }
    }

    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
        Input::lMousePressed = true;
        Input::lMouseDown = true;
    }

    if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left) {
        Input::lMouseReleased = true;
        Input::lMouseDown = false;
    }

    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Right) {
        Input::rMousePressed = true;
        Input::rMouseDown = true;
    }

    if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Right) {
        Input::rMouseReleased = true;
        Input::rMouseDown = false;
    }

    if (event.type == Event::KeyPressed && event.key.code == Keyboard::BackSpace) {
        Input::backSpace = true;
    }

    if (event.type == Event::KeyReleased && event.key.code == Keyboard::P) {
        Input::PReleased = true;
    }

    Input::mousePos = (Vector2f)Mouse::getPosition(window);
    Input::mousePos = Vector2f(mousePos.x < 0 ? 0 : (mousePos.x > window.getSize().x ? window.getSize().x : mousePos.x),
                        mousePos.y < 0 ? 0 : (mousePos.y > window.getSize().y ? window.getSize().y : mousePos.y));
}

/// Only call at start of frame
void Input::Reset()
{
    // reseting required values
    Input::lMousePressed = false;
    Input::lMouseReleased = false;
    Input::rMousePressed = false;
    Input::rMouseReleased = false;

    Input::backSpace = false;
    Input::PReleased = false;

    Input::UIInteraction = false;
    Input::KeyboardInput = false;
    Input::typingInput = "";
}
