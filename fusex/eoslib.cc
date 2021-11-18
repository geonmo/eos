#include <sys/types.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <eoslib.hh>

extern "C"
{
int access(const char *path, int amode)
{
  return 0;
}
}

/******************************************************************************/
/*                                 c h d i r                                  */
/******************************************************************************/

extern "C"
{
int     chdir(const char *path)
{
  return 0;
}
}

/******************************************************************************/
/*                                 c l o s e                                  */
/******************************************************************************/

extern "C"
{
int     close(int fildes)
{
  return 0;
}
}

/******************************************************************************/
/*                              c l o s e d i r                               */
/******************************************************************************/
extern "C"
{
int     closedir(DIR *dirp)
{
  return 0;
}
}
/******************************************************************************/
/*                                 c r e a t                                  */
/******************************************************************************/
extern "C"
{
int     creat64(const char *path, mode_t mode)
{
  return 0;
}
}
/******************************************************************************/
/*                                f c l o s e                                 */
/******************************************************************************/
extern "C"
{
int fclose(FILE *stream)
{
  return 0;
}
}
/******************************************************************************/
/*                               f c n t l 6 4                                */
/******************************************************************************/
extern "C"
{
int     fcntl64(int fd, int cmd, ...)
{
  return 0;
}
}
/******************************************************************************/
/*                             f d a t a s y n c                              */
/******************************************************************************/
// On Mac it is the same as fsync
#if !defined(__APPLE__)
extern "C"
{
int     fdatasync(int fildes)
{
  return 0;
}
}
#endif
/******************************************************************************/
/*                                f f l u s h                                 */
/******************************************************************************/
extern "C"
{
int    fflush(FILE *stream)
{
  return 0;
}
}
/******************************************************************************/
/*                                 f o p e n                                  */
/******************************************************************************/
extern "C"
{
FILE  *fopen64(const char *path, const char *mode)
{
  return 0;
}
}
/******************************************************************************/
/*                                 f r e a d                                  */
/******************************************************************************/
extern "C"
{
size_t fread(void *ptr, size_t size, size_t nitems, FILE *stream)
{
  return 0;
}
}
/******************************************************************************/
/*                                 f s e e k                                  */
/******************************************************************************/
extern "C"
{
int fseek(FILE *stream, long offset, int whence)
{
  return 0;
}
}
/******************************************************************************/
/*                                f s e e k o                                 */
/******************************************************************************/
extern "C"
{
int fseeko64(FILE *stream, off64_t offset, int whence)
{
  return 0;
}
}
/******************************************************************************/
/*                                 f s t a t                                  */
/******************************************************************************/
extern "C"
{
#if defined(__linux__) and defined(_STAT_VER) and __GNUC__ and __GNUC__ >= 2
int  __fxstat64(int ver, int fildes, struct stat64 *buf)
#else
int     fstat64(         int fildes, struct stat64 *buf)
#endif
{
  return 0;
}
}
/******************************************************************************/
/*                                 f s y n c                                  */
/******************************************************************************/
extern "C"
{
int     fsync(int fildes)
{
  return 0;
}
}
/******************************************************************************/
/*                                 f t e l l                                  */
/******************************************************************************/
extern "C"
{
long    ftell(FILE *stream)
{
  return 0;
}
}
/******************************************************************************/
/*                                f t e l l o                                 */
/******************************************************************************/
extern "C"
{
off64_t ftello64(FILE *stream)
{
  return 0;
}
}
/******************************************************************************/
/*                             f t r u n c a t e                              */
/******************************************************************************/
extern "C"
{
int ftruncate64(int fildes, off_t offset)
{
  return 0;
}
}
/******************************************************************************/
/*                                f w r i t e                                 */
/******************************************************************************/
extern "C"
{
size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream)
{
  return 0;
}
}
/******************************************************************************/
/*                             f g e t x a t t r                              */
/******************************************************************************/
#if defined(__linux__) || defined(__GNU__) || (defined(__FreeBSD_kernel__) && defined(__GLIBC__))
extern "C"
{
ssize_t fgetxattr (int fd, const char *name, void *value, size_t size)
{
  return 0;
}
}
#endif
/******************************************************************************/
/*                              g e t x a t t r                               */
/******************************************************************************/
#if defined(__linux__) || defined(__GNU__) || (defined(__FreeBSD_kernel__) && defined(__GLIBC__))
extern "C"
{
ssize_t getxattr (const char *path, const char *name, void *value, size_t size)
{
  return 0;
}
}
#endif
/******************************************************************************/
/*                             l g e t x a t t r                              */
/******************************************************************************/
#if defined(__linux__) || defined(__GNU__) || (defined(__FreeBSD_kernel__) && defined(__GLIBC__))
extern "C"
{
ssize_t lgetxattr (const char *path, const char *name, void *value, size_t size)
{
  return 0;
}
}
#endif
/******************************************************************************/
/*                                 l s e e k                                  */
/******************************************************************************/
extern "C"
{
off64_t lseek64(int fildes, off64_t offset, int whence)
{
  return 0;
}
}
/******************************************************************************/
/*                                l l s e e k                                 */
/******************************************************************************/
extern "C"
{
off_t      llseek(int fildes, off_t    offset, int whence)
{
  return 0;
}
}
/******************************************************************************/
/*                                 l s t a t                                  */
/******************************************************************************/
extern "C"
{
#if defined __linux__ && __GNUC__ && __GNUC__ >= 2
int     __lxstat64(int ver, const char *path, struct stat64 *buf)
#else
int        lstat64(         const char *path, struct stat64 *buf)
#endif
{
  return 0;
}
}
/******************************************************************************/
/*                                 m k d i r                                  */
/******************************************************************************/
extern "C"
{
int     mkdir(const char *path, mode_t mode)
{
  return 0;
}
}
/******************************************************************************/
/*                                  o p e n                                   */
/******************************************************************************/
extern "C"
{
int     open64(const char *path, int oflag, ...)
{
   va_list ap;
   int mode;

   va_start(ap, oflag);
   mode = va_arg(ap, int);
   va_end(ap);
   return 0;
}
}
/******************************************************************************/
/*                               o p e n d i r                                */
/******************************************************************************/
extern "C"
{
DIR*    opendir(const char *path)
{
  return 0;
}
}
/******************************************************************************/
/*                                 p r e a d                                  */
/******************************************************************************/
extern "C"
{
ssize_t pread64(int fildes, void *buf, size_t nbyte, off_t offset)
{
  return 0;
}
}
/******************************************************************************/
/*                                p w r i t e                                 */
/******************************************************************************/
extern "C"
{
ssize_t pwrite64(int fildes, const void *buf, size_t nbyte, off_t offset)
{
  return 0;
}
}
/******************************************************************************/
/*                                  r e a d                                   */
/******************************************************************************/
extern "C"
{
ssize_t read(int fildes, void *buf, size_t nbyte)
{
  return 0;
}
}
/******************************************************************************/
/*                                 r e a d v                                  */
/******************************************************************************/
extern "C"
{
ssize_t readv(int fildes, const struct iovec *iov, int iovcnt)
{
  return 0;
}
}
/******************************************************************************/
/*                               r e a d d i r                                */
/******************************************************************************/
extern "C"
{
struct dirent64* readdir64(DIR *dirp)
{
  return 0;
}
}
/******************************************************************************/
/*                             r e a d d i r _ r                              */
/******************************************************************************/
extern "C"
{
int     readdir64_r(DIR *dirp, struct dirent64 *entry, struct dirent64 **result)
{
  return 0;
}
}
/******************************************************************************/
/*                                r e n a m e                                 */
/******************************************************************************/
extern "C"
{
int     rename(const char *oldpath, const char *newpath)
{
  return 0;
}
}
/******************************************************************************/
/*                             r e w i n d d i r                              */
/******************************************************************************/
#ifndef rewinddir
extern "C"
{
void    rewinddir(DIR *dirp)
{
  return ;
}
}
#endif
/******************************************************************************/
/*                                 r m d i r                                  */
/******************************************************************************/
extern "C"
{
int     rmdir(const char *path)
{
  return 0;
}
}
/******************************************************************************/
/*                               s e e k d i r                                */
/******************************************************************************/
extern "C"
{
void    seekdir(DIR *dirp, long loc)
{
  return ;
}
}
/******************************************************************************/
/*                                  s t a t                                   */
/******************************************************************************/
extern "C"
{
#if defined __linux__ && __GNUC__ && __GNUC__ >= 2
int     __xstat64(int ver, const char *path, struct stat64 *buf)
#else
int        stat64(         const char *path, struct stat64 *buf)
#endif
{
  return 0;
}
}
/******************************************************************************/
/*                                s t a t f s                                 */
/******************************************************************************/
extern "C"
{
int        statfs64(       const char *path, struct statfs64 *buf)
{
  return 0;
}
}
/******************************************************************************/
/*                               s t a t v f s                                */
/******************************************************************************/
extern "C"
{
int        statvfs64(         const char *path, struct statvfs64 *buf)
{
  return 0;
}
}
/******************************************************************************/
/*                               t e l l d i r                                */
/******************************************************************************/
extern "C"
{
long    telldir(DIR *dirp)
{
  return 0;
}
}
/******************************************************************************/
/*                              t r u n c a t e                               */
/******************************************************************************/
extern "C"
{
int truncate64(const char *path, off_t offset)
{
  return 0;
}
}
/******************************************************************************/
/*                                u n l i n k                                 */
/******************************************************************************/
extern "C"
{
int     unlink(const char *path)
{
  return 0;
}
}
/******************************************************************************/
/*                                 w r i t e                                  */
/******************************************************************************/
extern "C"
{
ssize_t write(int fildes, const void *buf, size_t nbyte)
{
  return 0;
}
}
/******************************************************************************/
/*                                w r i t e v                                 */
/******************************************************************************/
extern "C"
{
ssize_t writev(int fildes, const struct iovec *iov, int iovcnt)
{
  return 0;
}
}
