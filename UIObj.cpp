#include "UIObj.h"

// initilizing static vars
UIObj* UIObj::SelectedInputBox = nullptr;
RectangleShape* UIObj::DescriptionBackground = nullptr;
Text* UIObj::DescriptionText = nullptr;
bool UIObj::drawDescription = false;
float UIObj::hoverTimer = 0.f;
UIObj* UIObj::hoveringObj = nullptr;
Input* UIObj::input = nullptr;
RenderWindow* UIObj::window = nullptr;
float* UIObj::deltaTime = nullptr;
// -----------------------

UIObj::UIObj(Input *input, RenderWindow *window, float *deltaTime, Font &textFont)
{
    // if one of the static vars are null then the others need to be set
    UIObj::text.setFont(textFont);

    if (UIObj::input == nullptr)
    {
        UIObj::input = input;
        UIObj::window = window;
        UIObj::deltaTime = deltaTime;


        UIObj::DescriptionBackground = new RectangleShape(Vector2f(1,1));
        UIObj::DescriptionText = new Text();
        UIObj::DescriptionText->setFont(textFont);

        UIObj::DescriptionBackground->setFillColor(Color::Black);
        UIObj::DescriptionBackground->setOutlineColor(Color::Red);
        UIObj::DescriptionBackground->setOutlineThickness(2);
        UIObj::DescriptionText->setCharacterSize(15);
    }
}

/// returns true if it can make button else false
/// in this case there should only ever be one image so mainImage is the button
bool UIObj::MakeButton(RectangleShape mainImage, Vector2f position)
{

    if (UIObj::Type == null)
    {
        UIObj::Type = UIType::Button;

        // initilizing the mainImage
        UIObj::Images.push_back(mainImage);
        RectangleShape &temp = UIObj::Images.back();
        UIObj::mainSize = temp.getSize(); // setting the original size of the mainObj for later use
        UIObj::OriginalSize = UIObj::mainSize;
        UIObj::ObjPosition = position;
        temp.setOutlineColor(Color(7,185,252,255));
        temp.setOrigin(Vector2f(mainSize.x/2, mainSize.y/2));
        temp.setPosition(position);

        return true;
    }
    else
        return false;
}

/// returns true if it can make slider else false
/// Slider is the object which the user uses
/// Size will be the total size of the slider in the x and y
/// make sure that slider fits within the background (Size determins the size of the slider)
bool UIObj::MakeSlider(RectangleShape Slider, float lengthOfSlider, Vector2f position, float minValue, float maxValue)
{
    if (UIObj::Type == null)
    {
        UIObj::Type = UIType::Slider;

        // setup for slider
        UIObj::Images.push_back(Slider);
        RectangleShape *temp = &UIObj::Images.back();
        UIObj::mainSize = temp->getSize(); // setting the original size of the mainObj for later use
        UIObj::OriginalSize = UIObj::mainSize;
        temp->setOutlineColor(Color(7,185,252,255));
        temp->setOrigin(Vector2f(mainSize.x/2, mainSize.y/2 + 1.75));
        temp->setPosition(Vector2f(position.x - lengthOfSlider/2, position.y));

        // setup for slider bar
        UIObj::Images.push_back(RectangleShape(Vector2f(lengthOfSlider, 3.5)));
        UIObj::Images.back().setOrigin(lengthOfSlider/2, 1.75);
        UIObj::Images.back().setPosition(position);

        UIObj::ObjPosition = position;

        // setting Slider specific value
        UIObj::maxSliderValue = maxValue;
        UIObj::minSliderValue = minValue;
        UIObj::sliderValue = minValue;

        UIObj::setSliderValue(0);

        return true;
    }
    else
        return false;
}

