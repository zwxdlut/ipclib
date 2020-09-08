#ifndef __DBUS_HELPER_H__
#define __DBUS_HELPER_H__

#define DBUS_API_SUBJECT_TO_CHANGE

#include <inttypes.h>

#include <vector>
#include <string>
#include <typeinfo>
#include <initializer_list>

#include <dbus/dbus.h>

#include "../log.h"

namespace ipc
{
	class dbus_helper
	{
	public:
		dbus_helper();

		~dbus_helper();

		DBusMessage* create_method_call(const std::string &_conn, const std::string &_path, const std::string &_iface, const std::string &_method);

		DBusMessage* create_signal(const std::string &_path, const std::string &_iface, const std::string &_signal);

		DBusMessage* create_reply(DBusMessage *_msg);

		DBusMessage* pop();

		int connect(const std::string &_name);

		int disconnect();

		int free(DBusMessage *_msg);

		int send(DBusMessage *_msg);

		int send_with_reply(DBusMessage *&_msg);

		int add_match(const std::string &_rule);

		int remove_match(const std::string &_rule);

		bool has_path(DBusMessage *_msg, const std::string &_path);

		bool is_method_call(DBusMessage *_msg, const std::string &_iface, const std::string &_member);

		bool is_signal(DBusMessage *_msg, const std::string &_iface, const std::string &_member);

		template <typename... ARGS>
		int pack_args(DBusMessage *_msg, const ARGS&... _args)
		{
			LOGD(TAG, "+");

			if (nullptr == _msg)
			{
				LOGE(TAG, "message is null!");
				return -1;
			}

			DBusMessageIter iter;

			dbus_message_iter_init_append(_msg, &iter);
			std::initializer_list<int>{(arg_in(iter, _args), 0)...};

			LOGD(TAG, "-");

			return 0;

		};

		template <typename... ARGS>
		int unpack_args(DBusMessage *_msg, ARGS&... _args)
		{
			LOGD(TAG, "+");

			if (nullptr == _msg)
			{
				LOGE(TAG, "message is null!");
				return -1;
			}

			DBusMessageIter iter;

			if(!dbus_message_iter_init(_msg, &iter))
			{
				LOGE(TAG, "initialize message iterator failed!");
				return -1;
			}

			std::initializer_list<int>{(arg_out(iter, _args), 0)...};

			LOGD(TAG, "-");

			return 0;
		}

	private:
		template <typename T>
		int signature(std::string &_sig)
		{
			LOGE(TAG, "type %s is not implement!", typeid(T).name());
			return -1;
		};

		template <typename T>
		int pack(DBusMessageIter &_iter, const T &_t)
		{
			LOGE(TAG, "type %s is not implement!", typeid(T).name());
			return -1;
		};

		template <typename T>
		int unpack(DBusMessageIter &_iter, T &_t)
		{
			LOGE(TAG, "type %s is not implement!", typeid(T).name());
			return -1;
		};

		template <typename T>
		int arg_in(DBusMessageIter &_iter, const T &_t)
		{
			const char *type_name = typeid(T).name();
			DBusMessageIter subIter;

			LOGD(TAG, "type is %s", type_name);

			if (!dbus_message_iter_open_container(&_iter, DBUS_TYPE_STRUCT, 0, &subIter))
			{
				LOGE(TAG, "open container failed!");
				return -1;		
			}

			if (0 != pack(subIter, _t))
			{
				LOGE(TAG, "pack type %s failed", type_name);
				dbus_message_iter_abandon_container(&_iter, &subIter);
				return -1;
			}

			if (!dbus_message_iter_close_container(&_iter, &subIter))
			{
				LOGE(TAG, "close container failed!");
				return -1;
			}

			return 0;
		};

