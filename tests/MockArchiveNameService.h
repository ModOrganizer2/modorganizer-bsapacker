#include <bsapacker/IArchiveNameService.h>
#include <gmock/gmock.h>

using namespace BsaPacker;

class MockArchiveNameService : public IArchiveNameService
{
public:
	MOCK_METHOD(QString, GetFileExtension, (), (const, override));
	MOCK_METHOD(QString, GetArchiveFullPath, (bsa_archive_type_e, const IModDto*), (const, override));
};
