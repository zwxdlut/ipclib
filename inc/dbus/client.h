#ifndef __IPC_DBUS_CLIENT_H__
#define __IPC_DBUS_CLIENT_H__

#include "core.h"

namespace ipc
{
    class client : public core
    {
    public:
        client();

        ~client();

        int connect(const std::string &_conn);

        template <typename... ARGS>
        int invoke(const std::string &_path, const std::string &_iface, const std::string &_member, ARGS&... _args)
        {
            if (detail::args_check<ARGS...>::has_out_)
            {
                return resolve(true, _path, _iface, _member, _args...);
            }
            else
            {
                return send(false, _path, _iface, _member, _args...);
            }
        }

    private:
        int add_match(const std::string &_path, const std::string &_iface, const std::string &_member);

        bool is_match(dbus_message _msg, const std::string &_iface, const std::string &_member);

        template <typename... ARGS>
        int send(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, ARGS&... _args)
        {    
            int ret = 0;
            dbus_message msg = nullptr;   
            std::lock_guard<std::mutex> lock(mutex_send_);

            if(0 == (msg = dbus_.create_method_call(conn_, _path, _iface, _member))
                || 0 != dbus_.pack_args(msg, _args...) 
                || (_has_out ? (0 != dbus_.send_with_reply(msg) || 0 != dbus_.unpack_args(msg, _args...)) : (0 != dbus_.send(msg))))
            {
                ret = -1;
            }
        
            dbus_.free(msg);

            LOGI(TAG, "path = %s, iface = %s, method = %s(%s) %s", _path.c_str(), _iface.c_str(), _member.c_str(), 
                (_has_out ? "with replay" : "without replay"), 0 == ret ? "successful" : "failed!");

            return ret;
        }



        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member)
        {
            return send(_has_out, _path, _iface, _member);
        }

