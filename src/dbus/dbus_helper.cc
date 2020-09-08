#include <dbus/dbus.h>

#include "../../inc/dbus/dbus_helper.h"

namespace ipc
{
	dbus_helper::dbus_helper()
	{
		conn_ = nullptr;
	};

	dbus_helper::~dbus_helper()
	{
		disconnect();
	}

	int dbus_helper::connect(const std::string &_name)
	{
		LOGD(TAG, "name = %s", _name.c_str());

		DBusError err;

		// Initialize the errors
		dbus_error_init(&err);

		// Connect to the system bus and check for errors
		conn_ = dbus_bus_get(DBUS_BUS_SESSION, &err);
		if (dbus_error_is_set(&err)) 
		{ 
			LOGE(TAG, "connection error(%s)!", err.message);
			dbus_error_free(&err);
		}
		
		if (nullptr == conn_) { 
			return -1;
		}

		if(!_name.empty()){
			// Request our connection on the bus
			int ret = dbus_bus_request_name(conn_, _name.c_str(), DBUS_NAME_FLAG_REPLACE_EXISTING , &err);

			if (dbus_error_is_set(&err)) 
			{ 
				LOGE(TAG, "connection error(%s)!", err.message);
				dbus_error_free(&err);
			}

			if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) 
			{ 
				return -1;
			}
		}
			
