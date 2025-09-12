#include "nob++.h"
#include <stddef.h>

#define SRC_FOLDER "src/"
#define BUILD_FOLDER "build/"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    File_Paths c_sources = {0};
    File_Paths asm_sources = {0};

    if (!walk_directory(NULL, &c_sources, &asm_sources, SRC_FOLDER)) return 1;

    mkdir_if_not_exists(BUILD_FOLDER);

    Cmd cmd = {0};
    
    for (size_t i = 0; i < c_sources.count; i++) {
        const char* build_path = temp_sprintf("%s%s.o", BUILD_FOLDER, c_sources.items[i] + strlen(SRC_FOLDER));
        nob_cmd_append(&cmd, 
            "cc", 
            "-Wall", 
            "-Wextra", 
            "-I", "../limine", 
            "-DLIMINE_API_REVISION=3",
            "-Wall",
            "-std=gnu11",
            "-ffreestanding",
            "-fno-stack-protector",
            "-fno-stack-check",
            "-fno-PIC",
            "-m64",
            "-march=x86-64",
            "-mno-80387",
            "-mno-mmx",
            "-Wextra",
            "-mno-sse",
            "-mno-sse2",
            "-mno-red-zone",
            "-mcmodel=kernel",
            "-o", build_path, c_sources.items[i]
        );
        if (!nob_cmd_run(&cmd)) return 1;
    }

}
