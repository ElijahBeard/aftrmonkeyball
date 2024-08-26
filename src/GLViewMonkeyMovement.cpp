#include "GLViewMonkeyMovement.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"
#include <cmath>

using namespace Aftr;

GLViewMonkeyMovement* GLViewMonkeyMovement::New( const std::vector< std::string >& args )
{
   GLViewMonkeyMovement* glv = new GLViewMonkeyMovement( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}

GLViewMonkeyMovement::GLViewMonkeyMovement( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewMonkeyMovement::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewMonkeyMovement::onCreate() is invoked after this module's LoadMap() is completed.
}

void GLViewMonkeyMovement::onCreate()
{
   //GLViewMonkeyMovement::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
   SDL_SetRelativeMouseMode(SDL_TRUE);
}


GLViewMonkeyMovement::~GLViewMonkeyMovement()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewMonkeyMovement::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.
   if (elapsed_time > 0) { time_start = true; }
   //MOVEMENT
   {
       //std::cout << cam->getPose() << std::endl;
       float move_velocity = std::lerp(0.0f, 40.0f, midi_data2 / 127.0f);
       std::cout << midi_data1 << std::endl;
       if (midi_data1 == 87 && time_start) {
           ball_body->body->addForce(PxVec3(-move_velocity, 0, 0));
       }

       if (midi_data1 == 1 && time_start)
       {
           ball_body->body->addForce(PxVec3(0, -move_velocity, 0));
       }

       if (midi_data1 == 3 && time_start) {
           ball_body->body->addForce(PxVec3(move_velocity, 0, 0));

       }

       if (midi_data1 == 81 && time_start) {
           ball_body->body->addForce(PxVec3(0, move_velocity, 0));
       }

   }

   //PHYSX
   scene->simulate(1.0 / 60.0);
   PxU32 errorState = 0;
   scene->fetchResults(true);
   {
       physx::PxU32 numActors = 0;
       physx::PxActor** actors = scene->getActiveActors(numActors);
       for (physx::PxU32 i = 0; i < numActors; ++i) {
           physx::PxActor* actor = actors[i];
           WOPhysX* wo = static_cast<WOPhysX*>(actor->userData);
           wo->updatePose();
       }
   }
   //CAMERA
   {
       //FOWARD
       if (midi_data1 == 1) {
           midi_rotation = std::lerp(35.0f, 90.0f, midi_data2 / 127.0f);
       }
       //BACK
       if (midi_data1 == 3) {
           midi_rotation = std::lerp(35.0f, 5.0f, midi_data2 / 127.0f);
       }
       //LEFT
       if (midi_data1 == 0) {
           midi_tiltation = std::lerp(0.f, -90.f, midi_data2 / 127.0f);
           std::cout << midi_tiltation;
       }
       //RIGHT
       if (midi_data1 == 2) {
           midi_tiltation = std::lerp(0.f, 90.f, midi_data2 / 127.0f);
       }
       float deltay = midi_rotation - currr;
       float deltax = midi_tiltation - currt;
       currr = midi_rotation;
       currt = midi_tiltation;
       anchor->rotateAboutRelY(-deltay * Aftr::DEGtoRAD);
       cam->rotateAboutGlobalZ(-deltax * Aftr::DEGtoRAD);
       //anchor->rotateAboutRelX(deltax * Aftr::DEGtoRAD);
       //aiai->setPosition(ball->getPosition());
   }

   if (ball) {
       Vector ballpos = this->ball->getPosition();

       //this->cam->setCameraLookAtPoint(ballpos);
       if (ballpos.z < -90) {
           cam->setPose(staticpose);
           ball_body->reset();
       }
       anchor->setPosition(ball->getPosition());
   }
   //anchor->rotateAboutGlobalY(0.01f);
   float angle = anchor->getPose().getAngleOfRotationAboutAxisOfRotationRads();
   //std::cout << angle << std::endl;
   float x = offset_radius * cos(angle) + anchor->getPosition().x;
   float y = 0.0f + anchor->getPosition().y;
   float z = offset_radius * sin(angle) + anchor->getPosition().z;

   Vector newPosition(x, y, z);
   if (ball->getPosition().z > -6 && !finished) {
       cam->setPosition(newPosition);
   }
   if (ball->getPosition().z < -7 && ball->getPosition().z > -8) {
       std::string fall_path = ManagerEnvironmentConfiguration::getLMM() + "sounds/fallout.wav";
       audio_engine->play2D(fall_path.c_str());
   }
   cam->setCameraLookAtPoint(anchor->getPosition());

   platforms[7]->rotateAboutGlobalZ(0.005);
   platform_bodies[7]->setPose(platforms[7]->getPose());
   aiai->setPose(ball->getPose());

   //END CONDITION
   if (is_hit_target(ball->getPosition(), goal->getPosition())) {
       finished = true;
   } else {
       cam->setCameraLookAtPoint(anchor->getPosition());
   }
   if (finished) {
       ball_body->body->addForce(PxVec3(0, 0, 300));
       if (static_cast<int>(elapsed_time) % 1 == 0) {
           timer->setTextColor(aftrColor4ub(255, 255, 255, 255));
       }
       else {
           timer->setColor(255, 174, 0, 255);
       }
   }

   //TIMER
   if (!finished) {
       timer->setText(format_time(seconds,milliseconds));
       timer_shaddow->setText(format_time(seconds, milliseconds));
   }
   elapsed_time += 0.016;
   seconds = static_cast<int>(elapsed_time);
   milliseconds = static_cast<int>((elapsed_time - seconds) * 1000);

   //AUDIO
   //ROLLING
   float velocity = ball_body->body->getAngularVelocity().magnitude();
   float playback = 0.5f + (velocity / 50) * 1.5f;
   roll->setVolume(velocity);
   roll->setPlaybackSpeed(playback);
   //BOUNCING
   float velocity_delta = velocity - previous_velocity;
   if (std::abs(velocity_delta) > 3) {
       std::string spike_path = ManagerEnvironmentConfiguration::getLMM() + "sounds/spike.wav";
       audio_engine->play2D(spike_path.c_str());
   } 
   previous_velocity = velocity;

   //TIMER
   click += 0.016;
   if (click > 1.0f && !finished) {
       std::string sec_path = ManagerEnvironmentConfiguration::getLMM() + "sounds/sec.wav";
       //audio_engine->play2D(sec_path.c_str());
       click = 0;
   }
   if (click > -1.5) {
       if (!ready_text.empty()) {
           ready_text.pop_back();
       }   
   }
   if (elapsed_time > 0) {
       go_text[0] = ('G');
       go_text[1] = ('O');
   }
   if (elapsed_time > .7) {
       if (!go_text.empty()) {
           go_text.pop_back();
       }
   }
   ready->setText(ready_text);
   go->setText(go_text);
}