		return 0;
	}

	int dbus_helper::disconnect()
	{
		if(nullptr != conn_)
		{
			dbus_connection_unref(conn_);
			conn_ = nullptr;
		}

		return 0;
	}

	DBusMessage* dbus_helper::create_method_call(const std::string &_conn, const std::string &_path, const std::string &_iface, const std::string &_method)
	{
		LOGD(TAG, "conn = %s, path = %s, iface = %s, method = %s", _conn.c_str(), _path.c_str(), _iface.c_str(), _method.c_str());
		
		// Create a new method call and check for errors
		DBusMessage *msg = dbus_message_new_method_call(
			_conn.c_str(),    // target for the method call
			_path.c_str(),    // object to call on
			_iface.c_str(),   // interface to call on
			_method.c_str()); // method name
		
		if (nullptr == msg) 
		{ 
			LOGE(TAG, "create method failed!");
		}

		return msg;
	}

	DBusMessage* dbus_helper::create_signal(const std::string &_path, const std::string &_iface, const std::string &_signal)
	{
		LOGD(TAG, "path = %s, iface = %s, signal = %s", _path.c_str(), _iface.c_str(), _signal.c_str());
		
		// Create a signal and check for errors
		DBusMessage* msg = dbus_message_new_signal(
			_path.c_str(),    // object name of the signal
			_iface.c_str(),   // interface name of the signal
			_signal.c_str()); // name of the signal
		
		if (nullptr == msg)
		{ 
			LOGE(TAG, "message is null!");
		}

		return msg;
	}

	DBusMessage* dbus_helper::create_reply(DBusMessage *_msg)
	{
		LOGD(TAG, "");

		if(nullptr == _msg)
		{
			LOGE(TAG, "message is null!");
			return nullptr;
		}

		// Create a reply from the message
		DBusMessage *reply = dbus_message_new_method_return(_msg);

		if (nullptr == reply) 
		{ 
			LOGE(TAG, "create reply failed!");
		}

		return reply;
	}

	DBusMessage* dbus_helper::pop()
	{
		if(nullptr == conn_)
		{
			LOGE(TAG, "connection is null!");
			return nullptr;
		}

		// Non-blocking read of the next available message
		dbus_connection_read_write(conn_, 0);
		
		return dbus_connection_pop_message(conn_);
	}

	int dbus_helper::free(DBusMessage *_msg)
	{
		if(nullptr != _msg)
		{
			// Free the message
			dbus_message_unref(_msg);		
		}

		return 0;
	}

	int dbus_helper::send(DBusMessage *_msg)
	{
		LOGD(TAG, "+"); 
		
		if(nullptr == conn_)
		{
			LOGE(TAG, "connection is null!");
			return -1;
		}

		if(nullptr == _msg)
		{
			LOGE(TAG, "message is null!");
			return -1;
		}

		// Send the message and flush the connection
		if (!dbus_connection_send(conn_, _msg, 0)) 
		{
			LOGE(TAG, "out of memory!"); 
			return -1;
		}

		dbus_connection_flush(conn_);
			
		LOGD(TAG, "-"); 

		return 0;
	}

	int dbus_helper::send_with_reply(DBusMessage *&_msg)
	{
		LOGD(TAG, "");

		if(nullptr == conn_)
		{
			LOGE(TAG, "connection is null!");
			return -1;
		}

		if(nullptr == _msg)
		{
			LOGE(TAG, "message is null!");
			return -1;
		}

		DBusPendingCall *pending = nullptr;

		/* Send message and get a handle for a reply, 
		-1 is default timeout */
		if (!dbus_connection_send_with_reply(conn_, _msg, &pending, -1))
		{ 
			LOGE(TAG, "out of memory!");  
			return -1;
		}

		if (nullptr == pending) 
		{ 
			LOGE(TAG, "pending call is null!"); 
			return -1; 
		}

		dbus_connection_flush(conn_);

		// Free message
		dbus_message_unref(_msg);

		// Block until we recieve a reply
		dbus_pending_call_block(pending);

		// Get the reply message
		_msg = dbus_pending_call_steal_reply(pending);
		
		if (nullptr == _msg) 
		{
			LOGE(TAG, "reply is null!"); 
			return -1; 
		}

		// Free the pending message handle
		dbus_pending_call_unref(pending);

		LOGD(TAG, "-"); 

		return 0;
	}

	int dbus_helper::add_match(const std::string &_rule)
	{
		LOGD(TAG, "rule = %s", _rule.c_str());

		if(nullptr == conn_)
		{
			LOGE(TAG, "connection is null!");
			return -1;
		}
		
		DBusError err;
		
		// Initialize the error
		dbus_error_init(&err);
		
		// Add a rule for which messages we want to see
		dbus_bus_add_match(conn_, _rule.c_str(), &err);
		dbus_connection_flush(conn_);

		if (dbus_error_is_set(&err))
		{ 
			LOGE(TAG, "match error(%s)!", err.message);
			return -1;
		}
		
		return 0;
	}

	int dbus_helper::remove_match(const std::string &_rule)
	{
		LOGD(TAG, "rule = %s", _rule.c_str());
		
		if(nullptr == conn_)
		{
			LOGE(TAG, "connection is null!");
			return -1;
		}
		
		DBusError err;
		
		// Initialize the error
		dbus_error_init(&err);
		
		// Remove a rule from which messages we want to see
		dbus_bus_remove_match(conn_, _rule.c_str(), &err);
		dbus_connection_flush(conn_);

		if (dbus_error_is_set(&err))
		{ 
			LOGE(TAG, "remove error(%s)!", err.message);
			return -1;
		}
		
		return 0;
	}

	bool dbus_helper::has_path(DBusMessage *_msg, const std::string &_path)
	{
		if (nullptr == _msg) 
		{
			LOGE(TAG, "message is null!");
			return false;
		}

		return dbus_message_has_path(_msg, _path.c_str());
	}

	bool dbus_helper::is_method_call(DBusMessage *_msg, const std::string &_iface, const std::string &_member)
	{
		if (nullptr == _msg) 
		{
			LOGE(TAG, "message is null!");
			return false;
		}

		return dbus_message_is_method_call(_msg, _iface.c_str(), _member.c_str());
	}

	bool dbus_helper::is_signal(DBusMessage *_msg, const std::string &_iface, const std::string &_member)
	{
		if (nullptr == _msg) 
		{
			LOGE(TAG, "message is null!");
			return false;
		}
		
		return dbus_message_is_signal(_msg, _iface.c_str(), _member.c_str());
	}

	/**
	 * Function template specialization.
	 */
	template <>
	int dbus_helper::pack_args(DBusMessage *_msg)
	{
		if (nullptr == _msg)
		{
			LOGE(TAG, "message is null");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::unpack_args(DBusMessage *_msg)
	{
		if (nullptr == _msg)
		{
			LOGE(TAG, "message is null");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_in(DBusMessageIter &_iter, const int8_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_BYTE, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_BYTE failed!"); 
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_in(DBusMessageIter &_iter, const uint8_t &_t)
	{
		return arg_in(_iter, (const int8_t &)_t);
	}

	template <>
	int dbus_helper::arg_in(DBusMessageIter &_iter, const int16_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_INT16, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_INT16 failed!");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_in(DBusMessageIter &_iter, const uint16_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_UINT16, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_UINT16 failed!");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_in(DBusMessageIter &_iter, const int32_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_INT32, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_INT32 failed!"); 
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_in(DBusMessageIter &_iter, const uint32_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_UINT32, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_UINT32 failed!");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_in(DBusMessageIter &_iter, const int64_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_INT64, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_INT64 failed!"); 
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_in(DBusMessageIter &_iter, const uint64_t &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_UINT64, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_UINT64 failed!"); 
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_in(DBusMessageIter &_iter, const double &_t)
	{
		if (!dbus_message_iter_append_basic(&_iter, DBUS_TYPE_DOUBLE, &_t))
		{
			LOGE(TAG, "append DBUS_TYPE_DOUBLE failed!"); 
			return -1;
		}
		
		return 0;
	}

	template <>
	int dbus_helper::arg_in(DBusMessageIter &_iter, const bool &_t)
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
	int dbus_helper::arg_in(DBusMessageIter &_iter, const std::string &_t)
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
	int dbus_helper::arg_out(DBusMessageIter &_iter, int8_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_BYTE != type)
		{
			LOGE(TAG, "argument type %d is not DBUS_TYPE_BYTE!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "iterator reach the end!");
			return -1;
		}		
		
		return 0;	
	}

	template <>
	int dbus_helper::arg_out(DBusMessageIter &_iter, uint8_t &_t)
	{
		return arg_out(_iter, (int8_t &)_t);
	}

	template <>
	int dbus_helper::arg_out(DBusMessageIter &_iter, int16_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_INT16 != type)
		{
			LOGE(TAG, "argument type %d is not DBUS_TYPE_INT16!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "iterator reach the end!");
			return -1;
		}		
		
		return 0;	
	}

	template <>
	int dbus_helper::arg_out(DBusMessageIter &_iter, uint16_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_UINT16 != type)
		{
			LOGE(TAG, "argument type %d is not DBUS_TYPE_UINT16!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "iterator reach the end!");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_out(DBusMessageIter &_iter, int32_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_INT32 != type)
		{
			LOGE(TAG, "argument type %d is not DBUS_TYPE_INT32!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "iterator reach the end!");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_out(DBusMessageIter &_iter, uint32_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_UINT32 != type)
		{
			LOGE(TAG, "argument type %d is not DBUS_TYPE_UINT32!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "iterator reach the end!");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_out(DBusMessageIter &_iter, int64_t &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_INT64 != type)
		{
			LOGE(TAG, "argument type %d is not DBUS_TYPE_INT64!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %" PRId64"", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "iterator reach the end!");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_out(DBusMessageIter &_iter, uint64_t &_t)
	{
		int type =dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_UINT64 != type)
		{
			LOGE(TAG, "argument type %d is not DBUS_TYPE_UINT64!", type);
			return -1;	
		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %" PRIu64"", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "iterator reach the end!");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_out(DBusMessageIter &_iter, double &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_DOUBLE != type)
		{
			LOGE(TAG, "argument type %d is not DBUS_TYPE_DOUBLE!", type);
			return -1;	

		}

		dbus_message_iter_get_basic(&_iter, &_t);
		LOGD(TAG, "argument type = %d, value = %lf", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "iterator reach the end!");
			return -1;
		}

		return 0;
	}

	template <>
	int dbus_helper::arg_out(DBusMessageIter &_iter, bool &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_BOOLEAN != type)
		{
			LOGE(TAG, "argument type %d is not DBUS_TYPE_BOOLEAN!", type);
			return -1;
		}

		dbus_bool_t t = _t;

		dbus_message_iter_get_basic(&_iter, &t);
		_t = t;
		LOGD(TAG, "argument type = %d, value = %d", type, _t);

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "iterator reach the end!");
			return -1;
		}
			
		return 0;
	}

	template <>
	int dbus_helper::arg_out(DBusMessageIter &_iter, std::string &_t)
	{
		int type = dbus_message_iter_get_arg_type(&_iter);

		if(DBUS_TYPE_STRING != type)
		{
			LOGE(TAG, "argument type %d is not DBUS_TYPE_STRING!", type);
			return -1;	
		}

		char *str = nullptr;

		dbus_message_iter_get_basic(&_iter, &str);
		_t = std::string(str);
		LOGD(TAG, "argument type = %d, value = %s", type, _t.c_str());

		if(!dbus_message_iter_next(&_iter))
		{
			LOGD(TAG, "iterator reach the end!");
			return -1;
		}
			
		return 0;
	}	
} // namespace ipc



