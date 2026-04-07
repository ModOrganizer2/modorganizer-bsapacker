#include "ModContext.h"

#include <array>
#include <functional>

#include "NexusId.h"
#include <imodinterface.h>
#include <imodlist.h>
#include <iplugingame.h>
#include <uibase/game_features/dataarchives.h>
#include <uibase/game_features/gamearchivehandler.h>
#include <uibase/game_features/igamefeatures.h>
#include <QtConcurrent/QtConcurrentFilter>
#include <QtConcurrent/QtConcurrentMap>

namespace BsaPacker
{
	const QStringList& ModContext::PLUGIN_TYPES = { "*.esm", "*.esp", "*.esl" };

	ModContext::ModContext(MOBase::IOrganizer* moInfo) : m_Organizer(moInfo)
	{
	}

	QString ModContext::GetAbsoluteModPath(const QString& modName) const
	{
		const MOBase::IModInterface* const mod = m_Organizer->modList()->getMod(modName);
		return mod->absolutePath();
	}

	int ModContext::GetNexusId() const
	{
		const MOBase::IPluginGame* managedGame = this->m_Organizer->managedGame();
		int nexusId = managedGame->nexusGameID();

		if (nexusId != 0) {
			return nexusId;
		}
		if (!managedGame->primarySources().isEmpty()) {
			QString primarySource = managedGame->primarySources().first();
			if (primarySource == "FalloutNV") {
				return NexusId::NewVegas;
			}
		}
		return nexusId;
	}

	QStringList ModContext::GetPlugins(const QDir& modDirectory) const
	{
		if (!CanUseBuiltInArchiveTools()) {
			return HasArchiveCreationHandler()
				? QStringList{ QStringLiteral("<new archive filename>") }
				: QStringList{};
		}

		const int nexusId = this->GetNexusId();
		const QString extension = (nexusId == NexusId::Fallout4 || nexusId == NexusId::Starfield) ? ".ba2" : ".bsa";
		const std::function<QString(QString)> replace_extension = [&](QString fname) {
			return fname.replace(fname.lastIndexOf('.'), 4, extension);
		};

		QStringList filenames = QtConcurrent::blockingMapped(modDirectory.entryList(PLUGIN_TYPES, QDir::Files), replace_extension);
		filenames.removeDuplicates();
		return filenames << QStringLiteral("<new filename>") + extension;
	}

	QStringList ModContext::GetValidMods() const
	{
		const MOBase::IModList* const list = m_Organizer->modList();
		const std::function<bool(const QString&)> modStateValid = [&](const QString& mod)
		{
			return !mod.endsWith("_separator", Qt::CaseInsensitive) && (list->state(mod) & MOBase::IModList::STATE_VALID);
		};
		return QtConcurrent::blockingFiltered(list->allMods(), modStateValid);
	}

	bool ModContext::CanUseBuiltInArchiveTools() const
	{
		return m_Organizer != nullptr && m_Organizer->gameFeatures() != nullptr &&
			m_Organizer->gameFeatures()->gameFeature<MOBase::DataArchives>() != nullptr;
	}

	bool ModContext::HasArchiveCreationHandler() const
	{
		return ArchiveHandler() != nullptr;
	}

	bool ModContext::CanCreateArchive(const QString& archivePath) const
	{
		auto handler = ArchiveHandler();
		return handler != nullptr && handler->canCreateArchive(archivePath);
	}

	bool ModContext::CreateArchive(const QString& sourceDirectory,
		const QString& archivePath,
		QString* errorMessage) const
	{
		auto handler = ArchiveHandler();
		return handler != nullptr && handler->createArchive(sourceDirectory, archivePath, {}, errorMessage);
	}

	std::shared_ptr<const MOBase::GameArchiveHandler> ModContext::ArchiveHandler() const
	{
		if (m_Organizer == nullptr || m_Organizer->gameFeatures() == nullptr) {
			return {};
		}

		return m_Organizer->gameFeatures()->gameFeature<MOBase::GameArchiveHandler>();
	}
} // namespace BsaPacker
