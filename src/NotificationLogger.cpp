#include "NotificationLogger.h"

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


auto NotificationLogger::GetCachedMessages(RE::StaticFunctionTag*)
-> Array_t
{
	Array_t arr;
	while (_notificationBuffer.size() >= arr.max_size()) {
		_notificationBuffer.pop_back();
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
	REL::Offset<std::uintptr_t> hookPoint(REL::ID(52050), 0x19B);
	auto trampoline = SKSE::GetTrampoline();
	_createHUDDataMessage = trampoline->Write5CallEx(hookPoint.GetAddress(), unrestricted_cast<std::uintptr_t>(&CreateHUDDataMessage));
}


void NotificationLogger::AddMessage(const char* a_message)
{
	while (_notificationBuffer.size() >= MAX_BUFFER_SIZE) {
		_notificationBuffer.pop_back();
	}
	_notificationBuffer.push_front(a_message);
}


decltype(NotificationLogger::_notificationBuffer) NotificationLogger::_notificationBuffer;
