#pragma once
#include "libretro.h"
#include "../Core/IMessageManager.h"
#include "../Core/MessageManager.h"

class LibretroMessageManager : public IMessageManager
{
private:
	retro_log_printf_t _log = nullptr;
	retro_environment_t _retroEnv = nullptr;
	unsigned _messageInterfaceVersion = 0;

	//Bit 3 (hard disable audio) marks the copy of the core that run-ahead runs, which would repeat every message
	bool IsRunAheadCopy()
	{
		int avEnable = 0;
		return _retroEnv(RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE, &avEnable) && (avEnable & 8);
	}

	void SetMessageExt(const string &message, retro_message_type type, int8_t progress, unsigned duration)
	{
		retro_message_ext msg = {};
		msg.msg = message.c_str();
		msg.duration = duration;
		msg.priority = 1;
		msg.level = RETRO_LOG_INFO;
		msg.target = RETRO_MESSAGE_TARGET_OSD;
		msg.type = type;
		msg.progress = progress;
		_retroEnv(RETRO_ENVIRONMENT_SET_MESSAGE_EXT, &msg);
	}

public:
	LibretroMessageManager(retro_log_printf_t logCallback, retro_environment_t retroEnv)
	{
		_log = logCallback;
		_retroEnv = retroEnv;
		if(!_retroEnv(RETRO_ENVIRONMENT_GET_MESSAGE_INTERFACE_VERSION, &_messageInterfaceVersion))
		{
			_messageInterfaceVersion = 0;
		}
		MessageManager::RegisterMessageManager(this);
	}

	virtual ~LibretroMessageManager()
	{
		MessageManager::RegisterMessageManager(nullptr);
	}

	// Inherited via IMessageManager
	virtual void DisplayMessage(string title, string message) override
	{
		if(title.empty())
		{
			if(_log)
				_log(RETRO_LOG_INFO, message.c_str());
		}
		else if(!IsRunAheadCopy())
		{
			string osdMessage = "[" + title + "] " + message;
			retro_message msg = { osdMessage.c_str(), 180 };
			_retroEnv(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
		}
	}

	void DisplayProgressMessage(string message, int8_t progress, unsigned duration, bool progressUpdate)
	{
		if(IsRunAheadCopy())
		{
			return;
		}

		if(_messageInterfaceVersion >= 1)
		{
			SetMessageExt(message, RETRO_MESSAGE_TYPE_PROGRESS, progress, duration);
		}
		else if(!progressUpdate)
		{
			retro_message msg = { message.c_str(), 120 };
			_retroEnv(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
		}
	}

	void DisplayTextMessage(string message, unsigned duration)
	{
		if(IsRunAheadCopy())
		{
			return;
		}

		if(_messageInterfaceVersion >= 1)
		{
			SetMessageExt(message, RETRO_MESSAGE_TYPE_NOTIFICATION_ALT, 0, duration);
		}
		else
		{
			retro_message msg = { message.c_str(), duration * 60 / 1000 };
			_retroEnv(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
		}
	}
};
