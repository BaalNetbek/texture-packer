/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "rect.h"

#include <cstdlib>

bool sRect::intersect(const sRect& region) const
{
    if(abs(left + right - region.left - region.right) < (right - left + region.right - region.left))
    {
        if(abs(top + bottom - region.top - region.bottom) < (bottom - top + region.bottom - region.top))
        {
            return true;
        }
    }
    return false;
}
