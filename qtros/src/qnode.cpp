/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date February 2011
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include "../include/qtros/qnode.hpp"


/*****************************************************************************
** Namespaces
*****************************************************************************/
using namespace std;

namespace qtros {

/*****************************************************************************
** Implementation
*****************************************************************************/

QNode::QNode(int argc, char** argv ) :
	init_argc(argc),
	init_argv(argv)
    {}

QNode::~QNode() {
    if(ros::isStarted()) {
      ros::shutdown(); // explicitly needed since we use ros::start();
      ros::waitForShutdown();
    }
	wait();
}

bool QNode::init() {
	ros::init(init_argc,init_argv,"qtros");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
	// Add your ros communications here.
    //chatter_publisher = n.advertise<std_msgs::String>("chatter11", 1000);

    //chatter_subscriber = n.subscribe("chatter", 1000, &QNode::myCallback, this);
    throttle_subscriber = n.subscribe("vehicle/throttle_info_report", 1000, &QNode::throttle_Callback, this);
    steeringreport_subscriber = n.subscribe("vehicle/steering_report", 1000, &QNode::steeringreport_Callback, this);
    gear_subscriber = n.subscribe("vehicle/steering_report", 1000, &QNode::gear_Callback, this);
    brake_subscriber = n.subscribe("vehicle/brake_report", 1000, &QNode::brake_Callback, this);
    fuel_subscriber = n.subscribe("vehicle/fuel_level_report", 1000, &QNode::fuel_Callback, this);
    turn_signal_subscriber = n.subscribe("vehicle/turn_signal_cmd", 1000, &QNode::turn_signal_Callback, this);
    wheel_speed_subscriber = n.subscribe("vehicle/wheel_speed_report", 1000, &QNode::wheel_speed_Callback, this);
    control_mode_subscriber = n.subscribe("vehicle/wheel_speed_report", 1000, &QNode::wheel_speed_Callback, this);


	start();
	return true;
}

bool QNode::init(const std::string &master_url, const std::string &host_url) {
    std::map<std::string,std::string> remappings;
	remappings["__master"] = master_url;
	remappings["__hostname"] = host_url;
	ros::init(remappings,"qtros");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;

    //Add your ros communications here.
    //chatter_publisher = n.advertise<std_msgs::String>("chatter11", 1000);
    //chatter_subscriber = n.subscribe("chatter", 1000, &QNode::myCallback, this);
    throttle_subscriber = n.subscribe("vehicle/throttle_info_report", 1000, &QNode::throttle_Callback, this);
    steeringreport_subscriber = n.subscribe("vehicle/steering_report", 1000, &QNode::steeringreport_Callback, this);
    gear_subscriber = n.subscribe("vehicle/gear_report", 1000, &QNode::gear_Callback, this);
    brake_subscriber = n.subscribe("vehicle/brake_report", 1000, &QNode::brake_Callback, this);
    fuel_subscriber = n.subscribe("vehicle/fuel_level_report", 1000, &QNode::fuel_Callback, this);
    turn_signal_subscriber = n.subscribe("vehicle/turn_signal_cmd", 1000, &QNode::turn_signal_Callback, this);
    wheel_speed_subscriber = n.subscribe("vehicle/wheel_speed_report", 1000, &QNode::wheel_speed_Callback, this);
    control_mode_subscriber = n.subscribe("vehicle/wheel_speed_report", 1000, &QNode::wheel_speed_Callback, this);


	start();
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Callback
///////////////////////////////////////////////////
///
//void QNode::control_mode_Callback(const dbw_mkz_msgs::WheelSpeedReportPtr  &control_mode_holder)
//{
////    if(turn_signal_holder->cmd.value == 0){
////        turn_signal = "NONE" ;
////    }
////    else if(turn_signal_holder->cmd.value == 1){
////        turn_signal = "LEFT" ;
////    }
//}


void QNode::wheel_speed_Callback(const dbw_mkz_msgs::WheelSpeedReportPtr  &wheel_speed_holder)
{
   wheel_speed=  wheel_speed_holder->rear_left+ wheel_speed_holder->rear_right;

}

void QNode::turn_signal_Callback(const dbw_mkz_msgs::TurnSignalCmdPtr  &turn_signal_holder)
{
    if(turn_signal_holder->cmd.value == 0){
        turn_signal = "NONE" ;
    }
    else if(turn_signal_holder->cmd.value == 1){
        turn_signal = "LEFT" ;
    }
    else if(turn_signal_holder->cmd.value == 2){
        turn_signal = "RIGHT" ;
    }

}



void QNode::fuel_Callback(const dbw_mkz_msgs::FuelLevelReportPtr  &fuel_holder)
{
   fuel_level=fuel_holder->fuel_level;

}



void QNode::brake_Callback(const dbw_mkz_msgs::BrakeReportPtr  &brake_holder)
{
    brake=brake_holder->torque_output;

}



void QNode::gear_Callback(const dbw_mkz_msgs::GearReportPtr  &gear_holder)
{


  if(gear_holder->state.gear == 1){
      gear = "P (PARK)" ;
  }
  else if(gear_holder->state.gear == 2){
      gear = "R (REVERSE)" ;
  }
  else if(gear_holder->state.gear == 3){
      gear = "N (NEUTRAL)" ;
  }
  else if(gear_holder->state.gear == 4){
      gear = "D (DRIVE)" ;
  }
  else if(gear_holder->state.gear == 5){
      gear = "L (LOW)" ;
  }

//  uint8 NONE=0
//  uint8 PARK=1
//  uint8 REVERSE=2
//  uint8 NEUTRAL=3
//  uint8 DRIVE=4
//  uint8 LOW=5

  cout << gear_holder->state.gear << endl;

}



void QNode::steeringreport_Callback(const dbw_mkz_msgs::SteeringReportPtr &steeringreport_holder)
{
    //steering_angle
    steering_angle = (steeringreport_holder->steering_wheel_angle)*180/3.14159265359;
    // Vehicle Speed
    vehicle_speed = (steeringreport_holder->speed)*3600/1000; //(m/s)to(km/hour)


}

void QNode::throttle_Callback(const dbw_mkz_msgs::ThrottleInfoReportConstPtr  &throttle_holder)
{   
    float temp=(throttle_holder->throttle_pc)*100;
    throttle= (int)temp;

 }


void QNode::myCallback(const std_msgs::Float64& message_holder)
{
    //std::stringstream ss;
    //ss << message_holder.data;
    log(Info, message_holder);
    ROS_INFO("=============received value is: %f===========",message_holder.data);
    //really could do something interesting here with the received data...but all we do is print it
}

//////////////////////////////////////////////////////////////////////////////

void QNode::run() {
//	ros::Rate loop_rate(1);
//	int count = 0;
//	while ( ros::ok() ) {
//        //chatter_subscriber = n.subscribe("chatter", 1000, &QNode::myCallback, this);

//		std_msgs::String msg;
//		std::stringstream ss;
//		ss << "hello world " << count;
//		msg.data = ss.str();
//		chatter_publisher.publish(msg);
//		log(Info,std::string("I sent: ")+msg.data);
//		ros::spinOnce();
//		loop_rate.sleep();
//		++count;
//	}
//	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
//	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)

    ros::NodeHandle n;
        chatter_subscriber = n.subscribe("chatter", 1000, &QNode::myCallback, this);
        throttle_subscriber = n.subscribe("vehicle/throttle_info_report", 1000, &QNode::throttle_Callback, this);
        steeringreport_subscriber = n.subscribe("vehicle/steering_report", 1000, &QNode::steeringreport_Callback, this);

        ros::spin();

        std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
    Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}


void QNode::log( const LogLevel &level, const std_msgs::Float64 &msg) {
    logging_model.insertRows(logging_model.rowCount(),1);
	std::stringstream logging_model_msg;
	switch ( level ) {
		case(Debug) : {
				ROS_DEBUG_STREAM(msg);
				logging_model_msg << "[DEBUG] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Info) : {
				ROS_INFO_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Warn) : {
				ROS_WARN_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Error) : {
				ROS_ERROR_STREAM(msg);
				logging_model_msg << "[ERROR] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Fatal) : {
				ROS_FATAL_STREAM(msg);
				logging_model_msg << "[FATAL] [" << ros::Time::now() << "]: " << msg;
				break;
		}
	}
	QVariant new_row(QString(logging_model_msg.str().c_str()));
	logging_model.setData(logging_model.index(logging_model.rowCount()-1),new_row);
	Q_EMIT loggingUpdated(); // used to readjust the scrollbar
}

}  // namespace qtros
