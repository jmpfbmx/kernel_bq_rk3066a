/*
 * include/linux/nfsd/nfsfh.h
 *
 * This file describes the layout of the file handles as passed
 * over the wire.
 *
 * Earlier versions of knfsd used to sign file handles using keyed MD5
 * or SHA. I've removed this code, because it doesn't give you more
 * security than blocking external access to port 2049 on your firewall.
 *
 * Copyright (C) 1995, 1996, 1997 Olaf Kirch <okir@monad.swb.de>
 */

#ifndef _LINUX_NFSD_FH_H
#define _LINUX_NFSD_FH_H

#include <asm/types.h>
#include <linux/nfsd/const.h>
#include <linux/nfsd/debug.h>

/*
 * This is the old "dentry style" Linux NFSv2 file handle.
 *
 * The xino and xdev fields are currently used to transport the
 * ino/dev of the exported inode.
 */
struct nfs_fhbase_old {
	__u32		fb_dcookie;	/* dentry cookie - always 0xfeebbaca */
	__u32		fb_ino;		/* our inode number */
	__u32		fb_dirino;	/* dir inode number, 0 for directories */
	__u32		fb_dev;		/* our device */
	__u32		fb_xdev;
	__u32		fb_xino;
	__u32		fb_generation;
};

/*
 * This is the new flexible, extensible style NFSv2/v3 file handle.
 * by Neil Brown <neilb@cse.unsw.edu.au> - March 2000
 *
 * The file handle is seens as a list of 4byte words.
 * The first word contains a version number (1) and four descriptor bytes
 * that tell how the remaining 3 variable length fields should be handled.
 * These three bytes are auth_type, fsid_type and fileid_type.
 *
 * All 4byte values are in host-byte-order.
 *
 * The auth_type field specifies how the filehandle can be authenticated
 * This might allow a file to be confirmed to be in a writable part of a
 * filetree without checking the path from it upto the root.
 * Current values:
 *     0  - No authentication.  fb_auth is 0 bytes long
 * Possible future values:
 *     1  - 4 bytes taken from MD5 hash of the remainer of the file handle
 *          prefixed by a secret and with the important export flags.
 *
 * The fsid_type identifies how the filesystem (or export point) is
 *    encoded.
 *  Current values:
 *     0  - 4 byte device id (ms-2-bytes major, ls-2-bytes minor), 4byte inode number
 *        NOTE: we cannot use the kdev_t device id value, because kdev_t.h
 *              says we mustn't.  We must break it up and reassemble.
 *     1  - 4 byte user specified identifier
 *     2  - 4 byte major, 4 byte minor, 4 byte inode number - DEPRECATED
 *     3  - 4 byte device id, encoded for user-space, 4 byte inode number
 *
 * The fileid_type identified how the file within the filesystem is encoded.
 * This is (will be) passed to, and set by, the underlying filesystem if it supports
 * filehandle operations.  The filesystem must not use the value '0' or '0xff' and may
 * only use the values 1 and 2 as defined below:
 *  Current values:
 *    0   - The root, or export point, of the filesystem.  fb_fileid is 0 bytes.
 *    1   - 32bit inode number, 32 bit generation number.
 *    2   - 32bit inode number, 32 bit generation number, 32 bit parent directory inode number.
 *
 */
struct nfs_fhbase_new {
	__u8		fb_version;	/* == 1, even => nfs_fhbase_old */
	__u8		fb_auth_type;
	__u8		fb_fsid_type;
	__u8		fb_fileid_type;
	__u32		fb_auth[1];
/*	__u32		fb_fsid[0]; floating */
/*	__u32		fb_fileid[0]; floating */
};

struct knfsd_fh {
	unsigned int	fh_size;	/* significant for NFSv3.
					 * Points to the current size while building
					 * a new file handle
					 */
	union {
		struct nfs_fhbase_old	fh_old;
		__u32			fh_pad[NFS4_FHSIZE/4];
		struct nfs_fhbase_new	fh_new;
	} fh_base;
};

#define ofh_dcookie		fh_base.fh_old.fb_dcookie
#define ofh_ino			fh_base.fh_old.fb_ino
#define ofh_dirino		fh_base.fh_old.fb_dirino
#define ofh_dev			fh_base.fh_old.fb_dev
#define ofh_xdev		fh_base.fh_old.fb_xdev
#define ofh_xino		fh_base.fh_old.fb_xino
#define ofh_generation		fh_base.fh_old.fb_generation

#define	fh_version		fh_base.fh_new.fb_version
#define	fh_fsid_type		fh_base.fh_new.fb_fsid_type
#define	fh_auth_type		fh_base.fh_new.fb_auth_type
#define	fh_fileid_type		fh_base.fh_new.fb_fileid_type
#define	fh_auth			fh_base.fh_new.fb_auth
#define	fh_fsid			fh_base.fh_new.fb_auth



#endif /* _LINUX_NFSD_FH_H */
