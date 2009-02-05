
/* ./.libs/lt-gdaltransform.c - temporary wrapper executable for .libs/gdaltransform.exe
   Generated by ltmain.sh (GNU libtool 1.2444 2007/04/10 19:09:26) 2.1a

   The gdaltransform program cannot be directly executed until all the libtool
   libraries that it depends on are installed.

   This wrapper executable should never be moved out of the build directory.
   If it is, it will not operate correctly.

   Currently, it simply execs the wrapper *script* "/bin/sh gdaltransform",
   but could eventually absorb all of the scripts functionality and
   exec .libs/gdaltransform.exe directly.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#if defined(PATH_MAX)
# define LT_PATHMAX PATH_MAX
#elif defined(MAXPATHLEN)
# define LT_PATHMAX MAXPATHLEN
#else
# define LT_PATHMAX 1024
#endif

#ifndef DIR_SEPARATOR
# define DIR_SEPARATOR '/'
# define PATH_SEPARATOR ':'
#endif

#if defined (_WIN32) || defined (__MSDOS__) || defined (__DJGPP__) || \
  defined (__OS2__)
# define HAVE_DOS_BASED_FILE_SYSTEM
# ifndef DIR_SEPARATOR_2
#  define DIR_SEPARATOR_2 '\\'
# endif
# ifndef PATH_SEPARATOR_2
#  define PATH_SEPARATOR_2 ';'
# endif
#endif

#ifndef DIR_SEPARATOR_2
# define IS_DIR_SEPARATOR(ch) ((ch) == DIR_SEPARATOR)
#else /* DIR_SEPARATOR_2 */
# define IS_DIR_SEPARATOR(ch) \
	(((ch) == DIR_SEPARATOR) || ((ch) == DIR_SEPARATOR_2))
#endif /* DIR_SEPARATOR_2 */

#ifndef PATH_SEPARATOR_2
# define IS_PATH_SEPARATOR(ch) ((ch) == PATH_SEPARATOR)
#else /* PATH_SEPARATOR_2 */
# define IS_PATH_SEPARATOR(ch) ((ch) == PATH_SEPARATOR_2)
#endif /* PATH_SEPARATOR_2 */

#define XMALLOC(type, num)      ((type *) xmalloc ((num) * sizeof(type)))
#define XFREE(stale) do { \
  if (stale) { free ((void *) stale); stale = 0; } \
} while (0)

/* -DDEBUG is fairly common in CFLAGS.  */
#undef DEBUG
#if defined DEBUGWRAPPER
# define DEBUG(format, ...) fprintf(stderr, format, __VA_ARGS__)
#else
# define DEBUG(format, ...)
#endif

const char *program_name = NULL;

void * xmalloc (size_t num);
char * xstrdup (const char *string);
const char * base_name (const char *name);
char * find_executable(const char *wrapper);
int    check_executable(const char *path);
char * strendzap(char *str, const char *pat);
void lt_fatal (const char *message, ...);

int
main (int argc, char *argv[])
{
  char **newargz;
  int i;

  program_name = (char *) xstrdup (base_name (argv[0]));
  DEBUG("(main) argv[0]      : %s\n",argv[0]);
  DEBUG("(main) program_name : %s\n",program_name);
  newargz = XMALLOC(char *, argc+2);
  newargz[0] = (char *) xstrdup("/bin/sh");
  newargz[1] = find_executable(argv[0]);
  if (newargz[1] == NULL)
    lt_fatal("Couldn't find %s", argv[0]);
  DEBUG("(main) found exe at : %s\n",newargz[1]);
  /* we know the script has the same name, without the .exe */
  /* so make sure newargz[1] doesn't end in .exe */
  strendzap(newargz[1],".exe");
  for (i = 1; i < argc; i++)
    newargz[i+1] = xstrdup(argv[i]);
  newargz[argc+1] = NULL;

  for (i=0; i<argc+1; i++)
  {
    DEBUG("(main) newargz[%d]   : %s\n",i,newargz[i]);
    ;
  }

  execv("/bin/sh",(char const **)newargz);
  return 127;
}

void *
xmalloc (size_t num)
{
  void * p = (void *) malloc (num);
  if (!p)
    lt_fatal ("Memory exhausted");

  return p;
}

char *
xstrdup (const char *string)
{
  return string ? strcpy ((char *) xmalloc (strlen (string) + 1), string) : NULL
;
}

const char *
base_name (const char *name)
{
  const char *base;

#if defined (HAVE_DOS_BASED_FILE_SYSTEM)
  /* Skip over the disk name in MSDOS pathnames. */
  if (isalpha ((unsigned char)name[0]) && name[1] == ':')
    name += 2;
#endif

  for (base = name; *name; name++)
    if (IS_DIR_SEPARATOR (*name))
      base = name + 1;
  return base;
}

