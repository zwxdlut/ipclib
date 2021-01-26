#ifndef __DBUS_HELPER_H__
#define __DBUS_HELPER_H__

#define DBUS_API_SUBJECT_TO_CHANGE

#include <inttypes.h>
#include <vector>
#include <string>
#include <typeinfo>
#include <initializer_list>

#include <dbus/dbus.h>

#include "log.h"

namespace ipc
{
	class dbus_helper
	{
	public:
		struct serializable
		{
			virtual void signature(std::string &_sig) = 0;

			virtual int pack(dbus_helper *_dbus, DBusMessageIter &_iter) const = 0;

			virtual int unpack(dbus_helper *_dbus, DBusMessageIter &_iter) = 0;
		};

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
			if (nullptr == _msg)
			{
				LOGE(TAG, "The message is null!");
				return -1;
			}

			LOGD(TAG, "+");

			DBusMessageIter iter;

			dbus_message_iter_init_append(_msg, &iter);
			std::initializer_list<int> { (arg_in(iter, _args), 0)... };

			LOGD(TAG, "-");

			return 0;

		};

		template <typename... ARGS>
		int unpack_args(DBusMessage *_msg, ARGS&... _args)
		{
			if (nullptr == _msg)
			{
				LOGE(TAG, "The message is null!");
				return -1;
			}

			LOGD(TAG, "+");

			DBusMessageIter iter;

			if(!dbus_message_iter_init(_msg, &iter))
			{
				LOGE(TAG, "initialize the message iterator failed!");
				return -1;
			}

			std::initializer_list<int>{(arg_out(iter, _args), 0)...};

			LOGD(TAG, "-");

			return 0;
		}

		template <typename T>
		int arg_in(DBusMessageIter &_iter, const T &_t)
		{
			if(!std::is_base_of<serializable, T>::value)
			{
				LOGE(TAG, "Type %s is not serialized!", typeid(T).name());
				return -1;
			}

			LOGD(TAG, "type = %s", typeid(T).name());

			DBusMessageIter subIter;

			if (!dbus_message_iter_open_container(&_iter, DBUS_TYPE_STRUCT, 0, &subIter))
			{
				LOGE(TAG, "open container failed!");
				return -1;		
			}

			if (0 != static_cast<const serializable &>(_t).pack(this, subIter))
			{
				LOGE(TAG, "pack type %s failed!", typeid(T).name());
				dbus_message_iter_abandon_container(&_iter, &subIter);
				return -1;
			}

			if (!dbus_message_iter_close_container(&_iter, &subIter))
			{
				LOGE(TAG, "close container failed!");
				return -1;
			}

			return 0;
		}

		template <typename T>
		int arg_in(DBusMessageIter &_iter, const std::vector<T> &_vt)
		{
			LOGD(TAG, "type = std::vector<%s>, size = %ld", typeid(T).name(), _vt.size());

			DBusMessageIter subIter;
			const char *sig = nullptr;
			std::string ssig = "";

			if (typeid(T) == typeid(int8_t))
			{
				sig = DBUS_TYPE_BYTE_AS_STRING;
			}
			else if (typeid(T) == typeid(uint8_t))
			{
				sig = DBUS_TYPE_BYTE_AS_STRING;
			}
		    else if (typeid(T) == typeid(int16_t))
			{
				sig = DBUS_TYPE_INT16_AS_STRING;
			}
			else if(typeid(T) == typeid(uint16_t))
			{
				sig = DBUS_TYPE_UINT16_AS_STRING;
			}
			else if (typeid(T) == typeid(int32_t))
			{
				sig = DBUS_TYPE_INT32_AS_STRING;
			}
			else if (typeid(T) == typeid(uint32_t))
			{
				sig = DBUS_TYPE_UINT32_AS_STRING;
			}
			else if (typeid(T) == typeid(int64_t))
			{
				sig = DBUS_TYPE_INT64_AS_STRING;
			}
			else if (typeid(T) == typeid(uint64_t))
			{
				sig = DBUS_TYPE_UINT64_AS_STRING;
			}
			else if (typeid(T) == typeid(double))
			{
				sig = DBUS_TYPE_DOUBLE_AS_STRING;
			}
			else if (typeid(T) == typeid(bool))
			{
				sig = DBUS_TYPE_BOOLEAN_AS_STRING;
			}
			else if (typeid(T) == typeid(std::string))
			{
				sig = DBUS_TYPE_STRING_AS_STRING;
			}
			else if (std::is_base_of<serializable, T>::value)
			{
				T t;
				// TODO: static_cast from T to serializable will be failed during template deduction
				((serializable &)t).signature(ssig);
				sig = ssig.c_str();
			}
			else
			{
				LOGE(TAG, "Type %s is not serialized!", typeid(T).name());
				return -1;
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
		}

		template <typename T>
		int arg_out(DBusMessageIter &_iter, T &_t)
		{
			if (!std::is_base_of<serializable, T>::value)
			{
				// TODO: static_cast from T to serializable will be failed during template deduction
				LOGE(TAG, "Type %s is not serialized!", typeid(T).name());
				return -1;
			}

			LOGD(TAG, "type = %s", typeid(T).name());	
			
			int type = dbus_message_iter_get_arg_type(&_iter);

			if(DBUS_TYPE_STRUCT != type)
			{
				LOGE(TAG, "Argument type %d is not DBUS_TYPE_STRUCT!", type);
				return -1;	
			}

			DBusMessageIter subIter;

			dbus_message_iter_recurse(&_iter, &subIter);

			if (0 != ((serializable &)_t).unpack(this, subIter))
			{
				// TODO: static_cast from T to serializable will be failed during template deduction
				LOGE(TAG, "unpack type %s failed!", typeid(T).name());
				return -1;
			}
			
			if(!dbus_message_iter_next(&_iter))
			{
				LOGE(TAG, "The iterator reaches the end!");
				return -1;
			}

			return 0;
		}

		template <typename T>
		int arg_out(DBusMessageIter &_iter, std::vector<T> &_vt)
		{
			LOGD(TAG, "type = std::vector<%s>, size = %ld", typeid(T).name(), _vt.size());

			int type = dbus_message_iter_get_arg_type(&_iter);

			if(DBUS_TYPE_ARRAY != type)
			{
				LOGE(TAG, "Argument type %d is not DBUS_TYPE_ARRAY!", type);
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
				LOGD(TAG, "The iterator reaches the end!");
			}

			return 0;
		}

	private:
		const char     *TAG = "ipc::dbus_helper";
		DBusConnection *conn_;
	};

