#include "stdafx.h"
#include "MessageManager.h"
#include "EmulationSettings.h"

std::list<string> MessageManager::_log;
bool MessageManager::_osdEnabled = false;
IMessageManager* MessageManager::_messageManager = nullptr;

void MessageManager::RegisterMessageManager(IMessageManager* messageManager)
{
	MessageManager::_messageManager = messageManager;
}

void MessageManager::UnregisterMessageManager(IMessageManager* messageManager)
{
	if(MessageManager::_messageManager == messageManager)
		MessageManager::_messageManager = nullptr;
}

void MessageManager::SetOsdState(bool enabled)
{
	_osdEnabled = enabled;
}

void MessageManager::DisplayMessage(string title, string message, string param1, string param2)
{
	if(MessageManager::_messageManager) {
		if(!MessageManager::_messageManager) {
			return;
		}

		size_t startPos = message.find(u8"%1");
		if(startPos != std::string::npos)
			message.replace(startPos, 2, param1);

		startPos = message.find(u8"%2");
		if(startPos != std::string::npos)
			message.replace(startPos, 2, param2);

		if(_osdEnabled) {
			MessageManager::_messageManager->DisplayMessage(title, message);
		} else {
			MessageManager::Log("[" + title + "] " + message);
		}
	}
}

void MessageManager::Log(string message)
{
	if(MessageManager::_messageManager)
		MessageManager::_messageManager->DisplayMessage("", message + "\n");
}
