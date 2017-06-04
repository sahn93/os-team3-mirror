#include <linux/gps.h>
#include <linux/uaccess.h>
#include <linux/gpscommon.h>
#include <linux/fs.h>

DEFINE_SPINLOCK(gps_lock);
struct gps_location gpsloc;

asmlinkage int sys_set_gps_location(struct gps_location __user *loc) {
    
    struct gps_location *kbuf;

    if (loc == NULL)
        return -EINVAL;

    kbuf = (struct gps_location*)kmalloc(sizeof(struct gps_location), GFP_KERNEL);
    if (kbuf == NULL)
        return -ENOMEM;

    if (copy_from_user(kbuf, loc, sizeof(struct gps_location))) {
        kfree(kbuf);
        return -EFAULT;
    }

    if (
            (kbuf->lat_fractional < 0 || kbuf->lat_fractional >= 1000000) ||
            (kbuf->lng_fractional < 0 || kbuf->lng_fractional >= 1000000) ||
            (kbuf->lat_integer > 90 || kbuf->lat_integer < -90) ||
            (kbuf->lng_integer > 180 || kbuf->lng_integer < -180) ||
            (kbuf->lat_integer == 90 && kbuf->lat_fractional !=0) ||
            (kbuf->lng_integer == 180 && kbuf->lng_fractional !=0) ||
            (kbuf->accuracy < 0)
       ) {
        kfree(kbuf);
        return -EINVAL;
    }

    spin_lock(&gps_lock);
        gpsloc.lat_integer = kbuf->lat_integer;
        gpsloc.lat_fractional = kbuf->lat_fractional;
        gpsloc.lng_integer = kbuf->lng_integer;
        gpsloc.lng_fractional = kbuf->lng_fractional;
        gpsloc.accuracy = kbuf->accuracy;
    spin_unlock(&gps_lock);

    kfree(kbuf);
    return 0;
}

asmlinkage int sys_get_gps_location(const char __user *pathname, struct gps_location __user *loc) {

	char *kpathname;
	int max_path_len = 200, err;
	struct file *fp;
	struct inode *finode;
	struct gps_location *kloc;

	if(pathname == NULL || loc == NULL)
		return -EINVAL;
	
	kpathname = (char *)kmalloc(sizeof(char)*max_path_len, GFP_KERNEL);

	if(kpathname == NULL)
		return -ENOMEM;
	err = copy_from_user(kpathname, pathname, sizeof(char)*max_path_len);
	if(err) {
		kfree(kpathname);
		return -EFAULT;
	}
		
	fp = filp_open(kpathname, O_RDONLY, 0);
	kfree(kpathname);
		
	if(IS_ERR(fp))
		return (int)PTR_ERR(fp);

	kloc = (struct gps_location *)kmalloc(sizeof(struct gps_location), GFP_KERNEL);
	
	if(kloc == NULL){
		filp_close(fp, NULL);
		return -ENOMEM;
	}

	finode = fp->f_dentry->d_inode;
	
	if(finode == NULL || finode->i_op->get_gps_location == NULL) {
		kfree(kloc);
		filp_close(fp, NULL);
		return -ENODEV;
	}
	
	(finode->i_op->get_gps_location)(finode, kloc);
	
	err = copy_to_user(loc, kloc, sizeof(struct gps_location));
	if(err < 0){
		kfree(kloc);
		filp_close(fp, NULL);
		return -EFAULT;
	}

	kfree(kloc);
	filp_close(fp, NULL);
	return 0;	
}
