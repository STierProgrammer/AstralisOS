#ifndef NOBPP_H_
#define NOBPP_H_

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX

#include "nob.h"

/*
    Taken from: dcraft bg (Github: https://github.com/Dcraftbg)
*/
static bool walk_directory(
    File_Paths* dirs,
    File_Paths* c_sources,
    File_Paths* asm_sources,
    const char* path
) {
    DIR *dir = opendir(path);
    if(!dir) {
        nob_log(ERROR, "Could not open directory %s: %s", path, strerror(errno));
        return false;
    }

    errno = 0;
    struct dirent *ent;
    while(ent = readdir(dir)) {
        if(*ent->d_name == '.') continue;
        
        size_t temp = temp_save();
        const char* p = temp_sprintf("%s/%s", path, ent->d_name); 

        String_View sv = sv_from_cstr(p);
        File_Type type = get_file_type(p);
        if (type == FILE_DIRECTORY) {
            if (dirs) da_append(dirs, p);
            if(!walk_directory(dirs, c_sources, asm_sources, p)) {
                closedir(dir);
                return false;
            }
            continue;
        }

        if(sv_end_with(sv, ".c")) {
            da_append(c_sources, p);
        } else if(sv_end_with(sv, ".asm")) {
            da_append(asm_sources, p);
        } else {
            temp_rewind(temp);
        }
    }
    closedir(dir);
    return true;
}

#endif