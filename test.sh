#!/bin/bash

ETAPA=
VALGRIND=false
VERBOSE=false
SIMULATOR=false

while [ "$1" != "" ]; do
	case $1 in
		-e | --etapa ) 			shift
								ETAPA=$1
								;;
		-m | --valgrind )		VALGRIND=true
								;;
		-v | --verbose ) 		VERBOSE=true
								;;
		-s | --simulator )		SIMULATOR=true
								;;
		* )						exit 1
	esac
	shift
done

if [ -z "$ETAPA" ]
then
	echo "Please provide the parameter (-e/--etapa), an integer between 1 and 3"
	exit 1
fi

rm test_result.txt

TEST_DIR="tests/e${ETAPA}/*"

for testfile in $TEST_DIR
do
	echo "************ TEST $testfile ************" >> test_result.txt
	if [ "$VERBOSE" = true ]; then
		# Print the contents of the test file
		cat $testfile >> test_result.txt
	fi
	if [ "$SIMULATOR" = false ]; then 
		if [ "$VALGRIND" = true ]; then
			# Executes with valgrind
			cat $testfile | valgrind ./etapa$ETAPA  >> test_result.txt 2>&1
		else
			# Executes without valgrind
			cat $testfile | ./etapa$ETAPA >> test_result.txt 2>&1
		fi
	else
		cat $testfile | ./etapa$ETAPA | ./ilocsim.py >> test_result.txt 2>&1
	fi
done
