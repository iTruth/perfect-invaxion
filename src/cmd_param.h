#ifndef CMD_PARAM_
#define CMD_PARAM_ 1

#include <argh.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace perfect_invaxion {
namespace cmd {

struct param : std::enable_shared_from_this<param> {
	bool trunc_mode;

	void print_param() const
	{
		spdlog::info("Run with:");
		spdlog::info("\tTrunc mode(-t): {}", trunc_mode ? "ON" : "OFF");
	}
};

std::shared_ptr<param> parse_cmd(int argc, char* argv[])
{
	auto p = std::make_shared<param>();
	auto cmdl = argh::parser(argc, argv);
	p->trunc_mode = cmdl[{"-t", "--trunc"}];

	return p->shared_from_this();
}

}  // namespace cmd
}  // namespace perfect_invaxion

#endif	// CMD_PARAM_