int
check_executable(const char * path)
{
  struct stat st;

  DEBUG("(check_executable)  : %s\n", path ? (*path ? path : "EMPTY!") : "NULL!");
  if ((!path) || (!*path))
    return 0;

  if ((stat (path, &st) >= 0) &&
      (
	/* MinGW & native WIN32 do not support S_IXOTH or S_IXGRP */
#if defined (S_IXOTH)
       ((st.st_mode & S_IXOTH) == S_IXOTH) ||
#endif
#if defined (S_IXGRP)
       ((st.st_mode & S_IXGRP) == S_IXGRP) ||
#endif
       ((st.st_mode & S_IXUSR) == S_IXUSR))
      )
    return 1;
  else
    return 0;
}

/* Searches for the full path of the wrapper.  Returns
   newly allocated full path name if found, NULL otherwise */
char *
find_executable (const char* wrapper)
{
  int has_slash = 0;
  const char* p;
  const char* p_next;
  /* static buffer for getcwd */
  char tmp[LT_PATHMAX + 1];
  int tmp_len;
  char* concat_name;

  DEBUG("(find_executable)  : %s\n", wrapper ? (*wrapper ? wrapper : "EMPTY!") : "NULL!");

  if ((wrapper == NULL) || (*wrapper == '\0'))
    return NULL;

  /* Absolute path? */
#if defined (HAVE_DOS_BASED_FILE_SYSTEM)
  if (isalpha ((unsigned char)wrapper[0]) && wrapper[1] == ':')
  {
    concat_name = xstrdup (wrapper);
    if (check_executable(concat_name))
      return concat_name;
    XFREE(concat_name);
  }
  else
  {
#endif
    if (IS_DIR_SEPARATOR (wrapper[0]))
    {
      concat_name = xstrdup (wrapper);
      if (check_executable(concat_name))
	return concat_name;
      XFREE(concat_name);
    }
#if defined (HAVE_DOS_BASED_FILE_SYSTEM)
  }
#endif

  for (p = wrapper; *p; p++)
    if (*p == '/')
    {
      has_slash = 1;
      break;
    }
  if (!has_slash)
  {
    /* no slashes; search PATH */
    const char* path = getenv ("PATH");
    if (path != NULL)
    {
      for (p = path; *p; p = p_next)
      {
	const char* q;
	size_t p_len;
	for (q = p; *q; q++)
	  if (IS_PATH_SEPARATOR(*q))
	    break;
	p_len = q - p;
	p_next = (*q == '\0' ? q : q + 1);
	if (p_len == 0)
	{
	  /* empty path: current directory */
	  if (getcwd (tmp, LT_PATHMAX) == NULL)
	    lt_fatal ("getcwd failed");
	  tmp_len = strlen(tmp);
	  concat_name = XMALLOC(char, tmp_len + 1 + strlen(wrapper) + 1);
	  memcpy (concat_name, tmp, tmp_len);
	  concat_name[tmp_len] = '/';
	  strcpy (concat_name + tmp_len + 1, wrapper);
	}
	else
	{
	  concat_name = XMALLOC(char, p_len + 1 + strlen(wrapper) + 1);
	  memcpy (concat_name, p, p_len);
	  concat_name[p_len] = '/';
	  strcpy (concat_name + p_len + 1, wrapper);
	}
	if (check_executable(concat_name))
	  return concat_name;
	XFREE(concat_name);
      }
    }
    /* not found in PATH; assume curdir */
  }
  /* Relative path | not found in path: prepend cwd */
  if (getcwd (tmp, LT_PATHMAX) == NULL)
    lt_fatal ("getcwd failed");
  tmp_len = strlen(tmp);
  concat_name = XMALLOC(char, tmp_len + 1 + strlen(wrapper) + 1);
  memcpy (concat_name, tmp, tmp_len);
  concat_name[tmp_len] = '/';
  strcpy (concat_name + tmp_len + 1, wrapper);

  if (check_executable(concat_name))
    return concat_name;
  XFREE(concat_name);
  return NULL;
}

char *
strendzap(char *str, const char *pat)
{
  size_t len, patlen;

  assert(str != NULL);
  assert(pat != NULL);

  len = strlen(str);
  patlen = strlen(pat);

  if (patlen <= len)
  {
    str += len - patlen;
    if (strcmp(str, pat) == 0)
      *str = '\0';
  }
  return str;
}

static void
lt_error_core (int exit_status, const char * mode,
	  const char * message, va_list ap)
{
  fprintf (stderr, "%s: %s: ", program_name, mode);
  vfprintf (stderr, message, ap);
  fprintf (stderr, ".\n");

  if (exit_status >= 0)
    exit (exit_status);
}

void
lt_fatal (const char *message, ...)
{
  va_list ap;
  va_start (ap, message);
  lt_error_core (EXIT_FAILURE, "FATAL", message, ap);
  va_end (ap);
}
