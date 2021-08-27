#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>   // stat
#include <stdbool.h>    // bool type

#ifndef _MSC_VER
#include <unistd.h>
#else
#include "..\msvc\win\getopt.h"
#endif

#include "misc.h"
#include "msg.h"
#include "hls.h"

struct hls_args hls_args;

static void print_help(const char* filename)
{
	printf("hlsdl v0.28 | Last build 08/27/2021\n");
	printf("(c) 2017-2020 @selsta, samsamsam@o2.pl, @xavave\n");
	printf("Usage: %s [options] url\n\n"
		"-b ... Automatically choose the best quality.\n"
		"-W ... Choose largest width lower or equal than this.\n"
		"-H ... Choose largest height lower or equal than this.\n"
		"-A ... Select audio language.\n"
		"-v ... Verbose more information.\n"
		"-o ... Choose name of output file (\"-\" alias for stdout).\n"
		"-O ... keep fragment files (don't save in 1 output file).\n"
		"-u ... Set custom HTTP User-Agent header.\n"
		"-h ... Set custom HTTP header.\n"
		"-p ... Set proxy uri.\n"
		"-k ... Allow to replace part of AES key uri - old.\n"
		"-n ... Allow to replace part of AES key uri - new.\n"
		"-f ... Force overwriting the output file.\n"
		"-F ... Force ignore detection of DRM.\n"
		"-K ... Force AES key value (hexstring)\n"
		"-q ... Print less to the console.\n"
		"-d ... Print the openssl decryption command (and then NOT processing files).\n"
		"-t ... Print the links to the .ts files.\n"
		"-s ... Set live start offset in seconds.\n"
		"-i ... Set live stream download duration in seconds.\n"
		"-I ... Ignore playlist url (for getting files from local folder).\n"
		"-e ... Set refresh delay in seconds.\n"
		"-r ... Set max retries at open.\n"
		"-w ... Set max download segment retries.\n"
		"-a ... Set additional url to the audio media playlist.\n"
		"-c ... Treat HTTP code 206 as 200 even if request was made without range header.\n"
		"-C ... the file name of file holding cookie data in the old Netscape / Mozilla cookie data format.\n", filename);
	exit(0);
}

int parse_argv(int argc, char* const argv[])
{
	int ret = 0;
	int c = 0;
	int custom_header_idx = 0;
	while ((c = getopt(argc, argv, "bH:W:A:vqbfFK:ctdo:O:u:h:s:i:I:r:w:e:p:k:n:a:C:")) != -1)
	{
		switch (c)
		{
		case 'v':
			hls_args.loglevel += 1;
			break;
		case 'q':
			hls_args.loglevel -= 1;
			break;
		case 'b':
			hls_args.use_best = true;
			break;
		case 'W':
			hls_args.maxwidth = atoi(optarg);
			break;
		case 'H':
			hls_args.maxheight = atoi(optarg);
			break;
		case 'A':
			hls_args.audiolang = optarg;
			break;
		case 'h':
			if (custom_header_idx < HLSDL_MAX_NUM_OF_CUSTOM_HEADERS) {
				hls_args.custom_headers[custom_header_idx] = optarg;
				custom_header_idx += 1;
			}
			break;
		case 'f':
			hls_args.force_overwrite = true;
			break;
		case 'F':
			hls_args.force_ignoredrm = true;
			break;
		case 'K':
			hls_args.key_value = malloc(KEYLEN);
			if (strlen(optarg) != 2 * KEYLEN || str_to_bin(hls_args.key_value, optarg, KEYLEN)) {
				MSG_ERROR("AES key value : 32 characters hexstring expected\n", c);
				ret = -1;
			}
			else
			{
				MSG_PRINT("AES key value :");
				bin_to_hex(hls_args.key_value, KEYLEN);
			}
			break;
		case 's':
			hls_args.live_start_offset_sec = atoi(optarg);
			break;
		case 'i':
			hls_args.live_duration_sec = atoi(optarg);
			break;
		case 'I':
			hls_args.ignore_playlist_url = atoi(optarg);
			break;
		case 'e':
			hls_args.refresh_delay_sec = atoi(optarg);
			break;
		case 'r':
			hls_args.open_max_retries = atoi(optarg);
			break;
		case 'w':
			hls_args.segment_download_retries = atoi(optarg);
			break;
		case 'o':
			hls_args.filename = optarg;
			break;
		case 'O':
			hls_args.keep_fragments = optarg;
			break;
		case 't':
			hls_args.dump_ts_urls = true;
			break;
		case 'd':
			hls_args.dump_dec_cmd = true;
			break;
		case 'u':
			hls_args.user_agent = optarg;
			break;
		case 'C':
			hls_args.cookie_file = optarg;
			break;
		case 'p':
			hls_args.proxy_uri = optarg;
			break;
		case 'k':
			hls_args.key_uri_replace_old = optarg;
			break;
		case 'n':
			hls_args.key_uri_replace_new = optarg;
			break;
		case 'a':
			hls_args.audio_url = optarg;
			break;
		case 'c':
			hls_args.accept_partial_content = true;
			break;
		default:
			MSG_ERROR("?? getopt returned character code 0%o ??\n", c);
			ret = -1;
		}
	}

	if (0 == ret && (optind + 1) == argc)
	{
		ret = 0;
		hls_args.url = argv[optind];
		return 0;
	}

	print_help(argv[0]);
	return 1;
}