		template <typename T>
		int arg_in(DBusMessageIter &_iter, const std::vector<T> &_vt)
		{
			LOGD(TAG, "type is std::vector<%s>, size = %ld", typeid(T).name(), _vt.size());

			DBusMessageIter subIter;
			const char *sig = nullptr;
			std::string ssig = "";

			if(typeid(T) == typeid(int8_t))
			{
				sig = DBUS_TYPE_BYTE_AS_STRING;
			}
			else if(typeid(T) == typeid(uint8_t))
			{
				sig = DBUS_TYPE_BYTE_AS_STRING;
			}
		    else if(typeid(T) == typeid(int16_t))
			{
				sig = DBUS_TYPE_INT16_AS_STRING;
			}
			else if(typeid(T) == typeid(uint16_t))
			{
				sig = DBUS_TYPE_UINT16_AS_STRING;
			}
			else if(typeid(T) == typeid(int32_t))
			{
				sig = DBUS_TYPE_INT32_AS_STRING;
			}
			else if(typeid(T) == typeid(uint32_t))
			{
				sig = DBUS_TYPE_UINT32_AS_STRING;
			}
			else if(typeid(T) == typeid(int64_t))
			{
				sig = DBUS_TYPE_INT64_AS_STRING;
			}
			else if(typeid(T) == typeid(uint64_t))
			{
				sig = DBUS_TYPE_UINT64_AS_STRING;
			}
			else if(typeid(T) == typeid(double))
			{
				sig = DBUS_TYPE_DOUBLE_AS_STRING;
			}
			else if(typeid(T) == typeid(bool))
			{
				sig = DBUS_TYPE_BOOLEAN_AS_STRING;
			}
			else if(typeid(T) == typeid(std::string))
			{
				sig = DBUS_TYPE_STRING_AS_STRING;
			}
			else
			{
				signature<T>(ssig);
				sig = ssig.c_str();
			}

			if (!dbus_message_iter_open_container(&_iter, DBUS_TYPE_ARRAY, sig, &subIter))
			{
				LOGE(TAG, "open container failed!");
				return -1;
			}

			for (auto &t : _vt)
			{
				if(0 != arg_in(subIter, t))
				{
					LOGE(TAG, "append failed!");
					dbus_message_iter_abandon_container (&_iter, &subIter);
					return -1;
				}		
			}

			if (!dbus_message_iter_close_container(&_iter, &subIter))
			{
				LOGE(TAG, "close container failed!");
				return -1;
			}

			return 0;
		};

		template <typename T>
		int arg_out(DBusMessageIter &_iter, T &_t)
		{
			LOGD(TAG, "type is %s", typeid(T).name());
			
			int type = dbus_message_iter_get_arg_type(&_iter);

			if(DBUS_TYPE_STRUCT != type)
			{
				LOGE(TAG, "argument type %d is not DBUS_TYPE_STRUCT!", type);
				return -1;	
			}

			DBusMessageIter subIter;

			dbus_message_iter_recurse(&_iter, &subIter);
			unpack(subIter, _t);

			if(!dbus_message_iter_next(&_iter))
			{
				LOGD(TAG, "iterator reach the end!");
				return -1;
			}

			return 0;
		}

		template <typename T>
		int arg_out(DBusMessageIter &_iter, std::vector<T> &_vt)
		{
			LOGD(TAG, "type is std::vector<%s>, size = %ld", typeid(T).name(), _vt.size());

			int type = dbus_message_iter_get_arg_type(&_iter);

			if(DBUS_TYPE_ARRAY != type)
			{
				LOGE(TAG, "argument type %d is not DBUS_TYPE_ARRAY!", type);
				return -1;	
			}

			DBusMessageIter subIter;
			int ret = 0;

			dbus_message_iter_recurse(&_iter, &subIter);

			while(0 == ret)
			{
				T t;
				ret = arg_out(subIter, t);
				_vt.push_back(t);
			}

			if(!dbus_message_iter_next(&_iter))
			{
				LOGD(TAG, "iterator reach the end!");
			}

			return 0;
		};

	private:
		const char     *TAG = "dbus_helper";
		DBusConnection *conn_;
	};
} // namespace ipc

#endif // __DBUS_HELPER_H__
