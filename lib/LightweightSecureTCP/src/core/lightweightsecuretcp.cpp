#include "lightweightsecuretcp.h"
#include "encryption/xtea256.h"
#include "utils/lwsdebug.h"


void LightweightSecureTCP::setHandshakeKey(const Key &key)
{
    XTEA256::setHandshakeKey(key);
}

void LightweightSecureTCP::setNumberOfRounds(const int &numberOfRounds)
{
    XTEA256::setNumberOfRounds(numberOfRounds);
}

void LightweightSecureTCP::enableDebug(bool enable)
{
    LWSDebugSettings::instance().setEnabled(enable);
}