/// returns true if it can make input box else false
///
/// main clock should be reset each frame, If the _Update... is true the update function with that name will be called on the default update
bool UIObj::MakeInputBox(Vector2f InputBoxSize, Vector2f position, bool InputMustBeNumber, bool _UpdateMainSize, bool _UpdatePointerSize, bool _AutoFitText, bool _InputMustBeInt_p)
{
    if (UIObj::Type == null)
    {
        UIObj::Type = UIType::InputBox;

        UIObj::mainSize = InputBoxSize; // setting the original size of the mainObj for later use
        UIObj::OriginalSize = UIObj::mainSize;

        // initilizing the textBox / mainImage
        UIObj::Images.push_back(RectangleShape(InputBoxSize));
        RectangleShape *temp = &UIObj::Images.back();
        temp->setFillColor(Color::Transparent);
        temp->setOutlineColor(Color(7,185,252,255));
        temp->setOrigin(Vector2f(UIObj::mainSize.x/2, UIObj::mainSize.y/2));
        temp->setPosition(position);

        UIObj::setString(InputMustBeNumber ? "123.0" : "New Text");
        UIObj::text.setPosition(position);

        // initilizing the pointer for the text box
        UIObj::Images.push_back(RectangleShape(Vector2f(3, UIObj::text.getCharacterSize())));
        temp = &UIObj::Images.back();
        temp->setFillColor(Color::Transparent);
        temp->setOutlineColor(Color(7,185,252,255));
        temp->setOrigin(Vector2f(temp->getSize().x/2, temp->getSize().y/2));
        temp->setPosition(position);

        UIObj::ObjPosition = position;
        UIObj::InputMustBeNumber = InputMustBeNumber;
        UIObj::_UpdateMainSize = _UpdateMainSize;
        UIObj::_UpdatePointerSize = _UpdatePointerSize;
        UIObj::_AutoFitText = _AutoFitText;
        UIObj::InputMustBeInt_p = _InputMustBeInt_p;

        return true;
    }
    else
        return false;
}

/// returns true if it can make Text else false,
/// this is a basic text box that can not be interacted by from the user
bool UIObj::MakeText(Vector2f mainSize,  Vector2f position, bool _AutoFitText, bool _UpdateMainSize)
{
    if (UIObj::Type == null)
    {
        UIObj::Type = UIType::TextUI;

        UIObj::mainSize = mainSize; // setting the original size of the mainObj for later use
        UIObj::OriginalSize = mainSize;

        UIObj::setString("New Text");
        UIObj::text.setPosition(position);

        UIObj::ObjPosition = position;
        UIObj::_UpdateMainSize = _UpdateMainSize;
        UIObj::_AutoFitText = _AutoFitText;

        return true;
    }
    else
        return false;
}

/// sets the position of this UI based on the center of the total size of the UI
void UIObj::SetPosition(Vector2f position)
{
    // using for loop to move the "images" or rectangle shapes by the difference as the slider for example
    // could be off from the center position and needs to stay like that
    for (int i = 0; i < UIObj::Images.size(); i++)
    {
        UIObj::Images[i].move(Vector2f(position - UIObj::ObjPosition));
    }
    if (UIObj::Type == UIType::InputBox || UIObj::Type == UIType::TextUI)
        UIObj::text.setPosition(position);

    UIObj::ObjPosition = position;
}

/// changes the string of the text if this UIObj has the ability to do so
void UIObj::setString(string String)
{
    if (UIObj::Type == UIType::InputBox || UIObj::Type == UIType::TextUI)
    {
        UIObj::text.setString(String);
        UIObj::setTextOriginToCenter();
        UIObj::currentString = String;
    }
}

/// adds the string to the current string
void UIObj::addString(string String)
{
    if (UIObj::Type == UIType::InputBox || UIObj::Type == UIType::TextUI)
    {
        UIObj::setString(UIObj::text.getString() + String);
    }
}

/// adds the char to the current string
void UIObj::addChar(char Char)
{
    if (UIObj::Type == UIType::InputBox || UIObj::Type == UIType::TextUI)
    {
        UIObj::setString(UIObj::text.getString() + Char);
    }
}

