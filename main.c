#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
/* somewhat unix-specific */
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <regex.h>
/* curl stuff */
#include <curl/mprintf.h>
/* example key */
#define AUTH "Authorization: Basic 14ac5499cfdd2bb2859e4476d2e5b1d2bad079bf"
struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void nukechar(char s[], char c)
{
    size_t j = 0;
    for (size_t i = 0; s[i] != '\0'; ++i) {
        if (s[i] != c) {
            s[j] = s[i];
            ++j;
        }
    }
    s[j] = '\0';
}

int main(int arg, char **argv) {

    curl_version_info_data  *data = curl_version_info( CURLVERSION_NOW );
    int     a;
    for ( a = 1; a < arg; a++ )
    {
        //printf( "%s\n", argv[a] );
    }
    /* 开始没有加const 编译器警告 curl.c:11:8: warning: initializing 'char *' with an expression of type 'const char *' */
    const char *ver = data->version;
    int length = strlen( ver );
    //printf( "curl version:" );

    for ( a = 0; a < length; a++ )
    {
        //putchar( ver[a] );
    }
    //printf( "\n" );

    CURL      *curl;
    CURLcode    res;
    struct curl_slist *headerlist = NULL;
    struct curl_httppost* post = NULL;
    struct curl_httppost* last = NULL;
    struct curl_fetch_st *fetch = NULL;
    curl_off_t speed_upload, total_time;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;
    int cflags=REG_EXTENDED | REG_ICASE;
    char ebuff[256];

    int ret;
    regex_t reg;
    regmatch_t rm[5];
    char *reg_str = "http.*(gif|png|jpg|jpeg)";
    ret = regcomp(&reg, reg_str, cflags);
    char *part_str = NULL;
    if (ret)
    {
        printf( "error\n" );
        regerror(ret, &reg, ebuff, 256);
        fprintf(stderr, "%s\n", ebuff);
        regfree(&reg);
        return 1;
    }

    for ( a = 1; a < arg; a++ ) {
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        /* Fill in the file upload field */
        curl_formadd(&post,
                     &last,
                     CURLFORM_COPYNAME, "smfile",
                     CURLFORM_FILE, argv[a],
                     CURLFORM_END);

        /* Fill in the filename field */
        curl_formadd(&post,
                     &last,
                     CURLFORM_COPYNAME, "filename",
                     CURLFORM_COPYCONTENTS, "postit2.c",
                     CURLFORM_END);
        headerlist  = curl_slist_append( headerlist, AUTH);
        /* Set the form info */
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
        curl_easy_setopt(curl, CURLOPT_URL, "https://sm.ms/api/v2/upload" );
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist );
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform( curl );
        if ( res != CURLE_OK ) {
            fprintf( stderr, "curl_easy_perform() failed: %s\n",
                     curl_easy_strerror(res));
        } else  {
            //printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
            //printf("ok");
            //printf("%s\n", chunk.memory);
            ret = regexec(&reg, chunk.memory, 5, rm, 0);
            if (ret)
            {   /* always cleanup */
                curl_formfree(post);
                curl_easy_cleanup( curl );
                curl_slist_free_all( headerlist );
                free(chunk.memory);
                regerror(ret, &reg, ebuff, 256);
                fprintf(stderr, "%s\n", ebuff);
                regfree(&reg);
                return 1;
            }
            int i;
            for (i=0; i<5; i++)
            {
                if (rm[i].rm_so > -1)
                {
                    part_str = strndup(chunk.memory+rm[i].rm_so, rm[i].rm_eo-rm[i].rm_so);
                    printf( "Upload Success:\n");
                    nukechar(part_str,'\\');
                    printf("%s\n", part_str);
                    free(part_str);
                    part_str = NULL;
                    break;

                }
            }

            //printf("%s\n", chunk.memory);
        }
        /* always cleanup */
        curl_formfree(post);
        curl_easy_cleanup( curl );
        curl_slist_free_all( headerlist );
        free(chunk.memory);

    }
    return 0;

}
