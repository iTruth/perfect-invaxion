#ifndef BASIC_SAVE_
#define BASIC_SAVE_ 1

#include <nlohmann/json.hpp>

namespace perfect_invaxion {
namespace save {

template<typename Executor>
class basic_save {
public:

	typedef Executor executor_type ;
	typedef typename executor_type::location_type location_type;

	basic_save() = default;

	explicit basic_save(const location_type& loc):
		executor_(loc)
	{ }

	bool open(const location_type& loc)
	{
		return executor_.open(loc);
	}

	bool is_open() const
	{
		return executor_.is_open();
	}

	bool is_exist() const
	{
		return executor_.is_exist();
	}

	bool create()
	{
		return executor_.create();
	}

	template<typename DataType>
	bool write(DataType&& data)
	{
		return executor_.write(std::forward<DataType>(data));
	}

	template<typename DataType>
	bool read(DataType& data)
	{
		return executor_.read(data);
	}

	bool close() const
	{
		return executor_.close();
	}

	bool read(nlohmann::json& j)
	{
		assert(is_open());

		std::string raw_json;
		bool succ = read(raw_json);

		if (!succ)
			return false;

		j = nlohmann::json::parse(raw_json);

		return true;
	}

	bool write(nlohmann::json j)
	{
		return write(j.dump());
	}

private:
	executor_type executor_;
};

} // namespace save
} // namespace perfect_invaxion

#endif // BASIC_SAVE_
