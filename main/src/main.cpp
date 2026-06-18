// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleVM/core/oop/array_view.h>

#include <BibbleVM/linker/linker.h>

#include <BibbleVM/init.h>
#include <BibbleVM/vm.h>

#include <libos/file.h>

#include <fstream>
#include <functional>
#include <optional>
#include <ranges>
#include <unordered_map>

using namespace bibblevm;
using namespace std::string_view_literals;

enum class ValueType {
    Bool,
    Integer,
    Float,
    Size,
    Duration,
    String
};

struct ConfigOption {
    std::string key;
    std::string description;
    ValueType type;
    bool optionalValue;
    std::function<void(Config&, std::optional<std::string_view>)> apply;
};

struct Options {
    Config config;
    std::optional<std::string> configProfile;

    std::optional<std::string> moduleOrExecutable;
    std::vector<std::string> modulePath;

    std::vector<std::string> programArgs; // args for bytecode program. whatever comes after -- in cli

    // Redirection of stdio to/from files
    std::optional<std::string> redirectStdIn;
    std::optional<std::string> redirectStdOut;
    std::optional<std::string> redirectStdErr;

    bool showHelp = false;
    bool showVersion = false;

    // clean up as much memory as possible
    void clear() {
        configProfile.reset();
        moduleOrExecutable.reset();
        modulePath.clear();
        programArgs = std::vector<std::string>();
        redirectStdIn.reset();
        redirectStdOut.reset();
        redirectStdErr.reset();
    }
};

static bool EndsWithIgnoreCase(std::string_view str, std::string_view suffix) {
    if (suffix.size() > str.size()) return false;

    std::string_view::size_type start = str.size() - suffix.size();

    for (size_t i = 0; i < suffix.size(); i++) {
        unsigned char a = static_cast<unsigned char>(str[start + i]);
        unsigned char b = static_cast<unsigned char>(suffix[i]);

        if (std::tolower(a) != std::tolower(b)) return false;
    }

    return true;
}

static bool EqualsIgnoreCase(std::string_view str1, std::string_view str2) {
    if (str1.size() != str2.size()) return false;

    for (size_t i = 0; i < str1.size(); i++) {
        unsigned char a = static_cast<unsigned char>(str1[i]);
        unsigned char b = static_cast<unsigned char>(str2[i]);
        if (std::tolower(a) != std::tolower(b)) return false;
    }

    return true;
}

static bool ParseBool(std::string_view str) {
    return str == "1"sv || EqualsIgnoreCase(str, "true"sv) || EqualsIgnoreCase(str, "yes"sv) || EqualsIgnoreCase(str, "on"sv);
}

static bool ParseBool(std::optional<std::string_view> str) {
    return !str.has_value() || ParseBool(*str);
}

static uint64_t ParseInteger(std::string_view str) {
    return std::stoull(std::string(str));
}

static double ParseFloat(std::string_view str) {
    return std::stod(std::string(str));
}

static size_t ParseSize(std::string_view str) {
    size_t multiplier = 1;

    if (EndsWithIgnoreCase(str, "k"sv)) {
        multiplier = 1024;
        str.remove_suffix(1);
    } else if (EndsWithIgnoreCase(str, "m"sv)) {
        multiplier = 1024 * 1024;
        str.remove_suffix(1);
    } else if (EndsWithIgnoreCase(str, "g"sv)) {
        multiplier = 1024 * 1024 * 1024;
        str.remove_suffix(1);
    }

    return std::stoull(std::string(str)) * multiplier;
}

static Nanoseconds ParseDuration(std::string_view str) {
    uint64_t multiplier = 1;

    if (EndsWithIgnoreCase(str, "us"sv)) {
        multiplier = 1'000;
        str.remove_suffix(2);
    } else if (EndsWithIgnoreCase(str, "ms"sv)) {
        multiplier = 1'000'000;
        str.remove_suffix(2);
    } else if (EndsWithIgnoreCase(str, "s"sv)) {
        multiplier = 1'000'000'000;
        str.remove_suffix(1);
    }

    return Nanoseconds(std::stoull(std::string(str)) * multiplier);
}

