#!/bin/sh
#
# UCSD p-System filesystem in user space
# Copyright (C) 2006-2008, 2010 Peter Miller
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at
# you option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>
#

TEST_SUBJECT="mknod"
. test_prelude

if test -w /dev/fuse -a -r /etc/mtab
then
    : ok
else
    echo
    echo "    This system does not appear to provide the /dev/fuse device,"
    echo "    needed to run the FUSE tests, probably because it is in a build"
    echo "    chroot.  This test is declared to pass by default."
    echo
    pass
fi

#
# Create a disk image
#
ucsdpsys_mkfs disk.image
test $? -eq 0 || fail

cp disk.image orig.image
test $? -eq 0 || no_result

mkdir mnt
test $? -eq 0 || no_result

#
# Mount the disk image.
#
ucsdpsys_mount $testdir/disk.image $testdir/mnt
test $? -eq 0 || fail

umount_and_fail()
{
    ucsdpsys_umount $testdir/mnt
    fail
}

umount_and_no_result()
{
    ucsdpsys_umount $testdir/mnt
    no_result
}

trap "umount_and_no_result" 1 2 3 15

#
# touch a file in that disk image
#
touch $testdir/mnt/abcdefgh
if test $? -ne 0
then
    umount_and_fail
fi

#
# list the directory contents
#
cat > test.ok << 'fubar'
ABCDEFGH
fubar
if test $? -ne 0
then
    umount_and_no_result
fi

ls $testdir/mnt > test.out
if test $? -ne 0
then
    umount_and_fail
fi

#
# unmount the file system
#
ucsdpsys_umount $testdir/mnt
test $? -eq 0 || fail

#
# Make sure the new file system is OK.
#
ucsdpsys_fsck disk.image
test $? -eq 0 || fail

#
# check the directory listing
#
diff test.ok test.out
test $? -eq 0 || fail

#
# Make sure that the file system image has been changed,
# not just the unix directory it was supposed to be mounted on.
#
cmp disk.image orig.image > /dev/null 2>&1 && fail

#
# The functionality exercised by this test worked.
# No other assertions are made.
#
pass
