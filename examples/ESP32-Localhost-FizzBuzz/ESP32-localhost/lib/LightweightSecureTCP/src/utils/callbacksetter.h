#pragma once

#define DEFINE_CALLBACK_SETTER(CALLBACK_NAME, CALLBACK_TYPE) \
void CURRENT_CLASS::setOn##CALLBACK_NAME(CALLBACK_TYPE callback) { \
        this->on##CALLBACK_NAME = std::move(callback); \
}
