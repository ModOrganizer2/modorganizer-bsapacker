#include "imoinfo.h"
#include <gmock/gmock.h>

using namespace MOBase;

class MockOrganizer : public IOrganizer
{
public:
	MOCK_METHOD(IModRepositoryBridge*, createNexusBridge, (), (const, override));
	MOCK_METHOD(QString, profileName, (), (const, override));
	MOCK_METHOD(QString, profilePath, (), (const, override));
	MOCK_METHOD(QString, downloadsPath, (), (const, override));
	MOCK_METHOD(QString, overwritePath, (), (const, override));
	MOCK_METHOD(QString, basePath, (), (const, override));
	MOCK_METHOD(QString, modsPath, (), (const, override));
	MOCK_METHOD(VersionInfo, appVersion, (), (const, override));
	MOCK_METHOD(IModInterface*, getMod, (const QString &name), (const, override));
	MOCK_METHOD(IModInterface*, createMod, (GuessedValue<QString> &name), (override));
	MOCK_METHOD(IPluginGame*, getGame, (const QString &gameName), (const, override));
	MOCK_METHOD(bool, removeMod, (IModInterface *mod), (override));
	MOCK_METHOD(void, modDataChanged, (IModInterface *mod), (override));
	MOCK_METHOD(QVariant, pluginSetting, (const QString &pluginName, const QString &key), (const, override));
	MOCK_METHOD(void, setPluginSetting, (const QString &pluginName, const QString &key, const QVariant &value), (override));
	MOCK_METHOD(QVariant, persistent, (const QString &pluginName, const QString &key, const QVariant &def), (const, override));
	MOCK_METHOD(void, setPersistent, (const QString &pluginName, const QString &key, const QVariant &value, bool sync), (override));
	MOCK_METHOD(QString, pluginDataPath, (), (const, override));
	MOCK_METHOD(IModInterface*, installMod, (const QString &fileName, const QString &nameSuggestion), (override));
	MOCK_METHOD(QString, resolvePath, (const QString &fileName), (const, override));
	MOCK_METHOD(QStringList, listDirectories, (const QString &directoryName), (const, override));
	MOCK_METHOD(QStringList, findFiles, (const QString &path, const std::function<bool(const QString&)> &filter), (const, override));
	MOCK_METHOD(QStringList, getFileOrigins, (const QString &fileName) ,(const, override));
	MOCK_METHOD(QList<FileInfo>, findFileInfos, (const QString &path, const std::function<bool(const FileInfo&)> &filter), (const, override));
	MOCK_METHOD(MOBase::IDownloadManager*, downloadManager, (), (const, override));
	MOCK_METHOD(MOBase::IPluginList*, pluginList, (), (const, override));
	MOCK_METHOD(MOBase::IModList*, modList, (), (const, override));
	MOCK_METHOD(MOBase::IProfile*, profile, (), (const, override));
	MOCK_METHOD(HANDLE, startApplication, (const QString &executable, const QStringList &args, const QString &cwd, const QString &profile, const QString &forcedCustomOverwrite, bool ignoreCustomOverwrite), (override));
	MOCK_METHOD(bool, waitForApplication, (HANDLE handle, LPDWORD exitCode), (const, override));
	MOCK_METHOD(bool, onModInstalled, (const std::function<void (const QString &)> &func), (override));
	MOCK_METHOD(bool, onAboutToRun, (const std::function<bool(const QString&)> &func), (override));
	MOCK_METHOD(bool, onFinishedRun, (const std::function<void(const QString&, unsigned int)> &func), (override));
	MOCK_METHOD(void, refreshModList,(bool saveChanges), (override));
	MOCK_METHOD(MOBase::IPluginGame const *, managedGame, (), (const, override));
	MOCK_METHOD(QStringList, modsSortedByProfilePriority, (), (const, override));
};