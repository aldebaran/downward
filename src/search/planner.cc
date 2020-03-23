#include "planner.h"
#include "command_line.h"
#include "option_parser.h"
#include "search_engine.h"

#include "options/registries.h"
#include "tasks/root_task.h"
#include "task_utils/task_properties.h"
#include "../utils/logging.h"
#include "utils/system.h"
#include "utils/timer.h"

#include <iostream>
#include <sstream>

using namespace std;
using utils::ExitCode;

int main(int argc, const char **argv) {
    utils::register_event_handlers();

    if (argc < 2) {
        utils::g_log << usage(argv[0]) << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }

    bool unit_cost = false;
    if (static_cast<string>(argv[1]) != "--help") {
        utils::g_log << "reading input..." << endl;
        tasks::read_root_task(cin);
        utils::g_log << "done reading input!" << endl;
        TaskProxy task_proxy(*tasks::g_root_task);
        unit_cost = task_properties::is_unit_cost(task_proxy);
    }

    shared_ptr<SearchEngine> engine;

    // The command line is parsed twice: once in dry-run mode, to
    // check for simple input errors, and then in normal mode.
    try {
        options::Registry registry(*options::RawRegistry::instance());
        parse_cmd_line(argc, argv, registry, true, unit_cost);
        engine = parse_cmd_line(argc, argv, registry, false, unit_cost);
    } catch (const ArgError &error) {
        error.print();
        usage(argv[0]);
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    } catch (const OptionParserError &error) {
        error.print();
        usage(argv[0]);
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    } catch (const ParseError &error) {
        error.print();
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }

    utils::Timer search_timer;
    engine->search();
    search_timer.stop();
    utils::g_timer.stop();

    engine->save_plan_if_necessary();
    engine->print_statistics();
    utils::g_log << "Search time: " << search_timer << endl;
    utils::g_log << "Total time: " << utils::g_timer << endl;

    ExitCode exitcode = engine->found_solution()
        ? ExitCode::SUCCESS
        : ExitCode::SEARCH_UNSOLVED_INCOMPLETE;
    utils::report_exit_code_reentrant(exitcode);
    return static_cast<int>(exitcode);
}

std::string plan_from_sas(const std::string& sas, const std::string& search_strategy) {
    // Parse the input SAS.
    std::istringstream string_input(sas);
    std::istream input(string_input.rdbuf());
    cout << "reading input... [t=" << utils::g_timer << "]" << endl;
    tasks::read_root_task(input);
    cout << "done reading input! [t=" << utils::g_timer << "]" << endl;
    TaskProxy task_proxy(*tasks::g_root_task);

    // Setup the search engine according to provided strategy.
    options::Registry registry(*options::RawRegistry::instance());
    options::Predefinitions predefinitions;
    static const bool dry_run = false;
    OptionParser parser(sanitize_arg_string(search_strategy), registry, predefinitions, dry_run);
    shared_ptr<SearchEngine> engine = parser.start_parsing<shared_ptr<SearchEngine>>();

    if (!engine) {
        throw std::runtime_error(
                std::string("Search engine specification \"")
                + search_strategy + "was not recognized");
    }

    PlanManager &plan_manager = engine->get_plan_manager();
    static const int num_previously_generated_plans = 0;
    plan_manager.set_num_previously_generated_plans(num_previously_generated_plans);
    static const bool is_part_of_anytime_portfolio = false;
    plan_manager.set_is_part_of_anytime_portfolio(is_part_of_anytime_portfolio);
    utils::Timer search_timer;
    engine->search();
    search_timer.stop();
    utils::g_timer.stop();
    engine->print_statistics();
    cout << "Search time: " << search_timer << endl;
    cout << "Total time: " << utils::g_timer << endl;

    switch (engine->get_status()) {
        case IN_PROGRESS:
            assert(false && "search is still in progress");
            throw std::runtime_error("search is still in progress");
        case TIMEOUT:
            throw std::runtime_error("search took too long");
        case FAILED:
            throw std::runtime_error("search failed, no solution found");
        case SOLVED:
            break;
    }

    if (!engine->found_solution()) {
        throw std::runtime_error("no solution found");
    }

    std::stringstream plan_ss;
    plan_manager.save_plan_to(engine->get_plan(), task_proxy, plan_ss);
    return plan_ss.str();
}