void GLViewMonkeyMovement::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}

void GLViewMonkeyMovement::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewMonkeyMovement::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewMonkeyMovement::onMouseMove( const SDL_MouseMotionEvent& e )
{
    GLView::onMouseMove(e);
    int delta_x = e.xrel;
    int delta_y = e.yrel;
    //cam->changeLookAtViaMouse(delta_x * mouse_sensitivity, delta_y * mouse_sensitivity);
}


void GLViewMonkeyMovement::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   if (key.keysym.sym == SDLK_w)
   {
       w_key = true;
   }
   if (key.keysym.sym == SDLK_s)
   {
       s_key = true;
   }
   if (key.keysym.sym == SDLK_a)
   {
       a_key = true;
   }
   if (key.keysym.sym == SDLK_d)
   {
       d_key = true;
   }
   if (key.keysym.sym == SDLK_f) {
       ball_body->body->addForce(PxVec3(0, 5, 0));
   }
}


void GLViewMonkeyMovement::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );

   if (key.keysym.sym == SDLK_w)
   {
       w_key = false;
       cam->moveRelative(Vector(10,01,01));
   }
   if (key.keysym.sym == SDLK_s)
   {
       s_key = false;
   }
   if (key.keysym.sym == SDLK_a)
   {
       a_key = false;
   }
   if (key.keysym.sym == SDLK_d)
   {
       d_key = false;
   }

}

