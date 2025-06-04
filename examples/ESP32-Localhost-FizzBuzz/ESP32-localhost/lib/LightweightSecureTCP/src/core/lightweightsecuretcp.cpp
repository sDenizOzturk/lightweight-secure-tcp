#include "lightweightsecuretcp.h"
#include "utils/randomgenerator.h"
#include "encryption/xtea256.h"

LightweightSecureTCP::LightweightSecureTCP() {
    RandomGenerator::initialize();
}

void LightweightSecureTCP::enableDebug(bool enable)
{
      LWSDebugSettings::instance().setEnabled(enable);
}

void LightweightSecureTCP::setHandshakeKey(const Key &key)
{
    XTEA256::setHandshakeKey(key);
}

void LightweightSecureTCP::setNumberOfRounds(const int &numberOfRounds)
{
    XTEA256::setNumberOfRounds(numberOfRounds);
}
