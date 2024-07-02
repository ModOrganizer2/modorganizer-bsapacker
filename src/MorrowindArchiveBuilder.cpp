#include <bsapacker/MorrowindArchiveBuilder.h>

#include <bsapacker/ArchiveBuilderHelper.h>
#include <QDirIterator>
#include <QApplication>
#include <QDebug>

using namespace libbsarch;

namespace BsaPacker
{
	MorrowindArchiveBuilder::MorrowindArchiveBuilder(const IArchiveBuilderHelper* archiveBuilderHelper, const QDir& rootDir, const bsa_archive_type_t& type)
		: m_ArchiveBuilderHelper(archiveBuilderHelper), m_RootDirectory(rootDir)
	{
		this->m_Cancelled = false;
		this->m_Archive = std::make_unique<libbsarch::bs_archive_auto>(type);
	}

	uint32_t MorrowindArchiveBuilder::setFiles()
	{
		uint32_t incompressibleFiles = 0;
		int count = 0;
		const auto& dirString = (this->m_RootDirectory.path() + '/').toStdWString();
		const auto& rootDirFiles = this->m_ArchiveBuilderHelper->getRootDirectoryFilenames(dirString);
		qDebug() << "root is: " << m_RootDirectory.path() + '/';

		QDirIterator iterator(this->m_RootDirectory.absolutePath(), QDir::Files, QDirIterator::Subdirectories);
		while (iterator.hasNext()) {
			QApplication::processEvents();

			if (this->m_Cancelled) {
				this->m_Archive.reset();
				return 0;
			}

			const QString& filepath = iterator.next();
			const bool ignored = this->m_ArchiveBuilderHelper->isFileIgnorable(filepath.toStdWString(), rootDirFiles);

			Q_EMIT this->valueChanged(++count);
			if (ignored) {
				continue;
			}

			++incompressibleFiles;
			auto fileBlob = disk_blob(
				 dirString,
				 filepath.toStdWString());
			this->m_Archive->add_file_from_disk(fileBlob);
			qDebug() << "file is: " << filepath;
		}
		this->m_Archive->set_compressed(false);
		return incompressibleFiles;
	}

	void MorrowindArchiveBuilder::setShareData(const bool value)
	{
		this->m_Archive->set_share_data(value);
	}

	std::unique_ptr<libbsarch::bs_archive_auto> MorrowindArchiveBuilder::getArchive()
	{
		return std::move(this->m_Archive);
	}

	uint32_t MorrowindArchiveBuilder::getFileCount() const
	{
		return this->m_ArchiveBuilderHelper->getFileCount(this->m_RootDirectory.path().toStdWString());
	}

	QString MorrowindArchiveBuilder::getRootPath() const
	{
		return this->m_RootDirectory.path();
	}

	void MorrowindArchiveBuilder::cancel()
	{
		this->m_Cancelled = true;
	}
} // namespace BsaPacker