/// required to update the UI with current input
/// make sure that this is run AFTER the event/input class is updated
void UIObj::Update(bool drawThis)
{

    bool isHovering = false;

    if (UIObj::Type != UIType::TextUI)
    {
        isHovering = IsHovering(UIObj::Images[0].getGlobalBounds());
    }

    // reseting the draw var
     if (isHovering)
     {
        if (UIObj::hoveringObj != this)
        {
            UIObj::hoveringObj = this;
            UIObj::hoverTimer = 0.f;
            UIObj::drawDescription = false;
        } else
        if (!(UIObj::input->lMouseDown || UIObj::input->lMouseReleased || UIObj::input->rMouseDown || UIObj::input->rMouseReleased))
        {
            UIObj::hoveringObj = this;
            UIObj::hoverTimer += *UIObj::deltaTime;
            if (UIObj::hoverTimer >= 0.75) UIObj::drawDescription = true; UIObj::DescriptionText->setString(UIObj::Description);
        } else {
            UIObj::hoverTimer = 0.f;
            UIObj::drawDescription = false;
        }
     } else
     if (UIObj::hoveringObj == this) {
        UIObj::hoveringObj = nullptr;
        UIObj::hoverTimer = 0.f;
        UIObj::drawDescription = false;
     }

    if (UIObj::IsActive()) {
        if (UIObj::Type == UIType::Button)
        {
            UIObj::buttonClicked = false;
            if (isHovering || UIObj::HoldingDown)
            {
                if (UIObj::input->lMouseDown || UIObj::input->rMouseDown)
                {
                    UIObj::HoldingDown = true;
                    UIObj::ShowVisual(isHovering ? 2 : 0);
                }
                else if (UIObj::input->lMouseReleased || UIObj::input->rMouseReleased)
                {
                    UIObj::buttonClicked = isHovering ? true : false;
                    UIObj::HoldingDown = false;
                }
                else
                {
                    UIObj::ShowVisual(isHovering ? 1 : 0);
                }
            }
            else
            {
                UIObj::HoldingDown = false;
                UIObj::ShowVisual(0);
            }
        }
        else if (UIObj::Type == UIType::Slider)
        {
            if (isHovering || UIObj::HoldingDown)
            {

                if (UIObj::input->lMouseDown || UIObj::input->rMouseDown)
                {
                    UIObj::HoldingDown = true;
                    UIObj::ShowVisual(2);

                    // moving the slider
                    float smallX = UIObj::Images[1].getPosition().x - UIObj::Images[1].getSize().x/2;
                    float bigX = UIObj::Images[1].getPosition().x + UIObj::Images[1].getSize().x/2;
                    UIObj::Images[0].setPosition(Vector2f(input->mousePos.x > bigX ? bigX : (input->mousePos.x < smallX ? smallX : input->mousePos.x)
                                                          , UIObj::Images[0].getPosition().y));

                    // updating the value of this slider
                    UIObj::sliderValue = abs((UIObj::Images[1].getPosition().x - UIObj::Images[1].getSize().x/2) - UIObj::Images[0].getPosition().x)
                                                / (UIObj::Images[1].getSize().x / (UIObj::maxSliderValue - UIObj::minSliderValue)) + UIObj::minSliderValue;
                }
                else
                {
                    UIObj::HoldingDown = false;
                    UIObj::ShowVisual(1);
                }

                if (UIObj::ConnectedValue != nullptr) *UIObj::ConnectedValue = sliderValue;
            }
            else
            {
                UIObj::HoldingDown = false;
                UIObj::ShowVisual(0);

                if (UIObj::ConnectedValue != nullptr && *UIObj::ConnectedValue != UIObj::sliderValue) UIObj::setSliderValue(*UIObj::ConnectedValue);
            }
        }
        else if (UIObj::Type == UIType::InputBox)
        {
            if (UIObj::SelectedInputBox != this)
            {
                UIObj::showPointer = false;

                if (UIObj::ConnectedValue != nullptr) {
                    // using only sig figs and setting the string to the slider value
                    string temp = to_string(*UIObj::ConnectedValue);

                    if (temp != UIObj::currentString)
                    {
                        UIObj::setString(temp);
                    }
                }
            }
            else
            {
                // handling for pointer
                UIObj::pointerTimer += *UIObj::deltaTime;
                if (UIObj::pointerTimer >= 0.75)
                {
                    UIObj::pointerTimer = 0;
                    UIObj::showPointer = !UIObj::showPointer;
                }

                // handling text input
                if (UIObj::input->KeyboardInput)
                {
                    string textInput = UIObj::input->typingInput;
                    string currentTextString = UIObj::text.getString();
                    if (UIObj::InputMustBeNumber) {
                        for (int i = 0; i < textInput.size(); i++) {
                            // if enter is clicked then submit the current text
                            if (UIObj::input->event.text.unicode == 13) {
                                UIObj::SelectedInputBox = nullptr;
                                textInput.erase(i);
                                continue;
                            }
                            string temp = UIObj::text.getString();
                            if (UIObj::InputMustBeInt_p) {
                                if (isdigit(textInput[i])) { continue; }
                                else { textInput.erase(i); }
                            } else
                            if (isdigit(textInput[i])
                                || (textInput[i] == 46 && currentTextString[currentTextString.find('.')] != '.' && isdigit(currentTextString[currentTextString.size()-1]))
                                || ((textInput[i] == 45 && temp.size() == 0))) {
                                continue;
                            } else {
                                textInput.erase(i);
                            }
                        }
                    }
                    if (UIObj::input->backSpace)
                    {
                        if (currentTextString.size() != 0)
                        {
                            currentTextString.erase(currentTextString.end()-1);
                            UIObj::setString(currentTextString);
                        }
                    }
                    else
                    {
                        if (textInput.size() + currentTextString.size() <= 38)
                        {
                            UIObj::addString(textInput);
                        }
                    }
                }

                // handeling the connected value
                if (UIObj::ConnectedValue != nullptr && UIObj::currentString.size() >= 1 && UIObj::currentString.back() != '-')
                {
                    *UIObj::ConnectedValue = stof(UIObj::currentString);
                }
            }

            // handling the text box visual
            if (isHovering)
            {
                if (UIObj::input->lMouseDown || UIObj::input->rMouseDown)
                {
                    UIObj::ShowVisual(2);
                }
                else if (UIObj::input->lMouseReleased || UIObj::input->rMouseReleased)
                {
                    UIObj::SelectedInputBox = this;
                    // making pointer show right away
                    UIObj::showPointer = true;
                }
                else
                    UIObj::ShowVisual(1);
            }
            else
            {
                if (UIObj::input->lMouseDown || UIObj::input->rMouseDown)
                {
                    UIObj::SelectedInputBox = nullptr;
                }
                UIObj::ShowVisual(0);
            }

            // handling the automatic updates
            if (UIObj::_UpdateMainSize) UIObj::UpdateMainSize();
            if (UIObj::_UpdatePointerSize) UIObj::UpdatePointerSize();
            if (UIObj::_AutoFitText) UIObj::AutoFitText();
        }
        // otherwise this UIObj is a text obj
        else
        {
            if (UIObj::ConnectedValue != nullptr)
            {
                UIObj::setString(to_string(*UIObj::ConnectedValue));
            }
            if (UIObj::_UpdateMainSize) UIObj::UpdateMainSize();
            if (UIObj::_AutoFitText) UIObj::AutoFitText();
        }

        // drawing the UIObj after updating all of the visual and logical components
        if (drawThis) {
            UIObj::Draw();
        }
    }
}