static std::vector<ConfigOption> BuildConfigOptionTable() {
    return {
        // memory

        {
            "memory.nursery-space-min",
            "minimum size for nursery spaces",
            ValueType::Size,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.nurseryMinSize = ParseSize(*value);
            }
        },

        {
            "memory.nursery-space-max",
            "maximum size for nursery spaces",
            ValueType::Size,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.nurseryMaxSize = ParseSize(*value);
            }
        },

        {
            "memory.nursery-growth-factor",
            "factor of growth for nursery spaces when resize happens",
            ValueType::Float,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.nurseryGrowthFactor = ParseFloat(*value);
            }
        },

        {
            "memory.nursery-growth-threshold",
            "nursery growth threshold measured in how much of the to-space is filled after collection",
            ValueType::Float,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.nurseryGrowthThreshold = ParseFloat(*value);
            }
        },

        {
            "memory.old-gen-min",
            "minimum size for the old generation heap",
            ValueType::Size,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.oldGenHeapMinSize = ParseSize(*value);
            }
        },

        {
            "memory.old-gen-max",
            "maximum size for the old generation heap",
            ValueType::Size,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.oldGenHeapMaxSize = ParseSize(*value);
            }
        },

        {
            "memory.old-gen-region-size",
            "minimum size for an old generation heap region",
            ValueType::Size,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.oldGenHeapRegionSize = ParseSize(*value);
            }
        },

        {
            "memory.old-gen-growth-factor",
            "factor of growth for the old generation heap when resize happens",
            ValueType::Float,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.oldGenHeapGrowthFactor = ParseFloat(*value);
            }
        },

        {
            "memory.large-object-threshold",
            "how large an allocation should be before it's considered a large object",
            ValueType::Size,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.largeObjectThreshold = ParseSize(*value);
            }
        },

        {
            "memory.implicit-intern-strings",
            "implicitly intern allocated strings to optimize memory usage at the cost of speed",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.implicitInternStrings = ParseBool(value);
            }
        },

        {
            "memory.enable-large-heap",
            "allocate large objects in a separate heap rather than in the old generation heap",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.memory.enableLargeHeap = ParseBool(value);
            }
        },

        // gc

        {
            "gc.pause-budget",
            "how long the program may be paused after reaching a safe point for the garbage collector",
            ValueType::Duration,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.gc.pauseBudget = ParseDuration(*value);
            }
        },

        {
            "gc.remembered-set-reserve",
            "how many spaces are initially reserved in the remembered set of old -> young references",
            ValueType::Integer,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.gc.rememberedSetReserve = ParseInteger(*value);
            }
        },

        {
            "gc.nursery-collect-threshold",
            "how full should the nursery be before collection",
            ValueType::Float,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.gc.nurseryCollectionThreshold = ParseFloat(*value);
            }
        },

        {
            "gc.old-gen-collect-threshold",
            "how full should the old generation heap before collection",
            ValueType::Float,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.gc.oldGenHeapCollectionThreshold = ParseFloat(*value);
            }
        },

        {
            "gc.old-gen-compact-threshold",
            "how fragmented should the old generation heap before compaction",
            ValueType::Float,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.gc.oldGenHeapCompactThreshold = ParseFloat(*value);
            }
        },

        {
            "gc.promotion-age",
            "how many nursery cycles should an object survive before promotion to the old generation heap",
            ValueType::Integer,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.gc.promotionAge = ParseInteger(*value);
            }
        },

        {
            "gc.disable-finalizers",
            "prevent the garbage collector from running finalizer methods; potentially unsafe",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.gc.disableFinalizers = ParseBool(value);
            }
        },

        {
            "gc.enable-concurrent-gc",
            "enable concurrent garbage collection (CURRENTLY UNIMPLEMENTED)",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.gc.enableConcurrentGC = ParseBool(value);
            }
        },

        // scheduler

        {
            "scheduler.max-tasks",
            "how many concurrent tasks can exist before the scheduler raises a VM-level error",
            ValueType::Integer,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.scheduler.maxTasks = ParseInteger(*value);
            }
        },

        {
            "scheduler.max-stack-size",
            "hard limit for a stacks size measured in bytes",
            ValueType::Size,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.scheduler.maxStackSize = ParseSize(*value);
            }
        },

        {
            "scheduler.auto-yielding.enabled",
            "enable auto yielding, which will automatically try to yield at certain points or after a certain instruction threshold",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.scheduler.autoYielding.enabled = ParseBool(value);
            }
        },

        {
            "scheduler.auto-yielding.threshold",
            "if auto yielding is enabled, this specifies how many instructions can run before auto yielding",
            ValueType::Integer,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.scheduler.autoYielding.threshold = ParseInteger(*value);
            }
        },

        {
            "scheduler.priority.use-exponential-weights",
            "use exponential weights instead of linear weights in the priority scheduler. Exponential weights mean higher priority tasks get way more execution time compared to linear",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.scheduler.priority.useExponentialWeights = ParseBool(value);
            }
        },

        {
            "scheduler.priority.levels",
            "how many levels of priority the scheduler has; 1 means no priority system",
            ValueType::Integer,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.scheduler.priority.levels = ParseInteger(*value);
            }
        },

        {
            "scheduler.priority.boost",
            "exponent value for generating weights for each priority level",
            ValueType::Integer,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.scheduler.priority.boost = ParseInteger(*value);
            }
        },

        // compiler

        {
            "compiler.aot.enabled",
            "enable ahead-of-time compilation of functions",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.compiler.aot.enabled = ParseBool(value);
            }
        },

        {
            "compiler.jit.enabled",
            "enable just-in-time compilation of functions",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.compiler.jit.enabled = ParseBool(value);
            }
        },

        {
            "compiler.jit.hot-threshold",
            "how many invocations a function has before it is considered hot. A hot function is a target for JIT compilation",
            ValueType::Integer,
            false,
            [](Config& config, std::optional<std::string_view> value) {
                config.compiler.jit.hotThreshold = ParseInteger(*value);
            }
        },

        // debug

        {
            "debug.enable-debug-logging",
            "enable debug logging to stdout",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.debug.enableDebugLogging = ParseBool(value);
            }
        },

        {
            "debug.verify-heap-after-gc",
            "verify the integrity of a heap after garbage collection",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.debug.verifyHeapAfterGC = ParseBool(value);
            }
        },

        {
            "debug.poison-freed-memory",
            "poison freed memory with predictable bytes",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.debug.poisonFreedMemory = ParseBool(value);
            }
        },

        {
            "debug.log-promotions",
            "log object promotions to stdout if debug logging is enabled",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.debug.logPromotions = ParseBool(value);
            }
        },

        {
            "debug.log-safepoints",
            "log GC safepoints to stdout if debug logging is enabled",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.debug.logSafepoints = ParseBool(value);
            }
        },

        {
            "debug.verbose-scheduler-logging",
            "enable verbose logging for the scheduler",
            ValueType::Bool,
            true,
            [](Config& config, std::optional<std::string_view> value) {
                config.debug.verboseSchedulerLogging = ParseBool(value);
            }
        }
    };
}