        template <typename T1>
        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, 
            T1 &_t1)
        {
            typename detail::type_redefine<T1>::type tt1 = _t1;

            return send(_has_out, _path, _iface, _member, tt1);
        }

        template <typename T1, typename T2>
        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, 
            T1 &_t1, T2 &_t2)
        {
            typename detail::type_redefine<T1>::type tt1 = _t1;
            typename detail::type_redefine<T2>::type tt2 = _t2;
            
            return send(!_has_out, _path, _iface, _member, tt1, tt2);
        }

        template <typename T1, typename T2, typename T3>
        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, 
            T1 &_t1, T2 &_t2, T3 &_t3)
        {
            typename detail::type_redefine<T1>::type tt1 = _t1;
            typename detail::type_redefine<T2>::type tt2 = _t2;
            typename detail::type_redefine<T3>::type tt3 = _t3;

            return send(_has_out, _path, _iface, _member, tt1, tt2, tt3);
        }

        template <typename T1, typename T2, typename T3, typename T4>
        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, 
            T1 &_t1, T2 &_t2, T3 &_t3, T4 &_t4)
        {
            typename detail::type_redefine<T1>::type tt1 = _t1;
            typename detail::type_redefine<T2>::type tt2 = _t2;
            typename detail::type_redefine<T3>::type tt3 = _t3;
            typename detail::type_redefine<T4>::type tt4 = _t4;

            return send(_has_out, _path, _iface, _member, tt1, tt2, tt3, tt4);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5>
        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, 
            T1 &_t1, T2 &_t2, T3 &_t3, T4 &_t4, T5 &_t5)
        {
            typename detail::type_redefine<T1>::type tt1 = _t1;
            typename detail::type_redefine<T2>::type tt2 = _t2;
            typename detail::type_redefine<T3>::type tt3 = _t3;
            typename detail::type_redefine<T3>::type tt4 = _t4;
            typename detail::type_redefine<T5>::type tt5 = _t5;

            return send(!_has_out, _path, _iface, _member, tt1, tt2, tt3, tt4, tt5);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, 
            T1 &_t1, T2 &_t2, T3 &_t3, T4 &_t4, T5 &_t5, T6 &_t6)
        {
            typename detail::type_redefine<T1>::type tt1 = _t1;
            typename detail::type_redefine<T2>::type tt2 = _t2;
            typename detail::type_redefine<T3>::type tt3 = _t3;
            typename detail::type_redefine<T3>::type tt4 = _t4;
            typename detail::type_redefine<T5>::type tt5 = _t5;
            typename detail::type_redefine<T6>::type tt6 = _t6;

            return send(_has_out, _path, _iface, _member, tt1, tt2, tt3, tt4, tt5, tt6);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, 
            T1 &_t1, T2 &_t2, T3 &_t3, T4 &_t4, T5 &_t5, T6 &_t6, T7 &_t7)
        {
            typename detail::type_redefine<T1>::type tt1 = _t1;
            typename detail::type_redefine<T2>::type tt2 = _t2;
            typename detail::type_redefine<T3>::type tt3 = _t3;
            typename detail::type_redefine<T3>::type tt4 = _t4;
            typename detail::type_redefine<T5>::type tt5 = _t5;
            typename detail::type_redefine<T6>::type tt6 = _t6;
            typename detail::type_redefine<T7>::type tt7 = _t7;
            
            return send(_has_out, _path, _iface, _member, tt1, tt2, tt3, tt4, tt5, tt6, tt7);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, 
            T1 &_t1, T2 &_t2, T3 &_t3, T4 &_t4, T5 &_t5, T6 &_t6, T7 &_t7, T8 &_t8)
        {
            typename detail::type_redefine<T1>::type tt1 = _t1;
            typename detail::type_redefine<T2>::type tt2 = _t2;
            typename detail::type_redefine<T3>::type tt3 = _t3;
            typename detail::type_redefine<T3>::type tt4 = _t4;
            typename detail::type_redefine<T5>::type tt5 = _t5;
            typename detail::type_redefine<T6>::type tt6 = _t6;
            typename detail::type_redefine<T7>::type tt7 = _t7;
            typename detail::type_redefine<T7>::type tt8 = _t8;

            return send(_has_out, _path, _iface, _member, tt1, tt2, tt3, tt4, tt5, tt6, tt7, tt8);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, 
            T1 &_t1, T2 &_t2, T3 &_t3, T4 &_t4, T5 &_t5, T6 &_t6, T7 &_t7, T8 &_t8, T9 &_t9)
        {
            typename detail::type_redefine<T1>::type tt1 = _t1;
            typename detail::type_redefine<T2>::type tt2 = _t2;
            typename detail::type_redefine<T3>::type tt3 = _t3;
            typename detail::type_redefine<T3>::type tt4 = _t4;
            typename detail::type_redefine<T5>::type tt5 = _t5;
            typename detail::type_redefine<T6>::type tt6 = _t6;
            typename detail::type_redefine<T7>::type tt7 = _t7; 
            typename detail::type_redefine<T7>::type tt8 = _t8;
            typename detail::type_redefine<T7>::type tt9 = _t9;

            return send(_has_out, _path, _iface, _member, tt1, tt2, tt3, tt4, tt5, tt6, tt7, tt8, tt9);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
        int resolve(const bool _has_out, const std::string &_path, const std::string &_iface, const std::string &_member, 
            T1 &_t1, T2 &_t2, T3 &_t3, T4 &_t4, T5 &_t5, T6 &_t6, T7 &_t7, T8 &_t8, T9 &_t9, T10 &_t10)
        {
            typename detail::type_redefine<T1>::type tt1 = _t1;
            typename detail::type_redefine<T2>::type tt2 = _t2;
            typename detail::type_redefine<T3>::type tt3 = _t3;
            typename detail::type_redefine<T3>::type tt4 = _t4;
            typename detail::type_redefine<T5>::type tt5 = _t5;
            typename detail::type_redefine<T6>::type tt6 = _t6;
            typename detail::type_redefine<T7>::type tt7 = _t7; 
            typename detail::type_redefine<T7>::type tt8 = _t8;
            typename detail::type_redefine<T7>::type tt9 = _t9;
            typename detail::type_redefine<T7>::type tt10 = _t10;

            return send(_has_out, _path, _iface, _member, tt1, tt2, tt3, tt4, tt5, tt6, tt7, tt8, tt9, tt10);
        }
    };
} // namespace ipc

#endif // __IPC_CLIENT_H__
