//
// Recommends presence of jeopardy.json file, which can be received at: https://www.reddit.com/r/datasets/comments/1uyd0t/200000_jeopardy_questions_in_a_json_file/
// Modified to use linear memory backing allocator for nodes
#define ZPL_IMPLEMENTATION
#define ZPL_NANO
#define ZPL_ENABLE_JSON
#define ZPL_ENABLE_OPTS
#include <zpl.h>

void exit_with_help(zpl_opts *opts) {
    zpl_opts_print_errors(opts);
    zpl_opts_print_help(opts);
    zpl_exit(-1);
}

int main(int argc, char **argv) {
    zpl_opts opts={0};

    zpl_opts_init(&opts, zpl_heap(), argv[0]);
    zpl_opts_add(&opts, "f", "file", "input file name.", ZPL_OPTS_STRING);
    zpl_opts_add(&opts, "m", "mem", "memory arena size.", ZPL_OPTS_INT);
    zpl_opts_positional_add(&opts, "file");
    zpl_b32 ok = zpl_opts_compile(&opts, argc, argv);
    zpl_isize mem_size = 132700000;

    char *filename = NULL;

    if (!ok || !zpl_opts_positionals_filled(&opts))
        exit_with_help(&opts);

    filename = zpl_opts_string(&opts, "file", NULL);

    if (filename == NULL)
        exit_with_help(&opts);

    zpl_printf("Filename: %s\n", filename);

    zpl_file_contents fc = zpl_file_read_contents(zpl_heap(), true, filename);

    zpl_printf("Parsing JSON5 file!\n");

    zpl_json_object root = {0};
    zpl_arena arena = {0};
    zpl_arena_init_from_allocator(&arena, zpl_heap(), mem_size);

    zpl_u8 err;
    zpl_u64 time = zpl_time_rel_ms();
    zpl_json_parse(&root, (char *)fc.data, zpl_arena_allocator(&arena), &err);
    zpl_u64 delta = zpl_time_rel_ms() - time;

    if (err == ZPL_JSON_ERROR_OBJECT_OR_SOURCE_WAS_NULL)
    {
        zpl_printf("File not found!\n");
        return -2;
    }

    zpl_printf("Delta: %lldms\nNo. of nodes: %td\nError code: %d\nFile size: %td bytes\n", delta*1000, zpl_array_count(root.nodes), err, fc.size);

    zpl_json_free(&root);
    zpl_file_free_contents(&fc);
    zpl_arena_free(&arena);

    return 0;
}
