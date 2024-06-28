#ifndef WINDOWS_REGISTRY_
#define WINDOWS_REGISTRY_ 1

#include <windows.h>

#include <atomic>
#include <memory>
#include <mutex>

namespace perfect_invaxion {
namespace save {
namespace detail {

#define LOCK_GUARD_2(LOCK1, LOCK2)                                             \
	std::lock(LOCK1, LOCK2);                                                   \
	std::lock_guard<std::mutex> lck1_##LOCK1{LOCK1, std::adopt_lock};          \
	std::lock_guard<std::mutex> lck2_##LOCK2{LOCK2, std::adopt_lock};

namespace windows_registry_helper {

template <typename T>
struct associate_type;

template <>
struct associate_type<int32_t> {
	static constexpr DWORD reg_type = REG_DWORD;
};

template <>
struct associate_type<uint32_t> {
	static constexpr DWORD reg_type = REG_DWORD;
};

template <>
struct associate_type<int64_t> {
	static constexpr DWORD reg_type = REG_QWORD;
};

template <>
struct associate_type<uint64_t> {
	static constexpr DWORD reg_type = REG_QWORD;
};

} // namespace windows_registry_helper

class windows_registry {
public:
	struct location_type {
		HKEY key;
		std::string subkey;
		std::string elem;
	};

	windows_registry() = default;

	explicit windows_registry(const location_type& loc)
	{
		open_no_check(loc);
	}

	~windows_registry()
	{
		close();
	}

	bool open(const location_type& loc)
	{
		if (loc.key == loc_.key && loc.subkey == loc_.subkey) {
			loc_.elem = loc.elem;
			return true;
		}

		close();
		return open_no_check(loc);
	}

	bool is_open() const noexcept
	{
		return open_flag_.load(std::memory_order_acquire);
	}

	bool is_exist() const noexcept
	{
		assert(is_open());

		LOCK_GUARD_2(key_lock_, loc_lock_);

		LSTATUS status = RegQueryValueExA(
			key_, loc_.elem.c_str(), 0, nullptr, nullptr, nullptr);

		return status == ERROR_SUCCESS;
	}

	bool create() noexcept
	{
		LOCK_GUARD_2(key_lock_, loc_lock_);

		return create_no_lock();
	}

	bool close() const noexcept
	{
		if (!is_open())
			return true;

		std::lock_guard<std::mutex> key_lg{key_lock_};
		LSTATUS status = RegCloseKey(key_);
		open_flag_.store(false, std::memory_order_release);
		return status == ERROR_SUCCESS;
	}

	template <typename T,
		typename = typename std::enable_if<
			windows_registry_helper::associate_type<T>::reg_type
			!= REG_NONE>::type>
	bool write(T v) const noexcept
	{
		assert(is_open());

		LOCK_GUARD_2(key_lock_, loc_lock_);

		LSTATUS status = RegSetValueExA(key_, loc_.elem.c_str(), 0,
			windows_registry_helper::associate_type<T>::reg_type,
			reinterpret_cast<const BYTE*>(&v), sizeof(T));
		return status == ERROR_SUCCESS;
	}

	bool write(const std::string& v) const noexcept
	{
		assert(is_open());

		LOCK_GUARD_2(key_lock_, loc_lock_);

		LSTATUS status = RegSetValueExA(key_, loc_.elem.c_str(), 0, REG_BINARY,
			reinterpret_cast<const BYTE*>(v.c_str()), v.length() + 1);
		return status == ERROR_SUCCESS;
	}

	template <typename T,
		typename = typename std::enable_if<
			windows_registry_helper::associate_type<T>::reg_type
			!= REG_NONE>::type>
	bool read(T& v) const noexcept
	{
		assert(is_open());

		LOCK_GUARD_2(key_lock_, loc_lock_);

		DWORD size = sizeof(T);

		LSTATUS status = RegQueryValueExA(key_, loc_.elem.c_str(), 0, nullptr,
			reinterpret_cast<LPBYTE>(&v), &size);

		return status == ERROR_SUCCESS;
	}

	bool read(std::string& v) const
	{
		assert(is_open());

		LOCK_GUARD_2(key_lock_, loc_lock_);

		DWORD size = 0;
		LSTATUS status = RegQueryValueExA(
			key_, loc_.elem.c_str(), 0, nullptr, nullptr, &size);

		std::unique_ptr<BYTE[]> buf{new BYTE[size]};

		status = RegQueryValueExA(
			key_, loc_.elem.c_str(), 0, nullptr, buf.get(), &size);

		v.assign(reinterpret_cast<char*>(buf.get()), size);

		return status == ERROR_SUCCESS;
	}

private:
	bool open_no_check(const location_type& loc)
	{
		LOCK_GUARD_2(key_lock_, loc_lock_);

		loc_ = loc;
		bool succ = create_no_lock();
		open_flag_.store(succ, std::memory_order_release);

		return succ;
	}

	bool create_no_lock()
	{
		DWORD disposition;
		LSTATUS status = RegCreateKeyExA(loc_.key, loc_.subkey.c_str(), 0,
			nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &key_,
			&disposition);
		return status == ERROR_SUCCESS;
	}

	HKEY key_;
	location_type loc_;

	mutable std::mutex key_lock_;
	mutable std::mutex loc_lock_;

	mutable std::atomic_bool open_flag_;
};

} // namespace detail
} // namespace save
} // namespace perfect_invaxion

#endif // WINDOWS_REGISTRY_
