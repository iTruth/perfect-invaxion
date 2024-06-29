#ifndef INVAION_SAVE_
#define INVAION_SAVE_ 1

#include <spdlog/spdlog.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "basic_save.h"
#include "detail/windows_registry.h"

#define EXECUTOR_REQUIRE(E)                                                    \
	template <typename =                                                       \
				  typename std::enable_if<std::is_same_v<Executor, E>>::type>

namespace perfect_invaxion {
namespace save {

template <typename Executor = detail::windows_registry>
class invaxion_save {
public:
	typedef typename Executor::location_type location_type;
	typedef basic_save<Executor> save_type;

	EXECUTOR_REQUIRE(detail::windows_registry)
	invaxion_save() :
		loc_(location_type{HKEY_CURRENT_USER, "Software\\Aquatrax\\INVAXION",
			"ServerEmulator/UserDatabase_h654456162"})
	{
		bool status = save_.open(loc_);

		if (!status || !save_.is_open()) {
			loc_.subkey = "Software\\Aquatrax";
			status = save_.open(loc_);
			loc_.subkey = "Software\\Aquatrax\\INVAXION";
			status = save_.open(loc_);
			if (!status || !save_.is_open()) {
				spdlog::critical("Failed to open invaxion registry!");
				std::exit(EXIT_FAILURE);
			}
		}
	}

	void set_trunc(bool b)
	{
		trunc_mode_ = b;
	}

	EXECUTOR_REQUIRE(detail::windows_registry)
	bool select(std::string_view save_name)
	{
		loc_.elem = save_name;
		return save_.open(loc_);
	}

	bool is_exist()
	{
		return save_.is_exist();
	}

	nlohmann::json read_json()
	{
		nlohmann::json data;
		save_.read(data);
		return data;
	}

	bool write_json(nlohmann::json data)
	{
		return save_.write(data);
	}

	EXECUTOR_REQUIRE(detail::windows_registry)
	bool fix_server_emulator_save(
		std::string_view target_field, std::string_view custom_save)
	{
		select(target_field);

		if (!save_.is_exist()) {
			spdlog::warn("Failed to fix server emulator save: ServerEmulator save not found!");
			spdlog::warn("Please register an account first.");
			return false;
		}

		std::ifstream save_file(custom_save.data());
		if (!save_file.is_open()) {
			spdlog::warn("Failed to fix {}: {} not found!", target_field, custom_save);
			return false;
		}

		auto save_data = nlohmann::json::parse(save_file);
		auto existing_save = read_json();
		if (trunc_mode_) {
			save_data["/0"_json_pointer]["name"] =
				existing_save["/0"_json_pointer]["name"];
			save_data["/0"_json_pointer]["token"] =
				existing_save["/0"_json_pointer]["token"];
			save_data["/0"_json_pointer]["steamId"] =
				existing_save["/0"_json_pointer]["steamId"];
			save_data["/0"_json_pointer]["sessionId"] =
				existing_save["/0"_json_pointer]["sessionId"];

			return write_json(save_data);

		} else {
			existing_save["/0"_json_pointer]["CharacterList"] =
				save_data["/0"_json_pointer]["CharacterList"];
			existing_save["/0"_json_pointer]["songList"] =
				save_data["/0"_json_pointer]["songList"];
			existing_save["/0"_json_pointer]["themeList"] =
				save_data["/0"_json_pointer]["themeList"];
			existing_save["/0"_json_pointer]["currencyInfo"] =
				save_data["/0"_json_pointer]["currencyInfo"];
			existing_save["/0"_json_pointer]["vipInfo"] =
				save_data["/0"_json_pointer]["vipInfo"];
			existing_save["/0"_json_pointer]["level"] =
				save_data["/0"_json_pointer]["level"];
			existing_save["/0"_json_pointer]["curExp"] =
				save_data["/0"_json_pointer]["curExp"];

			return write_json(existing_save);
		}
	}

	EXECUTOR_REQUIRE(detail::windows_registry)
	bool fix_offline_player_base_info(
		std::string_view target_field, std::string_view custom_save)
	{
		select(target_field);

		if (!trunc_mode_ && save_.is_exist()) {
			std::ifstream save_file(custom_save.data());
			if (!save_file.is_open()) {
				spdlog::warn("Failed to fix {}: {} not found!", target_field, custom_save);
				return false;
			}

			auto save_data = nlohmann::json::parse(save_file);
			auto existing_save = read_json();
			existing_save["level"] = save_data["level"];
			existing_save["curExp"] = save_data["curExp"];
			existing_save["maxExp"] = save_data["maxExp"];

			return write_json(existing_save);
		} else {
			return fix_offline_save(target_field, custom_save);
		}
	}

	EXECUTOR_REQUIRE(detail::windows_registry)
	bool fix_offline_save(
		std::string_view target_field, std::string_view custom_save)
	{
		select(target_field);

		std::ifstream save_file(custom_save.data());
		if (!save_file.is_open()) {
			spdlog::warn("Failed to fix {}: {} not found!", target_field, custom_save);
			return false;
		}

		auto save_data = nlohmann::json::parse(save_file);

		return write_json(save_data);
	}

private:
	save_type save_;
	location_type loc_;
	bool trunc_mode_ = false;
};

} // namespace save
} // namespace perfect_invaxion

#endif // INVAION_SAVE_