	/**
	 * Function template specialization.
	 */
	template <>
	inline int dbus_helper::pack_args(DBusMessage *_msg)
	{
		if (nullptr == _msg)
		{
			LOGE(TAG, "The message is null!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::unpack_args(DBusMessage *_msg)
	{
		if (nullptr == _msg)
		{
			LOGE(TAG, "The message is null!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const int8_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_BYTE, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_BYTE failed!"); 
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const uint8_t &_t)
	{
		return arg_in(_iter, (const int8_t &)_t);
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const int16_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_INT16, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_INT16 failed!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const uint16_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_UINT16, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_UINT16 failed!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const int32_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_INT32, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_INT32 failed!"); 
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const uint32_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_UINT32, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_UINT32 failed!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const int64_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_INT64, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_INT64 failed!"); 
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const uint64_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_UINT64, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_UINT64 failed!"); 
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const double &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_DOUBLE, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_DOUBLE failed!"); 
			return -1;
		}
		
		return 0;
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const bool &_t)
	{
		dbus_bool_t b = _t;

		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_BOOLEAN, &b))
		{
			LOGE(TAG, "append DBUS_TYPE_BOOLEAN failed!"); 
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_in(DBusMessageIter &_iter, const std::string &_t)
	{
		char *str = (char *)_t.c_str();

		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_STRING, &str))
		{
			LOGE(TAG, "append DBUS_TYPE_STRING failed!");  
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, int8_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_BYTE != type)
		{
			LOGE(TAG, "Argument type %d is not DBUS_TYPE_BYTE!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "The iterator reaches the end!");
			return -1;
		}		
		
		return 0;	
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, uint8_t &_t)
	{
		return arg_out(_iter, (int8_t &)_t);
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, int16_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_INT16 != type)
		{
			LOGE(TAG, "Argument type %d is not DBUS_TYPE_INT16!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "The iterator reaches the end!");
			return -1;
		}		
		
		return 0;	
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, uint16_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_UINT16 != type)
		{
			LOGE(TAG, "Argument type %d is not DBUS_TYPE_UINT16!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "The iterator reaches the end!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, int32_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_INT32 != type)
		{
			LOGE(TAG, "Argument type %d is not DBUS_TYPE_INT32!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "The iterator reaches the end!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, uint32_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_UINT32 != type)
		{
			LOGE(TAG, "Argument type %d is not DBUS_TYPE_UINT32!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "The iterator reaches the end!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, int64_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_INT64 != type)
		{
			LOGE(TAG, "Argument type %d is not DBUS_TYPE_INT64!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %" PRId64"", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "The iterator reaches the end!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, uint64_t &_t)
	{
		int type =dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_UINT64 != type)
		{
			LOGE(TAG, "Argument type %d is not DBUS_TYPE_UINT64!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %" PRIu64"", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "The iterator reaches the end!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, double &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_DOUBLE != type)
		{
			LOGE(TAG, "Argument type %d is not DBUS_TYPE_DOUBLE!", type);
			return -1;	

		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %lf", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "The iterator reaches the end!");
			return -1;
		}

		return 0;
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, bool &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_BOOLEAN != type)
		{
			LOGE(TAG, "Argument type %d is not DBUS_TYPE_BOOLEAN!", type);
			return -1;
		}

		dbus_bool_t t = _t;

		dbus_message_iter_get_basic(&_iter, &t);
		_t = t;
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "The iterator reaches the end!");
			return -1;
		}
			
		return 0;
	}

	template <>
	inline int dbus_helper::arg_out(DBusMessageIter &_iter, std::string &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_STRING != type)
		{
			LOGE(TAG, "Argument type %d is not DBUS_TYPE_STRING!", type);
			return -1;	
		}

		char *str = nullptr;

		dbus_message_iter_get_basic(&_iter, &str);
		_t = std::string(str);
		LOGD(TAG, "argument type = %d, value = %s", type, _t.c_str());

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "The iterator reaches the end!");
			return -1;
		}
			
		return 0;
	}
} // namespace ipc

#endif // __DBUS_HELPER_H__
