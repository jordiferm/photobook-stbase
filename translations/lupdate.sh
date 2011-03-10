#!/bin/bash
LUPDATE="lupdate -no-obsolete"
source languages.sh

STDIRS=""
for stdir in `ls ../` ;
do 
	if [ -d ../$stdir ]
	then 
		if [ $stdir != "openrpt" ]
		then 
			if [ $stdir != "translations" ]
			then
				STDIRS=$STDIRS" ../$stdir"
			fi
		fi
	fi
done

#Add manually dirs containing .mtd
STDIRS=$STDIRS" ../sindarl/database/*.mtd"

for i in $LANGUAGES ;
do
	$LUPDATE $STDIRS -ts stbase_"$i".ts
done 
