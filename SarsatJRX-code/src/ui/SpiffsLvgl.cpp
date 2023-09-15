#include <ui/SpiffsLvgl.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Filesystems.h>
#include <Hardware.h>

// Filesystem
static bool filesystemMounted = false;
static FS* fileSystem = nullptr;


static void * fs_open (lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    (void)drv; /*Unused*/
    if(!filesystemMounted)
    {
        return nullptr;
    }
    File file = SPIFFS.open(path, FILE_READ);
    return new File(file);
}

static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p)
{
    (void)drv; /*Unused*/
    File *fp = (File *)file_p;
    (*fp).close();
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p,
                       void *buf, uint32_t btr, uint32_t *br)
{
    (void)drv; /*Unused*/
    File *fp = (File *)file_p;
    *br = (*fp).read((uint8_t *)buf, btr);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_seek (lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    (void)drv; /*Unused*/
    File *fp = (File *)file_p;
    (*fp).seek(pos);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    (void)drv; /*Unused*/
    File *fp = (File *)file_p;
    uint32_t tmp = (*fp).position();
    pos_p = &tmp;
    return LV_FS_RES_OK;
}

void spiffs_drv_init()
{   // Check SPIFSS
    Filesystems* filesystems = Hardware::getHardware()->getFilesystems();
    filesystemMounted = filesystems->isSpiFilesystemMounted();
    fileSystem = filesystems->getSpiFilesystem();

    static lv_fs_drv_t drv;
    lv_fs_drv_init(&drv); /*Basic initialization*/

    drv.letter = 'J';             /*An uppercase letter to identify the drive */
    drv.open_cb = fs_open ;   /*Callback to open a file */
    drv.close_cb = fs_close; /*Callback to close a file */
    drv.read_cb = fs_read;   /*Callback to read a file */
    drv.seek_cb = fs_seek;   /*Callback to seek in a file (Move cursor) */
    drv.tell_cb = fs_tell;   /*Callback to tell the cursor position  */
    lv_fs_drv_register(&drv); /*Finally register the drive*/
}

