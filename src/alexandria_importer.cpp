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

        if (suiteId.size() > 1) throw bt::ImportError("More than 1 suite with the same name found in the library.");
        if (suiteId.empty()) return nullptr;
        return handler->get(suiteId.front());
    }

    std::vector<alex::InstanceId> getUnitTests(const btalex::Suite& suite, const btalex::UnitTestHandler& handler)
    {
        auto colId = handler->getPrimitiveColumn<1>();
        auto query = handler->find(colId == suite.id.get());
        return query();
    }
}  // namespace

namespace bt
{
    AlexandriaImporter::AlexandriaImporter(std::filesystem::path directory) : IImporter(std::move(directory)) {}

    bool AlexandriaImporter::readSuite(TestSuite& suite)
    {
        if (!exists(path / "suite.db")) return false;

        // Get library and object handlers.
        auto&      library         = btalex::getLibrary(path / "suite.db");
        const auto suiteHandler    = library.createObjectHandler<btalex::SuiteHandler>(library.getType("Suite"));
        const auto unitTestHandler = library.createObjectHandler<btalex::UnitTestHandler>(library.getType("UnitTest"));

        // Try to retrieve suite object with matching name.
        const auto suiteObj = getSuite(suite, suiteHandler);
        if (!suiteObj) return false;

        // Retrieve suite information.
        suite.getData().name        = suiteObj->name;
        suite.getData().dateCreated = suiteObj->dateCreated;
        suite.getData().dateLastRun = suiteObj->dateLastRun;
        suite.getData().passing     = suiteObj->passing > 0;
        suite.getData().runIndex    = suiteObj->runIndex;
        suite.getData().version     = suiteObj->version;

        // Retrieve all unit test information.
        for (const auto id : getUnitTests(*suiteObj, unitTestHandler))
        {
            const auto testObj = unitTestHandler->get(id);
            auto&      test    = *suite.getUnitTestSuite().getData().emplace_back(std::make_unique<TestData>());
            test.name          = testObj->name;
            test.dateCreated   = testObj->dateCreated;
            test.dateLastRun   = testObj->dateLastRun;
            test.passing       = testObj->passing > 0;
        }

        return true;
    }
}  // namespace bt
