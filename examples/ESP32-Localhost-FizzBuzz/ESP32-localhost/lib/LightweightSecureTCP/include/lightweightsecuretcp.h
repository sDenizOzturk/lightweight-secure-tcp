#ifndef LIGHTWEIGHTSECURETCP_H
#define LIGHTWEIGHTSECURETCP_H

#include "../src/core/lightweightsecureserver.h"
#include "../src/core/lightweightsecureclient.h"
#include "../src/utils/debugger.h"
#include "../src/encryption/key.h"


class LightweightSecureTCP
{
public:
    LightweightSecureTCP();

    static void enableDebug(bool enable);

    static void setHandshakeKey(const Key &key);

    static void setNumberOfRounds(const int &numberOfRounds);
};

#endif // LIGHTWEIGHTSECURETCP_H
