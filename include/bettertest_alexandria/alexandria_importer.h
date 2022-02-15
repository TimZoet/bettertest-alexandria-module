#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/output/importer_interface.h"

namespace bt
{
    class AlexandriaImporter final : public IImporter
    {
    public:
        static constexpr char type[] = "alexandria";

        AlexandriaImporter() = delete;

        AlexandriaImporter(const AlexandriaImporter&) = delete;

        AlexandriaImporter(AlexandriaImporter&&) = delete;

        explicit AlexandriaImporter(std::filesystem::path directory);

        ~AlexandriaImporter() noexcept override = default;

        AlexandriaImporter& operator=(const AlexandriaImporter&) = delete;

        AlexandriaImporter& operator=(AlexandriaImporter&&) = delete;

        bool readSuite(TestSuite& suite) override;
    };
}  // namespace bt
