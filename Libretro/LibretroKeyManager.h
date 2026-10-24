#pragma once
#include "libretro.h"
#include "LibretroMessageManager.h"
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
	LibretroMessageManager* _messageManager = nullptr;
	//The boot disk goes in while loading, which RetroArch does before setting the input callbacks, so start as if it was pending
	bool _swapPending = true;
	bool _announceSwap = false;
	bool _verboseDiskMessages = false;
	bool _textOnlyDiskMessages = false;
	uint32_t _swapEndFrame = 0;
	uint32_t _highestFrame = 0;
	int _progressDisk = -1;
	int _progressStep = 0;
	int _ejectingDisk = -1;

	//Run-ahead goes back at most 12 frames; 16 or more back is a state load or rewind
	static constexpr uint32_t LoadJumpFrames = 16;
	static constexpr const char* AutoInsertHint = "Disks are inserted automatically (FDS auto insert is on)";
	//Frontends without progress widgets queue every core message, so updates are spaced out and each lasts no longer than the interval
	static constexpr int SwapProgressInterval = 15; //frames
	static constexpr unsigned SwapProgressDuration = 250; //ms
	//Text only shows "Ejecting" for the first steps of a swap
	static constexpr int EjectingSteps = 2;

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

	void UpdateSwapProgress(std::shared_ptr<FdsSystemActionManager> &fdsSam)
	{
		uint32_t frame = _console->GetFrameCount();
		if(frame < _highestFrame && _highestFrame - frame >= LoadJumpFrames) {
			_swapEndFrame = 0;
			_highestFrame = 0;
		}
		_highestFrame = std::max(_highestFrame, frame);

		int disk = fdsSam->GetPendingInsertDisk();
		if(_announceSwap && disk >= 0) {
			//A new target, or two or more steps back (LoadJumpFrames or more), is a state load or rewind
			int step = fdsSam->GetInsertFramesElapsed() / SwapProgressInterval;
			bool jumped = step <= _progressStep - 2 || disk != _progressDisk;
			if(step > _progressStep || jumped) {
				bool ejectingEnds = _ejectingDisk >= 0 && step >= EjectingSteps;
				_progressDisk = disk;
				_progressStep = step;
				if(!_textOnlyDiskMessages) {
					_messageManager->DisplayProgressMessage("Inserting " + GetFdsDiskName(disk), fdsSam->GetInsertProgress(), SwapProgressDuration, true);
				} else if(jumped || ejectingEnds) {
					//After "Ejecting", or straight after a load: a state does not record the side that came out
					_ejectingDisk = -1;
					DisplayInsertingText(fdsSam, disk);
				}
			}
		} else if(_swapPending && !fdsSam->IsDiskSwapPending()) {
			FDS* fds = dynamic_cast<FDS*>(_console->GetMapper());
			if((_announceSwap || (_verboseDiskMessages && frame > _swapEndFrame)) && fds && fds->IsDiskInserted()) {
				string message = GetFdsDiskName(fds->GetCurrentDisk()) + " inserted";
				if(_textOnlyDiskMessages) {
					_messageManager->DisplayTextMessage(message, 1000);
				} else {
					_messageManager->DisplayProgressMessage(message, 100, 1000, false);
				}
			}
			_swapEndFrame = std::max(_swapEndFrame, frame);
			_announceSwap = false;
			_ejectingDisk = -1;
		}
	}

	void DisplayInsertingText(std::shared_ptr<FdsSystemActionManager> &fdsSam, int disk)
	{
		unsigned duration = fdsSam->GetInsertFramesLeft() * 1000 / 60;
		if(duration < SwapProgressDuration) {
			duration = SwapProgressDuration;
		}
		_messageManager->DisplayTextMessage("Inserting " + GetFdsDiskName(disk), duration);
	}

	void DisplayHint(const char* message)
	{
		if(_textOnlyDiskMessages) {
			_messageManager->DisplayTextMessage(message, 2000);
		} else {
			_messageManager->DisplayProgressMessage(message, -1, 2000, false);
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

	void SetVerboseDiskMessages(bool verbose)
	{
		_verboseDiskMessages = verbose;
	}

	void SetTextOnlyDiskMessages(bool textOnly)
	{
		_textOnlyDiskMessages = textOnly;
	}

	void SetMessageManager(LibretroMessageManager* messageManager)
	{
		_messageManager = messageManager;
	}

	void StartSwapProgress(std::shared_ptr<FdsSystemActionManager> &fdsSam)
	{
		int disk = fdsSam->GetPendingInsertDisk();
		if(disk >= 0) {
			_announceSwap = true;
			_progressDisk = disk;
			_progressStep = fdsSam->GetInsertFramesElapsed() / SwapProgressInterval;
			if(!_textOnlyDiskMessages) {
				_messageManager->DisplayProgressMessage("Inserting " + GetFdsDiskName(disk), 0, SwapProgressDuration, false);
			} else {
				//The disk comes out on the next frame
				FDS* fds = dynamic_cast<FDS*>(_console->GetMapper());
				_ejectingDisk = fds && fds->IsDiskInserted() ? fds->GetCurrentDisk() : -1;
				if(_ejectingDisk >= 0) {
					_messageManager->DisplayTextMessage("Ejecting " + GetFdsDiskName(_ejectingDisk), EjectingSteps * SwapProgressInterval * 1000 / 60);
				} else {
					DisplayInsertingText(fdsSam, disk);
				}
			}
		}
	}

	static string GetFdsDiskName(uint32_t disk)
	{
		return "Disk " + std::to_string(disk / 2 + 1) + ((disk & 1) ? " Side B" : " Side A");
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
				UpdateSwapProgress(fdsSam);

				FDS* fds = dynamic_cast<FDS*>(_console->GetMapper());
				//With the drive empty, L/R act on the last disk in (or the one disk control selected)
				int selectedDisk = fdsSam->GetSelectedDisk();
				bool useSelectedDisk = fds && !fds->IsDiskInserted() && selectedDisk >= 0;

				if(ProcessAction(RETRO_DEVICE_ID_JOYPAD_L) && !fdsSam->IsDiskSwapPending()) {
					if(fdsSam->IsAutoInsertDiskEnabled()) {
						DisplayHint(AutoInsertHint);
					} else if(fdsSam->GetSideCount() > 2) {
						if(useSelectedDisk) {
							fdsSam->InsertDisk(((selectedDisk & 0xFE) + 2) % fdsSam->GetSideCount());
						} else {
							fdsSam->InsertNextDisk();
						}
						StartSwapProgress(fdsSam);
					} else {
						DisplayHint("Only one disk: use Switch Disk Side");
					}
				}
				
				if(ProcessAction(RETRO_DEVICE_ID_JOYPAD_R) && !fdsSam->IsDiskSwapPending()) {
					if(fdsSam->IsAutoInsertDiskEnabled()) {
						DisplayHint(AutoInsertHint);
					} else {
						if(useSelectedDisk) {
							fdsSam->InsertDisk((selectedDisk ^ 0x01) % fdsSam->GetSideCount());
						} else {
							fdsSam->SwitchDiskSide();
						}
						StartSwapProgress(fdsSam);
					}
				}

				if(fds && fds->IsDiskInserted() && !fdsSam->IsDiskSwapPending()) {
					fdsSam->SetSelectedDisk(fds->GetCurrentDisk());
				}

				_swapPending = fdsSam->IsDiskSwapPending();
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