static std::unordered_map<std::string, ConfigOption*> BuildConfigOptionMap(std::vector<ConfigOption>& options) {
    std::unordered_map<std::string, ConfigOption*> map;
    map.reserve(options.size());

    for (auto& option : options) {
        map[option.key] = &option;
    }

    return map;
}

static bool ApplyConfigOption(Config& config, const std::unordered_map<std::string, ConfigOption*>& configOptions, const std::string& key, std::optional<std::string_view> value) {
    auto it = configOptions.find(key);
    if (it == configOptions.end()) return false;

    if (!it->second->optionalValue && !value.has_value()) return false;

    it->second->apply(config, value);

    return true;
}

static void ParseConfigFile(Config& config, const std::unordered_map<std::string, ConfigOption*>& configOptions, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return;

    auto trim = [](std::string& s) {
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
            s.erase(s.begin());
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
            s.pop_back();
    };

    std::string line;
    while (std::getline(file, line)) {
        auto comment = line.find(';');
        if (comment != std::string::npos) {
            line.erase(comment);
        }

        if (line.empty()) continue;

        auto equal = line.find('=');

        if (equal == std::string::npos) continue;

        std::string key = line.substr(0, equal);
        std::string value = line.substr(equal + 1);

        trim(key);
        trim(value);

        if (!ApplyConfigOption(config, configOptions, key, value)) {
            std::cerr << "bibble: unknown config option: " << key << std::endl;
        }
    }
}

