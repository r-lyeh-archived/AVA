// # cooker
// cooker is an asset cooker daemon + fileserver host
// 
// # usage
// - cooker [daemon]
// - cooker cook
// - cooker clear
// - cooker clear cook package
// 
// # daemon general behavior algorithm
// 1. for( each changed asset in disk ) asset = cook( asset );
// 2. sleep few seconds & repeat
//
// # cook description (lossless -> lossy)
// 1. foreach file extension (ext), find extc sh/dll/binary/lua compiler
//    (ie, .lua -> luac, .c -> cc, .glsl -> glslc, .obj -> objc, .jpg -> jpgc, etc...)
// 2. prepend toolchain if possible,
//    (ie, ps4+lua -> ps4-luac, xb1+dds->xb1-ddsc, pc+jpg->pc-jpgc, ...)
// 3. convert it to json by invoking:
//      `toolchain-extc [options] < /in/input.bin > /out/output.json`
//      or
//      `toolchain-extc [options] /in/input.bin /out/output.json`
//      (options can be also in environment vars)
// 4. report warnings and errors.
// 
// # todo
// - apply ini options
// - cook, out(json+meta)2(striped-mp)
// - repair, upgrade, resave, package
// - package, mp2zip -9
// - package, mp2pak -0
// - package, fileorder.log
// - package, unify names
// 
// # todo
// - fix leaks (im lazy)
// - incbin generators
// - bind.ini 0.0.0.0:2019,12019,5050-5060,16384
//
// # names evaluated:
// - hammer,forge,cooker,assio,webio,wake,ivy,kraken
//

#include <engine.h>

#define COOKER_VERSION "cooker(1.0.0)"

#define DEFAULT_CONFIG_INI \
"[cooker]\n" \
"input_folder=../../games\n" \
"output_folder=../../_cooked\n"

// os utils

void beep() {
    tty_color(255,0,0);
    //puts("\x7");
    fputc('\x7', stdout);
}

uint64_t checksum( const char *pathfile ) {
    void *x = file_read(pathfile);
    uint64_t l = file_size(pathfile);
    uint64_t crc = crc64( x, l );
    return crc;
}

char *vcs_branch() {
    return os_exec("git rev-parse --abbrev-ref HEAD");
}

char *vcs_version() {
    return os_exec("git rev-list --count --first-parent HEAD");
}

// program

int             app_jobs_done = 0, app_jobs_queued = 0;
int             app_errors = 0;
const char *    app_status = "Ok";
char            app_prompt[256] = {0};

int             app_cook_errors = 0;
int             app_files_scanned = 0;
int             app_files_processed = 0;

char            *app_log = 0;
char            *app_log_errors_only = 0;
char            *app_joblist = 0;

char            *app_toolchain = 0;

const char      *app_monitor_dir = "./**";

// program utils

bool clear_cooked_file( const char *pathfile ) {
    const char *meta_name = va("%s.meta", pathfile);
    const char *cook_name = va("%s.cook", pathfile);
    return file_delete( meta_name ) && file_delete( cook_name );
} 

void clear_cooked_files() {
    HEAP
    char *diff = dir_ls2(app_monitor_dir);
    char name[256], mode;
    int size, modt;
    int local_errors = 0;
    while(*diff && sscanf(diff,"%c%[^,],%d,%d\n",&mode,name,&size,&modt) == 4) {
        diff = strchr(diff,'\n') + 1;
        if( strmatch(name, "*.cook") ) {
            file_delete(name);
        }
        if( strmatch(name, "*.meta") ) {
            file_delete(name);
        }
    }
    FREE(diff);
}

bool cook_file( const char *pathfile ) {
    const char *meta_name = va("%s.meta", pathfile);
    const char *cook_name = va("%s.cook", pathfile);
 
    uint64_t    meta_date = file_stamp(meta_name);
    uint64_t    meta_size = file_size(meta_name);

    uint64_t    cook_date = file_stamp(cook_name);
    uint64_t    cook_size = file_size(cook_name);

    char date[32];
    date_format( date, date_gmt_today() );
    char *meta_data = va("; change\n"
        //"commit.revision=%d\n"
        "commit.date=%s\n"
        "commit.author=%s\n"
        "commit.tool=%s\n"
        "commit.crc64=%016llx\n"
        , date, env_user(), COOKER_VERSION, checksum(pathfile));

    char *cook_data = va("; cooked %s\n", date);

    bool ok1 = file_append( meta_name, meta_data, strlen(meta_data) );
    bool ok2 = 0; // file_write( cook_name, cook_data, strlen(cook_data) );

    char buf[4096];
    char *ext = uri_type( buf, pathfile );
    char *dot = strrchr( ext, '.');
    ext = dot ? dot + 1 : ext;
    printf("%s -> %s\n", pathfile, ext);

    char *cmd = va("%sc %s %s.cook", ext, pathfile, pathfile);

    if( app_toolchain ) {
        cmd = va("%s-%s", app_toolchain, cmd);
    }

    FILE *fp = fopen("cook.bat","a+b");
    fprintf(fp, "%s\r\n", cmd);
    fclose(fp);

    //printf("%s\n", cmd);
    //ok2 = 0 == system( cmd );
    //execlp("/bin/extc", "arg1", "arg2", NULL);

    return ok1 && ok2;
}

