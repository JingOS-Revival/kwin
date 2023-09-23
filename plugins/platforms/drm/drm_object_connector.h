/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2016 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef KWIN_DRM_OBJECT_CONNECTOR_H
#define KWIN_DRM_OBJECT_CONNECTOR_H

#include "drm_object.h"

namespace KWin
{

class DrmConnector : public DrmObject
{
public:
    DrmConnector(uint32_t connector_id, int fd);

    ~DrmConnector() override;

    bool init() override;

    enum class PropertyIndex {
        CrtcId = 0,
        NonDesktop = 1,
        Count
    };

    QVector<uint32_t> encoders() {
        return m_encoders;
    }

    bool initProps() override;
    bool isConnected();

    bool isNonDesktop() const {
        auto prop = m_props.at(static_cast<int>(PropertyIndex::NonDesktop));
        if (!prop) {
            return false;
        }
        return prop->value();
    }
private:
    QVector<uint32_t> m_encoders;
};

}

#endif

