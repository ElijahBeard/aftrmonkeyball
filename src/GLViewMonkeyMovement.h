#pragma once

#include "GLView.h"
#include "irrKlang.h"
#include "PxPhysicsAPI.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "extensions/PxDefaultAllocator.h"
#include "RtMidi.h"
#include "WOPhysX.h"
#include <cmath>
#include <vector>
#include "WOGUILabel.h"

namespace Aftr
{
   class Camera;

/**
   \class GLViewMonkeyMovement
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewMonkeyMovement : public GLView
{
public:
    static GLViewMonkeyMovement* New( const std::vector< std::string >& outArgs );
    virtual ~GLViewMonkeyMovement();
    virtual void updateWorld(); ///< Called once per frame
    virtual void loadMap(); ///< Called once at startup to build this module's scene
    virtual void createMonkeyMovementWayPoints();
    virtual void onResizeWindow( GLsizei width, GLsizei height );
    virtual void onMouseDown( const SDL_MouseButtonEvent& e );
    virtual void onMouseUp( const SDL_MouseButtonEvent& e );
    virtual void onMouseMove( const SDL_MouseMotionEvent& e );
    virtual void onKeyDown( const SDL_KeyboardEvent& key );
    virtual void onKeyUp( const SDL_KeyboardEvent& key );

    //MOVEMENT
    float mouse_sensitivity = 1.f;
    bool w_key = false;
    bool a_key = false;
    bool s_key = false;
    bool d_key = false;

    //SOUND ENGINE
    irrklang::ISoundEngine* audio_engine = irrklang::createIrrKlangDevice();
    irrklang::ISound* roll;
    irrklang::ISound* sec;
    irrklang::ISound* win;
    float previous_velocity = 0.f;

    //PHYSX ENGINE
    void createPhysicsObject(WO* wo, Vector boundingBox, std::string shape_type, WOPhysX* ref);
    WOPhysX* object = WOPhysX::New();
    physx::PxDefaultAllocator a;
    physx::PxDefaultErrorCallback e;
    physx::PxFoundation* f = PxCreateFoundation(PX_PHYSICS_VERSION, a, e);
    physx::PxPhysics* p = PxCreatePhysics(PX_PHYSICS_VERSION, *f, physx::PxTolerancesScale());
    physx::PxScene* scene = nullptr;
    //PHYSXOBJ
    WOPhysX* stage_body = nullptr;
    WO* stage_WO = nullptr;
    WO* ball = nullptr;
    WOPhysX* ball_body = WOPhysX::New();

    //CAM
    Vector camera_offset = (0, 10, 6);
    float left_target_angle = 45.f;
    float left_current_angle = 0.f;
    float right_target_angle = 45.f;
    float right_current_angle = 0.f;
    float tilt_speed = 5.f;
    Aftr::Mat4 staticpose;
    WO* anchor = nullptr;
    float offset_radius = 15.0f;
    float angle = 0.01f;
    float currx = 0.f;
    float dir_angle = 45.f;

    //Models
    WO* environment = nullptr;
    WO* aiai = nullptr;
    WO* goal = nullptr;

    //STATES
    bool finished = false;
    bool is_hit_target(Vector ball, Vector goal);

    //STAGE
    std::vector<WO*> platforms;
    std::vector<WOPhysX*> platform_bodies;
    void createStageObject(Vector size, Vector position);

    //timer
    WOGUILabel* timer;
    WOGUILabel* timer_shaddow;
    std::string format_time(int s, int m);
    double elapsed_time = -3.0;
    int seconds = 0;
    int milliseconds = 0;
    double click = -3.0;
    bool time_start = false;

    //TEXT
    WOGUILabel* ready;
    WOGUILabel* go;
    std::string ready_text = "READY";
    std::string go_text = "  ";

    //MIDI
    bool use_midi = false;
    RtMidiIn midi_in;
    float midi_data1 = 0.f;
    float midi_data2 = 0.f;

protected:
   GLViewMonkeyMovement( const std::vector< std::string >& args );
   virtual void onCreate();   
};

/** \} */

} //namespace Aftr
