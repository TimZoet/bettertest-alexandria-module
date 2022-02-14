#include "bettertest_alexandria/library.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////


namespace btalex
{
    alex::Library& getLibrary(const std::filesystem::path& file)
    {
        static alex::LibraryPtr library;

        if (!library)
        {
            auto [l, created] = alex::Library::openOrCreate(file);
            library           = std::move(l);

            if (created)
            {
                auto& suite = library->createType("Suite");
                suite.createStringProperty("name");
                suite.createStringProperty("dateCreated");
                suite.createStringProperty("dateLastRun");
                suite.createPrimitiveProperty("passing", alex::DataType::Uint32);
                suite.createPrimitiveProperty("runIndex", alex::DataType::Uint32);
                suite.createStringProperty("version");

                auto& unitTest = library->createType("UnitTest");
                unitTest.createReferenceProperty("suite", suite);
                unitTest.createStringProperty("dateCreated");
                unitTest.createStringProperty("dateLastRun");
                unitTest.createStringProperty("name");
                unitTest.createPrimitiveProperty("passing", alex::DataType::Uint32);

                auto& location = library->createType("Location");
                location.createStringProperty("file");
                location.createPrimitiveProperty("line", alex::DataType::Uint32);
                location.createPrimitiveProperty("column", alex::DataType::Uint32);

                auto& result = library->createType("Result");
                result.createTypeProperty("location", location);
                result.createStringProperty("status");
                result.createStringProperty("error");

                auto& stats = library->createType("Stats");
                stats.createPrimitiveProperty("total", alex::DataType::Uint32);
                stats.createPrimitiveProperty("successes", alex::DataType::Uint32);
                stats.createPrimitiveProperty("failures", alex::DataType::Uint32);
                stats.createPrimitiveProperty("exceptions", alex::DataType::Uint32);

                auto& mixin = library->createType("Mixin");
                mixin.createReferenceProperty("unitTest", unitTest);
                mixin.createStringProperty("name");
                mixin.createPrimitiveProperty("runIndex", alex::DataType::Uint32);
                mixin.createTypeProperty("stats", stats);
                mixin.createReferenceArrayProperty("results", result);

                library->commitTypes();
            }
            else
            {
                // TODO: Validate library.
            }
        }

        return *library;
    }
}  // namespace bt