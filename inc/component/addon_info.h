#ifndef ADDON_INFO_H
#define ADDON_INFO_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QDateTime>

// ---------------------------------------------------------------------------
// AddonFileInfo - platform-specific library file from addon.xml
// ---------------------------------------------------------------------------

struct AddonFileInfo {
    QString platform;   // "win", "mac", "linux", "all"
    QString fileName;   // e.g. "ts_analyzer.dll"
};

// ---------------------------------------------------------------------------
// AddonDescriptor - parsed from addon.xml (the addon's own metadata)
// ---------------------------------------------------------------------------

struct AddonDescriptor {
    QString name;           // unique id, e.g. "ts_analyzer"
    QString vendor;         // publisher
    QString displayName;    // human-readable
    QString version;        // "X.Y.Z"
    QString description;    // brief description
    QStringList capabilities; // "symbol_outline", "segment_info", etc.
    QList<AddonFileInfo> files; // platform-specific library files
    QString directory;      // absolute path to addon folder

    bool isValid() const {
        return !name.isEmpty() && !files.isEmpty();
    }
};

// ---------------------------------------------------------------------------
// AddonState - runtime state from installed.xml
// ---------------------------------------------------------------------------

struct AddonState {
    QString name;           // unique id, matches AddonDescriptor::name
    QString version;        // installed version
    bool    enabled;        // whether the addon is active
    QString installPath;    // relative path from app root, e.g. "addons/ts_analyzer"
    QDateTime installDate;

    AddonState() : enabled(true) {}
};

#endif // ADDON_INFO_H
