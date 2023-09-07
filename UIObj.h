#ifndef UIOBJ_H
#define UIOBJ_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Input.h"
#include <math.h>
#include <algorithm>

using namespace std;
using namespace sf;

enum UIType {

    null = 0,
    Button,
    Slider,
    InputBox,
    TextUI

};

struct UIObj
{
    public:

        UIObj(Input *input, RenderWindow *window, float *deltaTime, Font &font);

        // make functions
        /// returns true if it can make button else false,
        /// in this case there should only ever be one image so mainImage is the button
        bool MakeButton(RectangleShape mainImage, Vector2f position = Vector2f(0,0));
        /// returns true if it can make slider else false,
        /// Slider is the object which the user uses,
        /// Size will be the total size of the slider in the x and y,
        /// make sure that slider fits within the background (Size determins the size of the slider)
        bool MakeSlider(RectangleShape Slider, float lengthOfSlider, Vector2f position = Vector2f(0,0), float minValue = 0, float maxValue = 1);
        /// returns true if it can make input box else false,
        /// If the _Update... is true the update function with that name will be called on the default update
        bool MakeInputBox(Vector2f InputBoxSize, Vector2f position = Vector2f(0,0), bool _InputMustBeNumber = false, bool _UpdateMainSize = false, bool _UpdatePointerSize = false, bool _AutoFitText = false, bool _InputMustBeInt_p = false);
        /// returns true if it can make Text else false,
        /// this is a basic text box that can not be interacted by from the user
        bool MakeText(Vector2f mainSize, Vector2f position, bool _AutoFitText = false, bool _UpdateMainSize = false);
        // ---------------

        // ---------------
        /// sets the position of this UI based on the center of the total size of the UI
        void SetPosition(Vector2f position);
        /// changes the string of the text if this UIObj has the ability to do so
        void setString(string String);
        /// adds the string to the current string
        void addString(string String);
        /// adds the char to the current string
        void addChar(char Char);
        // ---------------

        // must be called to make the UI work
        /// required to update the UI with current input
        void Update(bool drawThis = true);
        /// if the update is drawing dont call this
        void Draw();
        // ---------------

        // Used to simplify usage of the UI
        /// check if the mouse is over the selected RectangleShape
        bool IsHovering(FloatRect GlobalBounds);
        /// reset the color and size of the UIObj if 0,
        /// increase the size and enable outline color of the UIObj if 1,
        /// increase the size and enable outline color (MORE) of the UIObj if 2,
        /// this uses the mainObj/first Obj in the images vector assuming it is the main Obj as it should be
        void ShowVisual(int show = 0);
        /// tries to set the size of the text as big as it can to fit with in the size of this obj,
        /// will only do anything if this UIObj has a text type
        void AutoFitText();
        /// Updates the main size in the x not the y and updates origin of image 0, making it the size of the 0th image in the vector of images if it was changed or the text if this is a text UIObj
        /// If the size of text (string) is 0 then the main size will revert to the original size
        void UpdateMainSize();
        /// Updates the main size, making it fit the string at the character size given,
        /// DOES NOT set the text to the string given, ONLY resizes the Mainsize to fit that string at the given char size or current char size if no charSize given
        void UpdateMainSizeFor(string String, int charSize = -1);
        /// Updates the size of the text to fit within current mainSize with the given string,
        /// DOES NOT change the text to the given string only changes size
        void UpdateCharacterSizeFor(string String);
        /// Updates the size of the pointer (if the text has changed make sure to call this unless the class is already doing so)
        void UpdatePointerSize();
        /// Updates the size of the Text and the pointer KEEPING size of box const
        void UpdateInputBox();
        /// Only works with Sliders and Buttons,
        /// Applys the texture given to the main image
        void setTexture(const Texture *texture);
        /// Sets the fill color of the main image
        void setColor(Color color);
        /// Sets this UIObj active or false
        /// This obj will allow its self to be drawn and updated if this is true else it will do nothing
        void SetActive(bool active);
        /// returns the current bool for if this obj is active or not
        bool IsActive();
        /// sets the value of the slider and updates its position
        void setSliderValue(float value);
        /// Sets the connected UIObj
        /// ONLY works if the UIObj is a slider to a input or input to a sliders OR a slider to a text and vise versa
        void connectUIObj(UIObj* UIObj);
        /// sets the char size of the description text
        void SetDescriptionCharSize(int Size);
        /// sets the description of this UIObj
        void SetDescription(string String);
        /// CALL THIS AFTER every UIObj has been drawn
        static void DrawDescription();
        // ---------------

        UIType Type = UIType::null;

        // all images used for UIObj
        vector<RectangleShape> Images;
        // used only for Text Components
        Text text;

        // size vars
        Vector2f ObjPosition; /// the position of the UIObj in center
        Vector2f mainSize; /// the size of the main image (button, slider, input box area)

        static Input* input;
        static RenderWindow* window;
        static float* deltaTime;

        string Description = "NA";

        // used for slider
        float maxSliderValue;
        float minSliderValue;
        float sliderValue = 0.f;
        // ---------------

        // used for button and slider
        bool HoldingDown = false;

        // used for button
        bool buttonClicked = false;
        // ----------------

        // used for input box
        float pointerTimer = 0;
        bool showPointer = false;
        bool InputMustBeNumber = false;
        bool InputMustBeInt_p = false;
        // ------------------

        // used for typing in input boxes
        static UIObj* SelectedInputBox;
        string inputString = "";
        string currentString = "";
        // ------------------------------

        bool Active = true;

        float* ConnectedValue = nullptr;

        // specific to description stuff
        static RectangleShape* DescriptionBackground;
        static Text* DescriptionText;
        static bool drawDescription;
        static UIObj* hoveringObj;
        static float hoverTimer;
        // -----------------------------

    private:

        // any var that SHOULD NOT ever be changed without function use is stored as a private var

        // used for input box
        bool _UpdateMainSize = false;
        bool _UpdatePointerSize = false;
        bool _AutoFitText = false;
        // -----------------

        Vector2f OriginalSize;

        // SFML text is annoying

        /// another function because I dont understand why SFML is like this,
        /// only for text
        void setTextOriginToCenter();

        Vector2f Round(Vector2f Vector);

        // ---------------------------------------
};

#endif // UIOBJ_H
