#include "NotificationLogger.h"

#include "skse64_common/BranchTrampoline.h"

#include "REL/Relocation.h"
#include "SKSE/API.h"


void NotificationLogger::Init()
{
	InstallHooks();
	auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(RegisterPapyrusFunctions);
	_MESSAGE("NotificationLogger Initialized");
}


bool NotificationLogger::RegisterPapyrusFunctions(RE::BSScript::Internal::VirtualMachine* a_vm)
{
	a_vm->RegisterFunction("GetCachedMessages", "NotificationLog", GetCachedMessages);
	return true;
}


void NotificationLogger::CreateHUDDataMessage(RE::HUDData::Type a_type, const char* a_message)
{
	AddMessage(a_message);
	_createHUDDataMessage(a_type, a_message);
}


RE::BSScript::VMArray<RE::BSFixedString> NotificationLogger::GetCachedMessages(RE::StaticFunctionTag*)
{
	Array_t arr;
	if (_notificationBuffer.size() > arr.max_size()) {
		do {
			_notificationBuffer.pop_back();
		} while (_notificationBuffer.size() > arr.max_size());
	}
	arr.resize(_notificationBuffer.size());

	std::size_t i = 0;
	for (auto& notif : _notificationBuffer) {
		arr[i++] = notif;
	}

	return arr;
}


void NotificationLogger::InstallHooks()
{
	// E8 ? ? ? ? 83 FE 0C
	constexpr std::uintptr_t FUNC_ADDR = 0x008DA3D0;	// 1_5_80
	REL::Offset<std::uintptr_t> funcBase(FUNC_ADDR);

	std::uintptr_t hookPoint = funcBase.GetAddress() + 0x19B;

	auto offset = reinterpret_cast<std::int32_t*>(hookPoint + 1);
	auto nextOp = hookPoint + 5;
	_createHUDDataMessage = unrestricted_cast<CreateHUDDataMessage_t*>(nextOp + *offset);

	g_branchTrampoline.Write5Call(hookPoint, unrestricted_cast<std::uintptr_t>(&CreateHUDDataMessage));
}


void NotificationLogger::AddMessage(const char* a_message)
{
	if (_notificationBuffer.size() >= MAX_BUFFER_SIZE) {
		do {
			_notificationBuffer.pop_back();
		} while (_notificationBuffer.size() >= MAX_BUFFER_SIZE);
	}
	_notificationBuffer.push_front(a_message);
}


decltype(NotificationLogger::_notificationBuffer) NotificationLogger::_notificationBuffer;

