#include "logs_model.h"


namespace UIModel {


    void LogMainModel::Clear()
    {
       Buf.clear();
       LineOffsets.clear();
       LineOffsets.push_back(0);
    }


    void LogMainModel::AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
       int old_size = Buf.size();
       va_list args;
       va_start(args, fmt);
       Buf.appendfv(fmt, args);
       va_end(args);
       for (int new_size = Buf.size(); old_size < new_size; old_size++)
           if (Buf[old_size] == '\n')
              LineOffsets.push_back(old_size + 1);
    }


   void LogMainModel::AddLog(const std::string& log)
   {
       int old_size = Buf.size();
       Buf.append(log.c_str());
       for (size_t new_size = Buf.size(); old_size < new_size; old_size++)
           if (Buf[old_size] == '\n')
               LineOffsets.push_back(old_size + 1);
   }


} // UIModel
