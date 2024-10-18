/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "Config.h"
#include "Log.h"
#include "Utils.h"

void sConfig::dump() const
{
    cLog::Info("Border {} px.", border);
    cLog::Info("Padding {} px.", padding);
    cLog::Info("Overlay: {}.", isEnabled(overlay));
    cLog::Info("Allow dupes: {}.", isEnabled(alowDupes));
    cLog::Info("Multi-atlas: {}.", isEnabled(multi));
    cLog::Info("Trim sprites: {}.", isEnabled(trim));
    cLog::Info("Power of Two: {}.", isEnabled(pot));
    cLog::Info("Packing method: {}.", slowMethod ? "Slow" : "KD-Tree");
    cLog::Info("Drop extension: {}.", isEnabled(dropExt));
    cLog::Info("Max atlas size {} px.", maxTextureSize);
}
