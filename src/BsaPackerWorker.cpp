#include "BsaPackerWorker.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

#include <bsapacker/ArchiveBuildDirector.h>
#include <bsapacker/ModDtoFactory.h>

namespace BsaPacker
{
	BsaPackerWorker::BsaPackerWorker(
		const ISettingsService* settingsService,
		const IModContext* modContext,
		const IModDtoFactory* modDtoFactory,
		const IArchiveBuilderFactory* archiveBuilderFactory,
		const IArchiveAutoService* archiveAutoService,
		const IDummyPluginServiceFactory* dummyPluginServiceFactory,
		const IHideLooseAssetService* hideLooseAssetService,
		const IArchiveNameService* archiveNameService,
		const IOverrideFileService* overrideFileService) :
		m_SettingsService(settingsService),
		m_ModContext(modContext),
		m_ModDtoFactory(modDtoFactory),
		m_ArchiveBuilderFactory(archiveBuilderFactory),
		m_ArchiveAutoService(archiveAutoService),
		m_DummyPluginServiceFactory(dummyPluginServiceFactory),
		m_HideLooseAssetService(hideLooseAssetService),
		m_ArchiveNameService(archiveNameService),
		m_OverrideFileService(overrideFileService)
	{
	}

	void BsaPackerWorker::DoWork() const
	{
		QStringList createdArchives;
		QStringList createdArchiveFileNames;
		const std::unique_ptr<IModDto> modDto = this->m_ModDtoFactory->Create();
		if (modDto->Directory().isEmpty()) {
			return;
		}

		const bool useBuiltInArchiveTools = this->m_ModContext != nullptr && this->m_ModContext->CanUseBuiltInArchiveTools();
		const bool useArchiveHandler = this->m_ModContext != nullptr && !useBuiltInArchiveTools && this->m_ModContext->HasArchiveCreationHandler();
		if (!useBuiltInArchiveTools && !useArchiveHandler) {
			QMessageBox::warning(nullptr, QString(), QObject::tr("Archive packing is not supported for the current game."));
			return;
		}

		if (useBuiltInArchiveTools) {
			const std::vector<bsa_archive_type_e> types = this->m_ArchiveBuilderFactory->GetArchiveTypes(modDto.get());
			for (auto&& type : types) {
				const std::unique_ptr<IArchiveBuilder> builder = this->m_ArchiveBuilderFactory->Create(type, modDto.get());
				ArchiveBuildDirector director(this->m_SettingsService, builder.get());
				director.Construct();
				const std::vector<std::unique_ptr<libbsarch::bs_archive_auto>> archives = builder->getArchives();
				for (const auto& archive : archives) {
					if (archive) {
						const QFileInfo fileInfo(this->m_ArchiveNameService->GetArchiveFullPath(type, modDto.get()));
						bool res = this->m_ArchiveAutoService->CreateBSA(archive.get(), fileInfo.absoluteFilePath(), type);
						if (res) {
							createdArchives.append(fileInfo.completeBaseName());
							createdArchiveFileNames.append(fileInfo.fileName());
						}
					}
				}
			}
		} else {
			const QString archivePath = QDir(modDto->Directory()).filePath(modDto->ArchiveName() + modDto->ArchiveExtension());
			const QFileInfo existingArchiveInfo(archivePath);
			if (existingArchiveInfo.exists() && existingArchiveInfo.isFile() && !QFile::remove(archivePath)) {
				QMessageBox::warning(nullptr, QString(),
					QObject::tr("Failed to replace existing archive: %1").arg(existingArchiveInfo.fileName()));
				return;
			}

			QString errorMessage;
			if (!this->m_ModContext->CreateArchive(modDto->Directory(), archivePath, &errorMessage)) {
				QMessageBox::warning(nullptr, QString(),
					errorMessage.isEmpty() ? QObject::tr("Failed to create archive.") : errorMessage);
				return;
			}

			const QFileInfo fileInfo(archivePath);
			createdArchives.append(fileInfo.completeBaseName());
			createdArchiveFileNames.append(fileInfo.fileName());
		}

		if (!createdArchiveFileNames.isEmpty()) {
			QMessageBox::information(nullptr, QString(),
				QObject::tr("Created archive(s):") + "\n" + createdArchiveFileNames.join(",\n"));
			if (useBuiltInArchiveTools) {
				this->m_OverrideFileService->CreateOverrideFile(modDto->NexusId(), modDto->Directory(), createdArchives);
			}
		}

		if (useBuiltInArchiveTools) {
			const std::unique_ptr<IDummyPluginService> pluginService = this->m_DummyPluginServiceFactory->Create();
			pluginService->CreatePlugin(modDto->Directory(), modDto->ArchiveName());
		}

		if (!createdArchiveFileNames.isEmpty() && !modDto->Directory().isEmpty()) {
			this->m_HideLooseAssetService->HideLooseAssets(modDto->Directory());
		}
	}
}


