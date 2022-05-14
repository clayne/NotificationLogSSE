#pragma once

namespace NotificationLogger::detail
{
	class Proxy
	{
	public:
		Proxy(const volatile Proxy&) = delete;
		Proxy& operator=(const volatile Proxy&) = delete;

		[[nodiscard]] static Proxy& GetSingleton() noexcept
		{
			static Proxy singleton;
			return singleton;
		}

		void AddMessage(std::string_view a_message) noexcept
		{
			const auto _ = std::unique_lock(this->_lock);
			while (this->_notifications.size() >= MAX_BUFFER_SIZE) {
				this->_notifications.pop_back();
			}

			this->_notifications.emplace_front(a_message);
		}

		[[nodiscard]] auto GetMessages() const noexcept
			-> std::vector<std::string>
		{
			const auto _ = std::unique_lock(this->_lock);
			return std::vector(this->_notifications.begin(), this->_notifications.end());
		}

	private:
		Proxy() = default;

		static constexpr std::size_t MAX_BUFFER_SIZE = 128;
		mutable std::mutex _lock;
		std::list<std::string> _notifications;
	};

	struct CreateHUDDataMessage
	{
		static void thunk(
			RE::HUDData::Type a_type,
			const char* a_message)
		{
			if (a_message) {
				auto& proxy = Proxy::GetSingleton();
				proxy.AddMessage(a_message);
			}

			func(a_type, a_message);
		}

		inline static REL::Relocation<decltype(thunk)> func;
	};
}

namespace NotificationLogger::Papyrus
{
	inline auto GetCachedMessages(RE::StaticFunctionTag*)
		-> std::vector<std::string>
	{
		auto& proxy = detail::Proxy::GetSingleton();
		return proxy.GetMessages();
	}

	inline bool Register(RE::BSScript::Internal::VirtualMachine* a_vm) noexcept
	{
		assert(a_vm != nullptr);
		a_vm->RegisterFunction("GetCachedMessages"sv, "NotificationLog"sv, GetCachedMessages, true);
		return true;
	}
}

namespace NotificationLogger::Hooks
{
	inline void Install()
	{
		const auto base = REL::ID(52933).address();
		const auto target = base + 0x31D;

		REL::make_pattern<"80 3D ?? ?? ?? ?? ?? 74 44 4D 85 F6 74 36 41 80 3E 00 74 30 48 8B 3D ?? ?? ?? ?? 48 8B 35 ?? ?? ?? ?? 49 8B D6 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 4C 8B C8 48 8D 97 ?? ?? ?? ?? 45 33 C0 48 8B CE E8 ?? ?? ?? ??">()
			.match_or_fail(base + 0x2F3);

		auto& trampoline = SKSE::GetTrampoline();
		detail::CreateHUDDataMessage::func = trampoline.write_call<5>(target, detail::CreateHUDDataMessage::thunk);
	}
}
