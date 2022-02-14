#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

namespace btalex
{
    // TODO: Make all uint32_t size_t?
    struct Suite
    {
        alex::InstanceId id;
        std::string      name;
        std::string      dateCreated;
        std::string      dateLastRun;
        uint32_t         passing;
        uint32_t         runIndex;
        std::string      version;
    };

    struct UnitTest
    {
        alex::InstanceId       id;
        alex::Reference<Suite> suite;
        std::string            name;
        std::string            dateCreated;
        std::string            dateLastRun;
        uint32_t               passing;
    };

    struct Location
    {
        std::string file;
        uint32_t    line;
        uint32_t    column;
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
        uint32_t total;
        uint32_t successes;
        uint32_t failures;
        uint32_t exceptions;
    };

    struct Mixin
    {
        alex::InstanceId             id;
        alex::Reference<UnitTest>    unitTest;
        std::string                  name;
        uint32_t                     runIndex;
        Stats                        stats;
        alex::ReferenceArray<Result> results;
    };

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
                                                alex::Member<&Suite::version>>;

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

    alex::Library& getLibrary(const std::filesystem::path& file);


}  // namespace btalex
