#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/output/exporter_interface.h"

namespace bt
{
    class AlexandriaExporter final : public IExporter
    {
    public:
        static constexpr char type[] = "alexandria";

        AlexandriaExporter() = delete;

        AlexandriaExporter(const AlexandriaExporter&) = delete;

        AlexandriaExporter(AlexandriaExporter&&) = delete;

        explicit AlexandriaExporter(std::filesystem::path directory);

        ~AlexandriaExporter() noexcept override;

        AlexandriaExporter& operator=(const AlexandriaExporter&) = delete;

        AlexandriaExporter& operator=(AlexandriaExporter&&) = delete;

        void writeSuite(const TestSuite& suite) override;

        void writeUnitTestResults(const TestSuite& suite, const IUnitTest& test, const std::string& name) override;
    };
}  // namespace bt
