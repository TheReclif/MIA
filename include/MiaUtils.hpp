#include <string>
#include <unordered_map>
#include <stdexcept>

template<typename T> class EnumConverter {};

template<typename T> T to_enum(const std::string& str)
{
	throw std::logic_error("Enum conversion for given type not implemented.");
}