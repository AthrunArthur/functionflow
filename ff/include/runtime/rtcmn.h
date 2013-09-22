#ifndef FF_RUNTIME_RTCMN_H_
#define FF_RUNTIME_RTCMN_H_
namespace ff {
namespace rt {

//Give other tasks opportunities to run!
void yield();
}//end namespace rt


bool	is_idle();

//bool	is_busy();

}//end namespace ff
#endif