static std::optional<Options> ParseCommandLine(const std::unordered_map<std::string, ConfigOption*>& configOptions, int argc, char** argv) {
    Options options;
    bool programArgsMode = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (programArgsMode) {
            options.programArgs.push_back(std::move(arg));
            continue;
        }

        if (arg == "--"sv) {
            programArgsMode = true;
            continue;
        }

        if (arg.starts_with("--")) {
            std::string keyValue = arg.substr(2);

            auto equal = keyValue.find('=');
            if (equal != std::string::npos) {
                std::string key = keyValue.substr(0, equal);
                std::string value = keyValue.substr(equal + 1);

                if (ApplyConfigOption(options.config, configOptions, key, value)) {
                    continue;
                }
            } else {
                auto it = configOptions.find(keyValue);
                if (it != configOptions.end()) {
                    if (it->second->optionalValue) {
                        ApplyConfigOption(options.config, configOptions, keyValue, std::nullopt);
                        continue;
                    }

                    if (i + 1 < argc) {
                        ApplyConfigOption(options.config, configOptions, keyValue, argv[++i]);
                        continue;
                    }
                }
            }
        }

        if (arg == "-h" || arg == "--help") {
            options.showHelp = true;
            continue;
        }

        if (arg == "-v" || arg == "--version") {
            options.showVersion = true;
            continue;
        }

        if (arg == "-mp" || arg == "--modulepath") {
            if (i + 1 >= argc) {
                std::cerr << "bibble: missing module path" << std::endl;
                return std::nullopt;
            }

            options.modulePath.emplace_back(argv[++i]);
            continue;
        }

        if (arg == "--redirect-stdin") {
            if (i + 1 >= argc) {
                std::cerr << "bibble: missing stdin redirect file" << std::endl;
                return std::nullopt;
            }

            options.redirectStdIn = argv[++i];
            continue;
        }

        if (arg == "--redirect-stdout") {
            if (i + 1 >= argc) {
                std::cerr << "bibble: missing stdout redirect file" << std::endl;
                return std::nullopt;
            }

            options.redirectStdOut = argv[++i];
            continue;
        }

        if (arg == "--redirect-stderr") {
            if (i + 1 >= argc) {
                std::cerr << "bibble: missing stderr redirect file" << std::endl;
                return std::nullopt;
            }

            options.redirectStdErr = argv[++i];
            continue;
        }

        if (arg == "--config-profile") {
            if (i + 1 >= argc) {
                std::cerr << "bibble: missing config profile name" << std::endl;
                return std::nullopt;
            }

            options.configProfile = argv[++i];
            continue;
        }

        if (!arg.starts_with("--")) {
            if (options.moduleOrExecutable.has_value()) {
                std::cerr << "bibble: cannot specify multiple modules or executables to execute" << std::endl;
                return std::nullopt;
            }

            options.moduleOrExecutable = std::move(arg);
        }
    }

    return options;
}

static void ApplyConfigProfile(Config& config, const std::string& profile) {
    //TODO: create config files for profiles, then load those
}

static void PrintHelpMessage(const std::vector<ConfigOption>& configOptions) {
    struct Option {
        std::optional<std::string> shortUsage;
        std::optional<std::string> longUsage;
        std::string description;
    };

    std::cout << "Usage: bibble [options...] modulefile [-- args...]\n";
    std::cout << "             (to execute a module)\n";
    std::cout << "    or bibble [options...] executable [-- args...]\n";
    std::cout << "             (to execute an executable. EXECUTABLES DON'T EXIST YET BUT WILL SOON)\n\n"; //TODO: add executables and update this
    std::cout << "where options include:\n";

    std::vector<Option> options = {
        {"-h", "--help", "print this help message"},
        {"-v", "--version", "print version"},
        {std::nullopt, "--redirect-stdin", "redirect stdin from file"},
        {std::nullopt, "--redirect-stdout", "redirect stdout to file"},
        {std::nullopt, "--redirect-stderr", "redirect stderr to file"},
        {std::nullopt, "--config-profile", "specify config profile"},
    };

    options.reserve(configOptions.size() + options.size());

    for (const ConfigOption& option : configOptions) {
        options.emplace_back(std::nullopt, "--" + option.key, option.description);
    }

    auto makeLeft = [](const Option& option) {
        std::string left;

        if (option.shortUsage.has_value()) {
            left += "-" + option.shortUsage.value();
        }

        if (option.longUsage.has_value()) {
            if (!left.empty()) left += ", ";
            left += "--" + option.longUsage.value();
        }

        return left;
    };

    auto widths = options | std::ranges::views::transform([&makeLeft](const Option& option) {
        return makeLeft(option).size();
    });

    auto maxIt = std::ranges::max_element(widths);
    size_t maxWidth = (maxIt != widths.end()) ? *maxIt : 0;

    constexpr size_t padding = 4;

    for (const Option& option : options) {
        std::string left = makeLeft(option);
        std::cout << left;

        if (left.size() < maxWidth) {
            std::cout << std::string(maxWidth - left.size(), ' ');
        }

        std::cout << std::string(padding, ' ') << option.description << '\n';
    }

    std::cout.flush();
}

