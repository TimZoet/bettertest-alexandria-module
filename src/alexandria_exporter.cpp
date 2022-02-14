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

        // TODO: More than 1 suites with same name, throw error?
        if (suiteId.size() > 1) { return nullptr; }

        // Create new suite or retrieve existing one.
        if (suiteId.empty())
        {
            auto suiteObj  = handler->create();
            suiteObj->name = suite.getName();
            handler->update(*suiteObj);
            return suiteObj;
        }

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

        // Create new test or retrieve existing one.
        if (testId.empty())
        {
            auto testObj   = handler->create();
            testObj->suite = suite;
            testObj->name  = test;
            handler->update(*testObj);
            return testObj;
        }

        return handler->get(testId.front());
    }
}  // namespace

namespace bt
{
    AlexandriaExporter::AlexandriaExporter(std::filesystem::path directory) : IExporter(std::move(directory)) {}

    AlexandriaExporter::~AlexandriaExporter() noexcept = default;

    void AlexandriaExporter::writeSuiteFile(const TestSuite& suite)
    {
        // Create directory if it does not exist.
        if (!exists(path)) create_directories(path);

        auto&      library         = btalex::getLibrary(path / "suite.db");
        const auto suiteHandler    = library.createObjectHandler<btalex::SuiteHandler>(library.getType("Suite"));
        const auto unitTestHandler = library.createObjectHandler<btalex::UnitTestHandler>(library.getType("UnitTest"));

        const auto suiteObj = getSuite(suite, suiteHandler);

        // Write suite data.
        suiteObj->name        = suite.getName();
        suiteObj->dateCreated = suite.getData().dateCreated;
        suiteObj->dateLastRun = suite.getData().dateLastRun;
        suiteObj->passing     = suite.getData().passing ? 1 : 0;
        suiteObj->runIndex    = static_cast<uint32_t>(suite.getData().runIndex);
        suiteObj->version     = suite.getData().version;
        suiteHandler->update(*suiteObj);

        // Write test data.
        for (const auto& test : suite.getUnitTestSuite().getData())
        {
            auto testObj         = getUnitTest(unitTestHandler, *suiteObj, test->name);
            testObj->suite       = *suiteObj;
            testObj->name        = test->name;
            testObj->dateCreated = test->dateCreated;
            testObj->dateLastRun = test->dateLastRun;
            testObj->passing     = test->passing ? 1 : 0;
            unitTestHandler->update(*testObj);
        }
    }

    void AlexandriaExporter::writeUnitTestFile(const TestSuite& suite, const IUnitTest& test, const std::string& name)
    {
        auto&      library         = btalex::getLibrary(path / "suite.db");
        const auto suiteHandler    = library.createObjectHandler<btalex::SuiteHandler>(library.getType("Suite"));
        const auto unitTestHandler = library.createObjectHandler<btalex::UnitTestHandler>(library.getType("UnitTest"));
        const auto resultHandler   = library.createObjectHandler<btalex::ResultHandler>(library.getType("Result"));
        const auto mixinHandler    = library.createObjectHandler<btalex::MixinHandler>(library.getType("Mixin"));

        const auto suiteObj = getSuite(suite, suiteHandler);
        const auto testObj  = getUnitTest(unitTestHandler, *suiteObj, name);

        const auto& mixins  = test.getMixins();
        const auto& getters = test.getResultsGetters();
        for (size_t i = 0; i < mixins.size(); i++)
        {
            const auto& getter = *getters[i];
            const auto& mixin  = mixins[i];

            auto mixinObj      = mixinHandler->create();
            mixinObj->unitTest = *testObj;
            mixinObj->name     = mixin;
            mixinObj->runIndex = static_cast<uint32_t>(suite.getData().runIndex);
            mixinObj->stats    = {.total      = static_cast<uint32_t>(getter.getTotal()),
                               .successes  = static_cast<uint32_t>(getter.getSuccesses()),
                               .failures   = static_cast<uint32_t>(getter.getFailures()),
                               .exceptions = static_cast<uint32_t>(getter.getExceptions())};

            for (const auto& res : getter.getResults())
            {
                auto resultObj = resultHandler->create();
                switch (res.status)
                {
                case result_t::success: resultObj->status = "success"; break;
                case result_t::failure: resultObj->status = "failure"; break;
                case result_t::exception: resultObj->status = "exception"; break;
                }
                resultObj->location.file   = res.location.file_name();
                resultObj->location.line   = res.location.line();
                resultObj->location.column = res.location.column();
                resultObj->error           = res.error;
                resultHandler->update(*resultObj);

                mixinObj->results.add(*resultObj);
            }

            mixinHandler->update(*mixinObj);
        }


#if 0
        // Create directory if it does not exist.
        const auto testPath = this->path / name;
        if (!exists(testPath)) create_directories(testPath);

        // Write test results.
        nlohmann::json data;
        const auto&    mixins  = test.getMixins();
        const auto&    getters = test.getResultsGetters();
        for (size_t i = 0; i < mixins.size(); i++)
        {
            const auto& getter = *getters[i];
            const auto& mixin  = mixins[i];
            data[mixin]        = getter;
        }

        // Generate filename as "unit_########.json".
        const std::string filename = std::format("unit_{:0>8}.json", suite.getData().runIndex);

        // Write JSON data to file.
        std::ofstream file(testPath / filename);
        if (!file) throw ExportError("Failed to open unit test file");
        file << data;
        file.close();
#endif
    }

    bool AlexandriaExporter::supportsMultithreading() const noexcept
    {
        // TODO: Implement thread-safe write methods.
        return false;
    }

}  // namespace bt