#include <string>
#include <map>

/// @brief A class processes the boost program options config file.
class ConfigFile{
public:
    /// @brief Default constructor, read boost program options config from file path.
    /// @param file_path The path of the config file
    ConfigFile(std::string const& file_path);

    /// @brief Get the value of a certain config from a boost program options config file, return while exists.
    /// @param section The section of the config
    /// @param entry The entry of the config
    /// @return The value of the config
    std::string const& value(std::string const& section, std::string const& entry) const;

private:
    std::map<std::string, std::string> content;
    
};
