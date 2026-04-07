#ifndef IMODCONTEXT_H
#define IMODCONTEXT_H

#include <QDir>
#include <QStringList>

namespace BsaPacker
{
	class IModContext
	{
	public:
		virtual ~IModContext() = default;
		[[nodiscard]] virtual QString GetAbsoluteModPath(const QString& modName) const = 0;
		[[nodiscard]] virtual int GetNexusId() const = 0;
		[[nodiscard]] virtual QStringList GetPlugins(const QDir& modDirectory) const = 0;
		[[nodiscard]] virtual QStringList GetValidMods() const = 0;
		[[nodiscard]] virtual bool CanUseBuiltInArchiveTools() const = 0;
		[[nodiscard]] virtual bool HasArchiveCreationHandler() const = 0;
		[[nodiscard]] virtual bool CanCreateArchive(const QString& archivePath) const = 0;
		[[nodiscard]] virtual bool CreateArchive(const QString& sourceDirectory,
			const QString& archivePath,
			QString* errorMessage = nullptr) const = 0;
	};
} // namespace BsaPacker

#endif // IMODCONTEXT_H
