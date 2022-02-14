#include "bettertest_alexandria/alexandria_importer.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "bettertest/exceptions/import_error.h"
#include "bettertest/suite/suite_data.h"
#include "bettertest/suite/test_data.h"
#include "bettertest/suite/test_suite.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "bettertest_alexandria/library.h"

namespace
{
    std::shared_ptr<btalex::Suite> getSuite(const bt::TestSuite& suite, const btalex::SuiteHandler& handler)
    {
        auto       colName = handler->getPrimitiveColumn<1>();
        auto       query   = handler->find(colName == suite.getName());
        const auto suiteId = std::vector<alex::InstanceId>(query());

        // TODO: More than 1 suites with same name, throw error?
        if (suiteId.size() > 1) return nullptr;
        if (suiteId.empty()) return nullptr;
        return handler->get(suiteId.front());
    }

    std::shared_ptr<btalex::UnitTest>
      getUnitTest(const btalex::UnitTestHandler& handler, const btalex::Suite& suite, const std::string& test)
    {
        auto       colId   = handler->getPrimitiveColumn<1>();
        auto       colName = handler->getPrimitiveColumn<2>();
        auto       query   = handler->find(colId == suite.id.get() && colName == test);
        const auto testId  = std::vector<alex::InstanceId>(query());

        // TODO: More than 1 tests with same name, throw error?
        if (testId.size() > 1) { return nullptr; }
        if (testId.empty()) return nullptr;
        return handler->get(testId.front());
    }
}  // namespace

namespace bt
{
    AlexandriaImporter::AlexandriaImporter(std::filesystem::path directory) : IImporter(std::move(directory)) {}

    bool AlexandriaImporter::readSuiteFile(TestSuite& suite)
    {
        if (!exists(path / "suite.db")) return false;

        auto&      library         = btalex::getLibrary(path / "suite.db");
        const auto suiteHandler    = library.createObjectHandler<btalex::SuiteHandler>(library.getType("Suite"));
        const auto unitTestHandler = library.createObjectHandler<btalex::UnitTestHandler>(library.getType("UnitTest"));
        const auto resultHandler   = library.createObjectHandler<btalex::ResultHandler>(library.getType("Result"));
        const auto mixinHandler    = library.createObjectHandler<btalex::MixinHandler>(library.getType("Mixin"));

        const auto suiteObj = getSuite(suite, suiteHandler);
        if (!suiteObj) return true;

        suite.getData().name        = suiteObj->name;
        suite.getData().dateCreated = suiteObj->dateCreated;
        suite.getData().dateLastRun = suiteObj->dateLastRun;
        suite.getData().passing     = suiteObj->passing > 0;
        suite.getData().runIndex    = suiteObj->runIndex;
        suite.getData().version     = suiteObj->version;

        auto colId = unitTestHandler->getPrimitiveColumn<1>();
        auto query = unitTestHandler->find(colId == suiteObj->id.get());
        for (const auto id : query())
        {
            auto testObj      = unitTestHandler->get(id);
            auto test         = std::make_unique<TestData>();
            test->name        = testObj->name;
            test->dateCreated = testObj->dateCreated;
            test->dateLastRun = testObj->dateLastRun;
            test->passing     = testObj->passing > 0;
            suite.getUnitTestSuite().getData().emplace_back(std::move(test));
        }

        return true;
    }
}  // namespace bt
