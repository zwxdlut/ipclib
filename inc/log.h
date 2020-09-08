#ifndef __LOG_H__
#define __LOG_H__

namespace ipc
{
    #define ESC_START     "\033["
    #define ESC_END       "\033[0m"
    #define COLOR_FATAL   "31;40;5m"
    #define COLOR_ALERT   "31;40;1m"
    #define COLOR_CRIT    "31;40;1m"
    #define COLOR_ERROR   "31;40;1m"
    #define COLOR_WARN    "33;40;1m"
    #define COLOR_NOTICE  "34;40;1m"
    #define COLOR_INFO    "32;40;1m"
    #define COLOR_DEBUG   "36;40;1m"
    #define COLOR_TRACE   "37;40;1m"

    #define LOGI(TAG, format, ...) do{printf(ESC_START COLOR_INFO "%s(%d)[%s::%s]: " format"\n" ESC_END, __FILE__, __LINE__, TAG, __FUNCTION__, ##__VA_ARGS__);}while(0)
    #define LOGW(TAG, format, ...) do{printf(ESC_START COLOR_WARN "%s(%d)[%s::%s]: " format"\n" ESC_END, __FILE__, __LINE__, TAG, __FUNCTION__, ##__VA_ARGS__);}while(0)
    #define LOGE(TAG, format, ...) do{printf(ESC_START COLOR_ERROR "%s(%d)[%s::%s]: " format"\n" ESC_END, __FILE__, __LINE__, TAG, __FUNCTION__, ##__VA_ARGS__);}while(0)

    #if defined(_DEBUG)
        #define LOGD(TAG, format, ...) do{printf(ESC_START COLOR_DEBUG "%s(%d)[%s::%s]: " format"\n" ESC_END, __FILE__, __LINE__, TAG, __FUNCTION__, ##__VA_ARGS__);}while(0)
    #else 
        #define LOGD(TAG, format, ...)
    #endif
 
} // namespace ipc

#endif // __LOG_H__