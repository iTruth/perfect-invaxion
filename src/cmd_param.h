#ifndef CMD_PARAM_
#define CMD_PARAM_ 1

#include <argh.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace perfect_invaxion {
namespace cmd {

class param : public std::enable_shared_from_this<param> {
public:
	using shared_this_t = std::shared_ptr<param>;

	shared_this_t set_trunc_mode(bool b)
	{
		trunc_mode_.store(b, std::memory_order_release);
		return shared_from_this();
	}

	bool get_trunc_mode() const noexcept
	{
		return trunc_mode_.load(std::memory_order_acquire);
	}

	shared_this_t set_help_mode(bool b)
	{
		help_mode_.store(b, std::memory_order_release);
		return shared_from_this();
	}

	bool get_help_mode() const noexcept
	{
		return help_mode_.load(std::memory_order_acquire);
	}

	void print_param_status() const
	{
		spdlog::info("Run with:");
		spdlog::info("\tTrunc mode(-t): {}", get_trunc_mode() ? "ON" : "OFF");
	}

	void print_help() const
	{
		spdlog::info("Usage:");
		print_single_help("-t,", "--trunc",
			"Write save files directly to the registry.",
			"You will lose your record by this.");
		print_single_help("-h,", "--help", "Print this message.");
	}

private:
	template <typename... DescArgs>
	void print_single_help(std::string_view abbr, std::string_view full,
		std::string_view desc, DescArgs... desc_args) const
	{
		spdlog::info("  {:<5}{:<10}{:<50}", abbr, full, desc);
		if constexpr (sizeof...(desc_args) >= 1)
			print_next_desc(desc_args...);
		spdlog::info("");
	}

	template <typename... DescArgs>
	void print_next_desc(std::string_view desc, DescArgs... desc_args) const
	{
		print_next_desc(desc);
		print_next_desc(desc_args...);
	}

	void print_next_desc(std::string_view desc) const
	{
		spdlog::info("  {:<15}{:<50}", "", desc);
	}

	std::atomic_bool trunc_mode_;
	std::atomic_bool help_mode_;
};

std::shared_ptr<param> parse_cmd(int argc, char* argv[])
{
	auto p = std::make_shared<param>();
	auto cmdl = argh::parser(argc, argv);
	p->set_trunc_mode(cmdl[{"-t", "--trunc"}])
		->set_help_mode(cmdl[{"-h", "--help"}]);

	return p;
}

} // namespace cmd
} // namespace perfect_invaxion

#endif // CMD_PARAM_