void Aftr::GLViewMonkeyMovement::midiCallback(double deltatime, std::vector<unsigned char>* message, void* userDatas) {
    GLViewMonkeyMovement* instance = static_cast<GLViewMonkeyMovement*>(userDatas);
    if (message->size() > 1) {
        instance->midi_data1 = static_cast<float>((*message)[1]);
        instance->midi_data2 = static_cast<float>((*message)[2]);
    }
}

void Aftr::GLViewMonkeyMovement::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition(10,0,0);
   anchor->setPosition(0,0,20);


   //MIDI
   midi_in.openPort(0);
   midi_in.setCallback(&GLViewMonkeyMovement::midiCallback, this);
   midi_in.ignoreTypes(false, false, false);
   //TEXT
   timer_shaddow = WOGUILabel::New(nullptr);
   timer_shaddow->setText("some text");
   timer_shaddow->setColor(0, 0, 0, 255);
   timer_shaddow->setFontSize(60); //font size is correlated with world size
   timer_shaddow->setPosition(Vector(.506, .899, 0));
   timer_shaddow->setFontOrientation(FONT_ORIENTATION::foCENTER);
   timer_shaddow->setFontPath(ManagerEnvironmentConfiguration::getLMM() + "fonts/skate.ttf");
   worldLst->push_back(timer_shaddow);
   timer = WOGUILabel::New(nullptr);
   timer->setText("some text");
   timer->setColor(255, 174, 0, 255);
   timer->setFontSize(60); //font size is correlated with world size
   timer->setPosition(Vector(.5, .9, 0));
   timer->setFontOrientation(FONT_ORIENTATION::foCENTER);
   timer->setFontPath(ManagerEnvironmentConfiguration::getLMM() + "fonts/skate.ttf");
   worldLst->push_back(timer);
   //READY??
   ready = WOGUILabel::New(nullptr);
   ready->setText(ready_text);
   ready->setColor(255, 174, 0, 255);
   ready->setFontSize(50); //font size is correlated with world siz
   ready->setPosition(Vector(.5, .5, .5));
   ready->setFontOrientation(FONT_ORIENTATION::foCENTER);
   ready->setFontPath(ManagerEnvironmentConfiguration::getLMM() + "fonts/ash.ttf");
   worldLst->push_back(ready);
   //GO
   go = WOGUILabel::New(nullptr);
   go->setText("  ");
   go->setColor(84, 140, 232, 255);
   go->setFontSize(50); //font size is correlated with world siz
   go->setPosition(Vector(.5, .5, .5));
   go->setFontOrientation(FONT_ORIENTATION::foCENTER);
   go->setFontPath(ManagerEnvironmentConfiguration::getLMM() + "fonts/ash.ttf");
   worldLst->push_back(go);

   //LIGHT
   {
      //Create a light
      float ga = 0.2f; //Global Ambient Light level for this module
      ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 10.0f ) );
      WOLight* light = WOLight::New();
      light->isDirectionalLight( true );
      light->setPosition( Vector( 0, 0, -100 ) );
      //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
      //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
      light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
      light->setLabel( "Light" );
      worldLst->push_back( light );
   }
   //SFX
   {
       std::string roll_path = ManagerEnvironmentConfiguration::getLMM() + "sounds/roll.wav";
       roll = audio_engine->play2D(roll_path.c_str(),false,false,true);
       std::string music_path = ManagerEnvironmentConfiguration::getLMM() + "sounds/ost.wav";
       //audio_engine->play2D(music_path.c_str());
   }
   //SKYBOX
   {
        //SkyBox Textures readily available
        std::vector< std::string > skyBoxImageNames; //vector to store texture paths
        skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getLMM() + "images/city_skybox.png");
        //Create the SkyBox
        WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
        wo->setPosition( Vector( 0, 0, 0 ) );
        wo->setLabel( "Sky Box" );
        wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        //worldLst->push_back( wo );
   }
   //ENVIRONMENT
   {
       environment = WO::New(ManagerEnvironmentConfiguration::getLMM() + "models/skyhigh/model.obj", Vector(5,5,5));
       environment->setPosition(-90,30, 100);
       worldLst->push_back(environment);
   }

   //AIAI
   {
       aiai = WO::New(ManagerEnvironmentConfiguration::getLMM() + "models/aiai/aiai.obj", Vector(3,3,3));
       aiai->setPosition(0, 0, 6);
       worldLst->push_back(aiai);
       aiai->upon_async_model_loaded([this] {
           ModelMeshSkin img_skin;
           img_skin.setMeshShadingType(MESH_SHADING_TYPE::mstFLAT);
           img_skin.setAmbient(aftrColor4f(1.f, 1.f, 1.f, 1.f));
           aiai->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0) = std::move(img_skin);
           aiai->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
       });
   }
   
   //PHYSX
   {
       physx::PxSceneDesc sc(p->getTolerancesScale());
       sc.filterShader = physx::PxDefaultSimulationFilterShader;
       sc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
       scene = p->createScene(sc);
       scene->setFlag(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
       scene->setGravity(PxVec3(0, 0, -30));
       //PxMaterial* gMaterial2 = p->createMaterial(0.5f, 0.5f, 0.6f);
       //PxRigidStatic* groundPlane = PxCreatePlane(*p, PxPlane(0, 0, 1, 0), *gMaterial2);//good for the grass
       //scene->addActor(*groundPlane);
   }

   //Stage 01
   {
       createStageObject(Vector(2.8f, 1, 1), Vector(-19, 15, 35.2));
       platforms[0]->setPose(platforms[0]->getPose().rotateAboutRelX(15 * DEGtoRAD));
       platforms[0]->setPose(platforms[0]->getPose().rotateAboutRelZ(90 * DEGtoRAD));
       createStageObject(Vector(1, 1, 1), Vector(0, 0, 35));
       createStageObject(Vector(1, 1, 1), Vector(-14.3, 0, 35));
       createStageObject(Vector(1, 1, 1), Vector(-14.610, -47.257, 25.206));
       createStageObject(Vector(1, 1, 1), Vector(-28.610, -47.257, 25.206));
       createStageObject(Vector(3, .5, 1), Vector(22.607, -30.914, 14.584));
       platforms[5]->setPose(platforms[5]->getPose().rotateAboutRelX(-15 * DEGtoRAD));
       platforms[5]->setPose(platforms[5]->getPose().rotateAboutRelZ(-90 * DEGtoRAD)); 
       createStageObject(Vector(1, 1, 1), Vector(-30.409, 4.241, 15.389));
       createStageObject(Vector(5, 1, 1), Vector(-56.697, 4.594, 8.905));
       createStageObject(Vector(1, 1, 1), Vector(-111.606, 4.478, 4.108));
       //GOAL
       goal = WO::New(ManagerEnvironmentConfiguration::getLMM() + "models/goal.obj");
       goal->setPosition(-112.871, 4.920, 6.375);
       worldLst->push_back(goal);
   }

   //BALL
   ball = WO::New(ManagerEnvironmentConfiguration::getLMM() + "models/ball.obj");
   ball->upon_async_model_loaded([this] {
       ModelMeshSkin img_skin(ManagerTex::loadTexAsync(ManagerEnvironmentConfiguration::getLMM() + "models/ball.png").value());
       img_skin.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
       img_skin.setAmbient(aftrColor4f(.5f, .5f, .5f, .5f));
       ball->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0) = std::move(img_skin);
       ball->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;

       Vector ball_bb = ball->getModel()->getModelDataShared()->getBoundingBox().getlxlylz() * 0.5;
       createPhysicsObject(ball, ball_bb, "sphere", ball_body);
       ball_body->body->addForce(PxVec3(0, 0, -0.1));
   });
   this->cam->setPosition(Vector(-10,-10,10));

   //GUI
   WOImGui* gui = WOImGui::New( nullptr );
   gui->setLabel( "My Gui" );
   gui->subscribe_drawImGuiWidget(
      [this, gui]() 
      {
         //ImGui::ShowDemoWindow(); //Displays the default ImGui demo from C:/repos/aburn/engine/src/imgui_implot/implot_demo.cpp
         //WOImGui::draw_AftrImGui_Demo( gui ); //Displays a small Aftr Demo from C:/repos/aburn/engine/src/aftr/WOImGui.cpp
         //ImPlot::ShowDemoWindow(); //Displays the ImPlot demo using ImGui from C:/repos/aburn/engine/src/imgui_implot/implot_demo.cpp
      } );
   this->worldLst->push_back( gui );
   createMonkeyMovementWayPoints();
}

