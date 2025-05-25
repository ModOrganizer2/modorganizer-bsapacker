#include <bsapacker/TextureArchiveBuilder.h>

#include <bsapacker/ArchiveBuilderHelper.h>
#include <QDirIterator>
#include <QApplication>
#include <QDebug>
#include "DirectXTex.h"

using namespace libbsarch;

namespace BsaPacker
{
	// 4 GiB limit for the Fallout 4 Creation Kit. The game itself has no limit so could make an optional setting
	// This does not consider size after compression or share data
	const qint64 TextureArchiveBuilder::SIZE_LIMIT = (qint64)1024 * 1024 * 1024 * 4;

	TextureArchiveBuilder::TextureArchiveBuilder(const IArchiveBuilderHelper* archiveBuilderHelper, const QDir& rootDir, const bsa_archive_type_t& type)
		: m_ArchiveBuilderHelper(archiveBuilderHelper), m_RootDirectory(rootDir), m_ArchiveType(type)
	{
		this->m_Cancelled = false;
		this->m_Archives.emplace_back(std::make_unique<libbsarch::bs_archive_auto>(this->m_ArchiveType));
	}

	uint32_t TextureArchiveBuilder::setFiles()
	{
		uint32_t compressibleFiles = 0;
		int count = 0;
		qint64 currentSize = 0;
		const auto& dirString = (this->m_RootDirectory.path() + '/').toStdWString();
		const auto& rootDirFiles = this->m_ArchiveBuilderHelper->getRootDirectoryFilenames(dirString);
		qDebug() << "root is: " << m_RootDirectory.path() + '/';

		QDirIterator iterator(this->m_RootDirectory.absolutePath(), QDir::Files, QDirIterator::Subdirectories);
		while (iterator.hasNext()) {
			QApplication::processEvents();

			if (this->m_Cancelled) {
				for (auto& archive : this->m_Archives) {
					archive.reset();
				}
				return 0;
			}

			const QFileInfo& fileInfo = iterator.nextFileInfo();
			const QString& filepath = fileInfo.absoluteFilePath();
			const bool ignored = this->m_ArchiveBuilderHelper->isFileIgnorable(filepath.toStdWString(), rootDirFiles);

			Q_EMIT this->valueChanged(++count);
			if (ignored || !filepath.endsWith(".dds", Qt::CaseInsensitive)) {
				continue;
			}

			currentSize += fileInfo.size();
			if (currentSize > SIZE_LIMIT) {
				currentSize = fileInfo.size();
				this->m_Archives.back()->set_compressed(true);
				this->m_Archives.back()->set_dds_callback(TextureArchiveBuilder::DDSCallback, this->getRootPath().toStdWString());
				compressibleFiles = 0;
				this->m_Archives.emplace_back(std::make_unique<libbsarch::bs_archive_auto>(this->m_ArchiveType));
				this->setShareData(true);
			}

			++compressibleFiles;
			auto fileBlob = disk_blob(
				 dirString,
				 filepath.toStdWString());
			this->m_Archives.back()->add_file_from_disk(fileBlob);
			qDebug() << "file is: " << filepath;
		}
		this->m_Archives.back()->set_compressed(true);
		this->m_Archives.back()->set_dds_callback(TextureArchiveBuilder::DDSCallback, this->getRootPath().toStdWString());
		return compressibleFiles;
	}

	void TextureArchiveBuilder::setShareData(const bool value)
	{
		this->m_Archives.back()->set_share_data(value);
	}

	std::vector<std::unique_ptr<libbsarch::bs_archive_auto>> TextureArchiveBuilder::getArchives()
	{
		return std::move(this->m_Archives);
	}

	uint32_t TextureArchiveBuilder::getFileCount() const
	{
		return this->m_ArchiveBuilderHelper->getFileCount(this->m_RootDirectory.path().toStdWString());
	}

	QString TextureArchiveBuilder::getRootPath() const
	{
		return this->m_RootDirectory.path();
	}

	void TextureArchiveBuilder::cancel()
	{
		this->m_Cancelled = true;
	}

	void TextureArchiveBuilder::DDSCallback(bsa_archive_t, const wchar_t* file_path, bsa_dds_info_t* dds_info, void* context)
	{
		const auto& path = *static_cast<std::wstring*>(context) + L'/' + std::wstring(file_path);

		auto image = std::make_unique<DirectX::ScratchImage>();
		DirectX::TexMetadata info;

		const auto hr = LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, &info, *image);

		if (FAILED(hr))
			throw std::runtime_error("Failed to open DDS");

		dds_info->width = static_cast<uint32_t>(info.width);
		dds_info->height = static_cast<uint32_t>(info.height);
		dds_info->mipmaps = static_cast<uint32_t>(info.mipLevels);
	}
} // namespace BsaPacker
