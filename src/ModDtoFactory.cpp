#include <bsapacker/ModDtoFactory.h>

#include "ModDto.h"
#include "NullModDto.h"
#include "PackerDialog.h"
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>

namespace BsaPacker
{
	const uint16_t FALLOUT_4_NEXUS_ID = 1151;
	const uint16_t STARFIELD_NEXUS_ID = 4187;

	ModDtoFactory::ModDtoFactory(
		const IModContext* modContext,
		IPackerDialog* packerDialog) :
		m_ModContext(modContext),
		m_PackerDialog(packerDialog)
	{
	}

	std::unique_ptr<IModDto> ModDtoFactory::Create() const
	{
		this->m_PackerDialog->RefreshModList();
		int result = m_PackerDialog->Exec();
		if (result != QDialog::DialogCode::Accepted)
		{
			return std::make_unique<NullModDto>();
		}

		const bool useBuiltInArchiveTools = this->m_ModContext->CanUseBuiltInArchiveTools();
		const bool useArchiveHandler = !useBuiltInArchiveTools && this->m_ModContext->HasArchiveCreationHandler();
		if (!useBuiltInArchiveTools && !useArchiveHandler) {
			QMessageBox::warning(nullptr, QStringLiteral("BSA Packer"),
				QObject::tr("Archive packing is not supported for the current game."));
			return std::make_unique<NullModDto>();
		}

		const int nexusId = this->m_ModContext->GetNexusId();
		const QString& modName = this->m_PackerDialog->SelectedMod();
		const QString& modDir = this->m_ModContext->GetAbsoluteModPath(modName);
		const QString& pluginName = this->m_PackerDialog->SelectedPluginItem();
		const bool needsNewName = this->m_PackerDialog->IsNewFilename();
		const QString archiveName = ModDtoFactory::ArchiveNameValidator(modName, pluginName, needsNewName, useArchiveHandler);
		if (archiveName.isNull()) {
			return std::make_unique<NullModDto>();
		}

		if (useArchiveHandler) {
			const QFileInfo archiveInfo(archiveName);
			const QString completeSuffix = archiveInfo.completeSuffix();
			if (completeSuffix.isEmpty()) {
				QMessageBox::warning(nullptr, QStringLiteral("BSA Packer"),
					QObject::tr("Archive filename must include an extension."));
				return std::make_unique<NullModDto>();
			}

			const QString archivePath = QDir(modDir).filePath(archiveInfo.fileName());
			if (!this->m_ModContext->CanCreateArchive(archivePath)) {
				QMessageBox::warning(nullptr, QStringLiteral("BSA Packer"),
					QObject::tr("Archive packing is not supported for \"%1\".").arg(archiveInfo.fileName()));
				return std::make_unique<NullModDto>();
			}

			return std::make_unique<ModDto>(nexusId, modDir, archiveInfo.completeBaseName(), QStringLiteral(".") + completeSuffix);
		}

		const QString archiveExtension = (nexusId == FALLOUT_4_NEXUS_ID || nexusId == STARFIELD_NEXUS_ID)
			? QStringLiteral(".ba2")
			: QStringLiteral(".bsa");

		return std::make_unique<ModDto>(nexusId, modDir, archiveName, archiveExtension);
	}

	QString ModDtoFactory::ArchiveNameValidator(
		const QString& modName,
		const QString& pluginName,
		const bool needsNewName,
		const bool requiresFullFilename)
	{
		QString archive_name_base;
		if (needsNewName) {
			bool ok = false;
			const QString title = QStringLiteral("BSA Packer");
			const QString prompt = requiresFullFilename
				? QObject::tr("Archive filename:")
				: QObject::tr("Archive name (no file extension):");
			const QString name = QInputDialog::getText(nullptr,
				title,
				prompt,
				QLineEdit::Normal,
				modName,
				&ok).simplified();
			if (!ok) {
				return QString();
			}
			else if (name.isEmpty()) {
				qWarning("Archive name cannot be empty. Cancelling archive creation.");
				return QString();
			}
			archive_name_base = name;
		} else {
			archive_name_base = requiresFullFilename ? pluginName : pluginName.chopped(4);
		}
		return archive_name_base;
	}

	bool ModDtoFactory::CanOverwriteFile(const QString& filePath,
		const QString& fileName)
	{
		const QString& absoluteFileName = filePath + '/' + fileName;
		const QFileInfo fileInfo(absoluteFileName);
		if (!fileInfo.exists() || !fileInfo.isFile()) {
			return true;
		}

		const QString& message = QObject::tr("File \"") + absoluteFileName + QObject::tr("\" already exists. Overwrite?");
		return QMessageBox::question(nullptr, QStringLiteral("BSA Packer"), message, QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Cancel;
	}
} // namespace BsaPacker
