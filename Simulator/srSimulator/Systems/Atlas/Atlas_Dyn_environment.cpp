#include "Atlas_Dyn_environment.h"
#include <iostream>
#include "srDyn/srSpace.h"
#include <stdio.h>

#include "common/utils.h"

#include <DynaController/Atlas_Controller/Atlas_interface.hpp>
#include <DynaController/Atlas_Controller/Atlas_DynaControl_Definition.h>
#include <RobotSystems/Atlas/Atlas_Definition.h>

#include <srTerrain/Ground.h>
#include <srConfiguration.h>

Atlas_Dyn_environment::Atlas_Dyn_environment():
    ang_vel_(3)
{
    /********** Space Setup **********/
    m_Space = new srSpace();
    m_ground = new Ground();

    m_Space->AddSystem(m_ground->BuildGround());
    /********** Robot Set  **********/
    robot_ = new Atlas();
    robot_->BuildRobot(Vec3 (0., 0., 0.), 
            srSystem::FIXED, srJoint::TORQUE, ModelPath"Atlas/atlas_v3_no_head.urdf");
    m_Space->AddSystem((srSystem*)robot_);

    /******** Interface set ********/
    interface_ = new Atlas_interface();
    data_ = new Atlas_SensorData();
    cmd_ = new Atlas_Command();


    m_Space->DYN_MODE_PRESTEP();
    m_Space->SET_USER_CONTROL_FUNCTION_2(ControlFunction);
    m_Space->SetTimestep(atlas::servo_rate);
    m_Space->SetGravity(0.0,0.0,-9.81);

    m_Space->SetNumberofSubstepForRendering(30);

    //std::cout<<robot_->link_[robot_->link_idx_map_.find("r_foot")->second]
        //->GetPosition()<<std::endl;;
    //std::cout<<robot_->link_[robot_->link_idx_map_.find("l_foot")->second]
        //->GetPosition()<<std::endl;;
    printf("[Atlas Dynamic Environment] Build Dynamic Environment\n");
}

void Atlas_Dyn_environment::ControlFunction( void* _data ) {
    static int count(0);
    ++count;

    Atlas_Dyn_environment* pDyn_env = (Atlas_Dyn_environment*)_data;
    Atlas* robot = (Atlas*)(pDyn_env->robot_);
    Atlas_SensorData* p_data = pDyn_env->data_;
    
    std::vector<double> torque_command(robot->num_act_joint_);

    for(int i(0); i<robot->num_act_joint_; ++i){
        p_data->jpos[i] = robot->r_joint_[i]->m_State.m_rValue[0];
        p_data->jvel[i] = robot->r_joint_[i]->m_State.m_rValue[1];
    }
    //pDyn_env->_CheckFootContact();
    for (int i(0); i<3; ++i){
        p_data->imu_ang_vel[i] = 
            robot->link_[robot->link_idx_map_.find("pelvis")->second]->GetVel()[i];
    }
    pDyn_env->interface_->GetCommand(p_data, pDyn_env->cmd_); 

    // Set Command
    for(int i(0); i<3; ++i){
        robot->vp_joint_[i]->m_State.m_rCommand = 0.0;
        robot->vr_joint_[i]->m_State.m_rCommand = 0.0;
    }

    if( count < 100 ){
        robot->vp_joint_[0]->m_State.m_rCommand = 
            -5000. * robot->vp_joint_[0]->m_State.m_rValue[0]
            - 10. * robot->vp_joint_[0]->m_State.m_rValue[1];
        robot->vp_joint_[1]->m_State.m_rCommand = 
            -5000. * robot->vp_joint_[1]->m_State.m_rValue[0]
            - 10. * robot->vp_joint_[1]->m_State.m_rValue[1];
    }

    double Kp(200.);
    double Kd(5.);
     //double Kp(30.);
     //double Kd(0.5);
    // Right
    double ramp(1.);
    if( count < 10 ){
        ramp = ((double)count)/10.;
    }
    for(int i(0); i<robot->num_act_joint_; ++i){
        robot->r_joint_[i]->m_State.m_rCommand = pDyn_env->cmd_->jtorque_cmd[i] + 
            Kp * (pDyn_env->cmd_->jpos_cmd[i] - p_data->jpos[i]) + 
            Kd * (pDyn_env->cmd_->jvel_cmd[i] - p_data->jvel[i]);
        
        //robot->r_joint_[i]->m_State.m_rCommand *= ramp;
    }
}


  void Atlas_Dyn_environment::Rendering_Fnc(){
  }
    void Atlas_Dyn_environment::_Get_Orientation(dynacore::Quaternion & rot){
        SO3 so3_body =  robot_->
            link_[robot_->link_idx_map_.find("pelvis")->second]->GetOrientation();

        Eigen::Matrix3d ori_mtx;
        for (int i(0); i<3; ++i){
            ori_mtx(i, 0) = so3_body[0+i];
            ori_mtx(i, 1) = so3_body[3+i];
            ori_mtx(i, 2) = so3_body[6+i];
        }
        dynacore::Quaternion ori_quat(ori_mtx);
        rot = ori_quat;
    }
    Atlas_Dyn_environment::~Atlas_Dyn_environment()
    {
        //SR_SAFE_DELETE(interface_);
        SR_SAFE_DELETE(robot_);
        SR_SAFE_DELETE(m_Space);
        SR_SAFE_DELETE(m_ground);
    }


