#/bin/bash
source languages.sh
LRELEASE="lrelease"

for lang in $LANGUAGES  ;
do
	$LRELEASE stbase_"$lang".ts -qm ./stbase_"$lang".qm
done 