void UIObj::Draw()
{
    if (UIObj::Type != UIType::null) {
        // drawing from the back of the vector so the less important images are drawn first (background)
        for (int i = UIObj::Images.size()-1; i >= 0; i--)
        {
            UIObj::window->draw(UIObj::Images[i]);
        }
        if (UIObj::Type == UIType::InputBox || UIObj::Type == UIType::TextUI)
        {
            UIObj::window->draw(UIObj::text);
        }
    }
}

bool UIObj::IsHovering(FloatRect GlobalBounds)
{
    if (GlobalBounds.contains(input->mousePos))
    {
        input->UIInteraction = true;
        return true;
    }
    input->UIInteraction = input->UIInteraction || UIObj::HoldingDown;
    return false;
}

/// reset the color and size of the UIObj if 0
/// increase the size and enable outline color of the UIObj if 1
/// increase the size and enable outline color (MORE) of the UIObj if 2
/// this uses the mainObj/first Obj in the images vector assuming it is the main Obj as it should be
void UIObj::ShowVisual(int show)
{
    UIObj::Images[0].setOutlineThickness(show == 0 ? 0 : show+1);
    // assuming that images are set correctly the pointer will be the second image in the vector
    if (UIObj::Type == UIType::InputBox)
    {
        if (UIObj::showPointer) {
            UIObj::Images[1].setOutlineThickness(2);
            UIObj::Images[1].setFillColor(Color::White);
            UIObj::Images[0].setOutlineThickness(1);
            // setting the position of the pointer to the last char in the string
            string temp = UIObj::text.getString();
            if (temp.size() > 0) UIObj::Images[1].setPosition(Vector2f(UIObj::text.findCharacterPos(temp.size()).x + 4,
                                                                UIObj::text.findCharacterPos(temp.size()).y + UIObj::text.getCharacterSize()/2
                                                                + UIObj::text.getLocalBounds().top/2));
            // if there is no string set position to the Obj position
            else UIObj::Images[1].setPosition(UIObj::ObjPosition);
        }
        else {
            UIObj::Images[1].setOutlineThickness(0);
            UIObj::Images[1].setFillColor(Color::Transparent);
            UIObj::Images[0].setOutlineThickness(UIObj::SelectedInputBox == this ? 1 : show == 0 ? 0 : show+1);
        }
    }
}

