/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "components.h"
#include "menumodel.h"
#include "inputcontrol.h"
#include "ttscontrol.h"
#include "voicecontrol.h"

void components::registerTypes(const char *uri)
{
    qmlRegisterType<MenuModel>(uri, 0, 1, "MenuModel");
    qmlRegisterType<InputControl>(uri, 0, 1, "InputControl");
    qmlRegisterType<TTSControl>(uri, 0, 1, "TTSControl");
    qmlRegisterType<VoiceControl>(uri, 0, 1, "VoiceControl");
}

Q_EXPORT_PLUGIN(components);
