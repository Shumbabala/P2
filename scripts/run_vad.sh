#!/bin/bash

adaptive=${1:-1}
IS=${2:-70} #initial standby
SS=${3:-90} #silence standby
VS=${4:-10} #voice standby
M=${5:-2}

# Be sure that this file has execution permissions:
# Use the nautilus explorer or chmod +x run_vad.sh

# Establecemos que el código de retorno de un pipeline sea el del último programa con código de retorno
# distinto de cero, o cero si todos devuelven cero.
set -o pipefail

# Write here the name and path of your program and database
DIR_P2=$HOME/PAV/P2
DB=$DIR_P2/db.v4
#CMD="$DIR_P2/bin/vad -1 $alpha1" #-1 6 are lab comments (also added "" to form the string)
CMD="$DIR_P2/bin/vad -A $adaptive --initial_standby $IS --silence_standby $SS --voice_standby $VS -M $M"

for filewav in $DB/*/*wav; do
#    echo
    echo "**************** $filewav ****************"
    if [[ ! -f $filewav ]]; then 
	    echo "Wav file not found: $filewav" >&2
	    exit 1
    fi

    filevad=${filewav/.wav/.vad}

    $CMD -i $filewav -o $filevad || exit 1

# Alternatively, uncomment to create output wave files
#    filewavOut=${filewav/.wav/.vad.wav}
#    $CMD $filewav $filevad $filewavOut || exit 1

done

perl scripts/vad_evaluation.pl $DB/*/*lab

exit 0