/// tries to set the size of the text as big as it can to fit with in the size of this obj \n
/// will only do anything if this UIObj has a text type
void UIObj::AutoFitText()
{
    if (UIObj::Type == UIType::InputBox || UIObj::Type == UIType::TextUI)
    {
        // Set the new text height
        UIObj::text.setCharacterSize(UIObj::mainSize.y);

        // The text still has too fit inside the area
        float width = UIObj::text.getLocalBounds().width;
        if (width > UIObj::mainSize.x)
        {
            // Adjust the text size
            UIObj::text.setCharacterSize((UIObj::mainSize.y * UIObj::mainSize.x) / width);
        }

        // reseting origin
        UIObj::setTextOriginToCenter();
    }
}

/// Updates the main size in the x not the y and updates origin of image 0, making it the size of the 0th image in the vector of images if it was changed or the text if this is a text UIObj
/// If the size of text (string) is 0 then the main size will revert to the original size
void UIObj::UpdateMainSize()
{
    if (UIObj::Type == UIType::InputBox)
    {
        string temp = UIObj::text.getString();
        if (temp.size() == 0)
        {
            UIObj::mainSize = UIObj::OriginalSize;
            UIObj::Images[0].setSize(UIObj::mainSize);
            UIObj::Images[0].setOrigin(Vector2f(UIObj::mainSize.x/2, UIObj::mainSize.y/2));
        }
        else
        {
            UIObj::mainSize = Vector2f(Vector2f(UIObj::text.getLocalBounds().left + UIObj::text.getGlobalBounds().width,
                                                UIObj::text.getLocalBounds().top + UIObj::text.getGlobalBounds().height));
            UIObj::Images[0].setSize(UIObj::mainSize);
            UIObj::Images[0].setOrigin(Vector2f(UIObj::mainSize.x/2, UIObj::mainSize.y/2));
        }
    }
    else if (UIObj::Type == UIType::TextUI)
    {
        UIObj::mainSize = Vector2f(Vector2f(UIObj::text.getLocalBounds().left + UIObj::text.getGlobalBounds().width,
                                            UIObj::text.getLocalBounds().top + UIObj::text.getGlobalBounds().height));
    }
    else
    {
        UIObj::mainSize = UIObj::Images[0].getSize();
        UIObj::Images[0].setOrigin(Vector2f(UIObj::mainSize.x/2, UIObj::mainSize.y/2));
    }
}

