#ifndef ABSTRACT_CLASS_SWING_LEG_CONTROL_VALKYRIE
#define ABSTRACT_CLASS_SWING_LEG_CONTROL_VALKYRIE

#include <Controller.hpp>
#include <Valkyrie_Controller/Valkyrie_DynaControl_Definition.h>
#include <Valkyrie/Valkyrie_Model.hpp>
#include <Valkyrie_Controller/Valkyrie_StateProvider.hpp>

class Valkyrie_StateProvider;
class Planner;
class WBDC_ContactSpec;
class WBDC;
class WBDC_ExtraData;


class SwingPlanningCtrl:public Controller{
    public:
        SwingPlanningCtrl(const RobotSystem* robot, int swing_foot, Planner* planner):
            Controller(robot),
            swing_foot_(swing_foot),
            num_planning_(0),
            planner_(planner),
            planning_frequency_(0.),
            replan_moment_(0.),
            ctrl_start_time_(0.),
            half_swing_time_(0.15),
            b_contact_switch_check_(false){   
                curr_foot_pos_des_.setZero();
                curr_foot_vel_des_.setZero();
                curr_foot_acc_des_.setZero();
                sp_ = Valkyrie_StateProvider::getStateProvider();
            }

        virtual ~SwingPlanningCtrl(){
        }

        void setPlanningFrequency(double freq){  planning_frequency_ = freq; }
        void setSwingTime(double swing_time){ 
            end_time_ = swing_time; 
            half_swing_time_ = end_time_/2.;
        }
        void setDoubleStanceRatio(double ratio){  double_stance_ratio_ = ratio;}
        void setTransitionPhaseRatio(double ratio){  transition_phase_ratio_ = ratio;}

        void notifyTransitionTime(double time){  transition_time_ = time; }
        void notifyStanceTime(double time){  stance_time_ = time; }

        void setPrimeTimeX(double t_p_x){ t_prime_x_ = t_p_x; }
        void setPrimeTimeY(double t_p_y){ t_prime_y_ = t_p_y; }
        void setStanceHeight(double height) {
            des_body_height_ = height;
            b_set_height_target_ = true;
        }
        void setContactSwitchCheck(bool switch_check){ b_contact_switch_check_ = switch_check; }

        dynacore::Vect3 curr_foot_pos_des_;
        dynacore::Vect3 curr_foot_vel_des_;
        dynacore::Vect3 curr_foot_acc_des_;

    protected:
        bool b_contact_switch_check_;
        bool b_set_height_target_;
        double des_body_height_;

        double double_stance_ratio_;
        double transition_phase_ratio_;
        double replan_moment_;

        int swing_foot_;
        double swing_height_;
        dynacore::Vect3 default_target_loc_;

        double planning_frequency_;
        int num_planning_;
        double t_prime_x_;
        double t_prime_y_;

        WBDC_ContactSpec* single_contact_;
        WBDC* wbdc_;
        WBDC_ExtraData* wbdc_data_;
        Planner* planner_;

        Valkyrie_StateProvider* sp_;
    
        // Timing parameters
        double end_time_;
        double half_swing_time_;
        double transition_time_;
        double stance_time_;
        double ctrl_start_time_;
};

#endif
