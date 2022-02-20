#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

namespace btalex
{
    struct Version
    {
        uint32_t major = 0;
        uint32_t minor = 0;
        uint32_t patch = 0;
    };

    struct Suite
    {
        alex::InstanceId id;
        std::string      name;
        std::string      dateCreated;
        std::string      dateLastRun;
        uint64_t         passing  = 0;
        uint64_t         runIndex = 0;
        Version          version;
    };

    struct UnitTest
    {
        alex::InstanceId       id;
        alex::Reference<Suite> suite;
        std::string            name;
        std::string            dateCreated;
        std::string            dateLastRun;
        uint64_t               passing = 0;
    };

    struct Location
    {
        std::string file;
        uint64_t    line   = 0;
        uint64_t    column = 0;
    };

    struct Result
    {
        alex::InstanceId id;
        Location         location;
        std::string      status;
        std::string      error;
    };

    struct Stats
    {
        uint64_t total      = 0;
        uint64_t successes  = 0;
        uint64_t failures   = 0;
        uint64_t exceptions = 0;
    };

    struct Mixin
    {
        alex::InstanceId             id;
        alex::Reference<UnitTest>    unitTest;
        std::string                  name;
        uint64_t                     runIndex = 0;
        Stats                        stats;
        alex::ReferenceArray<Result> results;
    };

    using version_t =
      alex::MemberList<alex::Member<&Version::major>, alex::Member<&Version::minor>, alex::Member<&Version::patch>>;

    using location_t =
      alex::MemberList<alex::Member<&Location::file>, alex::Member<&Location::line>, alex::Member<&Location::column>>;

    using result_t = alex::MemberList<alex::NestedMember<location_t, &Result::location>,
                                      alex::Member<&Result::status>,
                                      alex::Member<&Result::error>>;

    using stats_t = alex::MemberList<alex::Member<&Stats::total>,
                                     alex::Member<&Stats::successes>,
                                     alex::Member<&Stats::failures>,
                                     alex::Member<&Stats::exceptions>>;

    using SuiteHandler = alex::object_handler_t<alex::Member<&Suite::id>,
                                                alex::Member<&Suite::name>,
                                                alex::Member<&Suite::dateCreated>,
                                                alex::Member<&Suite::dateLastRun>,
                                                alex::Member<&Suite::passing>,
                                                alex::Member<&Suite::runIndex>,
                                                alex::NestedMember<version_t, &Suite::version>>;

    using UnitTestHandler = alex::object_handler_t<alex::Member<&UnitTest::id>,
                                                   alex::Member<&UnitTest::suite>,
                                                   alex::Member<&UnitTest::name>,
                                                   alex::Member<&UnitTest::dateCreated>,
                                                   alex::Member<&UnitTest::dateLastRun>,
                                                   alex::Member<&UnitTest::passing>>;

    using ResultHandler = alex::object_handler_t<alex::Member<&Result::id>,
                                                 alex::NestedMember<location_t, &Result::location>,
                                                 alex::Member<&Result::status>,
                                                 alex::Member<&Result::error>>;

    using MixinHandler = alex::object_handler_t<alex::Member<&Mixin::id>,
                                                alex::Member<&Mixin::unitTest>,
                                                alex::Member<&Mixin::name>,
                                                alex::Member<&Mixin::runIndex>,
                                                alex::NestedMember<stats_t, &Mixin::stats>,
                                                alex::Member<&Mixin::results>>;

    alex::Library& getLibrary(const std::filesystem::path& dir, const std::filesystem::path& file);
}  // namespace btalex