/// Updates the main size, making it fit the string at the character size given
///
/// DOES NOT set the text to the string given, ONLY resizes the Mainsize to fit that string at the given char size or current char size if no charSize given
/// TEXT ONLY
void UIObj::UpdateMainSizeFor(string String, int charSize)
{
    if (UIObj::Type == UIType::InputBox || UIObj::Type == UIType::TextUI) {
        string stringt = UIObj::text.getString();
        int sizet = UIObj::text.getCharacterSize();
        charSize = charSize <= 0 ? sizet : charSize;
        UIObj::setString(String);
        UIObj::text.setCharacterSize(charSize);
        UIObj::UpdateMainSize();
        UIObj::setString(stringt);
        UIObj::text.setCharacterSize(sizet);
    }
}

/// Updates the size of the text to fit within current mainSize with the given string
///
/// DOES NOT change the text to the given string only changes size
void UIObj::UpdateCharacterSizeFor(string String)
{
    if (UIObj::Type == UIType::InputBox || UIObj::Type == UIType::TextUI) {
        string stringt = UIObj::text.getString();
        UIObj::setString(String);
        UIObj::UpdateInputBox();
        UIObj::setString(stringt);
    }
}

/// Updates the size of the pointer (if the text has changed make sure to call this unless the class is already doing so)
void UIObj::UpdatePointerSize()
{
    // doing a extra check to make sure that this UIObj is a InputBox or else crashing will occur
    if (UIObj::Type == UIType::InputBox)
    {
        UIObj::Images[1].setSize(Vector2f(3, UIObj::text.getCharacterSize()));
        UIObj::Images[1].setOrigin(Vector2f(UIObj::Images[1].getSize().x/2, UIObj::Images[1].getSize().y/2));
    }
}

/// Updates the size of the Text and the pointer KEEPING size of box const
void UIObj::UpdateInputBox()
{
    AutoFitText();
    UpdatePointerSize();
}

/// Only works with Sliders and Buttons,
/// Applys the texture given to the main image
void UIObj::setTexture(const Texture *texture)
{
    if (UIObj::Type == UIType::Button || UIObj::Type == UIType::Slider) {
        UIObj::Images[0].setTexture(texture);
    }
}

/// Sets the fill color of the main image
void UIObj::setColor(Color color)
{
    if (UIObj::Type == UIType::Button || UIObj::Type == UIType::Slider) {
        UIObj::Images[0].setFillColor(color);
    } else
    {
        UIObj::text.setFillColor(color);
    }
}

/// Sets this UIObj active or false
/// This obj will allow its self to be drawn and updated if this is true else it will do nothing
void UIObj::SetActive(bool active)
{
    UIObj::Active = active;
}

/// returns the current bool for if this obj is active or not
bool UIObj::IsActive()
{
    return UIObj::Active;
}

/// sets the value of the slider and updates its position
void UIObj::setSliderValue(float value)
{
    if (UIObj::Type == UIType::Slider) {
        UIObj::sliderValue = value;
        float temp = value;
        temp = temp > UIObj::maxSliderValue ? UIObj::maxSliderValue : (temp < UIObj::minSliderValue ? UIObj::minSliderValue : temp);
        temp += (abs(UIObj::minSliderValue) - abs(UIObj::maxSliderValue))/2;
        temp /= abs(UIObj::minSliderValue) + abs(UIObj::maxSliderValue);
        temp *= UIObj::Images[1].getSize().x;
        UIObj::Images[0].setPosition(Vector2f(temp + (UIObj::Images[1].getPosition().x)
                                              , UIObj::Images[0].getPosition().y));
        if (UIObj::ConnectedValue != nullptr) *UIObj::ConnectedValue = value;
    }
}

