
#include "./json.hpp"

//template<typename BasicJsonType>
class json_sax_acceptor : public nlohmann::json::json_sax_t
{
  public:

    bool null()
    {
        return true;
    }

    bool boolean(bool /*unused*/)
    {
        return true;
    }

    bool number_integer(number_integer_t /*unused*/)
    {
        return true;
    }

    bool number_unsigned(number_unsigned_t /*unused*/)
    {
        return true;
    }

    bool number_float(number_float_t /*unused*/, const string_t& /*unused*/)
    {
        return true;
    }

    bool string(string_t& /*unused*/)
    {
        return true;
    }

    bool binary(binary_t& /*unused*/)
    {
        return true;
    }

    bool start_object(std::size_t /*unused*/ = static_cast<std::size_t>(-1))
    {
        return true;
    }

    bool key(string_t& /*unused*/)
    {
        return true;
    }

    bool end_object()
    {
        return true;
    }

    bool start_array(std::size_t /*unused*/ = static_cast<std::size_t>(-1))
    {
        return true;
    }

    bool end_array()
    {
        return true;
    }

    bool parse_error(std::size_t position, const std::string& last_token, const nlohmann::json::exception& ex)
    //bool parse_error(std::size_t /*unused*/, const std::string& /*unused*/, const nlohmann::detail::exception& /*unused*/)
    {
        std::cout << "parse_error(position=" + std::to_string(position) + ", last_token=" + last_token + ",\n            ex=" + std::string(ex.what()) + ")" << std::endl;
        return false;
    }
};
