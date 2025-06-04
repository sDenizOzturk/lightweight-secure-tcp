#pragma once

#include "core/lightweightsecureserver.h"
#include "core/lightweightsecureclient.h"
#include "encryption/key.h"
#include "utils/lwsdebug.h"

class LightweightSecureTCP
{
public:
    static void setHandshakeKey(const Key &key);

    static void setNumberOfRounds(const int &numberOfRounds);

    static void enableDebug(bool enable);

};

