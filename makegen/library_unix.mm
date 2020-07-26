#!/bin/sh

. $MACRO_DIR/makegen/makegen.cf

if test $OS_TYPE != "_OS_UNIX"
then
	exit
fi


if echo '\c' | grep -s c >/dev/null 2>&1
then
	ECHO_N="echo -n"
	ECHO_C=""
else
	ECHO_N="echo"
	ECHO_C='\c'
fi


lib=$1
libsrc=`echo $2 | sed "s/,/ /g"`
libobj=`echo $3 | sed "s/,/ /g"`

for src in $libsrc
do
	if test -c /dev/tty
	then
		$ECHO_N ".$ECHO_C" > /dev/tty
	fi
	base=`echo $src | sed "s/\..*//"`
	obj=`echo $src | sed "s/\.c\$/.o/"`
	libobj="$libobj $obj"
	echo	"$obj : $src Makefile.full"
	echo	'	$(CC) $(CC_FLAGS) -c '"$src"
	echo
	echo	"clean ::"
	echo	"	rm -f $obj"
	echo
done

echo	"all :: $lib"
echo
echo	"$lib : $libobj Makefile.full"
echo	'	$(AR) rc '"$lib $libobj"
echo	"	$ranlib $lib"
echo
echo	"clean :: "
echo	"	rm -f $lib"
echo
