#include"filesystem.h"
bool access(string filename,int upDirNum)
{
    if(upDirNum!=ROOTDIR)
        return true;
    if(curUser.u_uid!=0)
    {
        int dirnum=dirNum(filename,upDirNum);
        struct inode *pinode=iget(dir_buf[dirnum].d_ino);
        int filemode=pinode->di_mode;
        if(pinode->di_uid!=0)
        {
            if(filemode<=curUser.u_default_mode&&pinode->di_uid==curUser.u_uid)
            return true;
            else
            {
                return false;
            }
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}