static void PrintVersion() {
    std::cout << "BibbleVM version 0.1.0" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        PrintHelpMessage(BuildConfigOptionTable());
        return 0;
    }

    InitDependencies();

    Options options;

    {
        std::vector<ConfigOption> configOptionTable = BuildConfigOptionTable();
        std::unordered_map<std::string, ConfigOption*> configOptionMap = BuildConfigOptionMap(configOptionTable);

        std::optional<Options> optionsOpt = ParseCommandLine(configOptionMap, argc, argv);
        if (!optionsOpt.has_value()) return 1;

        options = std::move(optionsOpt.value());

        if (options.showHelp) {
            PrintHelpMessage(configOptionTable);
            return 0;
        }

        if (options.showVersion) {
            PrintVersion();
            return 0;
        }
    }

    if (options.configProfile.has_value()) {
        ApplyConfigProfile(options.config, *options.configProfile);
    }

    if (!options.moduleOrExecutable.has_value()) {
        std::cerr << "bibble: no module or executable specified" << std::endl;
        return 1;
    }

    //TODO: redirect stdio using libos when that feature is added to libos

    if (options.redirectStdIn.has_value()) {
        os_file* file = nullptr;
        os_result res = os_file_open(&file, options.redirectStdIn->c_str(), OS_FILE_READ | OS_FILE_SEQUENTIAL);
        if (res != OS_OK) {
            std::cerr << "bibble: could not open file '" << options.redirectStdIn.value() << "'" << std::endl;
            return 1;
        }

        res = os_file_set_stdfile(OS_STDIN, file);
        if (res != OS_OK) {
            std::cerr << "bibble: could not set stdin file to '" << options.redirectStdIn.value() << "'" << std::endl;
            return 1;
        }
    }

    if (options.redirectStdOut.has_value()) {
        os_file* file = nullptr;
        os_result res = os_file_open(&file, options.redirectStdOut->c_str(), OS_FILE_WRITE | OS_FILE_CREATE | OS_FILE_TRUNCATE | OS_FILE_SEQUENTIAL);
        if (res != OS_OK) {
            std::cerr << "bibble: could not open file '" << options.redirectStdOut.value() << "'" << std::endl;
            return 1;
        }

        res = os_file_set_stdfile(OS_STDOUT, file);
        if (res != OS_OK) {
            std::cerr << "bibble: could not set stdout file to '" << options.redirectStdOut.value() << "'" << std::endl;
            return 1;
        }
    }

    if (options.redirectStdErr.has_value()) {
        os_file* file = nullptr;
        os_result res = os_file_open(&file, options.redirectStdErr->c_str(), OS_FILE_WRITE | OS_FILE_CREATE | OS_FILE_TRUNCATE | OS_FILE_SEQUENTIAL);
        if (res != OS_OK) {
            std::cerr << "bibble: could not open file '" << options.redirectStdErr.value() << "'" << std::endl;
            return 1;
        }

        res = os_file_set_stdfile(OS_STDERR, file);
        if (res != OS_OK) {
            std::cerr << "bibble: could not set stderr file to '" << options.redirectStdErr.value() << "'" << std::endl;
            return 1;
        }
    }

    VM vm(options.config);

    // TODO: executable support

    vm.linker().addModulePath(".");
    vm.linker().addModulePath(std::move(options.modulePath));

    linker::Module* mainModule = vm.getModule(options.moduleOrExecutable.value_or("Main"));
    if (mainModule == nullptr) return 2;

    executor::Function* mainFunction = mainModule->linkedModule().getFunction(".main");
    if (mainFunction == nullptr) return 3;

    executor::Task* task = nullptr;

    if (mainFunction->getParameterCount() == 0) {
        task = vm.scheduler().schedule(vm, *mainFunction, executor::MapPriorityToLevel(vm, executor::TaskPriority::Critical), nullptr);
    } else if (mainFunction->getParameterCount() == 1) {
        oop::Object* argsObj = vm.memoryManager().allocateArray(vm, oop::Type::Reference, options.programArgs.size());
        oop::ArrayView<oop::Object*> args = argsObj->asArray();

        for (size_t i = 0; i < options.programArgs.size(); ++i) {
            args[i] = vm.memoryManager().allocateString(vm, options.programArgs[i]);
        }

        Value programArgs[1];
        programArgs[0].isObject = true;
        programArgs[0].obj = argsObj;

        task = vm.scheduler().schedule(vm, *mainFunction, executor::MapPriorityToLevel(vm, executor::TaskPriority::Critical), programArgs);
    }

    if (task == nullptr) return 4; // bad entry point

    options.clear();

    oop::Object** mainFutureRef = vm.memoryManager().newGlobalStrongReference(task->completionFuture->asObject());

    vm.scheduler().run(vm);

    if (!(*mainFutureRef)->asFuture()->ready) return 4;
    return (*mainFutureRef)->asFuture()->value.l;
}