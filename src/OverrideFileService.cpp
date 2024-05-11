#include "OverrideFileService.h"

#include <questionboxmemory.h>
#include <QApplication>
#include <QDialogButtonBox>

namespace BsaPacker {
	const uint16_t FALLOUT_3_NEXUS_ID = 120;
	const uint16_t NEW_VEGAS_NEXUS_ID = 130;

	OverrideFileService::OverrideFileService(
		const IFileWriterService* fileWriterService)
		: m_FileWriterService(fileWriterService)
	{
	}

	// TODO: Add detection for Command Extender and JIP LN NVSE and warn if missing
	bool OverrideFileService::CreateOverrideFile(const int nexusId,
		const QString& modPath,
		const QString& archiveNameBase) const {

		if (nexusId != FALLOUT_3_NEXUS_ID && nexusId != NEW_VEGAS_NEXUS_ID) {
			return false;
		}

		if (MOBase::QuestionBoxMemory::query(QApplication::activeModalWidget(), "BSAPacker", "Create .override file?",
			"Do you want to create an override file for the archive?",
			QDialogButtonBox::No | QDialogButtonBox::Yes, QDialogButtonBox::No) & QDialogButtonBox::No) {
			return false;
		}

		const QString& fileNameNoExtension = modPath + '/' + archiveNameBase;
		const std::string& absoluteFileName = fileNameNoExtension.toStdString() + ".override";
		return this->m_FileWriterService->Write(absoluteFileName, nullptr, 0);
	}
} // namespace BsaPacker