#include <bsapacker/IModContext.h>
#include <gmock/gmock.h>

using namespace BsaPacker;

class MockModContext : public IModContext
{
public:
	MOCK_METHOD(QString, GetAbsoluteModPath, (const QString& modName), (const, override));
	MOCK_METHOD(int, GetNexusId, (), (const, override));
	MOCK_METHOD(QStringList, GetPlugins, (const QDir& modDirectory), (const, override));
	MOCK_METHOD(QStringList, GetValidMods, (), (const, override));
	MOCK_METHOD(bool, CanUseBuiltInArchiveTools, (), (const, override));
	MOCK_METHOD(bool, HasArchiveCreationHandler, (), (const, override));
	MOCK_METHOD(bool, CanCreateArchive, (const QString& archivePath), (const, override));
	MOCK_METHOD(bool, CreateArchive, (const QString& sourceDirectory, const QString& archivePath, QString* errorMessage), (const, override));
};
