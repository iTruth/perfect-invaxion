#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <save/invaxion_save.h>
#include <platform/nt_helper.h>

int main(int argc, char *argv[])
{
	struct on_exit {
		~on_exit()
		{
			using namespace perfect_invaxion::winnt;

			if (is_launch_from_explorer())
				pause();
		}
	} _on_exit;

	spdlog::info("Perfect Invaxion Project v1.0");

	perfect_invaxion::save::invaxion_save save;

	save.fix_server_emulator_save(
			"ServerEmulator/UserDatabase_h654456162",
			"save/ServerEmulator_Save.json"
			);

	save.fix_offline_save(
			"Offline_PlayerBaseInfo_h911750931",
			"save/Offline_PlayerBaseInfo.json"
			);

	save.fix_offline_save(
			"Offline_PlayerSongList_h655833887",
			"save/Offline_PlayerSongList.json"
			);

	save.fix_offline_save(
			"Offline_PlayerCharList_h2836715314",
			"save/Offline_PlayerCharList.json"
			);

	save.fix_offline_save(
			"Offline_PlayerThemeList_h553588539",
			"save/Offline_PlayerThemeList.json"
			);

	save.fix_offline_save(
			"Offline_PlayerCurrency_h668743759",
			"save/Offline_PlayerCurrency.json"
			);

	spdlog::info("All done!");
}