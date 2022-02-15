#include "bettertest_alexandria/alexandria_exporter.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "bettertest/exceptions/export_error.h"
#include "bettertest/suite/suite_data.h"
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

        if (suiteId.size() > 1) throw bt::ExportError("More than 1 suite with the same name found in the library.");

        // Create new suite.
        if (suiteId.empty())
        {
            auto suiteObj  = handler->create();
            suiteObj->name = suite.getName();
            handler->update(*suiteObj);
            return suiteObj;
        }

        // Retrieve existing one.
        return handler->get(suiteId.front());
    }

    std::shared_ptr<btalex::UnitTest>
      getUnitTest(const btalex::UnitTestHandler& handler, const btalex::Suite& suite, const std::string& test)
    {
        const auto colSuite = handler->getPrimitiveColumn<1>();
        const auto colName  = handler->getPrimitiveColumn<2>();
        auto       query    = handler->find(colSuite == suite.id.get() && colName == test);
        const auto testId   = std::vector<alex::InstanceId>(query());

        if (testId.size() > 1) throw bt::ExportError("More than 1 unit test with the same name found in the library.");

        // Create new test.
        if (testId.empty())
        {
            auto testObj   = handler->create();
            testObj->suite = suite;
            testObj->name  = test;
            handler->update(*testObj);
            return testObj;
        }

        // Retrieve existing one.
        return handler->get(testId.front());
    }
}  // namespace

namespace bt
{
    AlexandriaExporter::AlexandriaExporter(std::filesystem::path directory) : IExporter(std::move(directory)) {}

    AlexandriaExporter::~AlexandriaExporter() noexcept = default;

    void AlexandriaExporter::writeSuite(const TestSuite& suite)
    {
        // Create directory if it does not exist.
        if (!exists(path)) create_directories(path);

        // Get library and object handlers.
        auto&      library         = btalex::getLibrary(path / "suite.db");
        const auto suiteHandler    = library.createObjectHandler<btalex::SuiteHandler>(library.getType("Suite"));
        const auto unitTestHandler = library.createObjectHandler<btalex::UnitTestHandler>(library.getType("UnitTest"));

        // Get or create suite object.
        const auto suiteObj = getSuite(suite, suiteHandler);

        // Write suite data.
        suiteObj->name        = suite.getName();
        suiteObj->dateCreated = suite.getData().dateCreated;
        suiteObj->dateLastRun = suite.getData().dateLastRun;
        suiteObj->passing     = suite.getData().passing ? 1 : 0;
        suiteObj->runIndex    = suite.getData().runIndex;
        suiteObj->version     = suite.getData().version;
        suiteHandler->update(*suiteObj);

        // Write unit test data.
        for (const auto& test : suite.getUnitTestSuite().getData())
        {
            // Get or create test object.
            auto testObj         = getUnitTest(unitTestHandler, *suiteObj, test->name);
            testObj->suite       = *suiteObj;
            testObj->name        = test->name;
            testObj->dateCreated = test->dateCreated;
            testObj->dateLastRun = test->dateLastRun;
            testObj->passing     = test->passing ? 1 : 0;
            unitTestHandler->update(*testObj);
        }
    }

    void
      AlexandriaExporter::writeUnitTestResults(const TestSuite& suite, const IUnitTest& test, const std::string& name)
    {
        // Get library and object handlers.
        auto&      library         = btalex::getLibrary(path / "suite.db");
        const auto suiteHandler    = library.createObjectHandler<btalex::SuiteHandler>(library.getType("Suite"));
        const auto unitTestHandler = library.createObjectHandler<btalex::UnitTestHandler>(library.getType("UnitTest"));
        const auto resultHandler   = library.createObjectHandler<btalex::ResultHandler>(library.getType("Result"));
        const auto mixinHandler    = library.createObjectHandler<btalex::MixinHandler>(library.getType("Mixin"));

        // Get or create suite and test.
        const auto suiteObj = getSuite(suite, suiteHandler);
        const auto testObj  = getUnitTest(unitTestHandler, *suiteObj, name);

        //
        const auto& mixins  = test.getMixins();
        const auto& getters = test.getResultsGetters();
        for (size_t i = 0; i < mixins.size(); i++)
        {
            const auto& getter = *getters[i];
            const auto& mixin  = mixins[i];

            // Create mixin.
            auto mixinObj      = mixinHandler->create();
            mixinObj->unitTest = *testObj;
            mixinObj->name     = mixin;
            mixinObj->runIndex = static_cast<uint32_t>(suite.getData().runIndex);
            mixinObj->stats    = {.total      = getter.getTotal(),
                               .successes  = getter.getSuccesses(),
                               .failures   = getter.getFailures(),
                               .exceptions = getter.getExceptions()};

            for (const auto& [status, location, error] : getter.getResults())
            {
                // Create result.
                auto resultObj = resultHandler->create();
                switch (status)
                {
                case result_t::success: resultObj->status = "success"; break;
                case result_t::failure: resultObj->status = "failure"; break;
                case result_t::exception: resultObj->status = "exception"; break;
                }
                resultObj->location.file   = location.file_name();
                resultObj->location.line   = location.line();
                resultObj->location.column = location.column();
                resultObj->error           = error;
                resultHandler->update(*resultObj);

                // Add result to mixin.
                mixinObj->results.add(*resultObj);
            }

            mixinHandler->update(*mixinObj);
        }
    }

    bool AlexandriaExporter::supportsMultithreading() const noexcept
    {
        // TODO: Implement thread-safe write methods.
        return false;
    }

}  // namespace bt