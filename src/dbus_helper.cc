#include <dbus/dbus.h>

#include "dbus_helper.h"

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

		// initialize the errors
		dbus_error_init(&err);

		// connect to the system bus and check for errors
		conn_ = dbus_bus_get(DBUS_BUS_SESSION, &err);

		if (dbus_error_is_set(&err)) 
		{ 
			LOGE(TAG, "connection error(%s)!", err.message);
			dbus_error_free(&err);
		}
		
		if (nullptr == conn_) 
		{ 
			return -1;
		}

		if (!_name.empty())
		{
			// request our connection on the bus
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
		LOGD(TAG, "");

		if (nullptr != conn_)
		{
			dbus_connection_unref(conn_);
			conn_ = nullptr;
		}

		return 0;
	}

	DBusMessage* dbus_helper::create_method_call(const std::string &_conn, const std::string &_path, const std::string &_iface, const std::string &_method)
	{
		LOGD(TAG, "conn = %s, path = %s, iface = %s, method = %s", _conn.c_str(), _path.c_str(), _iface.c_str(), _method.c_str());
		
		// create a new method call and check for errors
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
		
		// create a signal and check for errors
		DBusMessage* msg = dbus_message_new_signal(
			_path.c_str(),    // object name of the signal
			_iface.c_str(),   // interface name of the signal
			_signal.c_str()); // name of the signal
		
		if (nullptr == msg)
		{ 
			LOGE(TAG, "Message is null!");
		}

		return msg;
	}

	DBusMessage* dbus_helper::create_reply(DBusMessage *_msg)
	{
		if (nullptr == _msg)
		{
			LOGE(TAG, "Message is null!");
			return nullptr;
		}

		LOGD(TAG, "");

		// create a reply from the message
		DBusMessage *reply = dbus_message_new_method_return(_msg);

		if (nullptr == reply) 
		{ 
			LOGE(TAG, "create reply failed!");
		}

		return reply;
	}

	DBusMessage* dbus_helper::pop()
	{
		if (nullptr == conn_)
		{
			LOGE(TAG, "Connection is null!");
			return nullptr;
		}

		// non-blocking read of the next available message
		dbus_connection_read_write(conn_, 0);

		return dbus_connection_pop_message(conn_);
	}

	int dbus_helper::free(DBusMessage *_msg)
	{
		if (nullptr != _msg)
		{
			// free the message
			dbus_message_unref(_msg);		
		}

		return 0;
	}

	int dbus_helper::send(DBusMessage *_msg)
	{	
		if (nullptr == conn_)
		{
			LOGE(TAG, "Connection is null!");
			return -1;
		}

		if (nullptr == _msg)
		{
			LOGE(TAG, "Message is null!");
			return -1;
		}

		LOGD(TAG, "+");

		// send the message and flush the connection
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
		if (nullptr == conn_)
		{
			LOGE(TAG, "Connection is null!");
			return -1;
		}

		if (nullptr == _msg)
		{
			LOGE(TAG, "Message is null!");
			return -1;
		}

	    LOGD(TAG, "+");

		DBusPendingCall *pending = nullptr;

		// send message and get a handle for a reply, -1 is default timeout
		if (!dbus_connection_send_with_reply(conn_, _msg, &pending, -1))
		{ 
			LOGE(TAG, "out of memory!");  
			return -1;
		}

		if (nullptr == pending) 
		{ 
			LOGE(TAG, "Pending call is null!"); 
			return -1; 
		}

		dbus_connection_flush(conn_);

		// free message
		dbus_message_unref(_msg);

		// block until we recieve a reply
		dbus_pending_call_block(pending);

		// get the reply message
		_msg = dbus_pending_call_steal_reply(pending);
		
		if (nullptr == _msg) 
		{
			LOGE(TAG, "Reply is null!"); 
			return -1; 
		}

		// free the pending message handle
		dbus_pending_call_unref(pending);

		LOGD(TAG, "-"); 

		return 0;
	}

	int dbus_helper::add_match(const std::string &_rule)
	{
		if (nullptr == conn_)
		{
			LOGE(TAG, "Connection is null!");
			return -1;
		}

		LOGD(TAG, "rule = %s", _rule.c_str());

		DBusError err;
		
		// initialize the error
		dbus_error_init(&err);

		// add a rule for which messages we want to see
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
		if (nullptr == conn_)
		{
			LOGE(TAG, "Connection is null!");
			return -1;
		}

		LOGD(TAG, "rule = %s", _rule.c_str());
		
		DBusError err;
		
		// initialize the error
		dbus_error_init(&err);

		// remove a rule from which messages we want to see
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
			LOGE(TAG, "Message is null!");
			return false;
		}

		return dbus_message_has_path(_msg, _path.c_str());
	}

	bool dbus_helper::is_method_call(DBusMessage *_msg, const std::string &_iface, const std::string &_member)
	{
		if (nullptr == _msg) 
		{
			LOGE(TAG, "Message is null!");
			return false;
		}

		return dbus_message_is_method_call(_msg, _iface.c_str(), _member.c_str());
	}

	bool dbus_helper::is_signal(DBusMessage *_msg, const std::string &_iface, const std::string &_member)
	{
		if (nullptr == _msg) 
		{
			LOGE(TAG, "Message is null!");
			return false;
		}
		
		return dbus_message_is_signal(_msg, _iface.c_str(), _member.c_str());
	}	
} // namespace ipc



