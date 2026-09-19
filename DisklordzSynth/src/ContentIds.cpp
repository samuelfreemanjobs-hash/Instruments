#include "disklordz/content/ContentIds.h"

namespace disklordz::content
{

bool isValidContentId (std::string_view id, std::string_view expectedPrefix) noexcept
{
    if (id.size() <= expectedPrefix.size())
        return false;
    if (id.substr (0, expectedPrefix.size()) != expectedPrefix)
        return false;
    for (std::size_t i = expectedPrefix.size(); i < id.size(); ++i)
    {
        const char c = id[i];
        if (c < '0' || c > '9')
            return false;
    }
    return true;
}

} // namespace disklordz::content
