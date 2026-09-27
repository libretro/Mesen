#pragma once
#include "libretro.h"
#include "../Core/IKeyManager.h"
#include "../Core/KeyManager.h"
#include "../Core/FdsSystemActionManager.h"
#include "../Core/VsSystemActionManager.h"
#include "../Core/Console.h"

class LibretroKeyManager : public IKeyManager
{
private:
	std::shared_ptr<Console> _console;
	retro_input_state_t _getInputState = nullptr;
	int16_t _joypadButtons[5] = { };
	retro_input_poll_t _pollInput = nullptr;
	bool _mouseButtons[3] = { false, false, false };
	bool _supportsInputBitmasks = false;
	bool _wasPushed[16] = { };
	uint32_t _currentDisk = 0;

	bool ProcessAction(uint32_t button)
	{
		bool buttonPressed = false;
		if (_supportsInputBitmasks)
			buttonPressed = (_joypadButtons[0] & (1 << button)) != 0;
		else
			buttonPressed = _getInputState(0, RETRO_DEVICE_JOYPAD, 0, button) != 0;

		if(buttonPressed) {
			if(!_wasPushed[button]) {
				//Newly pressed, process action
				_wasPushed[button] = true;
				return true;
			}
		} else {
			_wasPushed[button] = false;
		}
		return false;
	}

	void ReportDiskChange()
	{
		if (_currentDisk & 0x100) {
			FDS* fds = dynamic_cast<FDS*>(_console->GetMapper());
			if(fds) {
				const uint32_t disk = fds->GetCurrentDisk();
				if(disk != 0xFF && disk != (_currentDisk & 0xFF)) {
					_currentDisk = disk;
					MessageManager::DisplayMessage("FDS", "Inserted disk " + std::to_string((disk / 2) + 1) + ((disk & 1) ? " Side B" : " Side A"));
				}
			}
		}
	}

public:
	LibretroKeyManager(std::shared_ptr<Console> console)
	{
		_console = console;
		KeyManager::RegisterKeyManager(this);
	}

	~LibretroKeyManager()
	{
		KeyManager::RegisterKeyManager(nullptr);
	}

	void SetGetInputState(retro_input_state_t getInputState)
	{
		_getInputState = getInputState;
	}

	void SetPollInput(retro_input_poll_t pollInput)
	{
		_pollInput = pollInput;
	}

	void SetSupportsInputBitmasks(bool supportsInputBitmasks)
	{
		_supportsInputBitmasks = supportsInputBitmasks;
	}

	// Inherited via IKeyManager
	virtual void RefreshState() override
	{
		if(_pollInput) {
			_pollInput();
		}

		if(_getInputState) {
			int32_t x = _getInputState(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
			int32_t y = _getInputState(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);

			x += 0x8000;
			y += 0x8000;

			KeyManager::SetMousePosition((double)x / 0x10000, (double)y / 0x10000);

			int16_t dx = _getInputState(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
			int16_t dy = _getInputState(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
			KeyManager::SetMouseMovement(dx, dy);

			if (_supportsInputBitmasks)
				for (int16_t port = 0; port < 5; port++)
					_joypadButtons[port] = _getInputState(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
			
			_mouseButtons[(int)MouseButton::LeftButton] = _getInputState(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT) != 0;
			_mouseButtons[(int)MouseButton::RightButton] = _getInputState(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT) != 0;
			_mouseButtons[(int)MouseButton::MiddleButton] = _getInputState(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE) != 0;

			std::shared_ptr<FdsSystemActionManager> fdsSam = _console->GetSystemActionManager<FdsSystemActionManager>();
			if(fdsSam) {
				if(ProcessAction(RETRO_DEVICE_ID_JOYPAD_L)) {
					if(!(_currentDisk & 0x100)) { // ignore until previous disk switch completes
						if(fdsSam->GetSideCount() > 2) { // ignore if multiple disks are not available
							MessageManager::DisplayMessage("FDS", "Ejected disk");
							_currentDisk |= 0x100;
							fdsSam->InsertNextDisk();
						}
					}
				}
				
				if(ProcessAction(RETRO_DEVICE_ID_JOYPAD_R)) {
					if (!(_currentDisk & 0x100)) { // ignore until previous disk switch completes
						MessageManager::DisplayMessage("FDS", "Ejected disk");
						FDS* fds = dynamic_cast<FDS*>(_console->GetMapper());
						if(fds)
							_currentDisk = fds->GetCurrentDisk() | 0x100;
						else
							_currentDisk |= 0x100;
						fdsSam->SwitchDiskSide();
					}
				}

				ReportDiskChange();
			}
			
			std::shared_ptr<VsSystemActionManager> vsSam = _console->GetSystemActionManager<VsSystemActionManager>();
			if(vsSam) {
				if(ProcessAction(RETRO_DEVICE_ID_JOYPAD_L2))
					vsSam->InsertCoin(0);
				if(ProcessAction(RETRO_DEVICE_ID_JOYPAD_R2))
					vsSam->InsertCoin(1);
			}
		}
	}

	virtual bool IsKeyPressed(uint32_t keyCode) override
	{
		if(keyCode > 0 && _getInputState)
		{
			if (_supportsInputBitmasks)
				return (_joypadButtons[keyCode >> 8] & (1 << ((keyCode - 1) & 0xFF))) != 0;
			else
				return _getInputState(keyCode >> 8, RETRO_DEVICE_JOYPAD, 0, (keyCode - 1) & 0xFF) != 0;
		}
		return false;
	}

	virtual bool IsMouseButtonPressed(MouseButton button) override
	{
		return _mouseButtons[(int)button];
	}

	virtual string GetKeyName(uint32_t keyCode) override
	{
		return string();
	}

	virtual uint32_t GetKeyCode(string keyName) override
	{
		return 0;
	}
};
