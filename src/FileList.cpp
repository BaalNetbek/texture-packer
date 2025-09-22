/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "FileList.h"

#include <algorithm>
#include <filesystem>


namespace fs = std::filesystem;

void cFileList::addFile(uint32_t trimCount, const std::string& path)
{
    m_files.push_back({ trimCount, path });
}

void cFileList::addPath(uint32_t trimCount, const std::string& root, bool recurse)
{
    std::error_code errc;
    fs::path rootPath(root);

    if (!fs::exists(rootPath, errc))
    {
        addFile(trimCount, root);
        return;
    }

    if (!fs::is_directory(rootPath, errc))
    {
        addFile(trimCount, root);
        return;
    }

    if (recurse)
    {
        for (auto const& entry : fs::recursive_directory_iterator(rootPath, fs::directory_options::skip_permission_denied, errc))
        {
            if (errc)
                continue;
            if (entry.is_regular_file(errc))
                addFile(trimCount, entry.path().string());
        }
    }
    else
    {
        for (auto const& entry : fs::directory_iterator(rootPath, fs::directory_options::skip_permission_denied, errc))
        {
            if (errc)
                continue;
            if (entry.is_regular_file(errc))
                addFile(trimCount, entry.path().string());
            
        }
    }
}

void cFileList::removeDupes()
{
    std::sort(m_files.begin(), m_files.end(), [](const FileInfo& a, const FileInfo& b) {
        return a.path < b.path;
    });
    auto it = std::unique(m_files.begin(), m_files.end(), [](const FileInfo& a, const FileInfo& b) {
        return a.path == b.path;
    });
    m_files.resize(std::distance(m_files.begin(), it));
}

uint32_t cFileList::getCount() const
{
    return static_cast<uint32_t>(m_files.size());
}

const cFileList::List& cFileList::getList() const
{
    return m_files;
}
