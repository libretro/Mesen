#pragma once
#include "stdafx.h"
#include <functional>
#include "BaseLoader.h"

struct RomData;

class FdsLoader : public BaseLoader
{
private:
	static constexpr size_t FdsDiskSideCapacity = 65500;

private:
	void AddGaps(vector<uint8_t>& diskSide, uint8_t* readBuffer);
	vector<uint8_t> LoadBios();

public:
	using BaseLoader::BaseLoader;

	//Set by a frontend that reads the BIOS through its own file system, such as the libretro VFS
	static std::function<bool(const string& path, vector<uint8_t>& data)> ReadBiosFile;

	vector<uint8_t> RebuildFdsFile(vector<vector<uint8_t>> diskData, bool needHeader);
	void LoadDiskData(vector<uint8_t>& romFile, vector<vector<uint8_t>> &diskData, vector<vector<uint8_t>> &diskHeaders);
	void LoadRom(RomData& romData, vector<uint8_t>& romFile);
};