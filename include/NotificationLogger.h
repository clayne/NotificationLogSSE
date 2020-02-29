#pragma once

#include <list>
#include <string>

#include "RE/Skyrim.h"
#include "REL/Relocation.h"


class NotificationLogger
{
public:
	using Array_t = RE::BSScript::VMArray<RE::BSFixedString>;


	static void Init();
	static bool RegisterPapyrusFunctions(RE::BSScript::Internal::VirtualMachine* a_vm);
	static void CreateHUDDataMessage(RE::HUDData::Type a_type, const char* a_message);
	static Array_t GetCachedMessages(RE::StaticFunctionTag*);

private:
	using CreateHUDDataMessage_t = decltype(&NotificationLogger::CreateHUDDataMessage);


	static void InstallHooks();
	static void AddMessage(const char* a_message);


	static constexpr std::size_t MAX_BUFFER_SIZE = 128;
	static std::list<std::string> _notificationBuffer;
	static inline REL::Function<CreateHUDDataMessage_t> _createHUDDataMessage;
};