void GLViewMonkeyMovement::createStageObject(Vector size, Vector position) {
    WO* platform = WO::New(ManagerEnvironmentConfiguration::getLMM() + "models/stage.obj", size);
    WOPhysX* platform_body = WOPhysX::New();
    platform->setPosition(position);

    platform->upon_async_model_loaded([platform, platform_body, this] {
        ModelMeshSkin img_skin(ManagerTex::loadTexAsync(ManagerEnvironmentConfiguration::getLMM() + "models/flat_tex.PNG").value());
        img_skin.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
        platform->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0) = std::move(img_skin);
        platform->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;

        PxMaterial* gMaterial = p->createMaterial(0.5f, 0.5f, 0.6f);
        Vector boundingBox = platform->getModel()->getBoundingBox().getlxlylz() * 0.5;
        platform_body->onCreatePhysX(platform, scene, p, p->createShape(PxBoxGeometry(boundingBox.x, boundingBox.y, boundingBox.z), *gMaterial, true));
        platform_body->body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
    });

    platforms.push_back(platform);
    platform_bodies.push_back(platform_body);
    worldLst->push_back(platform);
}

void GLViewMonkeyMovement::createPhysicsObject(WO* wo, Vector boundingBox, std::string shape_type, WOPhysX* ref) {
    PxMaterial* gMaterial = p->createMaterial(0.5f, 0.5f, 0.6f);
    PxShape* shape = nullptr;
    if (shape_type == "box") {
        shape = p->createShape(PxBoxGeometry(boundingBox.x, boundingBox.y, boundingBox.z), *gMaterial, true);
    }
    else if (shape_type == "sphere") {
        //SPHERE TAKES FIRST VALUE IN VECTOR AS RADIUS
        shape = p->createShape(PxSphereGeometry(boundingBox.x), *gMaterial, true);
    }
    else { return; } 
    wo->setPosition(Vector(0, 0, 40));
    worldLst->push_back(wo);
    ref->onCreatePhysX(wo, scene, p, shape);
    //object->body->addForce(PxVec3(1, 50, 2));
}

void GLViewMonkeyMovement::createMonkeyMovementWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = false;
   WOWayPointSpherical* wayPt = WOWayPointSpherical::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}

std::string GLViewMonkeyMovement::format_time(int s, int m) {
    m = m / 10;
    char buffer[10];
    sprintf(buffer, "%02d:%02d", s, m);
    return std::string(buffer);
}

bool GLViewMonkeyMovement::is_hit_target(Vector ball, Vector goal) {
    float deltax = ball.x - goal.x;
    float deltay = ball.y - goal.y;
    float dist_sq = deltax * deltax + deltay * deltay;
    return dist_sq <= 1;
}