/// Sets the connected UIObj
/// ONLY works if the UIObj is a slider to a input or input to a sliders OR a slider to a text and vise versa
// make the text work with this
void UIObj::connectUIObj(UIObj* UIObj)
{
    if ((UIObj->Type == UIType::Slider && this->Type == UIType::InputBox)
        || (UIObj->Type == UIType::InputBox && this->Type == UIType::Slider)
        || (UIObj->Type == UIType::Slider && this->Type == UIType::TextUI)
        || (UIObj->Type == UIType::TextUI && this->Type == UIType::Slider)) {
        this->ConnectedValue = new float(UIObj->Type == UIType::Slider ? UIObj->minSliderValue : this->minSliderValue);
        *this->ConnectedValue = *this->ConnectedValue < 0 ? 0 : *this->ConnectedValue;
        UIObj->ConnectedValue = this->ConnectedValue;
    }
}

/// sets the char size of the description text
void UIObj::SetDescriptionCharSize(int Size)
{
    UIObj::DescriptionText->setCharacterSize(Size);
}

/// sets the description of this UIObj
void UIObj::SetDescription(string String)
{
    UIObj::Description = String;
}

void UIObj::DrawDescription()
{
    if (UIObj::drawDescription) {
        UIObj::DescriptionBackground->setPosition(UIObj::input->mousePos + Vector2f(0,2));
        UIObj::DescriptionText->setPosition(UIObj::input->mousePos);

        UIObj::DescriptionBackground->setSize(Vector2f(Vector2f(UIObj::DescriptionText->getLocalBounds().left + UIObj::DescriptionText->getGlobalBounds().width,
                                                        UIObj::DescriptionText->getLocalBounds().top + UIObj::DescriptionText->getGlobalBounds().height)));

        FloatRect temp = UIObj::DescriptionBackground->getGlobalBounds();

        UIObj::DescriptionBackground->setOrigin(0,temp.height);
        UIObj::DescriptionText->setOrigin(0,temp.height);

        temp = UIObj::DescriptionBackground->getGlobalBounds();

        if (temp.left <= 0 || temp.left + temp.width >= UIObj::window->getSize().x)
        {
            float tempX = UIObj::DescriptionBackground->getOrigin().x + temp.width;
            tempX = tempX > temp.width ? 0 : temp.width;
            UIObj::DescriptionBackground->setOrigin(tempX, UIObj::DescriptionBackground->getOrigin().y);
            UIObj::DescriptionText->setOrigin(UIObj::DescriptionBackground->getOrigin());
        }
        if (temp.top <= 0 || temp.top + temp.height >= UIObj::window->getSize().y)
        {
            float tempY = UIObj::DescriptionBackground->getOrigin().y + temp.height;
            tempY = tempY > temp.height ? (UIObj::DescriptionBackground->getOrigin().x == 0 ? -20 : 0) : temp.height;
            UIObj::DescriptionBackground->setOrigin(UIObj::DescriptionBackground->getOrigin().x,
                                                    tempY);
            UIObj::DescriptionText->setOrigin(UIObj::DescriptionBackground->getOrigin());
        }

        UIObj::window->draw(*UIObj::DescriptionBackground);
        UIObj::window->draw(*UIObj::DescriptionText);
    }
}

/// another function because I dont understand why SFML is like this
///
/// only for text
void UIObj::setTextOriginToCenter()
{
//    // center of text without the spacing included
//    Vector2f center = Vector2f(UIObj::text.getGlobalBounds().width / 2, UIObj::text.getGlobalBounds().height / 2);
//    // adding in the extra spacing
//    center += Vector2f(UIObj::text.getLocalBounds().left, UIObj::text.getLocalBounds().top);
//    // rounding
//    center = UIObj::Round(center);
//
//    UIObj::text.setOrigin(center);

    // all of the above in one line
    UIObj::text.setOrigin(Round(Vector2f(UIObj::text.getLocalBounds().left + UIObj::text.getGlobalBounds().width / 2,
                                         UIObj::text.getLocalBounds().top + UIObj::text.getGlobalBounds().height / 2)));

}

Vector2f UIObj::Round(Vector2f Vector)
{
    return Vector2f{round(Vector.x), round(Vector.y)};
}
