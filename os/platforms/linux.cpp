#ifdef linux
#include <chrono>


#include "../utils.h"


namespace OS::Utils {


    int64_t GetCurrentTime1()
    {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }


    //time in format [%H:%M:%S]
    std::string GetTime(const int64_t timestamp)
    {
        std::tm* localTime = std::localtime(&timestamp);
        std::ostringstream oss;
        oss << std::put_time(localTime, "%H:%M:%S");
        return oss.str();
    }


} // namespace OS::Utils


#endif // linux