bool file_exists(char* filename) {
	struct stat   buffer;
	return (stat(filename, &buffer) == 0);
}

int EndsWith(const char* str, const char* suffix)
{
	if (!str || !suffix)
		return 0;
	size_t lenstr = strlen(str);
	size_t lensuffix = strlen(suffix);
	if (lensuffix > lenstr)
		return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

int str_to_bin(uint8_t* data, char* hexstring, int len)
{
	char* pos = hexstring;
	
	
	uint8_t str_len = strlen(hexstring);

	for (int i = 0; i < (str_len / 2); i++) {
		sscanf(hexstring + 2 * i, "%02x", &data[i]);
	}
	
	return 0;
}

int bin_to_hex(uint8_t* data, int len) {
	uint8_t str_len = strlen(data);
	for (int i = 0; i < (str_len); i++) {
		
		printf("%02x", data[i]);
	}
	printf("\n");
}
/* http://creativeandcritical.net/str-replace-c */
char* repl_str(const char* str, const char* from, const char* to)
{
	/* Adjust each of the below values to suit your needs. */

	/* Increment positions cache size initially by this number. */
	size_t cache_sz_inc = 16;
	/* Thereafter, each time capacity needs to be increased,
	 * multiply the increment by this factor. */
	const size_t cache_sz_inc_factor = 3;
	/* But never increment capacity by more than this number. */
	const size_t cache_sz_inc_max = 1048576;

	char* pret, * ret = NULL;
	const char* pstr2, * pstr = str;
	size_t i, count = 0;
#if (__STDC_VERSION__ >= 199901L)
	uintptr_t* pos_cache_tmp, * pos_cache = NULL;
#else
	ptrdiff_t* pos_cache_tmp, * pos_cache = NULL;
#endif
	size_t cache_sz = 0;
	size_t cpylen, orglen, retlen, tolen, fromlen = strlen(from);

	/* Find all matches and cache their positions. */
	while ((pstr2 = strstr(pstr, from)) != NULL) {
		count++;

		/* Increase the cache size when necessary. */
		if (cache_sz < count) {
			cache_sz += cache_sz_inc;
			pos_cache_tmp = realloc(pos_cache, sizeof(*pos_cache) * cache_sz);
			if (pos_cache_tmp == NULL) {
				goto end_repl_str;
			}
			else pos_cache = pos_cache_tmp;
			cache_sz_inc *= cache_sz_inc_factor;
			if (cache_sz_inc > cache_sz_inc_max) {
				cache_sz_inc = cache_sz_inc_max;
			}
		}

		pos_cache[count - 1] = pstr2 - str;
		pstr = pstr2 + fromlen;
	}

	orglen = pstr - str + strlen(pstr);

	/* Allocate memory for the post-replacement string. */
	if (count > 0) {
		tolen = strlen(to);
		retlen = orglen + (tolen - fromlen) * count;
	}
	else    retlen = orglen;
	ret = malloc(retlen + 1);
	if (ret == NULL) {
		goto end_repl_str;
	}

	if (count == 0) {
		/* If no matches, then just duplicate the string. */
		strcpy(ret, str);
	}
	else {
		/* Otherwise, duplicate the string whilst performing
		 * the replacements using the position cache. */
		pret = ret;
		memcpy(pret, str, pos_cache[0]);
		pret += pos_cache[0];
		for (i = 0; i < count; i++) {
			memcpy(pret, to, tolen);
			pret += tolen;
			pstr = str + pos_cache[i] + fromlen;
			cpylen = (i == count - 1 ? orglen : pos_cache[i + 1]) - pos_cache[i] - fromlen;
			memcpy(pret, pstr, cpylen);
			pret += cpylen;
		}
		ret[retlen] = '\0';
	}

end_repl_str:
	/* Free the cache and return the post-replacement string,
	 * which will be NULL in the event of an error. */
	free(pos_cache);
	return ret;
}
