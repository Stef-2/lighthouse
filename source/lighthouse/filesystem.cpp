#include "filesystem.hpp"

auto lh::file_system::root_path() -> std::filesystem::path
{
    // if we have discovered the root directory before, return it
    if (!m_engine_root_dir.first.empty())
        return m_engine_root_dir.first;

    // otherwise, discover, record and return the root path
    auto current_dir = std::filesystem::current_path();

    while (true)
    {
        if (current_dir.filename() == m_engine_root_dir.second)
        {
            m_engine_root_dir.first = current_dir;
            return current_dir;
        }

        current_dir = current_dir.parent_path();

        // check if we've reached system root, if we have, something has gone wrong
        // this could happen if the engine executable is outside of its root directory
        if (current_dir == current_dir.root_path())
        {
            lh::output::warning() << "could not find engine root directory, path queries will not work";
            return current_dir;
        }
        
    }
}

auto lh::file_system::data_path() -> std::filesystem::path
{
    // if we have discovered the data directory before, return it
    if (!m_data_dir.first.empty())
        return m_data_dir.first;

    // otherwise, discover, record and return the data path
    auto current_dir = std::filesystem::current_path();

    while (true)
    {
        if (current_dir.filename() == m_data_dir.second)
        {
            m_data_dir.first = current_dir;
            return current_dir;
        }

        current_dir = current_dir.parent_path();

        // check if we've reached system root, if we have, something has gone wrong
        // this could happen if the engine executable is outside of its root directory
        if (current_dir == current_dir.root_path())
        {
            lh::output::warning() << "could not find engine data directory, path queries will not work";
            return current_dir;
        }
    }

}

auto lh::file_system::find(std::string_view name) -> std::filesystem::path
{
    auto current_dir = root_path();

    for (const auto& dir : std::filesystem::recursive_directory_iterator(current_dir))
    {
        if (dir.path().filename() == name)
            return dir.path();
    }

    lh::output::warning() << "could not find the specified path: " << std::move(name);
    return root_path();
}
