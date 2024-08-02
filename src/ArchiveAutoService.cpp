#include <bsapacker/ArchiveAutoService.h>
#include <QProgressDialog>
#include <QtConcurrent>
#include <QLabel>

namespace BsaPacker
{
	bool ArchiveAutoService::CreateBSA(libbsarch::bs_archive_auto* archive, const QString& archiveName, const bsa_archive_type_e) const
	{
		QProgressDialog savingDialog;
		savingDialog.setWindowFlags(savingDialog.windowFlags() & ~Qt::WindowCloseButtonHint);
		savingDialog.setWindowTitle(QObject::tr("Writing Archive"));
		savingDialog.setCancelButton(0);
		QLabel text;
		text.setText(QObject::tr("Writing %1").arg(archiveName));
		savingDialog.setLabel(&text);
		savingDialog.setRange(0, 0);
		savingDialog.show();
		auto future = QtConcurrent::run([=]() -> bool {
			try {
				archive->save_to_disk(archiveName.toStdString());
			} catch (std::exception e) {
				return false;
			}
			return true;
			});
		while (!future.isFinished())
		{
			QCoreApplication::processEvents();
		}
		savingDialog.hide();
		return future.result();
	}
} // namespace BsaPacker
