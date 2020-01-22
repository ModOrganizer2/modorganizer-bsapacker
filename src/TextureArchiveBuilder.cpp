#include <bsapacker/TextureArchiveBuilder.h>

#include <bsapacker/ArchiveBuilderHelper.h>
#include <QDirIterator>
#include <QApplication>
#include "DirectXTex.h"

namespace BsaPacker
{
	TextureArchiveBuilder::TextureArchiveBuilder(const IArchiveBuilderHelper* archiveBuilderHelper, const QDir& rootDir)
		: m_ArchiveBuilderHelper(archiveBuilderHelper), m_RootDirectory(rootDir)
	{
		this->m_Cancelled = false;
		this->m_Archive = std::make_unique<BSArchiveAuto>(this->m_RootDirectory.path());
	}

	uint32_t TextureArchiveBuilder::setFiles()
	{
		uint32_t incompressibleFiles = 0;
		uint32_t compressibleFiles = 0;
		int count = 0;
		const QStringList& rootDirFiles = this->m_ArchiveBuilderHelper->getRootDirectoryFilenames(this->m_RootDirectory);
		QDirIterator iterator(this->m_RootDirectory, QDirIterator::Subdirectories);
		while (iterator.hasNext()) {
			QApplication::processEvents();

			if (this->m_Cancelled) {
				this->m_Archive.reset();
				return 0;
			}

			const QString& filepath = iterator.next();
			const bool ignored = this->m_ArchiveBuilderHelper->isFileIgnorable(filepath, rootDirFiles);

			Q_EMIT this->valueChanged(++count);
			if (ignored || !filepath.endsWith(".dds", Qt::CaseInsensitive)) {
				continue;
			}

			this->m_ArchiveBuilderHelper->isIncompressible(filepath) ? ++incompressibleFiles : ++compressibleFiles;
			this->m_Archive->addFileFromDiskRoot(filepath);
		}
		this->m_Archive->setCompressed(true);
		this->m_Archive->setDDSCallback(TextureArchiveBuilder::DDSCallback, (void*)this);
		return incompressibleFiles + compressibleFiles;
	}

	void TextureArchiveBuilder::setShareData(const bool value)
	{
		this->m_Archive->setShareData(value);
	}

	std::unique_ptr<BSArchiveAuto> TextureArchiveBuilder::getArchive()
	{
		return std::move(this->m_Archive);
	}

	uint32_t TextureArchiveBuilder::getFileCount() const
	{
		return this->m_ArchiveBuilderHelper->getFileCount(this->m_RootDirectory);
	}

	QString TextureArchiveBuilder::getRootPath() const
	{
		return this->m_RootDirectory.path();
	}

	void TextureArchiveBuilder::cancel()
	{
		this->m_Cancelled = true;
	}

	void TextureArchiveBuilder::DDSCallback(bsa_archive_t archive, const wchar_t* file_path, bsa_dds_info_t* dds_info, void* context)
	{
		auto image = std::make_unique<DirectX::ScratchImage>();
		DirectX::TexMetadata info;

		auto builder = (TextureArchiveBuilder*)context;
		const QString qsRootPath = builder->getRootPath();
		const std::wstring wsRootPath(PREPARE_PATH_LIBBSARCH(qsRootPath));
		const std::wstring path = wsRootPath + L"\\" + std::wstring(file_path);

		const auto hr = LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, &info, *image);

		if (FAILED(hr))
			throw std::runtime_error("Failed to open DDS");

		dds_info->width = static_cast<uint32_t>(info.width);
		dds_info->height = static_cast<uint32_t>(info.height);
		dds_info->mipmaps = static_cast<uint32_t>(info.mipLevels);
	}
} // namespace BsaPacker
