#include <iostream>
#define no_argument 0
#define required_argument 1
#define optional_argument 2

// namespace getopt {

struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

class getopt {

   public:
    getopt() {
        opt_arg = nullptr;
        opt_ind = 1;
        opt_err = 1;
        opt_opt = '?';
    }
    ~getopt(){};

    int getopt_long(int argc,
                    char *const *argv,
                    const char *optstring,
                    const struct option *longopts,
                    int *longindex);

    char *opt_arg;
    int opt_ind;
    int opt_err;
    int opt_opt;

    int current_index = 1;
    bool has_other    = false;
};

int getopt::getopt_long(int argc,
                        char *const *argv,
                        const char *optstring,
                        const struct option *longopts,
                        int *longindex) {
loop_start:
    if (current_index == argc) {
        if (has_other == false) {
            opt_ind = current_index;
        }
        return -1;
    }

    opt_arg = nullptr;
    std::string arg(argv[current_index]);

    if (arg[0] == '-' && arg[1] == '-') {
        // fprintf(stderr, "curr long opt\n");
        std::string arg_name(&arg[2]);

        for (int i = 0; longopts[i].name != 0; i++) {
            struct option lopt = longopts[i];
            std::string lopt_name(lopt.name);
            if (lopt_name == arg_name) {

                if (lopt.has_arg == 1) {
                    current_index++;
                    opt_arg = argv[current_index];
                    if (opt_arg == nullptr) {
                        return -1;
                    }
                }
                current_index++;
                return lopt.val;
            }
        }

    } else if (arg[0] == '-') {
        // fprintf(stderr, "curr short opt\n");
        char opt = arg[1];

        for (int i = 0; optstring[i] != '\0'; i++) {
            char optstr = optstring[i];
            if (opt == optstr) {

                if (optstring[i + 1] == ':') {
                    i++;

                    current_index++;
                    opt_arg = argv[current_index];
                    if (opt_arg == nullptr) {
                        return -1;
                    }
                }

                // printf("opt=%c\n", opt);
                // printf("optstr=%c\n", optstr);
                // printf("optarg=%s\n", opt_arg);
                current_index++;
                return opt;
            }
        }

    } else {
        has_other = true;
        opt_ind   = current_index;

        current_index++;
        if (argc == opt_ind) {
            return -1;
        }
        goto loop_start;
    }

    // printf("%s\n", arg.c_str());

    return opt_opt;
}
// } // namespace getopt
