#
# Set rcS vars
#

[ -f /etc/default/rcS ] && . /etc/default/rcS || true

# Ractrends SP 4.0 does not support swap devices. So disabling Swap
NOSWAP=yes
# check for bootoption 'noswap' and do not activate swap
# partitions/files when it is set.
#if [ -r /proc/cmdline ] && grep -q ' noswap' /proc/cmdline ; then
#    NOSWAP=yes
#else
#    NOSWAP=no
#fi

# Accept the same 'quiet' option as the kernel
if [ ! -e /proc/cmdline ] || egrep -qw 'quiet' /proc/cmdline ; then
    VERBOSE="no"
fi

# But allow both rcS and the kernel options 'quiet' to be overrided
# when INIT_VERBOSE=yes is used as well.
[ "$INIT_VERBOSE" ] && VERBOSE="$INIT_VERBOSE" || true
