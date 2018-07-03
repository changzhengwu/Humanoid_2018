#include "ConfigTask.hpp"
#include <Configuration.h>
#include <Atlas_Controller/Atlas_StateProvider.hpp>
#include <Atlas/Atlas_Definition.h>
#include <Utils/utilities.hpp>

ConfigTask::ConfigTask():Task(atlas::num_qdot),
                     Kp_vec_(atlas::num_qdot),
                     Kd_vec_(atlas::num_qdot)
{
  Kp_vec_.setZero();
  Kd_vec_.setZero();
  for(int i(0); i<atlas::num_qdot; ++i){
    Kp_vec_[i] = 150.;
    Kd_vec_[i] = 3.;
  }
  for(int i(0); i<atlas::num_virtual; ++i){
    Kp_vec_[i] = 0.;
    Kd_vec_[i] = 0.;
  }
  sp_ = Atlas_StateProvider::getStateProvider();
  Jt_ = dynacore::Matrix(atlas::num_qdot, atlas::num_qdot);
  JtDotQdot_ = dynacore::Vector::Zero(atlas::num_qdot);
}

ConfigTask::~ConfigTask(){}

bool ConfigTask::_UpdateCommand(void* pos_des,
                              const dynacore::Vector & vel_des,
                              const dynacore::Vector & acc_des){
  dynacore::Vector* pos_cmd = (dynacore::Vector*)pos_des;

// TODO: Implemented based on the assumption that 
// we do not control orientation
  for(int i(0); i<atlas::num_qdot; ++i){
    op_cmd_[i] = acc_des[i] 
        + Kp_vec_[i] * ((*pos_cmd)[i] - sp_->Q_[i]) 
        + Kd_vec_[i] * (vel_des[i] - sp_->Qdot_[i]);
  }
  // dynacore::pretty_print(Kp_vec_, std::cout, "Kp");
  // dynacore::pretty_print(Kd_vec_, std::cout, "Kd");
  // dynacore::pretty_print(acc_des, std::cout, "acc_des");
  // dynacore::pretty_print(op_cmd_, std::cout, "op cmd");
  // dynacore::pretty_print(*pos_cmd, std::cout, "pos cmd");
  // dynacore::pretty_print(sp_->Q_, std::cout, "config");

  return true;
}

bool ConfigTask::_UpdateTaskJacobian(){
  Jt_.setIdentity();
  return true;
}

bool ConfigTask::_UpdateTaskJDotQdot(){
  JtDotQdot_.setZero();
  return true;
}