bool process_file( int mode, const char *pathfile ) {
    bool ok = false;

    if( mode == '+' || mode == '*' ) {
        app_status = "Processing";
        ok = cook_file( pathfile );
        // play(+:add_ting.wav, *:mod_sierra.wav)
    }
    if( mode == '-' ) {
        app_status = "Deleting";
        ok = clear_cooked_file(pathfile);
        // play(-:rem_trash.wav)
    }
    return ok;
}

// program

void show_daemon_log();

void cook_list( const char *diff ) {
    int num_jobs = 0;
    strcpyf(&app_joblist, "");

    app_status = "Filtering jobs";
        char name[256], mode;
        int size, modt;
        int local_errors = 0;
        while(*diff && sscanf(diff,"%c%d,%d, %[^\n]",&mode,&size,&modt,name) == 4) {
            diff = strchr(diff,'\n') + 1;

            ++app_files_scanned;

            if( strmatch(name, "*/")     ) continue;
            if( strmatch(name, "*.meta") ) continue;
            if( strmatch(name, "*.cook") ) continue;

            strcatf(&app_joblist, "%c%s,\n", mode, name);
            ++num_jobs;
        }

    if( num_jobs <= 0 ) return;
    strcatf(&app_log, "Detected changes in filesystem. Processing changes...\n");

unlink("cook.bat");

    app_status = "Processing jobs";
    char *ptr = app_joblist;
        for(int i = 0; *ptr && sscanf(ptr,"%c%[^,]",&mode,name) == 2; ++i) {
            ptr = strchr(ptr,'\n') + 1;

            ++app_files_processed;

            bool ok = process_file(mode, name);
            int pct = (int)(((i+1) * 100.f) / num_jobs);
            strcatf(&app_log, "[%s][%03d%%] Imported %s [%c]\n", ok ? " OK " : "FAIL", pct, name, mode);

            if(!ok)
            strcatf(&app_log_errors_only, "[%s][%03d%%] Imported %s [%c]\n", ok ? " OK " : "FAIL", pct, name, mode);

            local_errors += !ok;
        }

system("cook.bat");
unlink("cook.bat");
show_daemon_log(); // eval_command("status");

    if( app_cook_errors > 0 ) {
        app_cook_errors = 0;
        beep();
    }
}

void daemon_webhost( void *dummy ) {
    webserver( 2019, "./" );
    // @todo: try +10k,+20k... on fail. ie, 12019, 22019, 32019, 42019, 52019, 62019
}

void daemon_watcher( void *userdata ) {
    HEAP char *then = dir_ls2(app_monitor_dir);
    HEAP char *now = then;

    uint64_t t = time_ss();

    for(;;) {

        // debug some activity every 5m.
        // also, do not print carriage return to minimize flood cluttering over working hours.
        if( (time_ss() - t) > 300 ) {
            t = time_ss();
            strcatf(&app_log, "(300s ping timeout)");
        }

        app_status = "Sleeping cpu";
            sleep_ss(3);

        app_status = "Scanning disk for local changes";
            HEAP char *diff = dir_diff(then, now = dir_ls2(app_monitor_dir));
            then = now;

            if(!diff[0]) continue;

        // @todo: cooldown
        // do { sleep(some spare secs) } while( keep disk activity )
        // at this point we can safely start processing.

        cook_list(diff);
    }
}

char *user_prompt( const char *heading ) {
    static int progress = 0;
    printf("%c# %s>", "\\|/-"[progress = ++progress&3], heading);
    int eof = fgets(app_prompt, 256, stdin) == 0 || feof(stdin);
    if( !eof ) {
        // remove \r\n at eos
        int L = strlen(app_prompt);
        while( app_prompt[L-1] == '\r' || app_prompt[L-1] == '\n' ) {
            app_prompt[--L] = 0;
        }
    }
    return eof ? NULL : app_prompt;
}

