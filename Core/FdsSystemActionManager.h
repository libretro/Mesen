#pragma once
#include "stdafx.h"
#include <memory>
#include "SystemActionManager.h"
#include "FDS.h"

class FdsSystemActionManager : public SystemActionManager
{
private:
	const uint8_t ReinsertDiskFrameDelay = 120;
	const int32_t SelectedDiskNotSaved = -2;

	std::weak_ptr<FDS> _mapper;

	bool _needEjectDisk = false;
	uint8_t _insertDiskNumber = 0;
	uint8_t _insertDiskDelay = 0;
	int32_t _selectedDisk = 0; //disk control's side for the next insert, -1 for none; L/R's base disk with the drive empty
	uint32_t _sideCount;

	//A state saved without the selection takes the side in the drive, read once the mapper is loaded and before a pending eject empties it
	void ResolveSelectedDisk()
	{
		if(_selectedDisk == SelectedDiskNotSaved) {
			std::shared_ptr<FDS> mapper = _mapper.lock();
			_selectedDisk = mapper && mapper->IsDiskInserted() ? (int32_t)mapper->GetCurrentDisk() : 0;
		}
	}

protected:
	string GetKeyNames() override
	{
		return string("RPE0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ").substr(0, _sideCount + 3);
	}

	void StreamState(bool saving) override
	{
		SystemActionManager::StreamState(saving);
		ValueInfo<int32_t> selectedDisk = { &_selectedDisk, SelectedDiskNotSaved };
		Stream(_needEjectDisk, _insertDiskNumber, _insertDiskDelay, selectedDisk);
	}

public:
	enum FdsButtons { EjectDiskButton = 2, InsertDisk1 };

	FdsSystemActionManager(std::shared_ptr<Console> console, std::shared_ptr<BaseMapper> mapper) : SystemActionManager(console)
	{
		_mapper = std::dynamic_pointer_cast<FDS>(mapper);
		_sideCount = std::dynamic_pointer_cast<FDS>(mapper)->GetSideCount();

		if(console->GetSettings()->CheckFlag(EmulationFlags::FdsAutoLoadDisk)) {
			InsertDisk(0);
		}
	}

	void OnAfterSetState() override
	{
		SystemActionManager::OnAfterSetState();
		ResolveSelectedDisk();
		if(_needEjectDisk) {
			SetBit(FdsSystemActionManager::FdsButtons::EjectDiskButton);
			_needEjectDisk = false;
		}
		if(_insertDiskDelay > 0) {
			_insertDiskDelay--;
			if(_insertDiskDelay == 0) {
				SetBit(FdsSystemActionManager::FdsButtons::InsertDisk1 + _insertDiskNumber);
			}
		}

		bool needEject = IsPressed(FdsSystemActionManager::FdsButtons::EjectDiskButton);
		int diskToInsert = -1;
		for(int i = 0; i < 16; i++) {
			if(IsPressed(FdsSystemActionManager::FdsButtons::InsertDisk1 + i)) {
				diskToInsert = i;
				break;
			}
		}

		if(needEject || diskToInsert >= 0) {
			std::shared_ptr<FDS> mapper = _mapper.lock();
			if(needEject)
				mapper->EjectDisk();

			if(diskToInsert >= 0)
				mapper->InsertDisk(diskToInsert);
		}
	}
	
	void EjectDisk()
	{
		_needEjectDisk = true;
	}

	void InsertDisk(uint8_t diskNumber)
	{
		std::shared_ptr<FDS> mapper = _mapper.lock();
		if(mapper) {
			if(mapper->IsDiskInserted()) {
				//Eject disk on next frame, then insert new disk 2 seconds later
				_needEjectDisk = true;
				_insertDiskNumber = diskNumber;
				_insertDiskDelay = FdsSystemActionManager::ReinsertDiskFrameDelay;
			} else {
				//Insert disk on next frame
				_insertDiskNumber = diskNumber;
				_insertDiskDelay = 1;
			}
		}
	}

	void SwitchDiskSide()
	{
		if(!IsAutoInsertDiskEnabled()) {
			std::shared_ptr<FDS> mapper = _mapper.lock();
			if(mapper && mapper->IsDiskInserted()) {
				InsertDisk((mapper->GetCurrentDisk() ^ 0x01) % mapper->GetSideCount());
			}
		}
	}

	void InsertNextDisk()
	{
		if(!IsAutoInsertDiskEnabled()) {
			std::shared_ptr<FDS> mapper = _mapper.lock();
			if(mapper) {
				InsertDisk(((mapper->GetCurrentDisk() & 0xFE) + 2) % mapper->GetSideCount());
			}
		}
	}

	bool IsDiskSwapPending()
	{
		return _needEjectDisk || _insertDiskDelay > 0;
	}

	int GetPendingInsertDisk()
	{
		return _insertDiskDelay > 0 ? _insertDiskNumber : -1;
	}

	int GetInsertFramesElapsed()
	{
		return ReinsertDiskFrameDelay - _insertDiskDelay;
	}

	int GetInsertFramesLeft()
	{
		return _insertDiskDelay;
	}

	int GetInsertProgress()
	{
		return GetInsertFramesElapsed() * 100 / ReinsertDiskFrameDelay;
	}

	int GetSelectedDisk()
	{
		ResolveSelectedDisk();
		return _selectedDisk;
	}

	void SetSelectedDisk(int disk)
	{
		_selectedDisk = disk;
	}

	uint32_t GetSideCount()
	{
		std::shared_ptr<FDS> mapper = _mapper.lock();
		if(mapper) {
			return mapper->GetSideCount();
		} else {
			return 0;
		}
	}

	bool IsAutoInsertDiskEnabled()
	{
		std::shared_ptr<FDS> mapper = _mapper.lock();
		if(mapper) {
			return mapper->IsAutoInsertDiskEnabled();
		} else {
			return false;
		}
	}
};
