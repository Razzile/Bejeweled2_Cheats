#pragma once
#include <initializer_list>
#include <array>
#include <vector>

using byte = uint8_t;
using bytes = std::initializer_list<byte>;

class Patch {
public:
	Patch(size_t addr, bytes bytes) : _address(addr), _bytes(bytes) {
		_orig.resize(_bytes.size());
		memcpy(_orig.data(), (void*)addr, _bytes.size());
	}

	Patch(size_t addr, uint32_t bytes) : _address(addr) {
		byte *data = (byte*)&bytes;
		_bytes = std::vector<byte>(*data, *(data + sizeof(uint32_t))); // eww

		_orig.resize(_bytes.size());
		memcpy(_orig.data(), (void*)addr, _bytes.size());
	}

	bool Apply() {
		// make page writeable
		DWORD protection;
		bool success = VirtualProtect((void *)_address, _bytes.size(), PAGE_READWRITE, &protection);
		if (!success) {
			return success;
		}

		// copy patch
		memcpy((void *)_address, _bytes.data(), _bytes.size());

		// reset page protection
		return VirtualProtect((void *)_address, _bytes.size(), protection, &protection);
	}

	bool Reset() {
		DWORD protection;
		bool success = VirtualProtect((void *)_address, _orig.size(), PAGE_READWRITE, &protection);
		if (!success) {
			return success;
		}

		// copy patch
		memcpy((void *)_address, _orig.data(), _orig.size());

		// reset page protection
		return VirtualProtect((void *)_address, _orig.size(), protection, &protection);
	}

private:
	size_t _address;
	std::vector<byte> _bytes, _orig;
};