void show_console_help() {
    puts("\t.?\tshow this help screen");
    puts("\t.status\tshow daemon status"); // (empty command is an alias as well)");
}

void show_app_header() {
    puts(COOKER_VERSION " - Loaded modules: console,logger,webserver,tray-icon.");
    puts("Type ctrl-z to exit, '.help' for usage help.");
    // puts("Empty command or '.status' to display daemon log+status.");
    puts("Any other command is redirected to bash/batch directly.");
    puts("Hosting cooked content at http://localhost:2019");
    puts("---");
}

void show_daemon_log() {
    if( app_log && app_log[0] ) {
        printf("Output background log:\n%s", app_log);
        strcpyf(&app_log, "");
    }
    if( app_log_errors_only && app_log_errors_only[0] ) {
        printf("Filtered errors found:\n%s", app_log_errors_only);
        strcpyf(&app_log_errors_only, "");
    } else {
        printf("%s\n", "No errors found");
    }
    printf("[Status: %d files scanned, %d files processed, %d/%d jobs, %d errors, %s]\n", app_files_scanned, app_files_processed, app_jobs_done, app_jobs_queued, app_errors, app_status );
    printf("vcs_version: %s\n", vcs_version());
}

void parse_config_ini() {
    // open config file, or recreate it if not found
    char *file = va("%s%s.ini", env_root(), env_arg0());
    if( !file_exist(file) ) {
        printf("Warning: Cannot find %s config file. Creating default config file...\n", file);
        file_write(file, DEFAULT_CONFIG_INI, strlen(DEFAULT_CONFIG_INI));
    }
    if( !file_exist(file) ) {
        printf("Error: Cannot open %s config file\n", file);
        exit(-1);
    }
    puts("Reading configuration file...");
    char *content = file_read( file );
    char *kv = ini( content );
    for( char *iter = kv; iter[0]; ) {
        char *key = iter; while( *iter++ );
        char *val = iter; while( *iter++ );
        // @todo:
        // apply options
        printf("option '%s'='%s'\n", key, val);
    }
    FREE(kv);
}

int eval_command( const char *command ) {
    /**/ if( !strcmp(command, "quit") || !strcmp(command, "q") ) exit(0);
    else if( !strcmp(command, "help") || !strcmp(command, "?") ) show_console_help();

    else if( !strcmp(command, "status") ) show_daemon_log();
    else if( !strcmp(command, "clear") ) clear_cooked_files();
    else if( !strcmp(command, "cook") ) cook_list( dir_ls2(app_monitor_dir) );

    else if( !strcmp(command, "win") || !strcmp(command, "win64") ) app_toolchain = "win";
    else if( !strcmp(command, "linux") || !strcmp(command, "linux64") ) app_toolchain = "lin";
    else if( !strcmp(command, "android") ) app_toolchain = "and";
    else if( !strcmp(command, "ps4") ) app_toolchain = "ps4";
    else if( !strcmp(command, "xb1") ) app_toolchain = "xb1";
    else if( !strcmp(command, "nsw") ) app_toolchain = "nsw";
    else if( !strcmp(command, "osx") ) app_toolchain = "osx";
    else if( !strcmp(command, "ios") ) app_toolchain = "ios";
    else if( !strcmp(command, "bsd") ) app_toolchain = "bsd";
    else return false;

    return true;
}

int main(int argc, char **argv) {

    //log_add(stdout, 2);
    show_app_header();

    window_icon( va("%s%s.ico", env_root(), env_arg0()) );

    parse_config_ini();

    window_tray(COOKER_VERSION, va("%s%s.ico", env_root(), env_arg0()) );

    for( int i = 1; i < argc; ++i ) {
        char **options = strsplit( argv[i], "+" );
        for( int i = 0; options[i]; ++i ) {
            eval_command( options[i] );
        }
        FREE( options );
        show_daemon_log();
        exit(0);
    }

    detach(daemon_watcher, 0);
    detach(daemon_webhost, 0);

#ifdef _WIN32
    //sleep_ms(500);
    //system("start http://localhost:2019");
#endif


    char cwd[256];
    while( user_prompt(env_curr(cwd)) ) {
        if( app_prompt[0] == 0 ) {
            continue;
            // strcpy(app_prompt, ".status");
        }
        if( !eval_command(app_prompt + 1) ) {
            //puts( os_exec( app_prompt ) );
            system( app_prompt );
        }
    }

    return 0;
}

