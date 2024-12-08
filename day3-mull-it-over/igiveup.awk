BEGIN {
    read_instruction_mode = 1
    print "read_instruction_mode = " read_instruction_mode;
}

/do\(\)/ {
    print "Resuming reading instructions"
    read_instruction_mode = 1
}

/don't\(\)/ {
    read_instruction_mode = 0
}

/mul\([0-9]{1-3},[0-9]{1,3}\)/ {
    print $0

}
