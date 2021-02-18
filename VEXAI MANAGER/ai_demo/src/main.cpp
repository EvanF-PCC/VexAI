/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       james                                                     */
/*    Created:      Mon Aug 31 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

// A global instance of competition
competition Competition;

// create instance of jetson class to receive location and other
// data from the Jetson nano
//
ai::jetson  jetson_comms;

/*----------------------------------------------------------------------------*/
// Create a robot_link on PORT1 using the unique name robot_32456_1
// The unique name should probably incorporate the team number
// and be at least 12 characters so as to generate a good hash
//
// The Demo is symetrical, we send the same data and display the same status on both
// manager and worker robots
// Comment out the following definition to build for the worker robot
#define  MANAGER_ROBOT    1

#if defined(MANAGER_ROBOT)
#pragma message("building for the manager")
ai::robot_link       link( PORT1, "robot_32456_1", linkType::manager );
#else
#pragma message("building for the worker")
ai::robot_link       link( PORT1, "robot_32456_1", linkType::worker );
#endif


//   FL (2)         FR (9)
//             ^
//    //       |      //
//    // ------------ //
//    //       |      //
//             |
//             |
//             |
//    //       |      //
//    // ------------ //
//    //              //
//
//    BL (11)        BR (20)

motor MotorFL = motor(PORT2, ratio18_1, false);
motor MotorFR = motor(PORT9, ratio18_1, true);

motor MotorBL = motor(PORT11, ratio18_1, false);
motor MotorBR = motor(PORT20, ratio18_1, true);

motor Motors[4] = {MotorFL, MotorFR, MotorBL, MotorBR};


motor DebuggingMotor(PORT6, ratio18_1, false);

inertial IntertiaSensor(PORT7);

int InstantiatePeripherals() {
  if (!IntertiaSensor.installed()){
    DebuggingMotor.spin(directionType::rev);
    this_thread::sleep_for(1000);
    DebuggingMotor.stop();
    return -1;
  }
  else {
    DebuggingMotor.spin(directionType::fwd);
    this_thread::sleep_for(1000);
    DebuggingMotor.stop();
  }
  
  for (int i = 0; i < 4; i++){
    if (!Motors[i].installed()){
      return -1;
    }
    Motors[i].spin(directionType::fwd);
    this_thread::sleep_for(1000);
    Motors[i].stop();
  }

  
  return 1;
  

  return 1;
}









/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          Auto_Isolation Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous isolation  */
/*  phase of a VEX AI Competition.                                           */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void auto_Isolation(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                        Auto_Interaction Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous interaction*/
/*  phase of a VEX AI Competition.                                           */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/


void auto_Interaction(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          AutonomousMain Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

bool firstAutoFlag = true;

void autonomousMain(void) {
  // ..........................................................................
  // The first time we enter this function we will launch our Isolation routine
  // When the field goes disabled after the isolation period this task will die
  // When the field goes enabled for the second time this task will start again
  // and we will enter the interaction period. 
  // ..........................................................................

  if(firstAutoFlag)
    auto_Isolation();
  else 
    auto_Interaction();

  firstAutoFlag = false;
}


/*----------------------------------------------------------------------------*/






// ----- Evan's Code starts here -----

vex::message_link msg = message_link(PORT1, "mywackylink", linkType::manager);

motor Motor10(PORT10, ratio18_1, false);

void started(const char *, const char *,double){
  Motor10.spin(fwd);
}
void stopped(const char *, const char *,double){
  Motor10.stop();
}

void EvanInit(){
  

  msg.received("start", started);
  msg.received("stop", stopped);
}

// evan's stops here







int main() {
    // Initializing Robot Configuration. DO NOT REMOVE!
    vexcodeInit();

    // local storage for latest data from the Jetson Nano
    static MAP_RECORD       local_map;

    // RUn at about 15Hz
    int32_t loop_time = 66;

    // start the status update display
    thread t1(dashboardTask);
    thread t2(EvanInit);
    thread t3(InstantiatePeripherals);
    // Set up callbacks for autonomous and driver control periods.
    Competition.autonomous(autonomousMain);

    // print through the controller to the terminal (vexos 1.0.12 is needed)
    // As USB is tied up with Jetson communications we cannot use
    // printf for debug.  If the controller is connected
    // then this can be used as a direct connection to USB on the controller
    // when using VEXcode.
    //
    //FILE *fp = fopen("/dev/serial2","wb");

    while(1) {
        // get last map data
        jetson_comms.get_data( &local_map );

        // set our location to be sent to partner robot
        link.set_remote_location( local_map.pos.x, local_map.pos.y, local_map.pos.az );

        //fprintf(fp, "%.2f %.2f %.2f\n", local_map.pos.x, local_map.pos.y, local_map.pos.az  );

        // request new data    
        // NOTE: This request should only happen in a single task.    
        jetson_comms.request_map();

        // Allow other tasks to run
        this_thread::sleep_for(loop_time);
    }
}



