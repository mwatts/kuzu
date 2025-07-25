#include <filesystem>
#include <fstream>

#include "c_api/kuzu.h"
#include "c_api_test/c_api_test.h"
#include "gtest/gtest.h"

using namespace kuzu::main;
using namespace kuzu::testing;
using namespace kuzu::common;

class CApiVersionTest : public CApiTest {
public:
    std::string getInputDir() override {
        return TestHelper::appendKuzuRootPath("dataset/tinysnb/");
    }
};

TEST_F(CApiVersionTest, GetVersion) {
    auto version = kuzu_get_version();
    ASSERT_NE(version, nullptr);
    ASSERT_STREQ(version, KUZU_CMAKE_VERSION);
    kuzu_destroy_string(version);
}

TEST_F(CApiVersionTest, GetStorageVersion) {
    auto storageVersion = kuzu_get_storage_version();
    if (databasePath == "" || databasePath == ":memory:") {
        return;
    }
    auto data = std::filesystem::path(databasePath);
    std::ifstream catalogFile;
    catalogFile.open(data, std::ios::binary);
    char magic[5];
    catalogFile.read(magic, 4);
    magic[4] = '\0';
    ASSERT_STREQ(magic, "KUZU");
    uint64_t actualVersion;
    catalogFile.read(reinterpret_cast<char*>(&actualVersion), sizeof(actualVersion));
    catalogFile.close();
    ASSERT_EQ(storageVersion, actualVersion);
}
