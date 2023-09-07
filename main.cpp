#include "physics.h"
#include "Input.h"
#include "BodySpawner.h"
#include "GravityBody.h"
#include "UIObj.h"
#include <vector>
#include <string>
#include <math.h>

using namespace std;
using namespace sf; //for graphics

// MAKE SURE THAT THE PHYSICS.H FILE IS THE ONE I HAVE UPDATED

enum SelectedAction
{
    none = 0,
    handSpawn,
    summonSpawner,
    summonGravityBody,
    magicForce,
    SelectDelete
};

bool IsHovering(FloatRect GlobalBounds, Input input)
{
    if (GlobalBounds.contains(input.mousePos))
    {
        return true;
    }
    return false;
}

int main()
{
    RenderWindow window(VideoMode::getDesktopMode(),"SFML Works", Style::Fullscreen);  //creates a window on the screen that is 800 by 600
    window.setFramerateLimit(60); //sets the game loop to run 60 times per second
    b2World world(b2Vec2(0,0));
    Input input;
    // making the simulation area double the size of the screen
    const Vector2f simulationArea = Vector2f(window.getSize().x*4, window.getSize().y*4);

    Block BlockTemp; //used when ever there is a need to hold a block in a var

    // creation of body vars
    vector<GravityBody*> gravityBodys;
    vector<Block> normalBodys;
    // have to store as a pointer or else it crashes
    vector<BodySpawner*> spawners;

    // text and visual information vars

    Shape* selectDeleteShape = nullptr;

    Font font;
    font.loadFromFile("Roboto-Medium.ttf");

    Texture trash_t;
    trash_t.loadFromFile("Trash.png");
    Texture selectDelete_t;
    selectDelete_t.loadFromFile("selectDelete.png");
    Texture HandSpawn_t;
    HandSpawn_t.loadFromFile("handSpawn.png");
    Texture SummonSpawner_t;
    SummonSpawner_t.loadFromFile("summonSpawner.png");
    Texture SummonGravityBody_t;
    SummonGravityBody_t.loadFromFile("summonGravityBody.png");
    Texture MagicForce_t;
    MagicForce_t.loadFromFile("MagicForce.png");
    Texture Undo_t;
    Undo_t.loadFromFile("Undo.png");
    Texture isCircle_t;
    isCircle_t.loadFromFile("isCircle.png");
    Texture DestroyNormalBodys_t;
    DestroyNormalBodys_t.loadFromFile("DestroyNormalBodys.png");
    Texture DestroyOnCollision_t;
    DestroyOnCollision_t.loadFromFile("DestroyOnCollision.png");
    Texture Velocity_t;
    Velocity_t.loadFromFile("Velocity.png");
    Texture Vacuum_t;
    Vacuum_t.loadFromFile("Vacuum.png");
    Texture Pause_t;
    Pause_t.loadFromFile("Pause.png");

    // --------

    // logical vars (clock, event, ect)

    Clock mainClock;
    float deltaTime;
    float oneSecondTimer;

    // ---------------------------------

    UIObj FPSText(&input, &window, &deltaTime, font);
    FPSText.MakeText(Vector2f(50,50), Vector2f(window.getSize().x-25, 25), true, false);
    FPSText.UpdateMainSizeFor("123");
    FPSText.setString("0");
    int FPSCounter = 0;

    UIObj Trash(&input, &window, &deltaTime, font);
    Trash.MakeButton(RectangleShape(Vector2f(50,50)), Vector2f(75,window.getSize().y-75));
    Trash.setTexture(&trash_t);
    Trash.SetDescription("Deletes EVERYTHING on screen");
    UIObj selectDelete(&input, &window, &deltaTime, font);
    selectDelete.MakeButton(RectangleShape(Vector2f(50,50)), Vector2f(75,window.getSize().y-150));
    selectDelete.setTexture(&selectDelete_t);
    selectDelete.SetDescription("Allows you to delete specific objects");
    UIObj Undo(&input, &window, &deltaTime, font);
    Undo.MakeButton(RectangleShape(Vector2f(50,50)), Vector2f(150,window.getSize().y-75));
    Undo.setTexture(&Undo_t);
    Undo.SetDescription("Allows you to undo any 'Summon' actions");
    UIObj DestoryNormalBodys(&input, &window, &deltaTime, font);
    DestoryNormalBodys.MakeButton(RectangleShape(Vector2f(50,50)), Vector2f(150,window.getSize().y-150));
    DestoryNormalBodys.setTexture(&DestroyNormalBodys_t);
    DestoryNormalBodys.SetDescription("Destroys every 'Normal' body in the simulation");
    UIObj PauseSimulation(&input, &window, &deltaTime, font);
    PauseSimulation.MakeButton(RectangleShape(Vector2f(50,50)), Vector2f(window.getSize().x-75,window.getSize().y-75));
    PauseSimulation.setTexture(&Pause_t);
    PauseSimulation.SetDescription("Pause/Unpause the gravity simulation\nallows you to use UI still");
    UIObj PauseText(&input, &window, &deltaTime, font);
    PauseText.MakeText(Vector2f(500,100), Vector2f(window.getSize().x/2, window.getSize().y/2), false, false);
    PauseText.setString("Paused");
    PauseText.setColor(Color::Red);
    PauseText.AutoFitText();

    bool isCircle = true;
    bool isCircle2 = true;
    bool isCircle3 = true;
    bool DestroyOnCollision = true;
    bool VelocityOnRelease = true;
    bool isVacuum = false;
    bool SimulationPaused = false;

    vector<UIObj> handSpawn;
    // 0
    // original button for menu
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeButton(RectangleShape(Vector2f(50,50)), Vector2f(75,75));
    handSpawn.back().setTexture(&HandSpawn_t);
    handSpawn.back().SetDescription("Selects the 'Hand Spawn'\nOpening the options for it");
    // 1
    // is circle text
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeText(Vector2f(75, 40), Vector2f(200, 75), true, false);
    handSpawn.back().setString("Circle");
    handSpawn.back().setColor(Color::Green);
    // 2
    // is circle button
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeButton(RectangleShape(Vector2f(40,40)), Vector2f(200,115));
    handSpawn.back().SetDescription("Changes weather the 'Hand Spawn'\nwill spawn a circle or square");
    handSpawn.back().setTexture(&isCircle_t);
    // 3
    // starting force
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeText(Vector2f(100, 30), Vector2f(200, 200), false, false);
    handSpawn.back().setString("Initial\nForce");
    handSpawn.back().AutoFitText();
    handSpawn.back().setColor(Color::Green);
    // 4
    // text x
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeText(Vector2f(75, 20), Vector2f(200, 245), false, false);
    handSpawn.back().setString("x");
    handSpawn.back().AutoFitText();
    handSpawn.back().UpdateMainSize();
    handSpawn.back().setColor(Color::Green);
    // 5
    // input x
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeInputBox(Vector2f(75,20), Vector2f(200, 265), true, false, true, true);
    handSpawn.back().SetDescription("The x value for the Initial Force\nValues can be entered into this box");
    // 6
    // slider x
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 290), -25, 25);
    handSpawn.back().SetDescription("This is the slider for the Initial Force in the x");
    handSpawn.back().connectUIObj(&handSpawn[handSpawn.size()-2]);
    // 7
    // text y
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeText(Vector2f(75, 20), Vector2f(200, 315), false, false);
    handSpawn.back().setString("y");
    handSpawn.back().AutoFitText();
    handSpawn.back().UpdateMainSize();
    handSpawn.back().setColor(Color::Green);
    // 8
    // input y
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeInputBox(Vector2f(75,20), Vector2f(200, 335), true, false, true, true);
    handSpawn.back().SetDescription("The y value for the Initial Force\nValues can be entered into this box");
    // 9
    // slider y
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 360), -25, 25);
    handSpawn.back().SetDescription("This is the slider for the Initial Force in the y");
    handSpawn.back().connectUIObj(&handSpawn[handSpawn.size()-2]);
    // 10
    // scale of the circle / cube text
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeText(Vector2f(100, 75), Vector2f(200, 430), false, false);
    handSpawn.back().setString("Shape\nScale");
    handSpawn.back().AutoFitText();
    handSpawn.back().setColor(Color::Green);
    // 11
    // scale of the circle / cube text visual
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeText(Vector2f(75, 75), Vector2f(200, 490), false, false);
    handSpawn.back().AutoFitText();
    // 12
    // scale of the circle / cube slider
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 520), 0.5, 7);
    handSpawn.back().SetDescription("This is the slider for the scale of the object");
    handSpawn.back().connectUIObj(&handSpawn[handSpawn.size()-2]);
    handSpawn.back().setSliderValue(0.5);
    // 13
    // Density text
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeText(Vector2f(100, 75), Vector2f(200, 560), false, false);
    handSpawn.back().setString("Density");
    handSpawn.back().AutoFitText();
    handSpawn.back().setColor(Color::Green);
    // 14
    // Density text visual
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeText(Vector2f(75, 75), Vector2f(200, 590), false, false);
    handSpawn.back().AutoFitText();
    // 15
    // Density slider
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 620), 0.5, 10);
    handSpawn.back().SetDescription("This is the slider for the density of the object");
    handSpawn.back().connectUIObj(&handSpawn[handSpawn.size()-2]);
    handSpawn.back().setSliderValue(1);
    // 16
    // spawn amount text
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeText(Vector2f(100, 30), Vector2f(200, 680), false, false);
    handSpawn.back().setString("Spawn\nAmount");
    handSpawn.back().AutoFitText();
    handSpawn.back().setColor(Color::Green);
    // 17
    // spawn amount input box
    handSpawn.push_back(UIObj(&input, &window, &deltaTime, font));
    handSpawn.back().MakeInputBox(Vector2f(75,20), Vector2f(200, 725), true, false, true, true, true);
    handSpawn.back().SetDescription("The spawn amount of normal bodys to spawn");
    handSpawn.back().setString("1");


    vector<UIObj> summonSpawner;

    // 0
    // base button to show and select the summon spawner menu
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeButton(RectangleShape(Vector2f(50,50)), Vector2f(75,150));
    summonSpawner.back().setTexture(&SummonSpawner_t);
    summonSpawner.back().SetDescription("Selects the 'Summon Spawner'\nOpening the options for it");
    // 1
    // is circle text
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeText(Vector2f(75, 40), Vector2f(200, 75), true, false);
    summonSpawner.back().setString("Circle");
    summonSpawner.back().setColor(Color::Green);
    // 2
    // is circle button
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeButton(RectangleShape(Vector2f(40,40)), Vector2f(200,115));
    summonSpawner.back().SetDescription("Changes weather the 'Summon Spawner'\nwill spawn a circle or square");
    summonSpawner.back().setTexture(&isCircle_t);
    // 3
    // spawn cooldown text
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeText(Vector2f(100, 30), Vector2f(200, 170), false, false);
    summonSpawner.back().setString("Spawn\nCooldown");
    summonSpawner.back().AutoFitText();
    summonSpawner.back().setColor(Color::Green);
    // 4
    // spawn cooldown input box
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeInputBox(Vector2f(75,20), Vector2f(200, 210), true, false, true, true);
    summonSpawner.back().SetDescription("The spawn cooldown for the spawner\nThe smaller the value the faster it spawns");
    // 5
    // spawn cooldown slider
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 235), 0, 5);
    summonSpawner.back().SetDescription("This is the slider for the spawn cooldown");
    summonSpawner.back().connectUIObj(&summonSpawner[summonSpawner.size()-2]);
    summonSpawner.back().setSliderValue(0.25);
    // 6
    // spawn amount text
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeText(Vector2f(100, 30), Vector2f(200, 290), false, false);
    summonSpawner.back().setString("Spawn\nAmount");
    summonSpawner.back().AutoFitText();
    summonSpawner.back().setColor(Color::Green);
    // 7
    // spawn amount input box
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeInputBox(Vector2f(75,20), Vector2f(200, 330), true, false, true, true, true);
    summonSpawner.back().SetDescription("The spawn amount for the spawner");
    summonSpawner.back().setString("1");
    // 8
    // starting force text
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeText(Vector2f(100, 30), Vector2f(200, 385), false, false);
    summonSpawner.back().setString("Initial\nForce");
    summonSpawner.back().AutoFitText();
    summonSpawner.back().setColor(Color::Green);
    // 9
    // text x
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeText(Vector2f(75, 20), Vector2f(200, 430), false, false);
    summonSpawner.back().setString("x");
    summonSpawner.back().AutoFitText();
    summonSpawner.back().UpdateMainSize();
    summonSpawner.back().setColor(Color::Green);
    // 10
    // input x
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeInputBox(Vector2f(75,20), Vector2f(200, 450), true, false, true, true);
    summonSpawner.back().SetDescription("The x value for the Initial Force\nValues can be entered into this box");
    // 11
    // slider x
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 475), -25, 25);
    summonSpawner.back().SetDescription("This is the slider for the Initial Force in the x");
    summonSpawner.back().connectUIObj(&summonSpawner[summonSpawner.size()-2]);
    // 12
    // text y
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeText(Vector2f(75, 20), Vector2f(200, 500), false, false);
    summonSpawner.back().setString("y");
    summonSpawner.back().AutoFitText();
    summonSpawner.back().UpdateMainSize();
    summonSpawner.back().setColor(Color::Green);
    // 13
    // input y
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeInputBox(Vector2f(75,20), Vector2f(200, 520), true, false, true, true);
    summonSpawner.back().SetDescription("The y value for the Initial Force\nValues can be entered into this box");
    // 14
    // slider y
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 545), -25, 25);
    summonSpawner.back().SetDescription("This is the slider for the Initial Force in the y");
    summonSpawner.back().connectUIObj(&summonSpawner[summonSpawner.size()-2]);
    // 15
    // scale of the circle / cube text
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeText(Vector2f(100, 75), Vector2f(200, 615), false, false);
    summonSpawner.back().setString("Shape\nScale");
    summonSpawner.back().AutoFitText();
    summonSpawner.back().setColor(Color::Green);
    // 16
    // scale of the circle / cube text visual
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeText(Vector2f(75, 75), Vector2f(200, 675), false, false);
    summonSpawner.back().AutoFitText();
    // 17
    // scale of the circle / cube slider
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 705), 0.5, 7);
    summonSpawner.back().SetDescription("This is the slider for the scale of the object");
    summonSpawner.back().connectUIObj(&summonSpawner[summonSpawner.size()-2]);
    summonSpawner.back().setSliderValue(0.5);
    // 18
    // Density text
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeText(Vector2f(100, 75), Vector2f(200, 745), false, false);
    summonSpawner.back().setString("Density");
    summonSpawner.back().AutoFitText();
    summonSpawner.back().setColor(Color::Green);
    // 19
    // Density text visual
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeText(Vector2f(75, 75), Vector2f(200, 775), false, false);
    summonSpawner.back().AutoFitText();
    // 20
    // Density slider
    summonSpawner.push_back(UIObj(&input, &window, &deltaTime, font));
    summonSpawner.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 805), 0.5, 10);
    summonSpawner.back().SetDescription("This is the slider for the density of the object");
    summonSpawner.back().connectUIObj(&summonSpawner[summonSpawner.size()-2]);
    summonSpawner.back().setSliderValue(1);


    vector<UIObj> summonGravityBody;

    // 0
    // button for selecting the action / UIMenu
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeButton(RectangleShape(Vector2f(50,50)), Vector2f(75,225));
    summonGravityBody.back().setTexture(&SummonGravityBody_t);
    summonGravityBody.back().SetDescription("Selects the 'Summon Gravity Body'\nOpening the options for it");
    // 1
    // is circle text
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeText(Vector2f(75, 40), Vector2f(200, 75), true, false);
    summonGravityBody.back().setString("Circle");
    summonGravityBody.back().setColor(Color::Green);
    // 2
    // is circle button
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeButton(RectangleShape(Vector2f(40,40)), Vector2f(200,115));
    summonGravityBody.back().SetDescription("Changes weather the 'Gravity Body'\nwill spawn a circle or square");
    summonGravityBody.back().setTexture(&isCircle_t);
    // 3
    // DestroyOnCollision text
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeText(Vector2f(75, 40), Vector2f(200, 160), true, false);
    summonGravityBody.back().setString("True");
    summonGravityBody.back().setColor(Color::Green);
    // 4
    // DestroyOnCollision button
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeButton(RectangleShape(Vector2f(40,40)), Vector2f(200,200));
    summonGravityBody.back().SetDescription("Changes weather the 'Gravity Body'\nwill spawn destroy the normal\nbodies on collision");
    summonGravityBody.back().setTexture(&DestroyOnCollision_t);
    // 5
    // size tile
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeText(Vector2f(100, 30), Vector2f(200, 250), false, false);
    summonGravityBody.back().setString("Size");
    summonGravityBody.back().AutoFitText();
    summonGravityBody.back().setColor(Color::Green);
    // 6
    // size Text
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeText(Vector2f(75,20), Vector2f(200, 275), true);
    // 7
    // size slider
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 300), 5, 100);
    summonGravityBody.back().SetDescription("This is the slider for the size");
    summonGravityBody.back().connectUIObj(&summonGravityBody[summonGravityBody.size()-2]);
    summonGravityBody.back().setSliderValue(25);
    // 8
    // gravity force text
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeText(Vector2f(100, 30), Vector2f(200, 350), false, false);
    summonGravityBody.back().setString("Gravity\nForce");
    summonGravityBody.back().AutoFitText();
    summonGravityBody.back().setColor(Color::Green);
    // 9
    // gravity force Text
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeInputBox(Vector2f(75,20), Vector2f(200, 395), true, false, true, true);
    summonGravityBody.back().SetDescription("The input box for the 'Gravity Body' gravity force");
    // 10
    // gravity force slider
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 420), -100, 100);
    summonGravityBody.back().SetDescription("This is the slider for the gravity force");
    summonGravityBody.back().connectUIObj(&summonGravityBody[summonGravityBody.size()-2]);
    summonGravityBody.back().setSliderValue(25);
    // 11
    // Rotation force text
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeText(Vector2f(100, 30), Vector2f(200, 450), false, false);
    summonGravityBody.back().setString("Rotation");
    summonGravityBody.back().AutoFitText();
    summonGravityBody.back().setColor(Color::Green);
    // 12
    // Rotation force Text
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeInputBox(Vector2f(75,20), Vector2f(200, 480), true, false, true, true);
    summonGravityBody.back().SetDescription("The input box for the 'Gravity Body' rotation");
    // 13
    // Rotation force slider
    summonGravityBody.push_back(UIObj(&input, &window, &deltaTime, font));
    summonGravityBody.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 505), 0, 360);
    summonGravityBody.back().SetDescription("This is the slider for the rotation");
    summonGravityBody.back().connectUIObj(&summonGravityBody[summonGravityBody.size()-2]);
    summonGravityBody.back().setSliderValue(0);


    vector<UIObj> magicForce;

    // 0
    // the button to selet action / UIMenu
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeButton(RectangleShape(Vector2f(50,50)), Vector2f(75,300));
    magicForce.back().setTexture(&MagicForce_t);
    magicForce.back().SetDescription("Selects the 'Magic Force'\nOpening the options for it");
    // 1
    // force value text
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeText(Vector2f(100, 30), Vector2f(200, 75), false, false);
    magicForce.back().setString("Force");
    magicForce.back().AutoFitText();
    magicForce.back().setColor(Color::Green);
    // 2
    // force value input box
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeInputBox(Vector2f(75,20), Vector2f(200, 105), true, false, true, true);
    magicForce.back().SetDescription("The force for the 'Magic Force'");
    // 3
    // force value slider
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 130), -25, 25);
    magicForce.back().SetDescription("This is the slider for the force");
    magicForce.back().connectUIObj(&magicForce[magicForce.size()-2]);
    magicForce.back().setSliderValue(5);
    // 4
    // no velocity on release text
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeText(Vector2f(125, 40), Vector2f(200, 185), false, false);
    magicForce.back().setString("No Velocity\nOn Release");
    magicForce.back().AutoFitText();
    magicForce.back().setColor(Color::Green);
    // 5
    // no velocity on release
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeText(Vector2f(55, 40), Vector2f(200, 230), true, false);
    magicForce.back().setString("False");
    // 6
    // no velocity on release
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeButton(RectangleShape(Vector2f(40,40)), Vector2f(200,270));
    magicForce.back().SetDescription("Changes weather the 'Magic Force'\nwill leave the normal bodies\nwith a velocity or not\non release");
    magicForce.back().setTexture(&Velocity_t);
    // 7
    // Vacuum text
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeText(Vector2f(100, 40), Vector2f(200, 325), false, false);
    magicForce.back().setString("Vacuum");
    magicForce.back().AutoFitText();
    magicForce.back().setColor(Color::Green);
    // 8
    // Vacuum text
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeText(Vector2f(55, 40), Vector2f(200, 355), true, false);
    magicForce.back().setString("True");
    // 9
    // Vacuum button
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeButton(RectangleShape(Vector2f(40,40)), Vector2f(200,395));
    magicForce.back().SetDescription("Changes weather the 'Magic Force'\nwill delete normal bodys when\nwith in range");
    magicForce.back().setTexture(&Vacuum_t);
    // 10
    // Vacuum text
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeText(Vector2f(100, 40), Vector2f(200, 455), false, false);
    magicForce.back().setString("Vacuum\nRadius");
    magicForce.back().AutoFitText();
    magicForce.back().setColor(Color::Green);
    // 11
    // Vacuum text
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeText(Vector2f(75, 40), Vector2f(200, 500), true, false);
    // 12
    // Vacuum slider
    magicForce.push_back(UIObj(&input, &window, &deltaTime, font));
    magicForce.back().MakeSlider(RectangleShape(Vector2f(20,20)), 75, Vector2f(200, 525), 0.25, 25);
    magicForce.back().SetDescription("This is the slider for the\nradius of vacuum");
    magicForce.back().connectUIObj(&magicForce[magicForce.size()-2]);
    magicForce.back().setSliderValue(5);


    SelectedAction selectedAction = SelectedAction::none;
    vector<SelectedAction> LastAction;

    while (window.isOpen()&& !Keyboard::isKeyPressed(Keyboard::Escape))     //the main game loop, exits if someone closes the window
    {
        window.clear(); //clears the screen
        input.Reset(); // resets all inputs required

        deltaTime = mainClock.getElapsedTime().asSeconds();
        oneSecondTimer += deltaTime;
        mainClock.restart();
        if (oneSecondTimer >= 1) {
            oneSecondTimer = 0;
            FPSText.setString(to_string(FPSCounter));
            FPSCounter = 0;
        } else
        {
            FPSCounter++;
        }

        Event event; //creates an event object, events include mouse clicks, mouse movement, keyboard presses, etc..
        while (window.pollEvent(event)) //loop that checks for events
        {
            if (event.type == Event::Closed) //checks if window is closed
                window.close();

            input.UpdateInput(event, window); //updates all inputs
        }   //ends the event loop

        FPSText.Update( false );
        Trash.Update( false );
        selectDelete.Update( false );
        Undo.Update( false );
        if (selectDelete.buttonClicked) selectedAction = selectedAction == SelectedAction::SelectDelete ? SelectedAction::none : SelectedAction::SelectDelete;
        DestoryNormalBodys.Update( false );
        PauseSimulation.Update( false );
        if (PauseSimulation.buttonClicked || input.PReleased) SimulationPaused = !SimulationPaused;

        for (int i = 0; i < handSpawn.size(); i++) { handSpawn[i].Update( false ); if (selectedAction != SelectedAction::handSpawn) { break; } }
        if (handSpawn[0].buttonClicked) selectedAction = selectedAction == SelectedAction::handSpawn ? SelectedAction::none : SelectedAction::handSpawn;
        if (handSpawn[2].buttonClicked) isCircle = !isCircle; handSpawn[1].setString(isCircle ? "Circle" : "Square");

        for (int i = 0; i < summonSpawner.size(); i++) { summonSpawner[i].Update( false ); if (selectedAction != SelectedAction::summonSpawner) { break; } }
        if (summonSpawner[0].buttonClicked) selectedAction = selectedAction == SelectedAction::summonSpawner ? SelectedAction::none : SelectedAction::summonSpawner;
        if (summonSpawner[2].buttonClicked) isCircle2 = !isCircle2; summonSpawner[1].setString(isCircle2 ? "Circle" : "Square");

        for (int i = 0; i < summonGravityBody.size(); i++) { summonGravityBody[i].Update( false ); if (selectedAction != SelectedAction::summonGravityBody) { break; } }
        if (summonGravityBody[0].buttonClicked) selectedAction = selectedAction == SelectedAction::summonGravityBody ? SelectedAction::none : SelectedAction::summonGravityBody;
        if (summonGravityBody[2].buttonClicked) isCircle3 = !isCircle3; summonGravityBody[1].setString(isCircle3 ? "Circle" : "Square");
        if (summonGravityBody[4].buttonClicked) DestroyOnCollision = !DestroyOnCollision; summonGravityBody[3].setString(DestroyOnCollision ? "True" : "False");

        for (int i = 0; i < magicForce.size(); i++) { magicForce[i].Update( false ); if (selectedAction != SelectedAction::magicForce) { break; } }
        if (magicForce[0].buttonClicked) selectedAction = selectedAction == SelectedAction::magicForce ? SelectedAction::none : SelectedAction::magicForce;
        if (magicForce[6].buttonClicked) VelocityOnRelease = !VelocityOnRelease; magicForce[5].setString(!VelocityOnRelease ? "True" : "False");
        if (magicForce[9].buttonClicked) isVacuum = !isVacuum; magicForce[8].setString(isVacuum ? "True" : "False");

        // if there is no UI interaction
        if (input.UIInteraction)
        {
            if (Trash.buttonClicked)
            {
                for (int i = normalBodys.size()-1; i >= 0; i--) {
                    physics::deleteBlock(world, normalBodys[i]);
                }
                for (int i = gravityBodys.size()-1; i >= 0; i--) {
                    gravityBodys[i]->DestoryGravityBody(world);
                }
                for (int i = spawners.size()-1; i >= 0; i--) {
                    delete(spawners[i]);
                }
                normalBodys.clear();
                gravityBodys.clear();
                spawners.clear();
                LastAction.clear();
            } else
            if (Undo.buttonClicked && LastAction.size() > 0)
            {
                if (LastAction.back() == SelectedAction::summonGravityBody)
                {
                    gravityBodys[gravityBodys.size()-1]->DestoryGravityBody(world);
                    //delete(gravityBodys[gravityBodys.size()-1]);
                    gravityBodys.erase(gravityBodys.end()-1);

                    LastAction.erase(LastAction.end()-1);
                } else
                if (LastAction.back() == SelectedAction::summonSpawner)
                {
                    delete(spawners[spawners.size()-1]);
                    spawners.erase(spawners.end()-1);

                    LastAction.erase(LastAction.end()-1);
                }
            } else
            if (DestoryNormalBodys.buttonClicked)
            {
                for (int i = 0; i < normalBodys.size(); i++) {
                    physics::deleteBlock(world, normalBodys[i]);
                }
                normalBodys.clear();
            }
        } else {
            // reseting the select delete visual
            if (selectDeleteShape != nullptr) {
                selectDeleteShape->setOutlineThickness(0);
                selectDeleteShape = nullptr;
            }

            if (selectedAction == SelectedAction::handSpawn && (input.lMouseDown || input.rMouseDown)) {
                for (int i = 0; i < stof(handSpawn[17].currentString); i++)
                {
                    if (isCircle){
                        BlockTemp = physics::createCircle(world, input.mousePos.x,  input.mousePos.y, 5*(*handSpawn[12].ConnectedValue), b2BodyType::b2_dynamicBody, *handSpawn[15].ConnectedValue);
                        // physics::setCollisionID(BlockTemp, -1); // dont collide
                        BlockTemp->ApplyForceToCenter(b2Vec2(*handSpawn[6].ConnectedValue, *handSpawn[9].ConnectedValue), true);
                        normalBodys.push_back(BlockTemp);
                    } else
                    {
                        BlockTemp = physics::createBox(world, input.mousePos.x,  input.mousePos.y, 10*(*handSpawn[12].ConnectedValue), 10*(*handSpawn[12].ConnectedValue), b2BodyType::b2_dynamicBody, *handSpawn[15].ConnectedValue);
                        // physics::setCollisionID(BlockTemp, -1); // dont collide
                        BlockTemp->ApplyForceToCenter(b2Vec2(*handSpawn[6].ConnectedValue, *handSpawn[9].ConnectedValue), true);
                        normalBodys.push_back(BlockTemp);
                    }
                }
            } else
            if (selectedAction == SelectedAction::summonGravityBody && (input.lMouseReleased || input.rMouseReleased)) {
                if (isCircle3)
                {
                    gravityBodys.push_back(new GravityBody(physics::createCircle(world,  0,  0,
                                                                              *summonGravityBody[7].ConnectedValue, b2_staticBody),
                                                           b2Vec2(input.mousePos.x, input.mousePos.y),
                                                           *summonGravityBody[7].ConnectedValue,
                                                           *summonGravityBody[10].ConnectedValue,
                                                           DestroyOnCollision,
                                                           *summonGravityBody[13].ConnectedValue));
                } else
                {
                    gravityBodys.push_back(new GravityBody(physics::createBox(world,  0,  0,
                                                                              *summonGravityBody[7].ConnectedValue,
                                                                              *summonGravityBody[7].ConnectedValue, b2_staticBody),
                                                           b2Vec2(input.mousePos.x, input.mousePos.y),
                                                           0,
                                                           *summonGravityBody[10].ConnectedValue,
                                                           DestroyOnCollision,
                                                           *summonGravityBody[13].ConnectedValue));
                }

                LastAction.push_back(selectedAction);
            } else
            if (selectedAction == SelectedAction::summonSpawner && (input.lMouseReleased || input.rMouseReleased)) {
                if (isCircle2)
                {
                    spawners.push_back(new BodySpawner(*summonSpawner[5].ConnectedValue,
                                                       stof(summonSpawner[7].currentString),
                                                       input.mousePos, 5*(*summonSpawner[17].ConnectedValue),
                                                       *summonSpawner[20].ConnectedValue,
                                                        Vector2f(*summonSpawner[11].ConnectedValue, *summonSpawner[14].ConnectedValue)));
                } else
                {
                    spawners.push_back(new BodySpawner(*summonSpawner[5].ConnectedValue,
                                                       stof(summonSpawner[7].currentString),
                                                       input.mousePos, Vector2f(10*(*summonSpawner[17].ConnectedValue), 10*(*summonSpawner[17].ConnectedValue)),
                                                       *summonSpawner[20].ConnectedValue,
                                                        Vector2f(*summonSpawner[11].ConnectedValue, *summonSpawner[14].ConnectedValue)));
                }

                LastAction.push_back(selectedAction);
            } else
            if (selectedAction == SelectedAction::SelectDelete) {
                for (int i = 0; i < gravityBodys.size(); i++) {
                    if (IsHovering(physics::getSFMLShape(gravityBodys[i]->body)->getGlobalBounds(), input))
                    {
                        if (selectDeleteShape == nullptr) selectDeleteShape = physics::getSFMLShape(gravityBodys[i]->body);
                        if ((input.lMouseReleased || input.lMouseReleased)) {
                            gravityBodys[i]->DestoryGravityBody(world);
                            gravityBodys.erase(gravityBodys.begin()+i);
                            i--;
                            selectDeleteShape = nullptr;
                            for (int i = 0; i < LastAction.size(); i++)
                            {
                                if (LastAction[i] == SelectedAction::summonGravityBody)
                                {
                                    LastAction.erase(LastAction.begin()+i);
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                // checking if there is any selected obj to delete and if it was clicked on to delete it
                for (int i = 0; i < spawners.size(); i++) {
                    if (IsHovering(spawners[i]->Spawner->getGlobalBounds(), input))
                    {
                        if (selectDeleteShape == nullptr) selectDeleteShape = spawners[i]->Spawner;
                        if ((input.lMouseReleased || input.lMouseReleased)) {
                            delete(spawners[i]);
                            spawners.erase(spawners.begin()+i);
                            i--;
                            selectDeleteShape = nullptr;
                            for (int i = 0; i < LastAction.size(); i++)
                            {
                                if (LastAction[i] == SelectedAction::summonSpawner)
                                {
                                    LastAction.erase(LastAction.begin()+i);
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                // updating visual effect for select delete
                if (selectDeleteShape != nullptr) {
                    Color temp = selectDeleteShape->getFillColor();
                    temp = Color(240 - temp.r, 240 - temp.g, 240 - temp.b);
                    selectDeleteShape->setOutlineColor(Color(temp));
                    selectDeleteShape->setOutlineThickness(5);
                }
            } else
            if (selectedAction == SelectedAction::magicForce && (input.lMouseDown || input.rMouseDown))
            {
                for (int i = 0; i < normalBodys.size(); i++)
                {
                    b2Vec2 x_y_comp = b2Vec2(input.mousePos.x / pixels_per_meter - normalBodys[i]->GetPosition().x,
                                                 input.mousePos.y / pixels_per_meter - normalBodys[i]->GetPosition().y);

                    if (isVacuum)
                    {
                        if (x_y_comp.Length() <= *magicForce[12].ConnectedValue)
                        {
                            physics::deleteBlock(world, normalBodys[i]);
                            normalBodys.erase(normalBodys.begin() + i);
                            i--;
                            continue;
                        }
                    }

                    x_y_comp.Normalize();

                    // applying calculated force to the body
                    normalBodys[i]->ApplyForceToCenter(b2Vec2((*magicForce[3].ConnectedValue)*x_y_comp.x, (*magicForce[3].ConnectedValue)*x_y_comp.y), true);
                }
            } else
            if (selectedAction == SelectedAction::magicForce && !VelocityOnRelease && (input.lMouseReleased || input.rMouseReleased))
            {
                for (int i = 0; i < normalBodys.size(); i++)
                {
                    normalBodys[i]->SetLinearVelocity(b2Vec2(0,0));
                }
            }
        }

        if (!SimulationPaused) {
            // updating spawners and drawing spawners
            for(int i = 0; i < spawners.size(); i++) {
                spawners[i]->CheckSpawn(deltaTime, world, normalBodys);
                spawners[i]->DrawSpawner(window);
            }

            // checking if any of the blocks are outside the simulation area
            for (int i = 0; i < normalBodys.size(); i++) {
                Vector2f pos = physics::getPosition(normalBodys[i]);
                // checking if the body is outside the simulation area
                if (pos.x >= simulationArea.x || pos.y >= simulationArea.y || pos.x <= -simulationArea.x/2 || pos.y <= -simulationArea.y/2) {
                    physics::deleteBlock(world, normalBodys[i]);
                    normalBodys.erase(normalBodys.begin()+i);
                    i--;
                }
            }

            // applying gravity Body forces
            for (int i = 0; i < gravityBodys.size(); i++) {
                gravityBodys[i]->ApplyGravityForce(normalBodys, world);
            }
        } else
        {
            for(int i = 0; i < spawners.size(); i++) {
                spawners[i]->DrawSpawner(window);
            }
        }

        physics::displayWorld(world, window, !SimulationPaused);

        FPSText.Draw();
        Trash.Draw();
        selectDelete.Draw();
        Undo.Draw();
        DestoryNormalBodys.Draw();
        PauseSimulation.Draw();
        if (SimulationPaused) PauseText.Draw();

        for (int i = 0; i < handSpawn.size(); i++) { handSpawn[i].Draw(); if (selectedAction != SelectedAction::handSpawn) { break; } }
        for (int i = 0; i < summonSpawner.size(); i++) { summonSpawner[i].Draw(); if (selectedAction != SelectedAction::summonSpawner) { break; } }
        for (int i = 0; i < summonGravityBody.size(); i++) { summonGravityBody[i].Draw(); if (selectedAction != SelectedAction::summonGravityBody) { break; } }
        for (int i = 0; i < magicForce.size(); i++) { magicForce[i].Draw(); if (selectedAction != SelectedAction::magicForce) { break; } }

        // drawing the Description for UI AFTER all UI is drawn so it will be on top
        UIObj::DrawDescription();

        window.display();   //displays everything on the video card to the monitor
    }   //ends the game loop

    return 0;
}
